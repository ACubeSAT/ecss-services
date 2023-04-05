#include "ECSS_Configuration.hpp"
#ifdef SERVICE_EVENTACTION

#include "Message.hpp"
#include "MessageParser.hpp"
#include "Services/EventActionService.hpp"

EventActionService::EventActionDefinition::EventActionDefinition(uint16_t applicationID, uint16_t eventDefinitionID, Message& message)
    : applicationID(applicationID), eventDefinitionID(eventDefinitionID), request(message.data + message.readPosition) {
	message.readPosition += ECSSTCRequestStringSize;
}

void EventActionService::addEventActionDefinitions(Message& message) {
	// TC[19,1]
	message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction);
	uint8_t numberOfEventActionDefinitions = message.readUint8();
	while (numberOfEventActionDefinitions-- != 0) {
		uint16_t applicationID = message.readEnum16();
		uint16_t eventDefinitionID = message.readEnum16();
		bool canBeAdded = true;

		for (auto& element: eventActionDefinitionMap) {
			if (element.first == eventDefinitionID) {
				canBeAdded = false;
				if (element.second.enabled) {
					ErrorHandler::reportError(message, ErrorHandler::EventActionEnabledError);
				} else if (not element.second.enabled) {
					element.second.applicationID = applicationID;
					element.second.request = message.data + message.readPosition;
					EventActionDefinition temporaryEventActionDefinition(applicationID, eventDefinitionID, message);
					auto mapPosition = eventActionDefinitionMap.erase(eventActionDefinitionMap.find(eventDefinitionID));
					eventActionDefinitionMap.insert(mapPosition, std::make_pair(eventDefinitionID, temporaryEventActionDefinition));
				}
				break;
			}
		}
		if (canBeAdded) {
			if (eventActionDefinitionMap.size() == ECSSEventActionStructMapSize) {
				ErrorHandler::reportError(message, ErrorHandler::EventActionDefinitionsMapIsFull);
			} else {
				EventActionDefinition temporaryEventActionDefinition(applicationID, eventDefinitionID, message);
				eventActionDefinitionMap.insert(std::make_pair(eventDefinitionID, temporaryEventActionDefinition));
			}
		}
	}
}

void EventActionService::deleteEventActionDefinitions(Message& message) {
	// TC[19,2]
	message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction);
	uint8_t numberOfEventActionDefinitions = message.readUint8();
	while (numberOfEventActionDefinitions-- != 0) {
		uint16_t applicationID = message.readEnum16();
		uint16_t eventDefinitionID = message.readEnum16();
		bool actionDefinitionExists = false;

		for (auto& element: eventActionDefinitionMap) {
			if (element.first == eventDefinitionID) {
				actionDefinitionExists = true;
				if (element.second.applicationID != applicationID) {
					ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
				} else if (element.second.enabled) {
					ErrorHandler::reportError(message, ErrorHandler::EventActionDeleteEnabledDefinitionError);
				} else {
					eventActionDefinitionMap.erase(eventActionDefinitionMap.find(eventDefinitionID));
				}
				break;
			}
		}
		if (not actionDefinitionExists) {
			ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
		}
	}
}

void EventActionService::deleteAllEventActionDefinitions(Message& message) {
	// TC[19,3]
	message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::DeleteAllEventAction);

	setEventActionFunctionStatus(false);
	eventActionDefinitionMap.clear();
}

void EventActionService::enableEventActionDefinitions(Message& message) {
	// TC[19,4]
	message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction);
	uint8_t numberOfEventActionDefinitions = message.readUint8();
	if (numberOfEventActionDefinitions != 0U) {
		while (numberOfEventActionDefinitions-- != 0) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			bool actionDefinitionExists = false;

			for (auto& element: eventActionDefinitionMap) {
				if (element.first == eventDefinitionID) {
					actionDefinitionExists = true;
					if (element.second.applicationID != applicationID) {
						ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
					} else {
						element.second.enabled = true;
					}
					break;
				}
			}
			if (not actionDefinitionExists) {
				ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
			}
		}
	} else {
		for (auto& element: eventActionDefinitionMap) {
			element.second.enabled = true;
		}
	}
}

void EventActionService::disableEventActionDefinitions(Message& message) {
	// TC[19,5]
	message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction);
	uint8_t numberOfEventActionDefinitions = message.readUint8();
	if (numberOfEventActionDefinitions != 0U) {
		while (numberOfEventActionDefinitions-- != 0) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			bool actionDefinitionExists = false;

			for (auto& element: eventActionDefinitionMap) {
				if (element.first == eventDefinitionID) {
					actionDefinitionExists = true;
					if (element.second.applicationID != applicationID) {
						ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
					} else {
						element.second.enabled = false;
					}
					break;
				}
			}
			if (not actionDefinitionExists) {
				ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
			}
		}
	} else {
		for (auto& element: eventActionDefinitionMap) {
			element.second.enabled = false;
		}
	}
}

void EventActionService::requestEventActionDefinitionStatus(Message& message) {
	// TC[19,6]
	message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::ReportStatusOfEachEventAction);

	eventActionStatusReport();
}

void EventActionService::eventActionStatusReport() {
	// TM[19,7]
	Message report = createTM(EventActionStatusReport);
	uint16_t count = eventActionDefinitionMap.size();
	report.appendUint16(count);
	for (const auto& element: eventActionDefinitionMap) {
		report.appendEnum16(element.second.applicationID);
		report.appendEnum16(element.second.eventDefinitionID);
		report.appendBoolean(element.second.enabled);
	}
	storeMessage(report);
}

void EventActionService::enableEventActionFunction(Message& message) {
	// TC[19,8]
	message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::EnableEventActionFunction);

	setEventActionFunctionStatus(true);
}

void EventActionService::disableEventActionFunction(Message& message) {
	// TC[19,9]
	message.assertTC(EventActionService::ServiceType, EventActionService::MessageType::DisableEventActionFunction);

	setEventActionFunctionStatus(false);
}

void EventActionService::executeAction(uint16_t eventDefinitionID) {
	// Custom function
	if (eventActionFunctionStatus) {
		auto range = eventActionDefinitionMap.equal_range(eventDefinitionID);
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
		case EnableEventActionFunction:
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
