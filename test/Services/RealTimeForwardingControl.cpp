#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "ECSS_Definitions.hpp"
#include "Services/RealTimeForwardingControlService.hpp"

RealTimeForwardingControlService& realTimeForwarding = Services.realTimeForwarding;

uint8_t applications[] = {1};
uint8_t services[] = {3, 5};
uint8_t allServices[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t redundantServices[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
uint8_t messages1[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                       HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};

uint8_t messages2[] = {EventReportService::MessageType::InformativeEventReport,
                       EventReportService::MessageType::DisabledListEventReport};

void validReportTypes(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
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

void duplicateReportTypes(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
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

void validInvalidReportTypes(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0) ? 17 : 2;
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

void validAllReportsOfService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 0;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
		}
	}
}

void validInvalidAllReportsOfService(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0) ? 17 : 2;
		uint8_t* servicesToPick = (i == 0) ? redundantServices : services;
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = servicesToPick[j];
			request.appendUint8(serviceType);
			uint8_t numOfMessages = (i == 0 or i == 1) ? 0 : numOfMessagesPerService;
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

void validAllReportsOfApp(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 0;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);
	}
}

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

void checkAppProcessConfig() {
	auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
	auto& isNotEmpty = realTimeForwarding.applicationProcessConfiguration.notEmpty;

	// Check if configuration is initialized properly
	for (auto appID : applications) {
		REQUIRE(applicationProcesses.find(appID) != applicationProcesses.end());
		REQUIRE(isNotEmpty.find(appID) != isNotEmpty.end());
		REQUIRE(applicationProcesses[appID].size() == 2);

		for (auto serviceType : services) {
			REQUIRE(applicationProcesses[appID].find(serviceType) != applicationProcesses[appID].end());
			REQUIRE(applicationProcesses[appID][serviceType].size() == 2);
			REQUIRE(isNotEmpty[appID].find(serviceType) != isNotEmpty[appID].end());

			for (auto messageType : messages1) {
				REQUIRE(std::find(applicationProcesses[appID][serviceType].begin(),
				                  applicationProcesses[appID][serviceType].end(),
				                  messageType) != applicationProcesses[appID][serviceType].end());
			}
		}
	}
}

void initializeAppProcessConfig() {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	for (auto appID : applications) {
		for (auto serviceType : services) {
			for (auto messageType : messages1) {
				realTimeForwarding.applicationProcessConfiguration.definitions[appID][serviceType].push_back(
				    messageType);
				realTimeForwarding.applicationProcessConfiguration.notEmpty[appID][serviceType] = true;
			}
		}
	}
	checkAppProcessConfig();
}

void checkAppProcessConfig2() {
	auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
	auto& isNotEmpty = realTimeForwarding.applicationProcessConfiguration.notEmpty;
	uint8_t applications2[] = {1, 2, 3};

	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	// Check if configuration is initialized properly
	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t appID = applications2[i];
		REQUIRE(applicationProcesses.find(appID) != applicationProcesses.end());
		REQUIRE(isNotEmpty.find(appID) != isNotEmpty.end());

		uint8_t numOfServices = (i == 2) ? 15 : 2;
		uint8_t* serviceTypes = (i == 2) ? allServices : services;
		REQUIRE(applicationProcesses[appID].size() == numOfServices);

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = serviceTypes[j];
			uint8_t* messages = (i == 2) ? messages2 : messages1;

			REQUIRE(applicationProcesses[appID].find(serviceType) != applicationProcesses[appID].end());
			REQUIRE(applicationProcesses[appID][serviceType].size() == 2);
			REQUIRE(isNotEmpty[appID].find(serviceType) != isNotEmpty[appID].end());

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				uint8_t messageType = messages[k];
				REQUIRE(std::find(applicationProcesses[appID][serviceType].begin(),
				                  applicationProcesses[appID][serviceType].end(),
				                  messageType) != applicationProcesses[appID][serviceType].end());
			}
		}
	}
}

void initializeAppProcessConfig2() {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t appID = applications2[i];
		uint8_t numOfServices = (i == 2) ? 15 : 2;
		uint8_t* serviceTypes = (i == 2) ? allServices : services;

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = serviceTypes[j];
			uint8_t* messages = (i == 2) ? messages2 : messages1;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				uint8_t messageType = messages[k];
				realTimeForwarding.applicationProcessConfiguration.definitions[appID][serviceType].push_back(
				    messageType);
				realTimeForwarding.applicationProcessConfiguration.notEmpty[appID][serviceType] = true;
			}
		}
	}
	checkAppProcessConfig2();
}

void serviceNotInApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t nonExistingService = 13; // add arbitrary, non-existing service type
			request.appendUint8(nonExistingService);
			request.appendUint8(numOfMessagesPerService);
			uint8_t* messages = (j == 0) ? messages1 : messages2;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages[k]);
			}
		}
	}
}

void messageNotInApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
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

void deleteValidReportTypes(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 1;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages1[k]);
			}
		}
	}
}

void deleteReportEmptyService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 1;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages1[k]);
			}
		}
	}
}

void deleteReportEmptyApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages1[k]);
			}
		}
	}
}

void deleteApplicationProcess(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 0;

	request.appendUint8(numOfApplications);
	request.appendUint8(applications[0]);
	request.appendUint8(numOfServicesPerApp);
}

void deleteService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 1;
	uint8_t numOfMessagesPerService = 0;

	request.appendUint8(numOfApplications);
	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
		}
	}
}

void deleteServiceEmptyApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 0;

	request.appendUint8(numOfApplications);
	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
		}
	}
}

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
			uint8_t* messages = (i == 2) ? validInvalidMessages : messages1;

			request.appendUint8(serviceType);
			request.appendUint8(numOfMessages);

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = messages[k];
				request.appendUint8(messageType);
			}
		}
	}
}

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
		REQUIRE(applicationProcesses.size() == 1);

		for (auto appID : applications) {
			REQUIRE(applicationProcesses.find(appID) != applicationProcesses.end());
			REQUIRE(applicationProcesses[appID].size() == 2);

			for (uint8_t j = 0; j < 2; j++) {
				uint8_t serviceType = services[j];
				REQUIRE(applicationProcesses[appID].find(serviceType) != applicationProcesses[appID].end());
				REQUIRE(applicationProcesses[appID][serviceType].size() == 2);
				uint8_t* messages = (j == 0) ? messages1 : messages2;

				for (uint8_t k = 0; k < 2; k++) {
					REQUIRE(std::find(applicationProcesses[appID][serviceType].begin(),
					                  applicationProcesses[appID][serviceType].end(),
					                  messages[k]) != applicationProcesses[appID][serviceType].end());
				}
			}
		}

		auto& servicesAllowed = realTimeForwarding.applicationProcessConfiguration.notEmpty;
		REQUIRE(servicesAllowed[applicationID].size() == 2);
		REQUIRE(servicesAllowed[applicationID][services[0]] == true);
		REQUIRE(servicesAllowed[applicationID][services[1]] == true);

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
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.notEmpty[applicationID].empty());

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

		realTimeForwarding.applicationProcessConfiguration.definitions[applicationID].clear();
		CHECK(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID].empty());
		realTimeForwarding.applicationProcessConfiguration.notEmpty[applicationID][services[0]] = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed) ==
		      1);
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID].empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max service types already reached") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		auto& applicationProcessConfig = realTimeForwarding.applicationProcessConfiguration;

		for (auto service : allServices) {
			applicationProcessConfig.definitions[applicationID][service].clear();
			applicationProcessConfig.notEmpty[applicationID][service] = true;
		}
		REQUIRE(applicationProcessConfig.definitions[applicationID].size() == 15);
		REQUIRE(applicationProcessConfig.notEmpty[applicationID].size() == 15);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 2);
		REQUIRE(applicationProcessConfig.definitions[applicationID].size() == 15);
		REQUIRE(applicationProcessConfig.notEmpty[applicationID].size() == 15);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All report types already allowed") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType = services[0];
		realTimeForwarding.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		realTimeForwarding.applicationProcessConfiguration.definitions[applicationID][serviceType].clear();
		CHECK(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID][serviceType].empty());
		realTimeForwarding.applicationProcessConfiguration.notEmpty[applicationID][serviceType] = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AllReportTypesAlreadyAllowed) ==
		      1);
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID].size() == 2);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max report types already reached") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType = services[0];
		realTimeForwarding.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		auto& applicationProcessConfig = realTimeForwarding.applicationProcessConfiguration;

		applicationProcessConfig.notEmpty[applicationID][serviceType] = true;
		for (uint8_t i = 0; i < ECSSMaxReportTypeDefinitions; i++) {
			applicationProcessConfig.definitions[applicationID][serviceType].push_back(i);
		}

		REQUIRE(applicationProcessConfig.definitions[applicationID].size() == 1);
		REQUIRE(applicationProcessConfig.notEmpty[applicationID].size() == 1);
		REQUIRE(applicationProcessConfig.definitions[applicationID][serviceType].size() ==
		        ECSSMaxReportTypeDefinitions);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) == 2);
		REQUIRE(applicationProcessConfig.definitions[applicationID].size() == 2);
		REQUIRE(applicationProcessConfig.notEmpty[applicationID].size() == 2);
		REQUIRE(applicationProcessConfig.definitions[applicationID][services[1]].size() == 2);
		REQUIRE(applicationProcessConfig.definitions[applicationID][serviceType].size() ==
		        ECSSMaxReportTypeDefinitions);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested  addition of duplicate report type definitions") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID);
		duplicateReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses.size() == 1);

		for (auto appID : applications) {
			REQUIRE(applicationProcesses.find(appID) != applicationProcesses.end());
			REQUIRE(applicationProcesses[appID].size() == 2);

			for (auto& serviceType : services) {
				REQUIRE(applicationProcesses[appID].find(serviceType) != applicationProcesses[appID].end());
				REQUIRE(applicationProcesses[appID][serviceType].size() == 1);

				for (uint8_t k = 0; k < 2; k++) {
					REQUIRE(std::find(applicationProcesses[appID][serviceType].begin(),
					                  applicationProcesses[appID][serviceType].end(),
					                  messages1[0]) != applicationProcesses[appID][serviceType].end());
				}
			}
		}

		auto& servicesAllowed = realTimeForwarding.applicationProcessConfiguration.notEmpty;
		REQUIRE(servicesAllowed[applicationID].size() == 2);
		REQUIRE(servicesAllowed[applicationID][services[0]] == true);
		REQUIRE(servicesAllowed[applicationID][services[1]] == true);

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

		realTimeForwarding.applicationProcessConfiguration.definitions[applicationID3].clear();
		CHECK(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID3].empty());
		realTimeForwarding.applicationProcessConfiguration.notEmpty[applicationID3][services[0]] = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed) ==
		      1);

		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		auto& servicesAllowed = realTimeForwarding.applicationProcessConfiguration.notEmpty;

		REQUIRE(applicationProcesses.size() == 2);
		REQUIRE(applicationProcesses.find(applicationID2) == applicationProcesses.end());
		REQUIRE(applicationProcesses.find(applicationID1) != applicationProcesses.end());
		REQUIRE(applicationProcesses.find(applicationID3) != applicationProcesses.end());

		REQUIRE(applicationProcesses[applicationID1].size() == 15);
		REQUIRE(servicesAllowed[applicationID1].size() == 15);
		for (auto serviceType : allServices) {
			REQUIRE(servicesAllowed[applicationID1][serviceType]);
		}
		REQUIRE(applicationProcesses[applicationID3].empty());

		for (auto& serviceType : applicationProcesses[applicationID1]) {
			REQUIRE(std::find(std::begin(allServices), std::end(allServices), serviceType.first) !=
			        std::end(allServices));
			REQUIRE(serviceType.second.size() == 2);
			auto& messagesToCheck = (serviceType.first == 1) ? messages1 : messages2;
			for (auto message : serviceType.second) {
				REQUIRE(std::find(std::begin(messagesToCheck), std::end(messagesToCheck), message) !=
				        std::end(messagesToCheck));
			}
		}

		REQUIRE(servicesAllowed[applicationID3].size() == 1);
		REQUIRE(servicesAllowed[applicationID3][services[0]] == true);

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
		REQUIRE(applicationProcesses[applicationID1].size() == 2);
		for (auto serviceType : services) {
			REQUIRE(applicationProcesses[applicationID1].find(serviceType) !=
			        applicationProcesses[applicationID1].end());
			REQUIRE(applicationProcesses[applicationID1][serviceType].empty());
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

		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses.size() == 2);
		REQUIRE(applicationProcesses[applicationID1].size() == 15);
		REQUIRE(applicationProcesses[applicationID2].size() == 2);

		for (auto& serviceType : applicationProcesses[applicationID1]) {
			REQUIRE(serviceType.second.empty());
		}
		for (auto& serviceType : applicationProcesses[applicationID2]) {
			REQUIRE(serviceType.second.empty());
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
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses[applicationID1].empty());

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
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(applicationProcesses.find(applicationID1) != applicationProcesses.end());
		REQUIRE(applicationProcesses.find(applicationID2) != applicationProcesses.end());
		REQUIRE(applicationProcesses.find(3) == applicationProcesses.end());
		REQUIRE(applicationProcesses[applicationID1].empty());
		REQUIRE(applicationProcesses[applicationID2].empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
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
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.notEmpty.empty());

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
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingApplication) == 1);
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
		          ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinition) == 2);
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
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinition) ==
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
		auto& isNotEmpty = realTimeForwarding.applicationProcessConfiguration.notEmpty;

		REQUIRE(applicationProcesses[applicationID].size() == 2);
		REQUIRE(applicationProcesses[applicationID][services[0]].size() == 1);
		REQUIRE(applicationProcesses[applicationID][services[1]].size() == 1);
		REQUIRE(isNotEmpty[applicationID].size() == 2);
		REQUIRE(isNotEmpty[applicationID][services[0]] == true);
		REQUIRE(isNotEmpty[applicationID][services[1]] == true);

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

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		auto& isNotEmpty = realTimeForwarding.applicationProcessConfiguration.notEmpty;

		REQUIRE(applicationProcesses[applicationID].size() == 1);
		REQUIRE(isNotEmpty[applicationID].size() == 1);
		REQUIRE(applicationProcesses[applicationID].find(services[1]) != applicationProcesses[applicationID].end());
		REQUIRE(isNotEmpty[applicationID].find(services[1]) != isNotEmpty[applicationID].end());

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
		auto& isNotEmpty = realTimeForwarding.applicationProcessConfiguration.notEmpty;

		REQUIRE(applicationProcesses.empty());
		REQUIRE(isNotEmpty.empty());

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
		auto& isNotEmpty = realTimeForwarding.applicationProcessConfiguration.notEmpty;

		REQUIRE(applicationProcesses.empty());
		REQUIRE(isNotEmpty.empty());

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
		auto& isNotEmpty = realTimeForwarding.applicationProcessConfiguration.notEmpty;

		REQUIRE(applicationProcesses[applicationID].size() == 1);
		REQUIRE(isNotEmpty[applicationID].size() == 1);
		REQUIRE(applicationProcesses[applicationID].find(services[0]) == applicationProcesses[applicationID].end());
		REQUIRE(isNotEmpty[applicationID].find(services[0]) == isNotEmpty[applicationID].end());

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

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		auto& isNotEmpty = realTimeForwarding.applicationProcessConfiguration.notEmpty;

		REQUIRE(applicationProcesses.empty());
		REQUIRE(isNotEmpty.empty());

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
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinition) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinition) ==
		      2);

		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		auto& isNotEmpty = realTimeForwarding.applicationProcessConfiguration.notEmpty;

		REQUIRE(applicationProcesses.size() == 2);
		for (auto appID : remainingApps) {
			REQUIRE(applicationProcesses.find(appID) != applicationProcesses.end());
		}

		// Check for appID = 1
		uint8_t appID1 = remainingApps[0];
		REQUIRE(applicationProcesses[appID1].size() == 1);
		REQUIRE(isNotEmpty[appID1].size() == 1);
		REQUIRE(applicationProcesses[appID1].find(services[0]) == applicationProcesses[appID1].end());
		REQUIRE(applicationProcesses[appID1][services[1]].size() == 2);
		REQUIRE(isNotEmpty[appID1][services[1]] == true);

		for (auto& message : messages1) {
			REQUIRE(std::find(applicationProcesses[appID1][services[1]].begin(),
			                  applicationProcesses[appID1][services[1]].end(),
			                  message) != applicationProcesses[appID1][services[1]].end());
		}

		// Check for appID = 2
		uint8_t appID2 = 2;
		REQUIRE(applicationProcesses.find(appID2) == applicationProcesses.end());
		REQUIRE(isNotEmpty.find(appID2) == isNotEmpty.end());

		// Check for appID = 3
		uint8_t appID3 = remainingApps[1];
		REQUIRE(applicationProcesses[appID3].size() == 15);
		REQUIRE(isNotEmpty[appID3].size() == 15);

		for (uint8_t i = 0; i < 15; i++) {
			uint8_t numOfMessages = (i == 0 or i == 7) ? 1 : 2; // we only deleted one report from services 1 and 8
			uint8_t* messages = (i == 0 or i == 7) ? remainingMessage : messages2;

			REQUIRE(applicationProcesses[appID3][allServices[i]].size() == numOfMessages);
			REQUIRE(isNotEmpty[appID3][allServices[i]] == true);

			for (uint8_t j = 0; j < numOfMessages; j++) {
				REQUIRE(std::find(applicationProcesses[appID3][allServices[i]].begin(),
				                  applicationProcesses[appID3][allServices[i]].end(),
				                  messages[j]) != applicationProcesses[appID3][allServices[i]].end());
			}
		}
		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}
}
