// ito math demo (2025)

#include <ito/ito.hpp>
#include <assert.h>

int main() {
    using namespace ito;

    // Test known values of the standard normal CDF
    // These are mathematically verified values
    constexpr double eps_7 = 1e-7;
    constexpr double eps_10 = 1e-10;

    dbg::println("Testing normal_cdf...");

    // Phi(0) should be 0.5 (median of normal distribution)
    double cdf_0 = math::normal_cdf(0.0);
    dbg::println("Phi(0) = {:.10f} (expected: 0.5000000000)", cdf_0);
    assert(std::abs(cdf_0 - 0.5) < eps_7);

    // Phi(1) ~= 0.8413447460685429
    double cdf_1 = math::normal_cdf(1.0);
    dbg::println("Phi(1) = {:.10f} (expected: 0.8413447461)", cdf_1);
    assert(std::abs(cdf_1 - 0.8413447460685429) < eps_7);

    // Phi(-1) ~= 0.1586552539314571 (by symmetry)
    double cdf_neg1 = math::normal_cdf(-1.0);
    dbg::println("Phi(-1) = {:.10f} (expected: 0.1586552539)", cdf_neg1);
    assert(std::abs(cdf_neg1 - 0.1586552539314571) < eps_7);

    // Phi(2) ~= 0.9772498680518208
    double cdf_2 = math::normal_cdf(2.0);
    dbg::println("Phi(2) = {:.10f} (expected: 0.9772498681)", cdf_2);
    assert(std::abs(cdf_2 - 0.9772498680518208) < eps_7);

    // Test symmetry: Phi(x) + Phi(-x) should equal 1
    double x = 1.5;
    double symmetry_test = math::normal_cdf(x) + math::normal_cdf(-x);
    dbg::println("Phi({}) + Phi(-{}) = {:.10f} (expected: 1.0)", x, x, symmetry_test);
    assert(std::abs(symmetry_test - 1.0) < eps_10);

    dbg::println("\nAll tests passed!");

    return 0;
}