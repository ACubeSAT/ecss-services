#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER
//#include "Statistics/SystemStatistics.hpp"
#include "Services/ParameterStatisticsService.hpp"

bool supportsStandardDeviation = false;

void ParameterStatisticsService :: reportParameterStatistics(Message& resetFlag) {

	Message statisticsReport(ParameterStatisticsService::ServiceType,
	                        ParameterStatisticsService::MessageType::ParameterStatisticsReport, Message::TM, 1);

	ErrorHandler::assertRequest(resetFlag.packetType == Message::TC, resetFlag,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.messageType ==ParameterStatisticsService::MessageType::ReportParameterStatistics,
	                            resetFlag, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.serviceType == ParameterStatisticsService::ServiceType, resetFlag,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	bool resetFlagValue = resetFlag.readBoolean();
	uint16_t numOfParameters = systemParameters.parametersArray.size();

	// Here is the end time
	// append start time
	// append end time

	statisticsReport.appendUint16(numOfParameters);

	for (uint16_t i = 0; i < numOfParameters; i++) {

	    uint16_t currId = i;
		std::reference_wrapper <StatisticBase> currentStatistic = systemStatistics.statisticsArray[currId].get();
	    uint16_t numOfSamples = currentStatistic.get().numOfSamplesCounter;

		if (numOfSamples == 0) {
			continue;
		}
		statisticsReport.appendUint16(currId);
		statisticsReport.appendUint16(numOfSamples);

		currentStatistic.get().appendStatisticsToMessage(currentStatistic, statisticsReport); // WORKS! MAGIC!
	}

	storeMessage(statisticsReport);

	if (resetFlagValue or ParameterStatisticsService :: hasAutomaticStatisticsReset) {
		resetParameterStatistics();
	}
	// Here add start time

}

void ParameterStatisticsService :: resetParameterStatistics() {

	//TODO: Stop the evaluation of parameter statistics
	uint16_t numOfParameters = systemParameters.parametersArray.size();
	for(int i = 0; i < numOfParameters; i++) {

		std::reference_wrapper <StatisticBase> currentStatistic = systemStatistics.statisticsArray[i].get();
		currentStatistic.get().clearStatisticSamples();
	}
	//TODO: Restart the evaluation of parameter statistics
}

void ParameterStatisticsService :: enablePeriodicStatisticsReporting(Message& request) {

	// Dummy value
	uint16_t SAMPLING_PARAMETER_INTERVAL = 5; //The sampling interval of each parameter, "timeInterval" requested should
	                                          //not exceed it. It has to be defined as a constant.

	uint16_t timeInterval = request.readUint16();

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == ParameterStatisticsService::MessageType::EnablePeriodicParameterReporting,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ParameterStatisticsService::ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	// Added error handler to check if the time interval asked is not a valid number.
	ErrorHandler::assertRequest(timeInterval >= SAMPLING_PARAMETER_INTERVAL, request,
	                            ErrorHandler::ExecutionStartErrorType::InvalidReportingRateError);

	ParameterStatisticsService :: periodicStatisticsReportingStatus = true; //Periodic reporting status changes to enabled
	ParameterStatisticsService :: periodicStatisticsReportingInterval = timeInterval;

	uint16_t numOfParameters = systemParameters.parametersArray.size();

	//Only generate ONE parameter statistics report after every interval passes.
	while (ParameterStatisticsService :: periodicStatisticsReportingStatus) {

		for (int i = 0; i < numOfParameters; i++) {

			Message statisticsReport(ParameterStatisticsService::ServiceType,
			                        ParameterStatisticsService::MessageType::ParameterStatisticsReport, Message::TM, 1);
			/*
			 * TODO:
			 *      1. append start time to parameterReport
			 *      2. append end time
			 */

			std::reference_wrapper <StatisticBase> currentStatistic = systemStatistics.statisticsArray[i].get();
			uint16_t numOfSamples = currentStatistic.get().numOfSamplesCounter;

			if(numOfSamples == 0) {
				continue;
			}
			statisticsReport.appendUint16(numOfParameters);  // step 3
			statisticsReport.appendUint16(i);

			currentStatistic.get().appendStatisticsToMessage(currentStatistic, statisticsReport);

			/*
			 * TODO: put the message into a queue and continue constructing the next report, and when
			 *      it's ready put that in the queue as well, another FreeRTOS task will be accountable of
			 *      keeping track of time, and when the interval passes, it's gonna pop the next
			 *      reportMessage from the queue and report it.
			 */
		}

		//TODO: systematically reset the parameters' statistics.

	}
}

void ParameterStatisticsService :: disablePeriodicStatisticsReporting(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == ParameterStatisticsService::MessageType::DisablePeriodicParameterReporting,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ParameterStatisticsService::ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	ParameterStatisticsService :: periodicStatisticsReportingStatus = false;
	ParameterStatisticsService :: periodicStatisticsReportingInterval = 0;
}

void ParameterStatisticsService :: addOrUpdateStatisticsDefinitions(Message& paramIds) {

	ErrorHandler::assertRequest(paramIds.packetType == Message::TC, paramIds,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.messageType == ParameterStatisticsService::MessageType::AddOrUpdateParameterStatisticsDefinitions,
	                            paramIds, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.serviceType == ParameterStatisticsService::ServiceType, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = paramIds.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {

		uint16_t currentId = paramIds.readUint16();

		if (currentId < systemParameters.parametersArray.size()) {
			ErrorHandler::assertRequest(ParameterStatisticsService::numOfStatisticsDefinitions < MAX_NUM_OF_DEFINITIONS, paramIds,
			                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

			// If there are intervals, get the value and check if it exceeds the sampling rate of the parameter.
			if (paramIds.hasTimeIntervals) {

				uint16_t interval = paramIds.readUint16();
				ErrorHandler::assertRequest(interval <=ParameterStatisticsService::periodicStatisticsReportingInterval, paramIds,
				                            ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
				// Get the sampling interval of the current parameter from the statistics vector
				std::reference_wrapper <StatisticBase> currentStatistic = systemStatistics.statisticsArray[currentId].get();
				uint16_t paramSamplingInterval = currentStatistic.get().selfSamplingInterval;

				if (paramSamplingInterval == 0) {

					systemStatistics.statisticsArray[currentId].get().setSelfTimeInterval(interval) ;
					systemParameters.parametersArray.at(currentId).get().setParameterIsActive(true);
					ParameterStatisticsService::nonDefinedStatistics--;
					//TODO: start the evaluation of statistics for this parameter. //add boolean value on statistic
					// that says if evaluation is enabled
				}
				else {
					systemStatistics.statisticsArray[currentId].get().setSelfTimeInterval(interval);
					// Statistics evaluation reset
					systemStatistics.statisticsArray[currentId].get().clearStatisticSamples();
				}
			}
		} else {
			ErrorHandler::reportError(paramIds, ErrorHandler::GetNonExistingParameter);
		}
	}
}

void ParameterStatisticsService :: deleteStatisticsDefinitions(Message& paramIds) {

	ErrorHandler::assertRequest(paramIds.packetType == Message::TC, paramIds,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.messageType == ParameterStatisticsService::MessageType::DeleteParameterStatisticsDefinitions,
	                            paramIds, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.serviceType == ParameterStatisticsService::ServiceType, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = paramIds.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {

		uint16_t currentId = paramIds.readUint16();
		if (currentId < systemParameters.parametersArray.size()) {

			systemStatistics.statisticsArray.at(currentId).get().setSelfTimeInterval(0);
			systemParameters.parametersArray.at(currentId).get().setParameterIsActive(false);
			ParameterStatisticsService::nonDefinedStatistics++;

		} else {
			ErrorHandler::reportError(paramIds, ErrorHandler::GetNonExistingParameter);
		}
	}
	// If list of definitions is empty, stop the periodic reporting.
	if (ParameterStatisticsService::nonDefinedStatistics == systemParameters.parametersArray.size()) {
		ParameterStatisticsService::periodicStatisticsReportingStatus = false;
	}
}

void ParameterStatisticsService :: deleteAllStatisticsDefinitions() {

	uint16_t numOfIds = systemParameters.parametersArray.size();
	for (uint16_t i = 0; i < numOfIds; i++) {
		systemStatistics.statisticsArray.at(i).get().setSelfTimeInterval(0);
		systemParameters.parametersArray.at(i).get().setParameterIsActive(false);
	}
	ParameterStatisticsService::nonDefinedStatistics = systemParameters.parametersArray.size();
	// Stop the periodic reporting because there are no defined parameters.
	ParameterStatisticsService::periodicStatisticsReportingStatus = false;
}

void ParameterStatisticsService :: reportStatisticsDefinitions(Message& request) {
	Message definitionsReport(ParameterStatisticsService::ServiceType,
	                          ParameterStatisticsService::MessageType::ParameterStatisticsDefinitionsReport,
	                          Message::TM, 1);

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType ==
	                                ParameterStatisticsService::MessageType::ReportParameterStatisticsDefinitions,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ParameterStatisticsService::ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t reportingInterval = 0;
	if (ParameterStatisticsService ::periodicStatisticsReportingStatus) {
		reportingInterval = ParameterStatisticsService ::periodicStatisticsReportingInterval;
	}

	uint16_t numOfParameters = systemParameters.parametersArray.size();
	definitionsReport.appendUint16(reportingInterval); // Append interval

	uint16_t numOfDefinedParameters = 0;
	for (int i = 0; i < numOfParameters; i++) {
		uint16_t currentId = i;
		uint16_t currentSamplingInterval = systemStatistics.statisticsArray.at(currentId).get().selfSamplingInterval;

		if (currentSamplingInterval != 0) {
			numOfDefinedParameters++;
		}
	}

	definitionsReport.appendUint16(numOfDefinedParameters);

	for (int i = 0; i < numOfParameters; i++) {
		uint16_t currentId = i;
		uint16_t samplingInterval = systemStatistics.statisticsArray.at(currentId).get().selfSamplingInterval;

		if (samplingInterval != 0) {
			definitionsReport.appendUint16(currentId);
			definitionsReport.appendUint16(samplingInterval);
		}
	}

	storeMessage(definitionsReport);

}

#endif