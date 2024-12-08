#include "ECSS_Configuration.hpp"
#ifdef SERVICE_EVENTREPORT

#include <Services/EventActionService.hpp>
#include <Services/EventReportService.hpp>
#include "Message.hpp"

/**
 * @todo (#252): Add message type in TCs
 * @todo (#253): this code is error prone, depending on parameters given, add fail safes (probably?)
 */
template <uint16_t Size>
void EventReportService::informativeEventReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	if (stateOfEvents[static_cast<EventDefinitionId>(eventID)]) {
		Message<Size> report = createTM<Size>(EventReportService::MessageType::InformativeEventReport);
		report.append<EventDefinitionId>(eventID);
		report.appendString(data);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);

		storeMessage(report);
	}
}

template <uint16_t Size>
void EventReportService::lowSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	lowSeverityEventCount++;
	if (stateOfEvents[static_cast<EventDefinitionId>(eventID)]) {
		lowSeverityReportCount++;
		Message<Size> report = createTM<Size>(EventReportService::MessageType::LowSeverityAnomalyReport);
		report.append<EventDefinitionId>(eventID);
		report.appendString(data);
		lastLowSeverityReportID = static_cast<EventDefinitionId>(eventID);

		storeMessage(report);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);
	}
}

template <uint16_t Size>
void EventReportService::mediumSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	mediumSeverityEventCount++;
	if (stateOfEvents[static_cast<EventDefinitionId>(eventID)]) {
		mediumSeverityReportCount++;
		Message report = createTM<Size>(EventReportService::MessageType::MediumSeverityAnomalyReport);
		report.append<EventDefinitionId>(eventID);
		report.appendString(data);
		lastMediumSeverityReportID = static_cast<EventDefinitionId>(eventID);

		storeMessage(report);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);
	}
}

template <uint16_t Size>
void EventReportService::highSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	highSeverityEventCount++;
	if (stateOfEvents[static_cast<EventDefinitionId>(eventID)]) {
		highSeverityReportCount++;
		Message report = createTM<Size>(EventReportService::MessageType::HighSeverityAnomalyReport);
		report.append<EventDefinitionId>(eventID);
		report.appendString(data);
		lastHighSeverityReportID = static_cast<EventDefinitionId>(eventID);

		storeMessage(report);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);
	}
}

template <uint16_t Size>
void EventReportService::enableReportGeneration(Message<Size>& message) {
	if (!message.assertTC(ServiceType, MessageType::EnableReportGenerationOfEvents)) {
		return;
	}

	/**
	 * @todo (#254): Report an error if length > numberOfEvents
	 */
	uint16_t const length = message.readUint16();
	if (length <= numberOfEvents) {
		for (uint16_t i = 0; i < length; i++) {
			stateOfEvents[message.read<EventDefinitionId>()] = true;
		}
	}
	disabledEventsCount = stateOfEvents.size() - stateOfEvents.count();
}

template <uint16_t Size>
void EventReportService::disableReportGeneration(Message<Size>& message) {
	if (!message.assertTC(ServiceType, MessageType::DisableReportGenerationOfEvents)) {
		return;
	}

	/**
	 * @todo: (#254) Report an error if length > numberOfEvents
	 */
	uint16_t const length = message.readUint16();
	if (length <= numberOfEvents) {
		for (uint16_t i = 0; i < length; i++) {
			stateOfEvents[message.read<EventDefinitionId>()] = false;
		}
	}
	disabledEventsCount = stateOfEvents.size() - stateOfEvents.count();
}

template <uint16_t Size>
void EventReportService::requestListOfDisabledEvents(const Message<Size>& message) {
	if (!message.assertTC(ServiceType, MessageType::ReportListOfDisabledEvents)) {
		return;
	}

	listOfDisabledEventsReport();
}

template <uint16_t Size>
void EventReportService::listOfDisabledEventsReport() {
	Message report = createTM<Size>(EventReportService::MessageType::DisabledListEventReport);

	uint16_t const numberOfDisabledEvents = stateOfEvents.size() - stateOfEvents.count(); // NOLINT(cppcoreguidelines-init-variables)
	report.appendHalfword(numberOfDisabledEvents);
	for (size_t i = 0; i < stateOfEvents.size(); i++) {
		if (not stateOfEvents[i]) {
			report.append<EventDefinitionId>(i);
		}
	}

	storeMessage(report);
}

template <uint16_t Size>
void EventReportService::execute(Message<Size>& message) {
	switch (message.messageType) {
		case EnableReportGenerationOfEvents:
			enableReportGeneration(message);
			break;
		case DisableReportGenerationOfEvents:
			disableReportGeneration(message);
			break;
		case ReportListOfDisabledEvents:
			requestListOfDisabledEvents(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
