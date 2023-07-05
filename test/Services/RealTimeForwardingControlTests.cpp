#include <iostream>
#include "ECSS_Definitions.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/RealTimeForwardingControlService.hpp"
#include "catch2/catch_all.hpp"

static RealTimeForwardingControlService& realTimeForwarding = Services.realTimeForwarding;

uint8_t applications[] = {1};
uint8_t services[] = {3, 5};
uint8_t allServices[] = {1, 3, 4, 5, 6, 11, 13, 17, 19, 20};
uint8_t redundantServices[] = {1, 3, 4, 5, 6, 11, 13, 17, 19, 20, 1, 3};
uint8_t messages1[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                       HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};

uint8_t messages2[] = {EventReportService::MessageType::InformativeEventReport,
                       EventReportService::MessageType::DisabledListEventReport};

/**
 * Fill the input message with a predefined sequence of valid report definitions.
 * @param request
 */
void validReportTypes(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
			uint8_t* messages = (j == 0) ? messages1 : messages2;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages[k]);
			}
		}
	}
}

/**
 * Fill the input message with a predefined sequence of duplicate report definitions.
 * @param request
 */
void duplicateReportTypes(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages1[0]);
			}
		}
	}
}

/**
 * Fill the input message with a predefined sequence of both valid and invalid report definitions.
 * @param request
 */
void validInvalidReportTypes(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0) ? 12 : 2;
		uint8_t* servicesToPick = (i == 0) ? redundantServices : services;
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = servicesToPick[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
			uint8_t* messages = (j == 0) ? messages1 : messages2;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages[k]);
			}
		}
	}
}

/**
 * Fill the input message with a predefined sequence of valid service report definitions.
 * @param request
 */
void validAllReportsOfService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 0;

	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
		}
	}
}

/**
 * Fill the input message with a predefined sequence of both valid
 * and invalid service report definitions.
 * @param request
 */
void validInvalidAllReportsOfService(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0) ? 12 : 2;
		uint8_t* servicesToPick = (i == 0) ? redundantServices : services;
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = servicesToPick[j];
			request.appendUint8(serviceType);
			uint8_t numOfMessages = (i < 2) ? 0 : numOfMessagesPerService;
			request.appendUint8(numOfMessages);
			if (i >= 2) {
				uint8_t* messages = (j == 0) ? messages1 : messages2;

				for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
					request.appendUint8(messages[k]);
				}
			}
		}
	}
}

/**
 * Fill the input message with a predefined sequence of valid application process report definitions.
 * @param request
 */
void validAllReportsOfApp(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 0;

	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);
	}
}

/**
 * Fill the input message with a predefined sequence of both valid
 * and invalid application process report definitions.
 * @param request
 */
void validInvalidAllReportsOfApp(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0 or i == 1) ? 0 : 2;
		uint8_t* servicesToPick = (i == 0) ? redundantServices : services;
		request.appendUint8(numOfServicesPerApp);

		if (i >= 2) {
			for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
				uint8_t serviceType = servicesToPick[j];
				request.appendUint8(serviceType);
				uint8_t numOfMessages = (i == 0 or i == 1) ? 0 : numOfMessagesPerService;
				request.appendUint8(numOfMessages);

				uint8_t* messages = (j == 0) ? messages1 : messages2;

				for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
					request.appendUint8(messages[k]);
				}
			}
		}
	}
}

/**
 * Reset the map containing the report definitions to an empty state.
 */
void resetAppProcessConfiguration() {
	realTimeForwarding.applicationProcessConfiguration.definitions.clear();
	REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.empty());
}

