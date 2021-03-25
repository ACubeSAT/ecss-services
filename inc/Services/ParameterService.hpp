#ifndef ECSS_SERVICES_PARAMETERSERVICE_HPP
#define ECSS_SERVICES_PARAMETERSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Parameter.hpp"
#include "Parameters/SystemParameters.hpp"

/*
 * ST[20] Parameter Management Service
 */

const uint8_t PARAMETER_MANAGEMENT = 20;

/*
 * ST[20] Parameter Management Sub-Services
 */

const uint8_t REPORT_PARAMETER_VALUES = 1;
const uint8_t PARAMETER_VALUES_REPORT = 2;
const uint8_t SET_PARAMETER_VALUES = 3;

/**
 * Implementation of the ST[20] parameter management service,
 * as defined in ECSS-E-ST-70-41C
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 * @author Athanasios Theocharis <athatheoc@gmail.com>
 */

/**
 * Parameter manager - ST[20]
 *
 * The purpose of this class is to handle functions regarding the access and modification
 * of the various parameters of the CubeSat.
 * The parameters to be managed are initialized and kept in \ref SystemParameters.
 */
class ParameterService : public Service {
public:
	ParameterService() = default;

	/**
	 * This function receives a TC[20, 1] packet and returns a TM[20, 2] packet
	 * containing the current configuration
	 * **for the parameters specified in the carried valid IDs**.
	 *
	 * @param paramId: a TC[20, 1] packet carrying the requested parameter IDs
	 * @return None (messages are stored using storeMessage())
	 */
	void reportParameters(Message& paramIds);

	/**
	 * This function receives a TC[20, 3] message and after checking whether its type is correct,
	 * iterates over all contained parameter IDs and replaces the settings for each valid parameter,
	 * while ignoring all invalid IDs.
	 *
	 * @param newParamValues: a valid TC[20, 3] message carrying parameter ID and replacement value
	 */
	void setParameters(Message& newParamValues);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param param Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);
};

#endif // ECSS_SERVICES_PARAMETERSERVICE_HPP
