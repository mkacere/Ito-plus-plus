#pragma once
#include <cmath>
#include <concepts>
#include <numbers>

namespace ito::math {
	template<typename T>
	concept Arithmetic = std::is_arithmetic_v<T>;
	
	namespace constants {
		template<Arithmetic T = double>
		inline constexpr T inv_sqrt_2pi = std::numbers::inv_sqrtpi_v<T> / std::numbers::sqrt2_v<T>;
	
		template<Arithmetic T = double>
		inline constexpr T sqrt_2 = static_cast<T>(std::numbers::sqrt2);
	}


	/**
	 * Standard normal probability density function (PDF)
	 * f(x) = (1/sqrt(2pi)) * exp(-(x^2)/2)
	 */

	template<Arithmetic T = double>
	constexpr T normal_pdf(T x) noexcept {
		return constants::inv_sqrt_2pi<T> * std::exp(-x * x / static_cast<T>(2));
	}

	/**
	 * Cumulative distribution function (CDF)
	 * Phi(x) = P(X <= x) for standard normal distribution
	 * no closed-form solution
	 * using Abramowitz & Stegun formula
	 */
	template<Arithmetic T = double>
	inline T normal_cdf(T x) noexcept {
		if (x < 0) {
			return static_cast<T>(1) - normal_cdf(-x);  // Phi(-x) = 1 - Phi(x)
		}

		// Abramowitz & Stegun approximation (1964)
		// Error < 7.5 * 10^-8

		constexpr T a1 = 0.319381530;
		constexpr T a2 = -0.356563782;
		constexpr T a3 = 1.781477937;
		constexpr T a4 = -1.821255978;
		constexpr T a5 = 1.330274429;
		constexpr T p = 0.2316419;

		const T t = static_cast<T>(1) / (static_cast<T>(1) + p * x);
		//const T t2 = t * t;
		//const T t3 = t2 * t;
		//const T t4 = t3 * t;
		//const T t5 = t4 * t;

		// Polynomial evaluation using Horner's method
		// alternatively: const T poly = a1 * t + a2 * t2 + a3 * t3 + a4 * t4 + a5 * t5; (less efficient)
		const T poly = t * (a1 + t * (a2 + t * (a3 + t * (a4 + t * a5))));

		return static_cast<T>(1) - normal_pdf(x) * poly;
	}
}