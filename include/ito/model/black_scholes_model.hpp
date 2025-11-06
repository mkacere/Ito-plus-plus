#pragma once
#include <ito/utils/math.hpp>
#include <cmath>
#include <stdexcept>

namespace ito::model {
    template<math::Arithmetic T = double>
    struct BlackScholesCreateInfo {
        T spot_price;           // S - current price of underlying
        T strike_price;         // K - strike/exercise price
        T risk_free_rate;       // r - risk-free interest rate (annualized)
        T volatility;           // σ (sigma) - volatility (annualized)
        T time_to_maturity;     // T - time to expiration (in years)

        constexpr void validate() const {
            if (spot_price <= 0)
                throw std::invalid_argument("Spot price must be positive");
            if (strike_price <= 0)
                throw std::invalid_argument("Strike price must be positive");
            if (volatility < 0)
                throw std::invalid_argument("Volatility cannot be negative");
            if (time_to_maturity <= 0)
                throw std::invalid_argument("Time to maturity must be positive");
            // Note: risk_free_rate can be negative (modern markets!)
        }
    };

    template<math::Arithmetic T = double>
    class BlackScholesModel {
    public:
        // Nested struct for Greeks (specific to Black-Scholes)
        struct Greeks {
            T delta = static_cast<T>(0);
            T gamma = static_cast<T>(0);
            T vega  = static_cast<T>(0);
            T theta = static_cast<T>(0);
            T rho   = static_cast<T>(0);
        };

    private:
        // Store the parameters
        T S_;      // Spot price
        T K_;      // Strike price
        T r_;      // Risk-free rate
        T sigma_;  // Volatility
        T T_;      // Time to maturity

        // Cached intermediate values
        mutable T d1_ = static_cast<T>(0);
        mutable T d2_ = static_cast<T>(0);
        mutable bool d_cached_ = false;

        // Cached Greeks
        mutable Greeks call_greeks_{};
        mutable Greeks put_greeks_{};
        mutable bool call_greeks_cached_ = false;
        mutable bool put_greeks_cached_ = false;

        // Private helper to compute d1 and d2
        void compute_d() const {
            // T = T_, not <T> for these equations:
            // d1 = [ln(S/K) + (r + sigma^2/2)*T] / (sigma * sqrt(T))
            // d2 = d1 - sigma * sqrt(T)
            if (d_cached_) return;
            const T sqrt_T = std::sqrt(T_);
            const T sigma_sqrt_T = sigma_ * sqrt_T;

            d1_ = (std::log(S_ / K_) + (r_ + sigma_ * sigma_ / static_cast<T>(2)) * T_) 
                / sigma_sqrt_T;
            d2_ = d1_ - sigma_sqrt_T;

            d_cached_ = true;
        }

        void compute_call_greeks() const {
            if (call_greeks_cached_) return;
            compute_d();  // Ensure d1, d2 are ready
            
            // reused
            auto Phi = math::normal_cdf<T>;
            auto phi = math::normal_pdf<T>;
            const T sqrt_T = std::sqrt(T_);
            const T exp_neg_rT = std::exp(-r_ * T_);
            const T phi_d1 = phi(d1_);

            // delta = Phi(d1)
            call_greeks_.delta = Phi(d1_);
            // gamma = phi(d1) / (S * sigma * sqrt(t))
            call_greeks_.gamma = phi_d1 / (S_ * sigma_ * sqrt_T);
            // vega = S * phi(d1) * sqrt(T)
            call_greeks_.vega = S_ * phi_d1 * sqrt_T;
            // theta = -(S * phi(d1) * sigma) / (2sqrt(T) - r * K * e^(-rT) * Phi(d2)
            call_greeks_.theta = -(S_ * phi_d1 * sigma_) / (static_cast<T>(2) * sqrt_T)
                - r_ * K_ * exp_neg_rT * Phi(d2_);
            // rho = K * T * e^(-rT) * Phi(d2)
            call_greeks_.rho = K_ * T_ * exp_neg_rT * Phi(d2_);

            call_greeks_cached_ = true;
        }

        void compute_put_greeks() const {
            if (put_greeks_cached_) return;
            compute_d();
            
            // reused
            auto Phi = math::normal_cdf<T>;
            auto phi = math::normal_pdf<T>;
            const T sqrt_T = std::sqrt(T_);
            const T exp_neg_rT = std::exp(-r_ * T_);
            const T phi_d1 = phi(d1_);

            //delta = Phi(d1) - 1 [or -Phi(-d1)]
            put_greeks_.delta = Phi(d1_) - static_cast<T>(1);
            // gamma = same as call
            put_greeks_.gamma = phi_d1 / (S_ * sigma_ * sqrt_T);
            // vega = same as call
            put_greeks_.vega = S_ * phi_d1 * sqrt_T;
            // theta = -(S * phi(d1) * sigma) / (2sqrt(T)) + r * K * e^(-rT) * Phi(-d2)
            put_greeks_.theta = -(S_ * phi_d1 * sigma_) / (static_cast<T>(2) * sqrt_T)
                + r_ * K_ * exp_neg_rT * Phi(-d2_);
            // rho = -K * T * e^(-rT) * Phi(-d2)
            put_greeks_.rho = -K_ * T_ * exp_neg_rT * Phi(-d2_);

            put_greeks_cached_ = true;
        }

    public:
        // Constructor takes the create info
        explicit BlackScholesModel(const BlackScholesCreateInfo<T>& info)
            : S_(info.spot_price)
            , K_(info.strike_price)
            , r_(info.risk_free_rate)
            , sigma_(info.volatility)
            , T_(info.time_to_maturity)
            , d_cached_(false)
        {
            // should be validated before passed, but doesnt hurt to double check
            // minimal performance lost (optional)
            info.validate();
        }

        // Pricing methods
        T call_price() const {
            compute_d();
            // C = S*Phi(d1_) - Ke^(-rT)*Phi(d2)
            auto Phi = ito::math::normal_cdf<T>;
            T C = S_ * Phi(d1_) - K_ * std::exp(-r_ * T_) * Phi(d2_);
            return C;
        }

        T put_price() const {

            // switch comments to use which ever one you want
            // Direct Formula:
            // P = Ke^(-rT)*Phi(d2) - S*Phi(d1_)
            //compute_d();
            //auto Phi = ito::math::normal_cdf<T>;
            //T P = K_ * std::exp(-r_ * T_) * Phi(d2_) - S_ * Phi(d1_);
            
            // Put-call arity (simpler to code):
            // P = C - S + K*e^(-rT)
            T P = call_price() - S_ + K_ * std::exp(-r_ * T_);
            return P;
        }

        const Greeks& call_greeks() const {
            compute_call_greeks();
            return call_greeks_;
        }

        const Greeks& put_greeks() const {
            compute_put_greeks();
            return put_greeks_;
        }
    };

} // namespace ito::model
