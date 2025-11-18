#include <ito/ito.hpp>
#include <chrono>

int main() {
    using namespace ito;
    using namespace std::chrono;
    using Policy = method::MonteCarloCreateInfo<double>::ExecutionPolicy;

    double S = 100.0, K = 100.0, r = 0.05, sigma = 0.20, T = 1.0;

    dbg::println("=== Sequential vs Parallel Benchmark ===\n");

    for (size_t N : {100'000, 1'000'000, 10'000'000}) {
        dbg::println("Testing with {} simulations:", N);

        // Sequential
        {
            method::MonteCarloCreateInfo<double> config{
                .num_simulations = N,
                .seed = 42,
                .policy = Policy::Sequential  // ← Explicit
            };
            method::MonteCarloPricer mc(config);

            auto start = high_resolution_clock::now();
            auto result = mc.price_european_call_and_put(S, K, r, sigma, T);
            auto end = high_resolution_clock::now();

            auto ms = duration_cast<milliseconds>(end - start).count();
            dbg::println("  Sequential: {:>6} ms", ms);
        }

        // Parallel
        {
            method::MonteCarloCreateInfo<double> config{
                .num_simulations = N,
                .seed = 42,
                .policy = Policy::Parallel  // ← Explicit
            };
            method::MonteCarloPricer mc(config);

            auto start = high_resolution_clock::now();
            auto result = mc.price_european_call_and_put(S, K, r, sigma, T);
            auto end = high_resolution_clock::now();

            auto ms = duration_cast<milliseconds>(end - start).count();
            dbg::println("  Parallel:   {:>6} ms", ms);
        }

        dbg::println("");
    }

    return 0;
}