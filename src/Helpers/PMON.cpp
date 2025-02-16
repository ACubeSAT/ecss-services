#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"
#include "Services/EventReportService.hpp"

PMON::PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber, CheckType checkType)
    : monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber),
      checkType(checkType),
      monitoredParameter(*Services.parameterManagement.getParameter(monitoredParameterId)),
      	newTrackedCheckingStatus(Unchecked) {
	if (!Services.parameterManagement.getParameter(monitoredParameterId)) {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}

void PMON::updateAfterCheck(const CheckingStatus newCheckingStatus) {
	if (newTrackedCheckingStatus == newCheckingStatus) {
		repetitionCounter++;
	} else {
		repetitionCounter = 1;
		newTrackedCheckingStatus = newCheckingStatus;
	}

	if (repetitionCounter < repetitionNumber) {
		return;
	}
	if (currentCheckingStatus == newTrackedCheckingStatus) {
		return;
	}
	const etl::pair<CheckingStatus, CheckingStatus> transition = etl::make_pair(currentCheckingStatus, newTrackedCheckingStatus);
	checkTransitions.push_back(transition);
	currentCheckingStatus = newTrackedCheckingStatus;

    if (pmonTransitionEventMap.find(transition) == pmonTransitionEventMap.end()) {
      return;
    }

    EventDefinitionId eventID = pmonTransitionEventMap[transition];

	if (eventID - 1 >= Services.eventReport.eventDefinitionSeverityArray.size()) {
		return;
	}

	Services.eventReport.raiseTransitionEvent(monitoredParameterId, transition, eventID);
}
