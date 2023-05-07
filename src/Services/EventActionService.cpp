#include "ECSS_Configuration.hpp"
#ifdef SERVICE_EVENTACTION

#include "Services/EventActionService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"


void EventActionService::addEventActionDefinitions(Message& message) {
	// TC[19,1]
	if(!message.assertTC(ServiceType, MessageType::AddEventAction))
		return;

	uint16_t applicationID = message.readEnum16();
	uint16_t eventDefinitionID = message.readEnum16();
	uint16_t eventActionDefinitionID = message.readEnum16();
	if (eventActionDefinitionMap.find(eventDefinitionID) != eventActionDefinitionMap.end()) {
		auto range = eventActionDefinitionMap.equal_range(eventDefinitionID);
		for (auto& element = range.first; element != range.second; ++element) {
			if (element->second.eventActionDefinitionID == eventActionDefinitionID) {
				ErrorHandler::reportError(message, ErrorHandler::EventActionDefinitionIDExistsError);
				return;
			}
		}
	}

	if ((message.dataSize - 6) > ECSSTCRequestStringSize) {
		ErrorHandler::reportInternalError(ErrorHandler::MessageTooLarge);
		return;
	}

	if (eventActionDefinitionMap.size() == ECSSEventActionStructMapSize) {
		ErrorHandler::reportError(message,ErrorHandler::EventActionDefinitionsMapIsFull);
		return;
	}

	char data[ECSSTCRequestStringSize] = { 0 };
	message.readString(data, message.dataSize - 6);
	EventActionDefinition temp;
	temp.enabled = false;
	temp.applicationId = applicationID;
	temp.eventDefinitionID = eventDefinitionID;
	temp.eventActionDefinitionID = eventActionDefinitionID;
	temp.request = String<ECSSTCRequestStringSize>(data);
	eventActionDefinitionMap.insert(std::make_pair(eventDefinitionID, temp));
}

void EventActionService::deleteEventActionDefinitions(Message& message) {
	if(!message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction))
		return;

	uint16_t numberOfEventActionDefinitions = message.readUint16();
	bool definitionIDexists = false;
	for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
		message.skipBytes(2);
		uint16_t eventDefinitionID = message.readEnum16();
		uint16_t eventActionDefinitionID = message.readEnum16();
		if (eventActionDefinitionMap.find(eventDefinitionID) != eventActionDefinitionMap.end()) {
			auto range = eventActionDefinitionMap.equal_range(eventDefinitionID);
			for (auto& element = range.first; element != range.second; ++element) {
				if (element->second.eventActionDefinitionID == eventActionDefinitionID) {
					definitionIDexists = true;
					if (element->second.enabled) {
						ErrorHandler::reportError(message, ErrorHandler::EventActionDeleteEnabledDefinitionError);
					} else {
						eventActionDefinitionMap.erase(element);
					}
				}
			}
			if (not definitionIDexists) {
				ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionIDError);
			}
		} else {
			ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventDefinitionError);
		}
	}
}

void EventActionService::deleteAllEventActionDefinitions(Message& message) {
	// TC[19,3]
	if(!message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::DeleteAllEventAction))
		return;

	setEventActionFunctionStatus(false);
	eventActionDefinitionMap.clear();
}

void EventActionService::enableEventActionDefinitions(Message& message) {
	// TC[19,4]
	if(!message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction))
		return;

	uint16_t numberOfEventActionDefinitions = message.readUint16();
	if (numberOfEventActionDefinitions != 0U) {
		for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
			message.skipBytes(2); // Skips reading the application ID
			uint16_t eventDefinitionID = message.readEnum16();
			uint16_t eventActionDefinitionID = message.readEnum16();
			if (eventActionDefinitionMap.find(eventDefinitionID) != eventActionDefinitionMap.end()) {
				bool definitionIDexists = false;
				auto range = eventActionDefinitionMap.equal_range(eventDefinitionID);
				for (auto& element = range.first; element != range.second; ++element) {
					if (element->second.eventActionDefinitionID == eventActionDefinitionID) {
						element->second.enabled = true;
						definitionIDexists = true;
						break;
					}
				}
				if (not definitionIDexists) {
					ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionIDError);
				}
			} else {
				ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventDefinitionError);
			}
		}
	} else {
		for (auto& element : eventActionDefinitionMap) {
			element.second.enabled = true;
		}
	}
}

