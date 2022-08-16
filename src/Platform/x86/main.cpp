#include <Logger.hpp>
#include <Platform/x86/Helpers/UTCTimestamp.hpp>
#include <Time/UTCTimestamp.hpp>
#include <arpa/inet.h>
#include <ctime>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "ErrorHandler.hpp"
#include "Helpers/CRCHelper.hpp"
#include "Helpers/Statistic.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "ServicePool.hpp"
#include "Services/EventActionService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/FunctionManagementService.hpp"
#include "Services/HousekeepingService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/OnBoardMonitoringService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/TestService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"
#include "etl/String.hpp"
HousekeepingService& housekeepingService = Services.housekeeping;
ParameterService parameterManagement;

void initializeHousekeepingStructures() {
	uint8_t ids[3] = {0, 4, 6};
	uint32_t interval = 7;
	etl::vector<uint16_t, 3> simplyCommutatedIds = {0, 1, 2};

	HousekeepingStructure structures[3];
	int i = 0;
	for (auto& newStructure: structures) {
		newStructure.structureId = ids[i];
		newStructure.collectionInterval = interval;
		newStructure.periodicGenerationActionStatus = false;
		for (uint16_t parameterId: simplyCommutatedIds) {
			newStructure.simplyCommutatedParameterIds.push_back(parameterId);
		}
		housekeepingService.housekeepingStructures.insert({ids[i], newStructure});
		i++;
	}
}


void storeSamplesToParameters(uint16_t id1, uint16_t id2, uint16_t id3) {
	//	Message samples(HousekeepingService::ServiceType,
	//	                HousekeepingService::MessageType::ReportHousekeepingPeriodicProperties, Message::TM, 1);

	static_cast<Parameter<uint8_t>&>(parameterManagement.getParameter(id1)->get()).setValue(33);
	static_cast<Parameter<uint8_t>&>(parameterManagement.getParameter(id2)->get()).setValue(77);
	static_cast<Parameter<uint8_t>&>(parameterManagement.getParameter(id3)->get()).setValue(99);
}
int main() {
	storeSamplesToParameters(0, 1, 2);
	initializeHousekeepingStructures();
	//	LOG_DEBUG << "Setting up YAMCS Connection";
	//	int addrlen, msglen;
	//	char message[300];
	//	int s;
	//	struct sockaddr_in yamcs, addr;
	//
	//	//UDP Socket creation
	//	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//	if (s < 0) {
	//		printf("\nUDP socket creation failed");
	//		return 1;
	//	} else {
	//		LOG_DEBUG << "Socket created successfully";
	//	}
	//
	//	//UDP set timeout
	//	struct timeval timeout;
	//	timeout.tv_sec = 30;
	//	timeout.tv_usec = 0;
	//	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) {
	//		printf("\nerror in setting timeout");
	//		return 1;
	//	} else {
	//		LOG_DEBUG << "Timeout set successfully to" << timeout.tv_sec << "seconds";
	//		yamcs.sin_port = htons(10025);
	//	}
	//	//UDP SOCKET Binding
	//	if (bind(s, (sockaddr*) &yamcs, sizeof(yamcs)) < 0) {
	//		printf("\nUDP socket binding failed\n");
	//		return 1;
	//	} else {
	//		LOG_DEBUG << "Binding finished successfully";
	//	}
	//	LOG_DEBUG << "Finished with setting up the connection";
	//
	//	LOG_NOTICE << "ECSS Services test application";
	//
	//	LOG_NOTICE << "Receiving TCs from YAMCS";
	//	//UDP Receiving broadcasted data
	//	addrlen = sizeof(addr);
	//	while (1) {
	//		std::ostringstream ss;
	//		msglen = recvfrom(s, message, sizeof(message), 0, (struct sockaddr*) &addr,
	//		                  reinterpret_cast<socklen_t*>(&addrlen));
	//		if (msglen < 0) {
	//			printf("Timeout exceeded\n");
	//			printf("No messages from YAMCS received\n");
	//			break;
	//		}
	//		for (int i = 0; i < msglen; i++) {
	//			ss << static_cast<int>(message[i]) << " ";
	//		}
	//		printf("size of message %d", msglen);
	//		LOG_NOTICE << ss.str();
	//		Message m = MessageParser::parse(reinterpret_cast<uint8_t*>(message), msglen);
	//		MessageParser::execute(m);
	//	}

	TestService& testService = Services.testService;
	Message testPacket = Message(TestService::ServiceType, TestService::MessageType::AreYouAliveTestReport, Message::TM, 1);
	//	testService.areYouAliveTestReport(testPacket);


	uint8_t structId = 6;
	std::cout << structId << " " << std::endl;
	while (1) {
	std:
		usleep(1000000);
		housekeepingService.housekeepingParametersReport(structId);
		//		housekeepingService.housekeepingStructureReport(6);
	}


	LOG_NOTICE << "ECSS Services test complete";

	std::cout << UTCTimestamp() << std::endl;
	return 0;
	LOG_NOTICE << "ECSS Services test complete";
	return 0;
}
