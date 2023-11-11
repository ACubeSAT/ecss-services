#include "Helpers/PMONBase.hpp"
#include "ServicePool.hpp"


PMONBase::PMONBase(PMONId monitoredParameterId, uint16_t repetitionNumber)
    : monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber)
{
	auto paramOpt = Services.parameterManagement.getParameter(monitoredParameterId);
	if (paramOpt.has_value()) {
		monitoredParameter = paramOpt->get();
	} else {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}