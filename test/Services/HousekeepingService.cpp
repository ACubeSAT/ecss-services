#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/HousekeepingService.hpp"

HousekeepingService& housekeepingService = Services.housekeeping;

/**
 * Helper function that forms the message that's going to be sent as request to create a new housekeeping structure.
 */
void buildRequest(Message& request, uint16_t idToCreate) {
	uint16_t interval = 7;
	uint16_t numOfSimplyCommutatedParams = 3;
	etl::vector <uint16_t, 50> simplyCommutatedIds = {1, 4, 5};
	uint16_t numOfSets = 2;
	etl::vector <std::pair<uint16_t, etl::vector <uint16_t, 5>>, 2> superCommutatedIds;
	etl::vector <uint16_t, 5> temp1 = {2, 3};
	etl::vector <uint16_t, 5> temp2 = {6, 7, 8};
	superCommutatedIds.push_back(std::make_pair(4, temp1));     //Num of samples followed by the list of IDs
	superCommutatedIds.push_back(std::make_pair(9, temp2));

	request.appendUint16(idToCreate);
	request.appendUint16(interval);
	request.appendUint16(numOfSimplyCommutatedParams);
	for (auto &id : simplyCommutatedIds) {
		request.appendUint16(id);
	}
	request.appendUint16(numOfSets);
	for (auto &set : superCommutatedIds) {
		request.appendUint16(set.first);
		request.appendUint16(set.second.size());
		for (auto &id : set.second) {
			request.appendUint16(id);
		}
	}
}

uint32_t samples3[9] = {12, 34, 21, 31, 54, 95, 67, 24, 55};
uint8_t samples4[9] = {11, 22, 33, 77, 88, 55, 99, 33, 44};

/**
 * Helper function that stores samples into parameters, which are passed as arguments.
 * @note: 2 parameters are simply-commutated and 2 are super-commutated
 */
void storeSamplesOfParameters( uint16_t simplyCommId1,
                               uint16_t simplyCommId2,
                               uint16_t superCommId1,
                               uint16_t superCommId2 ) {
	//Store samples for parameter with ID=simplyCommId1
	uint16_t sample1 = 45;
	housekeepingService.systemHousekeeping.housekeepingParameters.at(simplyCommId1).get().storeSamples(sample1);

	//Store samples for parameter with ID=simplyCommId2
	uint8_t sample2 = 21;
	housekeepingService.systemHousekeeping.housekeepingParameters.at(simplyCommId2).get().storeSamples(sample2);
	for (auto &value : samples3) {
		housekeepingService.systemHousekeeping.housekeepingParameters.at(superCommId1).get().storeSamples(value);
	}
	for (auto &value : samples4) {
		housekeepingService.systemHousekeeping.housekeepingParameters.at(superCommId2).get().storeSamples(value);
	}
}

void newParamsToAppend(Message& request, uint16_t idToAppend) {
	uint16_t numOfSimplyCommutatedParams = 7;
	etl::vector <uint16_t, 50> simplyCommutatedIds = {1, 4, 5, 9, 11, 10, 220};
	uint16_t numOfSets = 3;
	etl::vector <std::pair<uint16_t, etl::vector <uint16_t, 5>>, 3> superCommutatedIds;
	//Already existing
	etl::vector <uint16_t, 5> temp1 = {2, 3, 160};
	//2 already existing, 2 new
	etl::vector <uint16_t, 5> temp2 = {6, 7, 12, 13};
	//New set
	etl::vector <uint16_t, 5> temp3 = {14, 15, 16};
	superCommutatedIds.push_back(std::make_pair(4, temp1));     //Num of samples followed by the list of IDs
	superCommutatedIds.push_back(std::make_pair(11, temp2));
	superCommutatedIds.push_back(std::make_pair(7, temp3));

	request.appendUint16(idToAppend);
	request.appendUint16(numOfSimplyCommutatedParams);
	for (auto &id : simplyCommutatedIds) {
		request.appendUint16(id);
	}
	request.appendUint16(numOfSets);
	for (auto &set : superCommutatedIds) {
		request.appendUint16(set.first);
		request.appendUint16(set.second.size());
		for (auto &id : set.second) {
			request.appendUint16(id);
		}
	}
}

