#ifndef ECSS_SERVICES_PARAMETER_HPP
#define ECSS_SERVICES_PARAMETER_HPP

#include "etl/String.hpp"
#include "Message.hpp"
#include "ECSS_Definitions.hpp"

/**
 * Implementation of a Parameter field, as specified in ECSS-E-ST-70-41C.
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 * @author Athanasios Theocharis <athatheoc@gmail.com>
 *
 * @section Introduction
 * The Parameter class implements a way of storing and updating system parameters
 * of arbitrary size and type, while avoiding std::any and dynamic memory allocation.
 * It is split in two distinct parts:
 * 1) an abstract \ref ParameterBase class which provides a
 * common data type used to create any pointers to \ref Parameter objects, as well as
 * virtual functions for accessing the parameter's data part, and
 * 2) a templated \ref Parameter used to store any type-specific parameter information,
 * such as the actual data field where the parameter's value will be stored and any pointers
 * to suitable functions that will be responsible for updating the parameter's value.
 *
 * @section Architecture Rationale
 * The ST[20] Parameter service is implemented with the need of arbitrary type storage
 * in mind, while avoiding any use of dynamic memory allocation, a requirement for use
 * in embedded systems. Since lack of Dynamic Memory Access precludes usage of stl::any
 * and the need for truly arbitrary (even for template-based objects like etl::string) type storage
 * would exclude from consideration constructs like etl::variant due to limitations on
 * the number of supported distinct types, a custom solution was needed.
 * Furthermore, the \ref ParameterService should provide ID-based access to parameters.
 */
class ParameterBase {
public:
	virtual void appendValueToMessage(Message& message) = 0;
	virtual void setValueFromMessage(Message& message) = 0;
};

/**
 * Implementation of a parameter containing its value. See \ref ParameterBase for more information.
 * @tparam DataType
 */
template <typename DataType>
class Parameter : public ParameterBase {
private:
	DataType currentValue;

public:
	Parameter(DataType initialValue) {
		currentValue = initialValue;
	}

	inline void setValue(DataType value) {
		currentValue = value;
	}

	DataType getValue() {
		return currentValue;
	}

	inline void setValueFromMessage(Message& message) override;

	inline void appendValueToMessage(Message& message) override;
};

template<> inline void Parameter<uint8_t>::setValueFromMessage(Message& message) {
	this->currentValue = message.readUint8();
}
template<> inline void Parameter<uint16_t>::setValueFromMessage(Message& message) {
	this->currentValue = message.readUint16();
}

template<> inline void Parameter<uint32_t>::setValueFromMessage(Message& message) {
	this->currentValue = message.readUint32();
}

template<> inline void Parameter<uint8_t>::appendValueToMessage(Message& message) {
	message.appendUint8(this->currentValue);
}

template<> inline void Parameter<uint16_t>::appendValueToMessage(Message& message) {
	message.appendUint16(this->currentValue);
}

template<> inline void Parameter<uint32_t>::appendValueToMessage(Message& message) {
	message.appendUint32(this->currentValue);
}
#endif //ECSS_SERVICES_PARAMETER_HPP
