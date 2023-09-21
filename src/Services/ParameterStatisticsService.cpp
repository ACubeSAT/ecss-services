#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER
#include "ServicePool.hpp"
#include "Services/ParameterStatisticsService.hpp"

ParameterStatisticsService::ParameterStatisticsService() : evaluationStartTime(TimeGetter::getCurrentTimeDefaultCUC()) {
	initializeStatisticsMap();
	serviceType = ServiceType;
}

void ParameterStatisticsService::reportParameterStatistics(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportParameterStatistics)) {
		return;
	}
	parameterStatisticsReport();

	if (hasAutomaticStatisticsReset or request.readBoolean()) {
		resetParameterStatistics();
	}
}

void ParameterStatisticsService::reportParameterStatistics(bool reset) {
	parameterStatisticsReport();

	if (hasAutomaticStatisticsReset or reset) {
		resetParameterStatistics();
	}
}

void ParameterStatisticsService::parameterStatisticsReport() {
	Message report = createTM(ParameterStatisticsReport);
	report.append(evaluationStartTime);
	auto evaluationStopTime = TimeGetter::getCurrentTimeDefaultCUC();
	report.append(evaluationStopTime);

	uint16_t numOfValidParameters = 0;
	for (auto& currentStatistic: statisticsMap) {
		uint16_t numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		numOfValidParameters++;
	}
	report.appendUint16(numOfValidParameters);

	for (auto& currentStatistic: statisticsMap) {
		uint16_t currentId = currentStatistic.first;
		uint16_t numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		report.appendUint16(currentId);
		report.appendUint16(numOfSamples);
		currentStatistic.second.appendStatisticsToMessage(report);
	}
	storeMessage(report);
}

void ParameterStatisticsService::resetParameterStatistics(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ResetParameterStatistics)) {
		return;
	}
	resetParameterStatistics();
}

void ParameterStatisticsService::resetParameterStatistics() {
	for (auto& it: statisticsMap) {
		it.second.resetStatistics();
	}
	evaluationStartTime = TimeGetter::getCurrentTimeDefaultCUC();
}

void ParameterStatisticsService::enablePeriodicStatisticsReporting(Message& request) {
	constexpr uint16_t SamplingParameterInterval = 5;

	if (!request.assertTC(ServiceType, MessageType::EnablePeriodicParameterReporting)) {
		return;
	}

	uint16_t timeInterval = request.readUint16();

	if (timeInterval < SamplingParameterInterval) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
		return;
	}

	periodicStatisticsReportingStatus = true;
	reportingIntervalMs = timeInterval;
}

void ParameterStatisticsService::disablePeriodicStatisticsReporting(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DisablePeriodicParameterReporting)) {
		return;
	}

	periodicStatisticsReportingStatus = false;
	reportingIntervalMs = 0;
}

void ParameterStatisticsService::addOrUpdateStatisticsDefinitions(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::AddOrUpdateParameterStatisticsDefinitions)) {
		return;
	}

	uint16_t numOfIds = request.readUint16();
	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currentId = request.readUint16();
		if (!Services.parameterManagement.parameterExists(currentId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingParameter);
			if (supportsSamplingInterval) {
				request.skipBytes(2);
			}
			continue;
		}
		bool exists = statisticsMap.find(currentId) != statisticsMap.end();
		uint16_t interval = 0;
		if (supportsSamplingInterval) {
			interval = request.readUint16();
			if (interval < reportingIntervalMs) {
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
				continue;
			}
		}
		if (not exists) {
			if (statisticsMap.size() >= ECSSMaxStatisticParameters) {
				ErrorHandler::reportError(request,
				                          ErrorHandler::ExecutionStartErrorType::MaxStatisticDefinitionsReached);
				return;
			}
			Statistic newStatistic;
			if (supportsSamplingInterval) {
				newStatistic.setSelfSamplingInterval(interval);
			}
			statisticsMap.insert({currentId, newStatistic});
			// TODO: start the evaluation of statistics for this parameter.
		} else {
			if (supportsSamplingInterval) {
				statisticsMap.at(currentId).setSelfSamplingInterval(interval);
			}
			statisticsMap.at(currentId).resetStatistics();
		}
	}
}

void ParameterStatisticsService::deleteStatisticsDefinitions(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DeleteParameterStatisticsDefinitions)) {
		return;
	}

	uint16_t numOfIds = request.readUint16();
	if (numOfIds == 0) {
		statisticsMap.clear();
		periodicStatisticsReportingStatus = false;
		return;
	}
	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currentId = request.readUint16();
		if (!Services.parameterManagement.parameterExists(currentId)) {
			ErrorHandler::reportError(request, ErrorHandler::GetNonExistingParameter);
			continue;
		}
		statisticsMap.erase(currentId);
	}
	if (statisticsMap.empty()) {
		periodicStatisticsReportingStatus = false;
	}
}

void ParameterStatisticsService::reportStatisticsDefinitions(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportParameterStatisticsDefinitions)) {
		return;
	}
	statisticsDefinitionsReport();
}

void ParameterStatisticsService::statisticsDefinitionsReport() {
	Message definitionsReport = createTM(ParameterStatisticsDefinitionsReport);

	uint16_t currentReportingIntervalMs = 0;
	if (periodicStatisticsReportingStatus) {
		currentReportingIntervalMs = reportingIntervalMs;
	}
	definitionsReport.appendUint16(currentReportingIntervalMs);
	definitionsReport.appendUint16(statisticsMap.size());

	for (auto& currentParam: statisticsMap) {
		uint16_t currentId = currentParam.first;
		uint16_t samplingInterval = currentParam.second.selfSamplingInterval;
		definitionsReport.appendUint16(currentId);
		if (supportsSamplingInterval) {
			definitionsReport.appendUint16(samplingInterval);
		}
	}
	storeMessage(definitionsReport);
}

void ParameterStatisticsService::execute(Message& message) {
	DefaultTimestamp currentTime;
	switch (message.messageType) {
		case ReportParameterStatistics:
			reportParameterStatistics(message);
			break;
		case ResetParameterStatistics:
			resetParameterStatistics(message);
			currentTime = getCurrentTime();
			break;
		case EnablePeriodicParameterReporting:
			currentTime = getCurrentTime();
			enablePeriodicStatisticsReporting(message);
			break;
		case DisablePeriodicParameterReporting:
			disablePeriodicStatisticsReporting(message);
			break;
		case AddOrUpdateParameterStatisticsDefinitions:
			addOrUpdateStatisticsDefinitions(message);
			break;
		case DeleteParameterStatisticsDefinitions:
			deleteStatisticsDefinitions(message);
			break;
		case ReportParameterStatisticsDefinitions:
			currentTime = getCurrentTime();
			reportStatisticsDefinitions(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

TimeStamp<4, 0, 1, 10> ParameterStatisticsService::getCurrentTime() {
	return TimeGetter::getCurrentTimeDefaultCUC();
}

#endif