TEST_CASE("Add report types to the Application Process Configuration") {
	SECTION("Successful addition of report types to the Application Process Configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses.size() == 2);

		for (auto appID: applications) {
			for (uint8_t j = 0; j < 2; j++) {
				uint8_t serviceType = services[j];
				auto appServicePair = std::make_pair(appID, serviceType);
				REQUIRE(applicationProcesses.find(appServicePair) != applicationProcesses.end());
				REQUIRE(applicationProcesses[appServicePair].size() == 2);
				uint8_t* messages = (j == 0) ? messages1 : messages2;

				for (uint8_t k = 0; k < 2; k++) {
					REQUIRE(std::find(applicationProcesses[appServicePair].begin(),
					                  applicationProcesses[appServicePair].end(),
					                  messages[k]) != applicationProcesses[appServicePair].end());
				}
			}
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested Application Process is not controlled by the service") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		validReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All service types already allowed") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		for (uint8_t i = 1; i < ECSSMaxServiceTypeDefinitions + 1; i++) {
			realTimeForwarding.applicationProcessConfiguration.definitions[std::make_pair(applicationID, i)];
		}
		CHECK(realTimeForwarding.applicationProcessConfiguration.definitions.size() == ECSSMaxServiceTypeDefinitions);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed) ==
		      1);
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.size() == ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max service types already reached") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType1 = services[0]; // st03
		uint8_t serviceType2 = services[1]; // st05

		realTimeForwarding.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		auto& applicationProcessConfig = realTimeForwarding.applicationProcessConfiguration.definitions;

		for (uint8_t i = 100; i < ECSSMaxServiceTypeDefinitions + 99; i++) {
			applicationProcessConfig[std::make_pair(applicationID, i)];
		}
		CHECK(applicationProcessConfig.size() == ECSSMaxServiceTypeDefinitions - 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 1);
		REQUIRE(applicationProcessConfig.size() == ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All report types already allowed") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType = services[0]; // st03
		realTimeForwarding.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		for (auto& message: AllMessageTypes::MessagesOfService.at(serviceType)) {
			realTimeForwarding.applicationProcessConfiguration.definitions[std::make_pair(applicationID, serviceType)]
			    .push_back(message);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) ==
		      2);
		REQUIRE(
		    realTimeForwarding.applicationProcessConfiguration.definitions[std::make_pair(applicationID, serviceType)]
		        .size() == AllMessageTypes::MessagesOfService.at(serviceType).size());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max report types already reached") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType1 = services[0]; // st03
		uint8_t serviceType2 = services[1]; // st05

		realTimeForwarding.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		auto& applicationProcessConfig = realTimeForwarding.applicationProcessConfiguration;

		auto appServicePair1 = std::make_pair(applicationID, serviceType1);
		auto appServicePair2 = std::make_pair(applicationID, serviceType2);
		REQUIRE(applicationProcessConfig.definitions[appServicePair1].empty());
		REQUIRE(applicationProcessConfig.definitions[appServicePair2].empty());

		auto numOfMessages1 = AllMessageTypes::MessagesOfService.at(serviceType1).size();
		auto numOfMessages2 = AllMessageTypes::MessagesOfService.at(serviceType2).size();

		for (uint8_t i = 0; i < numOfMessages1 - 1; i++) {
			applicationProcessConfig.definitions[appServicePair1].push_back(i);
		}
		for (uint8_t i = 16; i < numOfMessages2 + 15; i++) {
			applicationProcessConfig.definitions[appServicePair2].push_back(i);
		}
		REQUIRE(applicationProcessConfig.definitions[appServicePair1].size() == numOfMessages1 - 1);
		REQUIRE(applicationProcessConfig.definitions[appServicePair2].size() == numOfMessages2 - 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) == 2);
		REQUIRE(applicationProcessConfig.definitions.size() == 2);
		REQUIRE(applicationProcessConfig.definitions[appServicePair1].size() == numOfMessages1);
		REQUIRE(applicationProcessConfig.definitions[appServicePair2].size() == numOfMessages2);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested addition of duplicate report type definitions") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID);
		duplicateReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses.size() == 2);

		for (auto appID: applications) {
			for (auto& serviceType: services) {
				auto appServicePair = std::make_pair(appID, serviceType);
				REQUIRE(applicationProcesses.find(appServicePair) != applicationProcesses.end());
				REQUIRE(applicationProcesses[appServicePair].size() == 1);
				REQUIRE(std::find(applicationProcesses[appServicePair].begin(),
				                  applicationProcesses[appServicePair].end(),
				                  messages1[0]) != applicationProcesses[appServicePair].end());
			}
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid and invalid application-related requests combined") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		uint8_t applicationID2 = 2;
		uint8_t applicationID3 = 3;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		realTimeForwarding.controlledApplications.push_back(applicationID3);
		validInvalidReportTypes(request);

		for (uint8_t i = 100; i < ECSSMaxServiceTypeDefinitions + 99; i++) {
			realTimeForwarding.applicationProcessConfiguration.definitions[std::make_pair(applicationID3, i)];
		}
		CHECK(realTimeForwarding.applicationProcessConfiguration.definitions.size() ==
		      ECSSMaxServiceTypeDefinitions - 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 7);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) == 3);

		auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(definitions.size() == 20);
		for (auto serviceType: allServices) {
			REQUIRE(definitions.find(std::make_pair(applicationID1, serviceType)) != definitions.end());
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid addition of all report types of a specified service type") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		validAllReportsOfService(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		for (auto serviceType: services) {
			REQUIRE(applicationProcesses[std::make_pair(applicationID1, serviceType)].size() ==
			        AllMessageTypes::MessagesOfService.at(serviceType).size());
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Addition of all report types of a service type, combined with invalid requests") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		uint8_t applicationID2 = 2;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		realTimeForwarding.controlledApplications.push_back(applicationID2);
		validInvalidAllReportsOfService(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 2);

		auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(definitions.size() == 12);

		int cnt1 = 0;
		int cnt2 = 0;
		for (auto& pair: definitions) {
			if (pair.first.first == applicationID1) {
				cnt1++;
			} else if (pair.first.first == applicationID2) {
				cnt2++;
			}
		}
		REQUIRE(cnt1 == 10);
		REQUIRE(cnt2 == 2);

		for (auto& serviceType: allServices) {
			REQUIRE(definitions[std::make_pair(applicationID1, serviceType)].size() ==
			        AllMessageTypes::MessagesOfService.at(serviceType).size());
		}
		for (auto& serviceType: services) {
			REQUIRE(definitions[std::make_pair(applicationID2, serviceType)].size() ==
			        AllMessageTypes::MessagesOfService.at(serviceType).size());
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid addition of all report types of an application process") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		validAllReportsOfApp(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(definitions.size() == ECSSMaxServiceTypeDefinitions);

		for (auto serviceType: allServices) {
			REQUIRE(std::equal(definitions[std::make_pair(applicationID1, serviceType)].begin(),
			                   definitions[std::make_pair(applicationID1, serviceType)].end(),
			                   AllMessageTypes::MessagesOfService.at(serviceType).begin()));
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Addition of all report types of an application process, combined with invalid request") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		uint8_t applicationID2 = 2;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		realTimeForwarding.controlledApplications.push_back(applicationID2);
		validInvalidAllReportsOfApp(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(definitions.size() == 2 * ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}
}

uint8_t applicationsToFill[] = {1};
uint8_t servicesToFill[] = {3, 5};
uint8_t allServicesToFill[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t redundantServicesToFill[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
uint8_t messagesToFill1[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                        HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};

uint8_t messagesToFill2[] = {EventReportService::MessageType::InformativeEventReport,
                        EventReportService::MessageType::DisabledListEventReport};

/**
 * Returns true, if the defined application exists in the application process configuration map.
 */
bool isApplicationEnabled(uint8_t targetAppID) {
	auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;
	return std::any_of(std::begin(definitions), std::end(definitions), [targetAppID](auto definition) { return targetAppID == definition.first.first; });
}

/**
 * Returns true, if the defined service type exists in the application process configuration map.
 */
bool isServiceTypeEnabled(uint8_t applicationID, uint8_t targetService) {
	auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;
	return std::any_of(std::begin(definitions), std::end(definitions), [applicationID, targetService](auto definition) { return applicationID == definition.first.first and targetService == definition.first.second; });
}

/**
 * Check if configuration is initialized properly for testing
 */
void checkAppProcessConfig() {
	auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

	for (auto appID: applicationsToFill) {
		REQUIRE(isApplicationEnabled(appID));

		for (auto serviceType: services) {
			auto appServicePair = std::make_pair(appID, serviceType);
			REQUIRE(isServiceTypeEnabled(appID, serviceType));
			REQUIRE(applicationProcesses[appServicePair].size() == 2);

			for (auto messageType: messagesToFill1) {
				REQUIRE(std::find(applicationProcesses[appServicePair].begin(),
				                  applicationProcesses[appServicePair].end(),
				                  messageType) != applicationProcesses[appServicePair].end());
			}
		}
	}
}

/**
 * Initialize the configuration for testing
 */
void initializeAppProcessConfig() {
	for (auto appID: applicationsToFill) {
		for (auto serviceType: services) {
			auto appServicePair = std::make_pair(appID, serviceType);
			for (auto messageType: messagesToFill1) {
				realTimeForwarding.applicationProcessConfiguration.definitions[appServicePair].push_back(
				    messageType);
			}
		}
	}
	checkAppProcessConfig();
}

/**
 * Check if configuration is initialized properly for testing with a 2nd sequence of data
 */
void checkAppProcessConfig2() {
	auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
	uint8_t applications2[] = {1, 2, 3};

	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t appID = applications2[i];
		REQUIRE(isApplicationEnabled(appID));

		uint8_t numOfServices = (i == 2) ? 15 : 2;
		uint8_t* serviceTypes = (i == 2) ? allServicesToFill : servicesToFill;

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = serviceTypes[j];
			uint8_t* messages = (i == 2) ? messagesToFill2 : messagesToFill1;

			REQUIRE(isServiceTypeEnabled(appID, serviceType));
			auto appServicePair = std::make_pair(appID, serviceType);
			REQUIRE(applicationProcesses[appServicePair].size() == 2);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				uint8_t messageType = messages[k];
				REQUIRE(std::find(applicationProcesses[appServicePair].begin(),
				                  applicationProcesses[appServicePair].end(),
				                  messageType) != applicationProcesses[appServicePair].end());
			}
		}
	}
}

/**
 * Initialize the configuration for testing with a 2nd sequence of data
 */
void initializeAppProcessConfig2() {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t appID = applications2[i];
		uint8_t numOfServices = (i == 2) ? 15 : 2;
		uint8_t* serviceTypes = (i == 2) ? allServicesToFill : servicesToFill;

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = serviceTypes[j];
			uint8_t* messages = (i == 2) ? messagesToFill2 : messagesToFill1;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				uint8_t messageType = messages[k];
				realTimeForwarding.applicationProcessConfiguration.definitions[std::make_pair(appID, serviceType)].push_back(
				    messageType);
			}
		}
	}
	checkAppProcessConfig2();
}

