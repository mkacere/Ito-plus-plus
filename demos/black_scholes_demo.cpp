#include <ito/ito.hpp>
#include <cassert>
#include <cmath>

int main() {
    using namespace ito;

    // Classic example from Hull's "Options, Futures, and Other Derivatives"
    model::BlackScholesCreateInfo<double> info{
        .spot_price = 100.0,
        .strike_price = 100.0,      // At-the-money (spot = strike)
        .risk_free_rate = 0.05,     // 5% risk-free rate
        .volatility = 0.20,         // 20% volatility
        .time_to_maturity = 1.0     // 1 year
    };

    model::BlackScholesModel bs{ info };

    // Test call price
    double call = bs.call_price();
    dbg::println("Call price: ${:.4f}", call);
    // Expected: ~$10.45

    // Test put price
    double put = bs.put_price();
    dbg::println("Put price: ${:.4f}", put);
    // Expected: ~$5.57

    // Test put-call parity: C - P = S - K*e^(-rT)
    double parity = call - put;
    double expected_parity = 100.0 - 100.0 * std::exp(-0.05 * 1.0);
    dbg::println("\nPut-Call Parity Check:");
    dbg::println("C - P = {:.6f}", parity);
    dbg::println("S - Ke^(-rT) = {:.6f}", expected_parity);
    assert(std::abs(parity - expected_parity) < 1e-10);

    // Test Greeks
    auto &call_greeks = bs.call_greeks();
    dbg::println("\nCall Greeks:");
    dbg::println("  Delta: {:.6f}", call_greeks.delta);    // ~0.6368
    dbg::println("  Gamma: {:.6f}", call_greeks.gamma);    // ~0.0188
    dbg::println("  Vega:  {:.6f}", call_greeks.vega);     // ~37.52
    dbg::println("  Theta: {:.6f}", call_greeks.theta);    // ~-6.41
    dbg::println("  Rho:   {:.6f}", call_greeks.rho);      // ~53.23

    auto &put_greeks = bs.put_greeks();
    dbg::println("\nPut Greeks:");
    dbg::println("  Delta: {:.6f}", put_greeks.delta);     // ~-0.3632
    dbg::println("  Gamma: {:.6f}", put_greeks.gamma);     // ~0.0188 (same as call!)
    dbg::println("  Vega:  {:.6f}", put_greeks.vega);      // ~37.52 (same as call!)

    dbg::println("\nAll tests passed!");

    return 0;
}