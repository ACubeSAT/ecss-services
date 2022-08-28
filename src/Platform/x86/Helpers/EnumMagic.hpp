#pragma once

#include <string>
#include <utility>

/**
 * @file
 *
 * A simplified version of https://github.com/Neargye/magic_enum, this file uses some gcc compiler options to provide an @ref enum_name function.
 * This function converts an enum value to a string.
 *
 * Use with care. NOT intended for a microcontroller.
 *
 * @license MIT, Daniil Goncharov
 */

namespace EnumMagic_ {
	template <typename T, std::size_t N, std::size_t... I>
	constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N], std::index_sequence<I...>) noexcept {
		return {{a[I]...}};
	}

	template <typename E>
	constexpr auto values() noexcept {
		static_assert(std::is_enum_v<E>, "values() requires enum type.");
		constexpr std::size_t count = 256;

		E values[count] = {};
		for (std::size_t i = 0; i < count; ++i) {
			values[i] = static_cast<E>(i);
		}

		return to_array(values, std::make_index_sequence<count>{});
	}

	template <typename E>
	inline constexpr std::array values_v = values<E>();

	template <typename E>
	inline constexpr auto count_v = values_v<E>.size();

	template <typename E, E V>
	constexpr std::basic_string_view<char> enum_hack() {
		static_assert(std::is_enum_v<E>, "enum_hack() requires enum type.");

		std::string_view function = __PRETTY_FUNCTION__;
		auto pos = function.find("E V = ");
		std::string_view output = function.substr(pos + 6);
		output.remove_suffix(1);
		return output;
	}

	template <typename E, E V>
	inline constexpr auto enum_name_v = enum_hack<E, V>();

	template <typename E, std::size_t... I>
	constexpr auto names(std::index_sequence<I...>) noexcept {
		static_assert(std::is_enum_v<E>, "names() requires enum type.");

		return std::array<std::string_view, sizeof...(I)>{{enum_name_v<E, values_v<E>[I]>...}};
	}

	template <typename E>
	inline constexpr std::array names_v = names<E>(std::make_index_sequence<count_v<E>>{});

	constexpr std::string_view remove_namespace(std::string_view in) {
		auto pos = in.find_last_of("::");
		return in.substr(pos + 1);
	}
}

template <typename E>
std::string enum_name(E v) {
	using namespace EnumMagic_;

	int index = static_cast<int>(v);

	if (index >= names_v<E>.size()) {
		return "(Unknown)";
	}

	return std::string(remove_namespace(names_v<E>[index]));
}
