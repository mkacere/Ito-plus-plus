#include <ito/ito.hpp>

int main() {
    using namespace ito;

    // Market parameters
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.20;
    double T = 1.0;

    dbg::println("=== Option Pricing Comparison ===\n");

    // Black-Scholes (Analytical)
    model::BlackScholesCreateInfo<double> bs_info{
        .spot_price = S,
        .strike_price = K,
        .risk_free_rate = r,
        .volatility = sigma,
        .time_to_maturity = T
    };
    model::BlackScholesModel bs(bs_info);

    // Monte Carlo (Numerical) - Price BOTH together
    method::MonteCarloCreateInfo<double> mc_config{
        .num_simulations = 100'000'000,
        .seed = 42
    };
    method::MonteCarloPricer mc(mc_config);

    // Price call and put using SAME paths
    auto mc_results = mc.price_european_call_and_put(S, K, r, sigma, T);

    double bs_call = bs.call_price();
    double bs_put = bs.put_price();

    dbg::println("CALL OPTION:");
    dbg::println("  Black-Scholes: ${:.4f}", bs_call);
    dbg::println("  Monte Carlo:   ${:.4f} +- {:.4f}",
        mc_results.call.price, mc_results.call.confidence_interval());
    dbg::println("  Difference:    ${:.4f}", std::abs(bs_call - mc_results.call.price));
    dbg::println("  Within 95% CI: {}",
        std::abs(bs_call - mc_results.call.price) < mc_results.call.confidence_interval()
        ? "Yes" : "No");

    dbg::println("\nPUT OPTION:");
    dbg::println("  Black-Scholes: ${:.4f}", bs_put);
    dbg::println("  Monte Carlo:   ${:.4f} +- {:.4f}",
        mc_results.put.price, mc_results.put.confidence_interval());
    dbg::println("  Difference:    ${:.4f}", std::abs(bs_put - mc_results.put.price));
    dbg::println("  Within 95% CI: {}",
        std::abs(bs_put - mc_results.put.price) < mc_results.put.confidence_interval()
        ? "Yes" : "No");

    // Verify put-call parity with MC
    double mc_parity = mc_results.call.price - mc_results.put.price;
    double expected_parity = S - K * std::exp(-r * T);

    dbg::println("\nPUT-CALL PARITY:");
    dbg::println("  C - P (MC):    {:.6f}", mc_parity);
    dbg::println("  S - Ke^(-rT):  {:.6f}", expected_parity);
    dbg::println("  Difference:    {:.6f}", std::abs(mc_parity - expected_parity));
    dbg::println("  Valid:         {}",
        std::abs(mc_parity - expected_parity) < 0.01 ? "Yes" : "No");

    return 0;
}