/**
 * Fill the input message with a predefined sequence of report definitions where the
 * requested service type is not present in the application process configuration
 * @param request
 */
void serviceNotInApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applicationsToFill) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t nonExistingService = 13; // add arbitrary, non-existing service type
			request.appendUint8(nonExistingService);
			request.appendUint8(numOfMessagesPerService);
			uint8_t* messages = (j == 0) ? messagesToFill1 : messagesToFill2;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages[k]);
			}
		}
	}
}

/**
 * Fill the input message with a predefined sequence of report definitions where the
 * requested message type is not present in the application process configuration
 * @param request
 */
void messageNotInApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applicationsToFill) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				uint8_t nonExistingMessage =
				    RealTimeForwardingControlService::MessageType::EventReportConfigurationContentReport;
				request.appendUint8(nonExistingMessage);
			}
		}
	}
}

/**
 * Fill the input message with a predefined sequence of valid report definitions that
 * need to be deleted.
 * @param request
 */
void deleteValidReportTypes(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 1;

	request.appendUint8(numOfApplications);

	for (auto appID: applicationsToFill) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messagesToFill1[k]);
			}
		}
	}
}

/**
 * Fill the input message with a predefined sequence of report definitions that
 * once deleted result in an empty service report definition.
 * @param request
 */