void EventActionService::disableEventActionDefinitions(Message& message) {
	// TC[19,5]
	if(!message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction))
		return;

	uint16_t numberOfEventActionDefinitions = message.readUint16();
	if (numberOfEventActionDefinitions != 0U) {
		for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
			message.skipBytes(2); // Skips reading applicationID
			uint16_t eventDefinitionID = message.readEnum16();
			uint16_t eventActionDefinitionID = message.readEnum16();
			if (eventActionDefinitionMap.find(eventDefinitionID) != eventActionDefinitionMap.end()) {
				bool definitionIDexists = false;
				auto range = eventActionDefinitionMap.equal_range(eventDefinitionID);
				for (auto& element = range.first; element != range.second; ++element) {
					if (element->second.eventActionDefinitionID == eventActionDefinitionID) {
						element->second.enabled = false;
						definitionIDexists = true;
					}
				}
				if (not definitionIDexists) {
					ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionIDError);
				}
			} else {
				ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventDefinitionError);
			}
		}
	} else {
		for (auto& element : eventActionDefinitionMap) {
			element.second.enabled = false;
		}
	}
}

void EventActionService::requestEventActionDefinitionStatus(Message& message) {
	// TC[19,6]
	if(!message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::ReportStatusOfEachEventAction))
		return;

	eventActionStatusReport();
}

void EventActionService::eventActionStatusReport() {
	// TM[19,7]
	Message report = createTM(EventActionStatusReport);
	uint16_t count = eventActionDefinitionMap.size();
	report.appendUint16(count);
	for (const auto& element : eventActionDefinitionMap) {
		report.appendEnum16(element.second.applicationId);
		report.appendEnum16(element.second.eventDefinitionID);
		report.appendEnum16(element.second.eventActionDefinitionID);
		report.appendBoolean(element.second.enabled);
	}
	storeMessage(report);
}

void EventActionService::enableEventActionFunction(Message& message) {
	// TC[19,8]
	if(!message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::EnableEventActionFunction))
		return;

	setEventActionFunctionStatus(true);
}

void EventActionService::disableEventActionFunction(Message& message) {
	// TC[19,9]
	if(!message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::DisableEventActionFunction))
		return;

	setEventActionFunctionStatus(false);
}

// TODO: Should I use applicationID too?
void EventActionService::executeAction(uint16_t eventID) {
	// Custom function
	if (eventActionFunctionStatus) {
		auto range = eventActionDefinitionMap.equal_range(eventID);
		for (auto& element = range.first; element != range.second; ++element) {
			if (element->second.enabled) {
				Message message = MessageParser::parseECSSTC(element->second.request);
				MessageParser::execute(message);
			}
		}
	}
}

void EventActionService::execute(Message& message) {
	switch (message.messageType) {
		case AddEventAction:
			addEventActionDefinitions(message);
			break;
		case DeleteEventAction:
			deleteEventActionDefinitions(message);
			break;
		case DeleteAllEventAction:
			deleteAllEventActionDefinitions(message);
			break;
		case EnableEventAction:
			enableEventActionDefinitions(message);
			break;
		case DisableEventAction:
			disableEventActionDefinitions(message);
			break;
		case ReportStatusOfEachEventAction:
			requestEventActionDefinitionStatus(message);
			break;
		case EnableEventActionFunction :
			enableEventActionFunction(message);
			break;
		case DisableEventActionFunction:
			disableEventActionFunction(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
