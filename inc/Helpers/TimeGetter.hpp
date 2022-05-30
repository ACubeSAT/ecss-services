#ifndef ECSS_SERVICES_TIMEGETTER_HPP
#define ECSS_SERVICES_TIMEGETTER_HPP

#include <cstdint>
#include <ctime>
#include "Time/TimeStamp.hpp"
#include "Time/UTCTimestamp.hpp"

/**
 * @brief Get the current time
 */
class TimeGetter {
public:
	/**
	 * @brief Gets the current time in UNIX epoch
	 * @return Current UNIX epoch time, in elapsed seconds
	 */
	static inline uint32_t getSeconds() {
		return static_cast<uint32_t>(time(nullptr));
	}

	/**
	 * Returns the current UTC time.
	 * @note
	 * The information needed to compute the UTC time is implementation-specific. This function should
	 * be reimplemented to work for every format of the time-related parameters.
	 */
	static UTCTimestamp getCurrentTimeUTC();

	/**
	 * Converts the current UTC time, to a CUC formatted timestamp.
	 * @note
	 * The original format of the CUC (etl array of bits), is not used here, because it's easier to append
	 * a type uint64_t to a message object, rather than a whole array. Thus, we use the custom CUC format.
	 *
	 * @return CUC timestamp, formatted as elapsed 100ms ticks.
	 * @see Time.hpp
	 */
	static Time::CustomCUC_t getCurrentTimeCustomCUC();
};

#endif // ECSS_SERVICES_TIMEGETTER_HPP