void deleteReportEmptyService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 1;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applicationsToFill) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messagesToFill1[k]);
			}
		}
	}
}

/**
 * Fill the input message with a predefined sequence of message report definitions that
 * once deleted result in an empty application process report definition.
 * @param request
 */
void deleteReportEmptyApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applicationsToFill) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = servicesToFill[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messagesToFill1[k]);
			}
		}
	}
}

/**
 * Fill the input message with a predefined sequence of application process report definitions that
 * need to be deleted.
 * @param request
 */
void deleteApplicationProcess(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 0;

	request.appendUint8(numOfApplications);
	request.appendUint8(applicationsToFill[0]);
	request.appendUint8(numOfServicesPerApp);
}

/**
 * Fill the input message with a predefined sequence of service report definitions that
 * need to be deleted.
 * @param request
 */
void deleteService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 1;
	uint8_t numOfMessagesPerService = 0;

	request.appendUint8(numOfApplications);
	for (auto appID: applicationsToFill) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
		}
	}
}

/**
 * Fill the input message with a predefined sequence of service report definitions that
 * once deleted result in an empty application process report definition.
 * @param request
 */
void deleteServiceEmptyApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 0;

	request.appendUint8(numOfApplications);
	for (auto appID: applicationsToFill) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
		}
	}
}

/**
 * Fill the input message with a predefined sequence of both valid and invalid report definitions
 * that need to be deleted.
 * @param request
 */
void deleteValidInvalidReportTypes(Message& request) {
	uint8_t numOfApplications = 4;
	uint8_t numOfServices = 2;
	uint8_t numOfMessages = 2;
	uint8_t validInvalidApplications[] = {1, 99, 3, 2}; // one invalid and three valid
	uint8_t validInvalidServices[][2] = {{3, 9}, {3, 5}, {1, 8}, {3, 5}};
	uint8_t validInvalidMessages[] = {EventReportService::MessageType::DisabledListEventReport,
	                                  HousekeepingService::MessageType::HousekeepingParametersReport};

	request.appendUint8(numOfApplications);
	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t appID = validInvalidApplications[i];

		request.appendUint8(appID);
		request.appendUint8(numOfServices);

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = validInvalidServices[i][j];
			uint8_t* messages = (i == 2) ? validInvalidMessages : messagesToFill1;

			request.appendUint8(serviceType);
			request.appendUint8(numOfMessages);

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = messages[k];
				request.appendUint8(messageType);
			}
		}
	}
}

