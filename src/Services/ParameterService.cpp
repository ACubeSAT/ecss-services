#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Helpers/Parameter.hpp"
#include "Services/ParameterService.hpp"


void ParameterService::reportParameters(Message& paramIds) {

	if (!paramIds.assertTC(ServiceType, ReportParameterValues)) {
		return;
	}
	Message parameterReport = createTM(ParameterValuesReport);

	uint16_t numOfIds = paramIds.readUint16();
	uint16_t numberOfValidIds = 0;
	for (uint16_t i = 0; i < numOfIds; i++) {
		if (parameterExists(paramIds.readUint16())) {
			numberOfValidIds++;
		}
	}
	parameterReport.appendUint16(numberOfValidIds);
	paramIds.resetRead();

	numOfIds = paramIds.readUint16();
	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = paramIds.readUint16();
		auto parameter = getParameter(currId);
		if (!parameter) {
			ErrorHandler::reportError(paramIds, ErrorHandler::GetNonExistingParameter);
			continue;
		}
		parameterReport.appendUint16(currId);
		parameter->get().appendValueToMessage(parameterReport);
	}

	storeMessage(parameterReport);
}

void ParameterService::setParameters(Message& newParamValues) const {
	if (!newParamValues.assertTC(ServiceType, MessageType::SetParameterValues)) {
		return;
	}

	uint16_t numOfIds = newParamValues.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = newParamValues.readUint16();
		auto parameter = getParameter(currId);
		if (!parameter) {
			ErrorHandler::reportError(newParamValues, ErrorHandler::SetNonExistingParameter);
			break; // Setting next parameters is impossible, since the size of value to be read is unknown
		}
		parameter->get().setValueFromMessage(newParamValues);
	}
}

void ParameterService::execute(Message& message) {
	switch (message.messageType) {
		case ReportParameterValues:
			reportParameters(message);
			break;
		case SetParameterValues:
			setParameters(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
