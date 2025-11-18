#include <ito/ito.hpp>

int main() {
    using namespace ito;

    method::MonteCarloCreateInfo<double> config{
        .num_simulations = 10,  // Just 10 for now
        .seed = 42              // Fixed seed for reproducibility
    };

    method::MonteCarloPricer<double> mc(config);

    // Market parameters
    double S0 = 100.0;
    double r = 0.05;
    double sigma = 0.20;
    double T = 1.0;

    dbg::println("Simulating 10 stock paths:");
    for (int i = 0; i < 10; ++i) {
        // Note: You'll need to make simulate_gbm_terminal public temporarily for testing
        // Or create a test method
        double ST = mc.simulate_gbm_terminal(S0, r, sigma, T);
        dbg::println("  Path {}: S(T) = {:.2f}", i + 1, ST);
    }

    return 0;
}