TEST_CASE("Delete report types from the Application Process Configuration") {
	SECTION("Empty the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t numOfApplications = 0;
		request.appendUint8(numOfApplications);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested application, is not present in the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t numOfApplications = 1;
		uint8_t applicationID = 2;
		request.appendUint8(numOfApplications);
		request.appendUint8(applicationID);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistentApplicationProcess) == 1);
		checkAppProcessConfig();

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested service type, not present in the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		serviceNotInApplication(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition) == 2);
		checkAppProcessConfig();

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested report type, not present in the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		messageNotInApplication(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistentReportTypeDefinition) ==
		      4);
		checkAppProcessConfig();

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid deletion of report types from the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteValidReportTypes(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(applicationProcesses.size() == 2);
		REQUIRE(applicationProcesses[std::make_pair(applicationID, servicesToFill[0])].size() == 1);
		REQUIRE(applicationProcesses[std::make_pair(applicationID, servicesToFill[1])].size() == 1);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Report type deletion, results in empty service type") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteReportEmptyService(request);
		initializeAppProcessConfig();

		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.size() == 2);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(applicationProcesses.size() == 1);
		REQUIRE(isServiceTypeEnabled(applicationID, servicesToFill[1]));

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Report type deletion, results in empty application process definition") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteReportEmptyApplication(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(applicationProcesses.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Delete an application process, from the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		deleteApplicationProcess(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(applicationProcesses.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Delete a service type, from the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteService(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(applicationProcesses.size() == 1);
		REQUIRE(not isServiceTypeEnabled(applicationID, servicesToFill[0]));

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Service type deletion, results in empty application process") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteServiceEmptyApplication(request);
		initializeAppProcessConfig();
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(not applicationProcesses.empty());

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(applicationProcesses.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid and invalid requests to delete report types, combined") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t remainingApps[] = {1, 3};
		uint8_t remainingMessage[] = {EventReportService::MessageType::InformativeEventReport};

		deleteValidInvalidReportTypes(request);
		initializeAppProcessConfig2();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistentApplicationProcess) == 1);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistentReportTypeDefinition) ==
		      2);

		auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(definitions.size() == 16);
		for (auto appID: remainingApps) {
			REQUIRE(isApplicationEnabled(appID));
		}
		REQUIRE(std::count_if(std::begin(definitions), std::end(definitions), [&remainingApps](auto& definition) { return std::find(std::begin(remainingApps), std::end(remainingApps), definition.first.first) != std::end(remainingApps); }) == 16);

		// Check for appID = 1
		uint8_t appID1 = remainingApps[0];
		REQUIRE(std::count_if(std::begin(definitions), std::end(definitions), [appID1](auto& definition) { return appID1 == definition.first.first; }) == 1);
		REQUIRE(not isServiceTypeEnabled(appID1, servicesToFill[0]));
		auto appServicePair = std::make_pair(appID1, servicesToFill[1]);
		REQUIRE(definitions[appServicePair].size() == 2);

		for (auto& message: messagesToFill1) {
			REQUIRE(std::find(definitions[appServicePair].begin(),
			                  definitions[appServicePair].end(),
			                  message) != definitions[appServicePair].end());
		}

		// Check for appID = 2
		uint8_t appID2 = 2;
		REQUIRE(std::count_if(std::begin(definitions), std::end(definitions), [appID2](auto& definition) { return appID2 == definition.first.first; }) == 0);

		// Check for appID = 3
		uint8_t appID3 = remainingApps[1];
		REQUIRE(std::count_if(std::begin(definitions), std::end(definitions), [appID3](auto& definition) { return appID3 == definition.first.first; }) == 15);

		for (uint8_t i = 0; i < 15; i++) {
			uint8_t numOfMessages = (i == 0 or i == 7) ? 1 : 2; // we only deleted one report from services 1 and 8
			uint8_t* messages = (i == 0 or i == 7) ? remainingMessage : messagesToFill2;
			auto appServicePair1 = std::make_pair(appID3, allServicesToFill[i]);

			REQUIRE(definitions[appServicePair1].size() == numOfMessages);
			for (uint8_t j = 0; j < numOfMessages; j++) {
				REQUIRE(std::find(definitions[appServicePair1].begin(),
				                  definitions[appServicePair1].end(),
				                  messages[j]) != definitions[appServicePair1].end());
			}
		}
		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}
}
