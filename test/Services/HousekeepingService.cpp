#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/HousekeepingService.hpp"
#include "etl/algorithm.h"

HousekeepingService& housekeepingService = Services.housekeeping;

/**
 * Helper function that forms the message that's going to be sent as request to create a new housekeeping structure.
 */
void buildRequest(Message& request, uint8_t idToCreate) {
	uint32_t interval = 7;
	uint16_t numOfSimplyCommutatedParams = 3;
	etl::vector<uint16_t, 3> simplyCommutatedIds = {8, 4, 5};

	request.appendUint8(idToCreate);
	request.appendUint32(interval);
	request.appendUint16(numOfSimplyCommutatedParams);
	for (auto& id : simplyCommutatedIds) {
		request.appendUint16(id);
	}
}

/**
 * Initializes 3 housekeeping structures with IDs = {0, 4, 6}
 */
void initializeHousekeepingStructures() {
	Message request(HousekeepingService::ServiceType,
	                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
	uint8_t ids[3] = {0, 4, 6};
	uint32_t interval = 7;
	uint16_t numOfSimplyCommutatedParams = 3;
	etl::vector<uint16_t, 3> simplyCommutatedIds = {8, 4, 5};

	HousekeepingStructure structures[3];
	int i = 0;
	for (auto& newStructure : structures) {
		newStructure.structureId = ids[i];
		newStructure.collectionInterval = interval;
		newStructure.periodicGenerationActionStatus = false;
		for (uint16_t parameterId : simplyCommutatedIds) {
			newStructure.simplyCommutatedParameterIds.push_back(parameterId);
		}
		housekeepingService.housekeepingStructures.insert({ids[i++], newStructure});
	}

	REQUIRE(housekeepingService.housekeepingStructures.size() == 3);
	REQUIRE(housekeepingService.housekeepingStructures.find(0) != housekeepingService.housekeepingStructures.end());
	REQUIRE(housekeepingService.housekeepingStructures.find(4) != housekeepingService.housekeepingStructures.end());
	REQUIRE(housekeepingService.housekeepingStructures.find(6) != housekeepingService.housekeepingStructures.end());
}

/**
 * Helper function that stores samples into simply commutated parameters of different data type each.
 */
void storeSamplesToParameters(uint16_t id1, uint16_t id2, uint16_t id3) {
	Message samples(HousekeepingService::ServiceType,
	                HousekeepingService::MessageType::ReportHousekeepingPeriodicProperties, Message::TM, 1);
	samples.appendUint16(static_cast<uint16_t>(33));
	samples.appendUint8(static_cast<uint8_t>(77));
	samples.appendUint32(static_cast<uint32_t>(99));

	systemParameters.parametersArray[id1].get().setValueFromMessage(samples);
	systemParameters.parametersArray[id2].get().setValueFromMessage(samples);
	systemParameters.parametersArray[id3].get().setValueFromMessage(samples);
}

/**
 * Helper function that forms the request to append new parameters to a housekeeping structure
 *
 * @param idToAppend housekeeping structure id to append the new parameters
 */
void appendNewParameters(Message& request, uint8_t idToAppend) {
	uint16_t numOfSimplyCommutatedParams = 7;
	etl::vector<uint16_t, 7> simplyCommutatedIds = {8, 4, 5, 9, 11, 10, 220};

	request.appendUint8(idToAppend);
	request.appendUint16(numOfSimplyCommutatedParams);
	for (auto& id : simplyCommutatedIds) {
		request.appendUint16(id);
	}
}

TEST_CASE("Create housekeeping structure") {
	SECTION("Valid structure creation request") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		uint8_t idToCreate = 2;
		uint32_t interval = 7;
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::vector<uint16_t, 3> simplyCommutatedIds = {8, 4, 5};

		request.appendUint8(idToCreate);
		request.appendUint32(interval);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id : simplyCommutatedIds) {
			request.appendUint16(id);
		}

		MessageParser::execute(request);
		HousekeepingStructure newStruct = housekeepingService.housekeepingStructures[idToCreate];

		CHECK(ServiceTests::count() == 0);
		CHECK(newStruct.structureId == idToCreate);
		CHECK(newStruct.simplyCommutatedParameterIds.size() == numOfSimplyCommutatedParams);
		CHECK(newStruct.collectionInterval == interval);
		for (auto& id : simplyCommutatedIds) {
			CHECK(std::find(std::begin(newStruct.simplyCommutatedParameterIds),
			                std::end(newStruct.simplyCommutatedParameterIds),
			                id) != std::end(newStruct.simplyCommutatedParameterIds));
		}
		for (auto& parameterId : newStruct.simplyCommutatedParameterIds) {
			CHECK(std::find(simplyCommutatedIds.begin(), simplyCommutatedIds.end(), parameterId) !=
			      simplyCommutatedIds.end());
		}
	}

	SECTION("Invalid structure creation request") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		uint8_t structureId = 9;
		HousekeepingStructure structure1;
		structure1.structureId = structureId;
		housekeepingService.housekeepingStructures.insert({structureId, structure1});

		uint8_t idToCreate = 9;
		request.appendUint8(idToCreate);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure) == 1);
		housekeepingService.housekeepingStructures.erase(structureId);
	}

	SECTION("Exceeding max number of housekeeping structures") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);

		uint8_t idsToCreate[15] = {1, 3, 5, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::vector<uint16_t, 3> simplyCommutatedIds = {8, 4, 5};
		uint32_t interval = 12;

		REQUIRE(housekeepingService.housekeepingStructures.size() == 1);

		for (auto& structId : idsToCreate) {
			request.appendUint8(structId);
			request.appendUint32(interval);
			request.appendUint16(numOfSimplyCommutatedParams);
			for (auto& parameterId : simplyCommutatedIds) {
				request.appendUint16(parameterId);
			}
			MessageParser::execute(request);
		}

		REQUIRE(housekeepingService.housekeepingStructures.size() == 10);
		CHECK(ServiceTests::count() == 7);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure) == 2);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfHousekeepingStructures) == 5);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Delete housekeeping structure") {
	SECTION("One valid request, the rest invalid") {
		Message createStruct(HousekeepingService::ServiceType,
		                     HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);

		buildRequest(createStruct, 2);
		MessageParser::execute(createStruct);

		REQUIRE(housekeepingService.housekeepingStructures.size() == 1);
		REQUIRE(housekeepingService.housekeepingStructures.find(2) != housekeepingService.housekeepingStructures.end());
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::DeleteHousekeepingReportStructure, Message::TC, 1);

		uint16_t numOfStructs = 5;
		uint8_t ids[5] = {2, 3, 4, 7, 8};
		request.appendUint16(numOfStructs);
		for (auto& id : ids) {
			request.appendUint8(id);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      4);
	}

	SECTION("Invalid request of periodic structure") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::DeleteHousekeepingReportStructure, Message::TC, 1);
		HousekeepingStructure periodicStruct;
		periodicStruct.structureId = 4;
		periodicStruct.periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures.insert({4, periodicStruct});

		uint16_t numOfStructs = 1;
		uint8_t structureId = 4;
		request.appendUint16(numOfStructs);
		request.appendUint8(structureId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 5);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfPeriodicStructure) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Enable the periodic generation of housekeeping structures") {
	SECTION("Both valid and invalid structure IDs in same request") {
		initializeHousekeepingStructures();
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport, Message::TC, 1);
		uint16_t numOfStructs = 5;
		uint8_t idsToEnable[5] = {1, 3, 4, 6, 7};
		request2.appendUint16(numOfStructs);
		for (auto& id : idsToEnable) {
			request2.appendUint8(id);
		}
		REQUIRE(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		REQUIRE(not housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		REQUIRE(not housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);

		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      3);
		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Disable the periodic generation of housekeeping structures") {
	SECTION("Both valid and invalid structure Ids in request") {
		initializeHousekeepingStructures();
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport, Message::TC, 1);
		uint16_t numOfStructs = 4;
		uint8_t idsToDisable[4] = {0, 1, 4, 6};
		request2.appendUint16(numOfStructs);
		for (auto& id : idsToDisable) {
			request2.appendUint8(id);
		}
		housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus = true;

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      1);
		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping structures") {
	SECTION("Both valid and invalid structure IDs in request") {
		initializeHousekeepingStructures();
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::ReportHousekeepingStructures, Message::TC, 1);
		uint16_t numOfStructs = 3;
		uint8_t idsToReport[3] = {9, 4, 2};
		request2.appendUint16(numOfStructs);
		for (auto& id : idsToReport) {
			request2.appendUint8(id);
		}
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      2);

		Message report = ServiceTests::get(1); // Both 0 and 2 are the error messages because only id=4 was valid.

		uint8_t validId = 4;
		CHECK(report.readUint8() == validId);
		CHECK(not report.readBoolean()); // periodic status
		CHECK(report.readUint32() == 7); // interval
		CHECK(report.readUint16() == 3); // number of simply commutated ids
		CHECK(report.readUint16() == 8);
		CHECK(report.readUint16() == 4); // ids
		CHECK(report.readUint16() == 5);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping structures without a TC message as argument") {
	SECTION("Check the report message generated by the non-TC function") {
		initializeHousekeepingStructures();
		uint8_t structureId = 4;

		housekeepingService.housekeepingStructureReport(structureId);
		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == HousekeepingService::MessageType::HousekeepingStructuresReport);
		CHECK(report.readUint8() == structureId);
		CHECK(not report.readBoolean());
		CHECK(report.readUint32() == 7);
		CHECK(report.readUint16() == 3);
		CHECK(report.readUint16() == 8);
		CHECK(report.readUint16() == 4);
		CHECK(report.readUint16() == 5);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping parameters") {
	SECTION("Valid structure request") {
		storeSamplesToParameters(8, 4, 5);
		initializeHousekeepingStructures();
		uint8_t structId = 6;

		housekeepingService.reportHousekeepingParameters(structId);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);
		REQUIRE(report.readUint8() == structId);
		CHECK(report.readUint16() == 33);
		CHECK(report.readUint8() == 77);
		CHECK(report.readUint32() == 99);
	}

	SECTION("Invalid structure request") {
		uint8_t structId = 8;
		housekeepingService.reportHousekeepingParameters(structId);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistingStructure) == 1);

		structId = 12;
		housekeepingService.reportHousekeepingParameters(structId);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistingStructure) == 2);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping parameters without a TC request") {
	SECTION("Check the report message generated by the non-TC function") {
		storeSamplesToParameters(8, 4, 5);
		initializeHousekeepingStructures();
		uint8_t structureId = 6;

		housekeepingService.housekeepingParametersReport(structureId);
		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);
		REQUIRE(report.readUint8() == structureId);
		CHECK(report.readUint16() == 33);
		CHECK(report.readUint8() == 77);
		CHECK(report.readUint32() == 99);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("One-shot housekeeping parameter report generation") {
	SECTION("Both valid and invalid structure IDs in request") {
		storeSamplesToParameters(8, 4, 5);
		initializeHousekeepingStructures();
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::GenerateOneShotHousekeepingReport, Message::TC, 1);
		uint16_t numOfStructs = 5;
		uint8_t structIds[5] = {0, 4, 7, 8, 11};
		request2.appendUint16(numOfStructs);
		for (auto& id : structIds) {
			request2.appendUint8(id);
		}
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 5);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      3);
		Message report1 = ServiceTests::get(0);
		Message report2 = ServiceTests::get(1);

		REQUIRE(report1.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);
		REQUIRE(report2.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);

		REQUIRE(report1.readUint8() == 0);
		CHECK(report1.readUint16() == 33);
		CHECK(report1.readUint8() == 77);
		CHECK(report1.readUint32() == 99);

		REQUIRE(report2.readUint8() == 4);
		CHECK(report2.readUint16() == 33);
		CHECK(report2.readUint8() == 77);
		CHECK(report2.readUint32() == 99);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Append parameters in housekeeping report structure") {
	SECTION("Non existing structure") {
		initializeHousekeepingStructures();
		Message request1(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		uint8_t structId = 2;
		request1.appendUint8(structId);
		MessageParser::execute(request1);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      1);
	}

	SECTION("Periodic structure") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport, Message::TC, 1);
		// Enable 1 periodic struct with id=0
		request.appendUint16(1);
		request.appendUint8(0);
		MessageParser::execute(request);

		REQUIRE(housekeepingService.housekeepingStructures.at(0).periodicGenerationActionStatus);
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		uint8_t structId = 0;
		request2.appendUint8(structId);
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::RequestedAppendToEnabledHousekeeping) == 1);
	}

	SECTION("Valid request including both valid and invalid parameters") {
		Message request3(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		uint8_t structId = 6;
		appendNewParameters(request3, structId);
		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 3);

		MessageParser::execute(request3);
		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter) == 1);

		uint16_t currentlyExistingParameters[] = {8, 4, 5, 9, 10, 11};
		HousekeepingStructure structToCheck = housekeepingService.housekeepingStructures[structId];
		REQUIRE(structToCheck.simplyCommutatedParameterIds.size() == 6);
		for (auto& existingParameter : currentlyExistingParameters) {
			CHECK(std::find(std::begin(structToCheck.simplyCommutatedParameterIds),
			                std::end(structToCheck.simplyCommutatedParameterIds),
			                existingParameter) != std::end(structToCheck.simplyCommutatedParameterIds));
		}
	}

	SECTION("Exceeding the maximum number of simply commutated parameters for the specified structure") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		uint8_t structId = 6;
		uint16_t numOfSimplyCommutatedParams = 12;
		etl::vector<uint16_t, 12> simplyCommutatedIds = {0, 1, 2, 3, 6, 7, 8, 12, 13, 14, 15, 16};

		request.appendUint8(structId);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id : simplyCommutatedIds) {
			request.appendUint16(id);
		}
		HousekeepingStructure& structToCheck = housekeepingService.housekeepingStructures[structId];
		REQUIRE(housekeepingService.housekeepingStructures.find(structId) !=
		        housekeepingService.housekeepingStructures.end());
		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 6);

		MessageParser::execute(request);

		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 10);
		CHECK(ServiceTests::count() == 7);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfSimplyCommutatedParameters) == 1);
	}

	SECTION("Continue exceeding the maximum number of parameters just to be sure") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		uint8_t structId = 6;
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::vector<uint16_t, 3> simplyCommutatedIds = {17, 18, 21};

		request.appendUint8(structId);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id : simplyCommutatedIds) {
			request.appendUint16(id);
		}
		HousekeepingStructure& structToCheck = housekeepingService.housekeepingStructures[structId];
		REQUIRE(housekeepingService.housekeepingStructures.find(structId) !=
		        housekeepingService.housekeepingStructures.end());
		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 10);

		MessageParser::execute(request);

		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 10);
		CHECK(ServiceTests::count() == 8);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfSimplyCommutatedParameters) == 2);
		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Modification of housekeeping structures' interval") {
	SECTION("Both valid and invalid requests") {
		initializeHousekeepingStructures();
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::ModifyCollectionIntervalOfStructures, Message::TC, 1);
		uint16_t numOfStructs = 4;
		uint8_t structIds[4] = {0, 4, 9, 10};
		uint32_t intervals[4] = {12, 21, 32, 17};
		request.appendUint16(numOfStructs);
		int i = 0;
		for (auto& id : structIds) {
			request.appendUint8(id);
			request.appendUint32(intervals[i++]);
		}
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      2);
		REQUIRE(housekeepingService.housekeepingStructures[0].collectionInterval == 12);
		REQUIRE(housekeepingService.housekeepingStructures[4].collectionInterval == 21);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping structure periodic properties") {
	SECTION("Both valid and invalid structure IDs in request") {
		initializeHousekeepingStructures();
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::ReportHousekeepingPeriodicProperties, Message::TC, 1);
		uint16_t numOfStructs = 6;
		uint8_t structIds[6] = {0, 4, 1, 6, 9, 10};
		request.appendUint16(numOfStructs);
		for (auto& id : structIds) {
			request.appendUint8(id);
		}
		housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures[4].collectionInterval = 24;
		housekeepingService.housekeepingStructures[6].collectionInterval = 13;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      3);

		Message report = ServiceTests::get(3);
		CHECK(report.readUint16() == 3); // Number of valid ids
		CHECK(report.readUint8() == 0); // Id
		CHECK(report.readBoolean() == true); // Periodic status
		CHECK(report.readUint32() == 7); // Interval
		CHECK(report.readUint8() == 4); // Id
		CHECK(report.readBoolean() == false); // Periodic status
		CHECK(report.readUint32() == 24); // Interval
		CHECK(report.readUint8() == 6); // Id
		CHECK(report.readBoolean() == false); // Periodic status
		CHECK(report.readUint32() == 13); // Interval

		ServiceTests::reset();
		Services.reset();
	}
}