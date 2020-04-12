#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"

TEST_CASE("Parameter Report Subservice") {
	SECTION("All requested parameters invalid") {
		Message request = Message(20, 1, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(54432);
		request.appendUint16(60000);
		request.appendUint16(65535);

		MessageParser::execute(request);
		CHECK(ServiceTests::get(0).serviceType == 1);
		CHECK(ServiceTests::get(0).messageType == 2);
		CHECK(ServiceTests::get(1).serviceType == 1);
		CHECK(ServiceTests::get(1).messageType == 2);
		CHECK(ServiceTests::get(2).serviceType == 1);
		CHECK(ServiceTests::get(2).messageType == 2);

		Message report = ServiceTests::get(3);
		CHECK(report.serviceType == 20);
		CHECK(report.messageType == 2);
		CHECK(report.readUint16() == 0);  // the message shall be empty

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Some requested parameters invalid") {
		Message request = Message(20, 1, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(1);
		request.appendUint16(2);
		request.appendUint16(10000);

		MessageParser::execute(request);
		CHECK(ServiceTests::get(0).serviceType == 1);
		CHECK(ServiceTests::get(0).messageType == 2);

		Message report = ServiceTests::get(1);
		CHECK(report.serviceType == 20);
		CHECK(report.messageType == 2);
		CHECK(report.readUint16() == 2);
		CHECK(report.readUint16() == 1);
		CHECK(report.readUint16() == 7);
		CHECK(report.readUint16() == 2);
		CHECK(report.readUint32() == 10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Parameters are of different types") {
		Message request = Message(20, 1, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(0);
		request.appendUint16(1);
		request.appendUint16(2);

		MessageParser::execute(request);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == 20);
		CHECK(report.messageType == 2);
		CHECK(report.readUint16() == 3);
		CHECK(report.readUint16() == 0);
		CHECK(report.readUint8() == 3);
		CHECK(report.readUint16() == 1);
		CHECK(report.readUint16() == 7);
		CHECK(report.readUint16() == 2);
		CHECK(report.readUint32() == 10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Wrong Message Type Handling Test") {
		Message falseRequest(62, 3, Message::TM, 1); // a totally wrong message

		MessageParser::execute(falseRequest);
		CHECK(ServiceTests::thrownError(ErrorHandler::InternalErrorType::OtherMessageType));

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Parameter Setting Subservice") {
	SECTION("All parameter ids are invalid") {
		Message request = Message(20, 3, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(54432);
		request.appendUint16(1);
		request.appendUint16(60000);
		request.appendUint16(1);
		request.appendUint16(65534);
		request.appendUint16(1);

		MessageParser::execute(request);
		CHECK(ServiceTests::get(0).serviceType == 1);
		CHECK(ServiceTests::get(0).messageType == 2); // Just one error, because it should break after on invalid param

		CHECK(systemParameters.parameter1.getValue() == 3);
		CHECK(systemParameters.parameter2.getValue() == 7);
		CHECK(systemParameters.parameter3.getValue() == 10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("The last parameter ids are invalid") {
		Message request = Message(20, 3, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(0);
		request.appendUint8(1);
		request.appendUint16(1);
		request.appendUint16(2);
		request.appendUint16(65534);
		request.appendUint16(1);

		MessageParser::execute(request);
		CHECK(ServiceTests::get(0).serviceType == 1);
		CHECK(ServiceTests::get(0).messageType == 2);

		CHECK(systemParameters.parameter1.getValue() == 1);
		CHECK(systemParameters.parameter2.getValue() == 2);
		CHECK(systemParameters.parameter3.getValue() == 10);

		systemParameters.parameter1.setValue(3);
		systemParameters.parameter2.setValue(7);
		systemParameters.parameter3.setValue(10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("The last parameter ids are invalid") {
		Message request = Message(20, 3, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(0);
		request.appendUint8(1);
		request.appendUint16(65534);
		request.appendUint16(1);
		request.appendUint16(2);
		request.appendUint16(3);

		MessageParser::execute(request);
		CHECK(ServiceTests::get(0).serviceType == 1);
		CHECK(ServiceTests::get(0).messageType == 2);

		CHECK(systemParameters.parameter1.getValue() == 1);
		CHECK(systemParameters.parameter2.getValue() == 7);
		CHECK(systemParameters.parameter3.getValue() == 10);

		systemParameters.parameter1.setValue(3);
		systemParameters.parameter2.setValue(7);
		systemParameters.parameter3.setValue(10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All ids are valid") {
		Message request = Message(20, 3, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(0);
		request.appendUint8(1);
		request.appendUint16(1);
		request.appendUint16(2);
		request.appendUint16(2);
		request.appendUint32(3);

		MessageParser::execute(request);

		CHECK(systemParameters.parameter1.getValue() == 1);
		CHECK(systemParameters.parameter2.getValue() == 2);
		CHECK(systemParameters.parameter3.getValue() == 3);

		systemParameters.parameter1.setValue(3);
		systemParameters.parameter2.setValue(7);
		systemParameters.parameter3.setValue(10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Wrong Message Type Handling Test") {
		Message falseRequest(62, 3, Message::TM, 1); // a totally wrong message

		MessageParser::execute(falseRequest);
		CHECK(ServiceTests::thrownError(ErrorHandler::InternalErrorType::OtherMessageType));

		ServiceTests::reset();
		Services.reset();
	}
}