TEST_CASE("Housekeeping Reporting Sub-service") {
	SECTION("Create housekeeping structure") {
		// Valid structure creation request
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure,Message::TC,1);
		uint16_t idToCreate = 2;
		uint16_t interval = 7;
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::vector <uint16_t, 50> simplyCommutatedIds = {1, 4, 5};
		uint16_t numOfSets = 2;
		etl::vector <std::pair<uint16_t, etl::vector <uint16_t, 5>>, 2> superCommutatedIds;
		etl::vector <uint16_t, 5> temp1 = {2, 3};
		etl::vector <uint16_t, 5> temp2 = {6, 7, 8};
		superCommutatedIds.push_back(std::make_pair(4, temp1));     //Num of samples followed by the list of IDs
		superCommutatedIds.push_back(std::make_pair(9, temp2));

		request.appendUint16(idToCreate);
		request.appendUint16(interval);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto &id : simplyCommutatedIds) {
			request.appendUint16(id);
		}
		request.appendUint16(numOfSets);
		for (auto &set : superCommutatedIds) {
			request.appendUint16(set.first);
			request.appendUint16(set.second.size());
			for (auto &id : set.second) {
				request.appendUint16(id);
			}
		}

		MessageParser::execute(request);
		HousekeepingStructure newStruct = housekeepingService.housekeepingStructures[idToCreate];
		uint16_t allIds[8] = {1, 2, 3, 4, 5, 6, 7, 8};

		CHECK(ServiceTests::count() == 0);
		CHECK(newStruct.structureId == idToCreate);
		CHECK(newStruct.numOfSimplyCommutatedParams == numOfSimplyCommutatedParams);
		CHECK(newStruct.numOfSuperCommutatedParameterSets == numOfSets);
		CHECK(newStruct.collectionInterval == interval);
		for (auto &id : allIds) {
			CHECK(housekeepingService.existsInVector(id, newStruct.containedParameterIds));
		}
		for (auto &id : newStruct.simplyCommutatedIds) {
			CHECK(housekeepingService.existsInVector(id, simplyCommutatedIds));
		}
		for (int set = 0; set < newStruct.numOfSuperCommutatedParameterSets; set++) {
			CHECK(newStruct.superCommutatedIds[set].first == superCommutatedIds[set].first);
			for(int id = 0; id < newStruct.superCommutatedIds[set].second.size(); id++) {
				CHECK(newStruct.superCommutatedIds[set].second[id] == superCommutatedIds[set].second[id]);
			}
		}
		// Invalid structure creation request
		Message request2(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure,Message::TC,1);
		uint16_t idToCreate2 = 2;
		request2.appendUint16(idToCreate2);
		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure) == 1);
	}

	SECTION("Delete housekeeping structure") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::DeleteHousekeepingReportStructure,Message::TC,1);
		uint16_t numOfStructs = 5;
		uint16_t ids[5] = {2, 3, 4, 7, 8};
		request.appendUint16(numOfStructs);
		for (auto &id : ids) {
			request.appendUint16(id);
		}
		//Add periodic structure
		HousekeepingStructure periodicStruct;
		periodicStruct.structureId = 4;
		periodicStruct.periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures.insert({4, periodicStruct});

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 5);  //4 + 1 from previous test which did not reset the system.
		CHECK(ServiceTests::countThrownErrors
		      (ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) == 3);
		CHECK(ServiceTests::countThrownErrors
		      (ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfPeriodicStructure) == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Enable periodic generation of housekeeping structures") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure,Message::TC,1);
		uint16_t ids[3] = {0, 4, 6};
		for (auto &id : ids) {              //Create 3 structures first, these will be used by next test cases as well.
			buildRequest(request, id);
			MessageParser::execute(request);
		}
		CHECK(housekeepingService.housekeepingStructures.size() == 3);
		Message request2(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport,Message::TC,1);
		uint16_t numOfStructs = 5;
		uint16_t idsToEnable[5] = {1, 3, 4, 6, 7};
		request2.appendUint16(numOfStructs);
		for (auto &id : idsToEnable) {
			request2.appendUint16(id);
		}
		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);

		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) == 3);

		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);
	}

	SECTION("Disable periodic generation of housekeeping structures") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport,Message::TC,1);
		uint16_t numOfStructs = 4;
		uint16_t idsToDisable[4] = {0, 1, 4, 6};
		request.appendUint16(numOfStructs);
		for (auto &id : idsToDisable) {
			request.appendUint16(id);
		}
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure)== 4);
		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);
	}

	SECTION("Reporting of housekeeping structures") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::ReportHousekeepingStructures,Message::TC,1);
		uint16_t numOfStructs = 3;
		uint16_t idsToReport[3] = {9, 4, 2};
		request.appendUint16(numOfStructs);
		for (auto &id : idsToReport) {
			request.appendUint16(id);
		}
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 7);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure)== 6);

		Message report = ServiceTests::get(5); //Both 4 and 6 are the error messages because only id=4 was valid.
		uint16_t validId = 4;
		CHECK(report.readUint16() == validId);
		CHECK(not report.readBoolean());    //periodic status
		CHECK(report.readUint16() == 7);    //interval
		CHECK(report.readUint16() == 3);    //number of simply commutated ids
		CHECK(report.readUint16() == 1);    //ids
		CHECK(report.readUint16() == 4);
		CHECK(report.readUint16() == 5);
		CHECK(report.readUint16() == 2);    //number of super sets
		//Set-1
		CHECK(report.readUint16() == 4);    //number of samples
		CHECK(report.readUint16() == 2);    //number of ids
		CHECK(report.readUint16() == 2);    //ids
		CHECK(report.readUint16() == 3);
		//Set-2
		CHECK(report.readUint16() == 9);    //number of samples
		CHECK(report.readUint16() == 3);    //number of ids
		CHECK(report.readUint16() == 6);    //ids
		CHECK(report.readUint16() == 7);
		CHECK(report.readUint16() == 8);
	}

	SECTION("Reporting of housekeeping parameters") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::ReportHousekeepingParameters,Message::TC,1);
		uint16_t structId = 6;
		request.appendUint16(structId);

		for (auto &paramId : housekeepingService.housekeepingStructures[structId].containedParameterIds) {
			CHECK(housekeepingService.systemHousekeeping.housekeepingParameters.at(paramId).get().sampleCounter == 0);
		}

		storeSamplesOfParameters(4, 5, 7, 8);
		CHECK(housekeepingService.systemHousekeeping.housekeepingParameters.at(4).get().sampleCounter == 1);
		CHECK(housekeepingService.systemHousekeeping.housekeepingParameters.at(5).get().sampleCounter == 1);
		CHECK(housekeepingService.systemHousekeeping.housekeepingParameters.at(7).get().sampleCounter == 9);
		CHECK(housekeepingService.systemHousekeeping.housekeepingParameters.at(8).get().sampleCounter == 9);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 8);
		Message report = ServiceTests::get(7);

		CHECK(report.readUint16() == structId);
		//Simply commutated parameter samples
		CHECK(report.readUint16() == 0);
		CHECK(report.readUint16() == 45);
		CHECK(report.readUint8() == static_cast <uint8_t> (21));
		//Super commutated parameter samples
		for (int i = 0; i < 4; i++) {       //Ids={2,3} are in the same set, with no samples. 4 is the sample-counter
											//of the current group, as created by the "buildRequest" function above.
			CHECK(report.readUint32() == 0);
			CHECK(report.readUint32() == 0);
		}
		for (int i = 0; i < 9; i++) {		//Ids={6,7,8} are in the same set, 7 and 8 have samples. 9 is the
											//sample-counter of the current group, as created by the "buildRequest" function above.
			CHECK(report.readUint16() == 0);
			CHECK(report.readUint32() == samples3[i]);
			CHECK(report.readUint8() == static_cast <uint8_t> (samples4[i]));
		}
		/**
		 * @note: this is the order that the parameter samples should be reported as per 6.3.3.3(c) from the standard.
		 */
	}

	SECTION("Generation of one-shot housekeeping parameter report") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::GenerateOneShotHousekeepingReport,Message::TC,1);
		uint16_t numOfStructs = 5;
		uint16_t structIds[5] = {0, 4, 7, 8, 11};
		request.appendUint16(numOfStructs);
		for (auto &id : structIds) {
			request.appendUint16(id);
		}
		storeSamplesOfParameters(4, 5, 7, 8);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 13);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure)== 9);
		Message report = ServiceTests::get(9);

		CHECK(report.readUint16() == 4);
		//Simply commutated parameter samples
		CHECK(report.readUint16() == 0);
		CHECK(report.readUint16() == 45);
		CHECK(report.readUint8() == static_cast <uint8_t> (21));
		//Super commutated parameter samples
		for (int i = 0; i < 4; i++) {
			CHECK(report.readUint32() == 0);
			CHECK(report.readUint32() == 0);
		}
		for (int i = 0; i < 9; i++) {
			CHECK(report.readUint16() == 0);
			CHECK(report.readUint32() == samples3[i]);
			CHECK(report.readUint8() == static_cast <uint8_t> (samples4[i]));
		}
    }

	SECTION("Append parameters in housekeeping report structure") {
		//Non existing structure
		Message request1(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::AppendParametersToHousekeepingStructure,Message::TC,1);
		uint16_t structId = 2;
		request1.appendUint16(structId);
		MessageParser::execute(request1);
		CHECK(ServiceTests::count() == 14);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure)== 10);

		//Periodic structure
		Message request(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport,Message::TC,1);
		request.appendUint16(1);    //Enable 1 periodic struct with id=0
		request.appendUint16(0);
		MessageParser::execute(request);
		CHECK(housekeepingService.housekeepingStructures.at(0).periodicGenerationActionStatus);
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::AppendParametersToHousekeepingStructure,Message::TC,1);
		structId = 0;
		request2.appendUint16(structId);
		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 15);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedAppendToPeriodicStructure)== 1);

		//Valid request including both valid and invalid parameters
		Message request3(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::AppendParametersToHousekeepingStructure,Message::TC,1);
		structId = 6;
		newParamsToAppend(request3, structId);

		MessageParser::execute(request3);
		CHECK(ServiceTests::count() == 24);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter)== 7);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter)== 2);

		uint16_t allNewIds[8] = {9, 11, 10, 12, 13, 14, 15, 16};
		uint16_t newSimplyCommutatedIds[6] = {1, 4, 5, 9, 11, 10};
		uint16_t newSuperCommutatedIds[10] = {2, 3, 6, 7, 8, 12, 13, 14, 15, 16};
		uint16_t newSampleCounters[4] = {4, 9, 11, 7};

		HousekeepingStructure structToCheck = housekeepingService.housekeepingStructures[structId];
		CHECK(structToCheck.containedParameterIds.size() == 16);
		for (auto &newId : allNewIds) {
			CHECK(housekeepingService.existsInVector(newId, structToCheck.containedParameterIds));
		}
		CHECK(structToCheck.numOfSimplyCommutatedParams == 6);
		CHECK(structToCheck.numOfSimplyCommutatedParams == structToCheck.simplyCommutatedIds.size());
		int i = 0;
		for (auto &id : structToCheck.simplyCommutatedIds) {
			CHECK(id == newSimplyCommutatedIds[i++]);
		}
		CHECK(structToCheck.numOfSuperCommutatedParameterSets == 4);    //2 added, 1 whole was rejected (2,3,160)
		CHECK(structToCheck.numOfSuperCommutatedParameterSets == structToCheck.superCommutatedIds.size());
		int j = 0;
		i = 0;
		for (auto &set : structToCheck.superCommutatedIds) {
			CHECK(set.first == newSampleCounters[i++]);
			for (auto &id : set.second) {
				CHECK(id == newSuperCommutatedIds[j++]);
			}
		}
	}

	SECTION("Modification of housekeeping structures' interval") {
		Message request(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::ModifyCollectionIntervalOfStructures,Message::TC,1);
		uint16_t numOfStructs = 4;
		uint16_t structIds[4] = {0, 4, 9, 10};
		uint16_t intervals[4] = {12, 21, 32, 17};
		request.appendUint16(numOfStructs);
		int i = 0;
		for (auto &id : structIds) {
			request.appendUint16(id);
			request.appendUint16(intervals[i++]);
		}
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 26);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure)== 12);
		CHECK(housekeepingService.housekeepingStructures[0].collectionInterval == 12);
		CHECK(housekeepingService.housekeepingStructures[4].collectionInterval == 21);
	}

	SECTION("Reporting of housekeeping structure periodic properties") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::ReportHousekeepingPeriodicProperties,Message::TC,1);
		uint16_t numOfStructs = 6;
		uint16_t structIds[6] = {0, 4, 1, 6, 9, 10};
		request.appendUint16(numOfStructs);
		for (auto &id : structIds) {
			request.appendUint16(id);
		}
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 30);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure)== 15);

		Message report = ServiceTests::get(29);
		CHECK(report.readUint16() == 3);        //Number of valid ids
		CHECK(report.readUint16() == 0);        //Id
		CHECK(report.readBoolean() == true);    //Periodic status
		CHECK(report.readUint16() == 12);       //Interval
		CHECK(report.readUint16() == 4);        //Id
		CHECK(report.readBoolean() == false);   //Periodic status
		CHECK(report.readUint16() == 21);       //Interval
		CHECK(report.readUint16() == 6);        //Id
		CHECK(report.readBoolean() == false);   //Periodic status
		CHECK(report.readUint16() == 7);        //Interval

		ServiceTests::reset();
		Services.reset();
	}

}