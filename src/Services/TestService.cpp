#include "ECSS_Configurations.hpp"
#ifdef SERVICE_TEST

#include "ServicePool.hpp"
#include "Services/TestService.hpp"

void TestService::areYouAlive(Message& request) {
	if (!request.assertTC(TestService::ServiceType, TestService::MessageType::AreYouAliveTest)) {
		return;
	}
	areYouAliveReport();
}

void TestService::areYouAliveReport() {
	Message report = createTM(TestService::MessageType::AreYouAliveTestReport);
	storeMessage(report);
}

void TestService::onBoardConnection(Message& request) {
	if (!request.assertTC(TestService::ServiceType, TestService::MessageType::OnBoardConnectionTest)) {
		return;
	}
	uint16_t applicationProcessId = request.readUint16();
	onBoardConnectionReport(applicationProcessId);
}

void TestService::onBoardConnectionReport(uint16_t applicationProcessId) {
	Message report = createTM(TestService::MessageType::OnBoardConnectionTestReport);
	report.appendUint16(applicationProcessId);
	storeMessage(report);
}

void TestService::execute(Message& message) {
	switch (message.messageType) {
		case AreYouAliveTest:
			areYouAlive(message);
			break;
		case OnBoardConnectionTest:
			onBoardConnection(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
