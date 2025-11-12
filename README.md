# Ito++

A modern C++23 header-only library for quantitative finance and option pricing. Named after Kiyoshi Ito, the mathematician who developed Ito calculus—the mathematical foundation for modern financial modeling.

## Current Status

### ✓ Implemented Features

#### Black-Scholes Model
- **Complete option pricing implementation** (`include/ito/model/black_scholes_model.hpp`)
  - Call and put option pricing
  - Full Greeks calculation: Delta, Gamma, Vega, Theta, Rho
  - Smart caching mechanism for intermediate calculations
  - Put-call parity validation
  - Input parameter validation with clear error messages
  - Precision: < 7.5 × 10⁻⁸ for CDF approximations

#### Mathematical Utilities
- **Statistical functions** (`include/ito/utils/math.hpp`)
  - Standard normal probability density function (PDF)
  - Cumulative distribution function (CDF) using Abramowitz & Stegun approximation
  - Mathematical constants (inv_sqrt_2pi, sqrt_2)
  - Modern C++ concepts for type safety

#### Debug Utilities
- **Formatting helpers** (`include/ito/utils/utils.hpp`)
  - C++20 std::format-based printing functions
  - Compile-time checked formatting

#### Working Demos
- `demos/math_demo.cpp` - Comprehensive testing of normal distribution functions
- `demos/black_scholes_demo.cpp` - Black-Scholes pricing with Greeks and put-call parity validation

## Quick Start

```cpp
#include <ito/ito.hpp>
#include <iostream>

int main() {
    using namespace ito;

    // Create a Black-Scholes model
    model::BlackScholesModel bs(
        100.0,  // spot price
        100.0,  // strike price
        0.05,   // risk-free rate
        1.0,    // time to maturity (years)
        0.2     // volatility
    );

    // Price options
    std::cout << "Call price: " << bs.call_price() << "\n";
    std::cout << "Put price: " << bs.put_price() << "\n";

    // Calculate Greeks
    std::cout << "Delta: " << bs.delta() << "\n";
    std::cout << "Gamma: " << bs.gamma() << "\n";
    std::cout << "Vega: " << bs.vega() << "\n";
    std::cout << "Theta: " << bs.theta() << "\n";
    std::cout << "Rho: " << bs.rho() << "\n";

    return 0;
}
```

## Roadmap

### 🚧 Planned Features

The following components are planned for future releases:

- **Monte Carlo Methods** (`include/ito/method/monte_carlo.hpp`)
  - Monte Carlo simulation for option pricing
  - Random path generation
  - Variance reduction techniques
  - Parallel processing support

- **European Options** (`include/ito/option/european_option.hpp`)
  - European option contract definitions
  - Option specifications and payoff functions
  - Foundation for American and exotic options

- **Unified Pricer Interface** (`include/ito/core/option_pricer.hpp`)
  - Abstract pricing framework
  - Strategy pattern for different pricing methods
  - Flexible architecture for multiple models

## Requirements

- **C++23 compatible compiler**
  - MSVC 19.34+ (Visual Studio 2022 17.4+)
  - GCC 12+
  - Clang 15+
- **CMake 3.20 or higher**

## Installation

Ito++ is a header-only library. Simply include the headers in your project:

### Option 1: Direct Include
Copy the `include/ito` directory to your project and add it to your include path.

### Option 2: CMake Integration
```cmake
add_subdirectory(path/to/Ito++)
target_link_libraries(your_target PRIVATE ito)
```

## Building the Demos

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build

# Run demos
./build/demos/math_demo
./build/demos/black_scholes_demo
```

## Project Structure

```
Ito++/
├── include/ito/           # Header-only library
│   ├── ito.hpp           # Main convenience header
│   ├── core/             # Core abstractions (planned)
│   ├── method/           # Pricing methods (planned)
│   ├── model/            # Financial models (Black-Scholes ✓)
│   ├── option/           # Option types (planned)
│   └── utils/            # Utilities (math ✓, debug ✓)
└── demos/                # Example applications
```

## Design Philosophy

- **Modern C++**: Leveraging C++23 features for type safety and performance
- **Header-only**: Easy integration with no compilation required
- **Template-based**: Generic programming for flexibility
- **Zero-overhead**: Compile-time optimizations and lazy evaluation
- **Mathematical rigor**: Implementations based on authoritative sources

## References

- Hull, John C. "Options, Futures, and Other Derivatives"
- Abramowitz, Milton, and Irene A. Stegun. "Handbook of Mathematical Functions" (1964)
- Ito, Kiyoshi. "On Stochastic Differential Equations" (1951)

## License

This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details.

## Acknowledgments

Named in honor of Kiyoshi Ito (1915-2008), whose pioneering work in stochastic calculus laid the mathematical foundation for modern quantitative finance.
