#pragma once
#include <ito/utils/math.hpp>
#include <random>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <execution>

namespace ito::method {

    template<math::Arithmetic T = double>
    struct MonteCarloCreateInfo {
        size_t num_simulations = 100'000;
        unsigned seed = std::random_device{}();
        
        enum class ExecutionPolicy {
            Auto,        // Library decides (default)
            Sequential,  // Force sequential
            Parallel     // Force parallel
        };

        ExecutionPolicy policy = ExecutionPolicy::Auto;
    };

    template<math::Arithmetic T = double>
    struct MonteCarloResult {
        T price;
        T standard_error;

        T confidence_interval() const {
            return static_cast<T>(1.96) * standard_error;
        }
    };

    template<math::Arithmetic T = double>
    class MonteCarloPricer {
        //private:
    public:
        MonteCarloCreateInfo<T> config_;
        mutable std::mt19937 rng_;
        mutable std::normal_distribution<T> normal_;

        // GBM simulation - PRIVATE helper
        T simulate_gbm_terminal(
            T S0,
            T r,
            T sigma,
            T time
        ) const {
            // Step 1 - Generate random Z ~ N(0,1)
            T Z = normal_(rng_);

            // Step 2 - Compute drift term: (r - σ²/2) * T
            T drift = (r - (sigma * sigma) / static_cast<T>(2)) * time;

            // Step 3 - Compute diffusion term: σ * √T * Z
            T diffusion = sigma * std::sqrt(time) * Z;

            // Step 4 - Compute S(T) = S₀ * exp(drift + diffusion)
            return S0 * std::exp(drift + diffusion);
        }

        // Statistics computation - PRIVATE helper
        MonteCarloResult<T> compute_statistics(
            const std::vector<T>& payoffs,
            T discount_factor
        ) const {
            const size_t N = payoffs.size();
            // Step 1 - Compute mean of payoffs
            T sum = std::accumulate(payoffs.begin(), payoffs.end(), T{});
            T mean = sum / N;

            // Step 2 - Compute variance
            // variance = (1/(N-1)) * Σ(payoff - mean)²
            T variance = static_cast<T>(0);
            for (const auto& payoff : payoffs) {
                T diff = payoff - mean;
                variance += diff * diff;
            }
            variance /= N - 1;

            // Step 3 - Compute standard error
            // std_error = sqrt(variance / N)
            T std_error = std::sqrt(variance / N);

            // Return discounted results
            return {
                .price = discount_factor * mean,
                .standard_error = discount_factor * std_error
            };
        }

    public:
        explicit MonteCarloPricer(const MonteCarloCreateInfo<T>& config = {})
            : config_(config)
            , rng_(config.seed)
            , normal_(0.0, 1.0)
        {
        }

        struct CallPutResult {
            MonteCarloResult<T> call;
            MonteCarloResult<T> put;
        };

        CallPutResult price_european_call_and_put(
            T S0, T K, T r, T sigma, T time
        ) const {
            switch (config_.policy) {
            case MonteCarloCreateInfo<T>::ExecutionPolicy::Sequential:
                return price_european_call_and_put_sequential(S0, K, r, sigma, time);

            case MonteCarloCreateInfo<T>::ExecutionPolicy::Parallel:
                return price_european_call_and_put_parallel(S0, K, r, sigma, time);

            case MonteCarloCreateInfo<T>::ExecutionPolicy::Auto:
            default:
                // Auto-decide based on simulation count
                if (config_.num_simulations >= 10'000) {
                    return price_european_call_and_put_parallel(S0, K, r, sigma, time);
                }
                return price_european_call_and_put_sequential(S0, K, r, sigma, time);
            }
        }

    private:
        CallPutResult price_european_call_and_put_parallel(
            T S0, T K, T r, T sigma, T time
        ) const {
            // Step 1: Pre-generate all random numbers (thread-safe)
            std::vector<T> random_normals(config_.num_simulations);

            // Generate randoms sequentially (RNG is NOT thread-safe)
            for (size_t i = 0; i < config_.num_simulations; ++i) {
                random_normals[i] = normal_(rng_);
            }

            // Step 2: Compute terminal prices in parallel
            std::vector<T> terminal_prices(config_.num_simulations);

            // Precompute constants
            const T drift = (r - (sigma * sigma) / static_cast<T>(2)) * time;
            const T vol_sqrt_t = sigma * std::sqrt(time);

            // PARALLEL TRANSFORM: Each thread computes S(T) independently
            std::transform(
                std::execution::par,  // ← PARALLEL!
                random_normals.begin(),
                random_normals.end(),
                terminal_prices.begin(),
                [S0, drift, vol_sqrt_t](T Z) {
                    return S0 * std::exp(drift + vol_sqrt_t * Z);
                }
            );

            // Step 3: Compute payoffs in parallel
            std::vector<T> call_payoffs(config_.num_simulations);
            std::vector<T> put_payoffs(config_.num_simulations);

            std::transform(
                std::execution::par,  // ← PARALLEL!
                terminal_prices.begin(),
                terminal_prices.end(),
                call_payoffs.begin(),
                [K](T ST) { return std::max(ST - K, T{}); }
            );

            std::transform(
                std::execution::par,  // ← PARALLEL!
                terminal_prices.begin(),
                terminal_prices.end(),
                put_payoffs.begin(),
                [K](T ST) { return std::max(K - ST, T{}); }
            );

            // Step 4: Compute statistics (discount and return)
            T DF = std::exp(-r * time);

            return {
                .call = compute_statistics(call_payoffs, DF),
                .put = compute_statistics(put_payoffs, DF)
            };
        }

        // Price both call and put using the SAME simulated paths
        CallPutResult price_european_call_and_put_sequential(
            T S0,
            T K,
            T r,
            T sigma,
            T time
        ) const {
            std::vector<T> call_payoffs;
            std::vector<T> put_payoffs;
            call_payoffs.reserve(config_.num_simulations);
            put_payoffs.reserve(config_.num_simulations);

            for (size_t i{}; i < config_.num_simulations; ++i) {
                // Simulate terminal price ONCE
                T ST = simulate_gbm_terminal(S0, r, sigma, time);

                // Compute BOTH payoffs from the SAME simulated price
                call_payoffs.push_back(std::max(ST - K, T{}));
                put_payoffs.push_back(std::max(K - ST, T{}));
            }

            // Discount factor
            T DF = std::exp(-r * time);

            // Return both results
            return {
                .call = compute_statistics(call_payoffs, DF),
                .put = compute_statistics(put_payoffs, DF)
            };
        }
    };
} // namespace ito::method