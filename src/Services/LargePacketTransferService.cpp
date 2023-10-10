#include "ECSS_Configuration.hpp"
#ifdef SERVICE_LARGEPACKET

#include <Services/LargePacketTransferService.hpp>
#include <etl/String.hpp>
#include "Message.hpp"

void LargePacketTransferService::firstDownlinkPartReport(LargeMessageTransactionId largeMessageTransactionIdentifier,
                                                         PartSequenceNum partSequenceNumber,
                                                         const String<ECSSMaxFixedOctetStringSize>& string) {
	Message report = createTM(LargePacketTransferService::MessageType::FirstDownlinkPartReport);
	report.append<LargeMessageTransactionId>(largeMessageTransactionIdentifier); // large message transaction identifier
	report.append<PartSequenceNum>(partSequenceNumber);                // part sequence number
	report.appendOctetString(string);                       // fixed octet-string
	storeMessage(report);
}

void LargePacketTransferService::intermediateDownlinkPartReport(
    LargeMessageTransactionId largeMessageTransactionIdentifier, PartSequenceNum partSequenceNumber,
    const String<ECSSMaxFixedOctetStringSize>& string) {

	Message report = createTM(LargePacketTransferService::MessageType::InternalDownlinkPartReport);
	report.append<LargeMessageTransactionId>(largeMessageTransactionIdentifier); // large message transaction identifier
	report.append<PartSequenceNum>(partSequenceNumber);                // part sequence number
	report.appendOctetString(string);                       // fixed octet-string
	storeMessage(report);
}

void LargePacketTransferService::lastDownlinkPartReport(LargeMessageTransactionId largeMessageTransactionIdentifier,
                                                        PartSequenceNum partSequenceNumber,
                                                        const String<ECSSMaxFixedOctetStringSize>& string) {
	Message report = createTM(LargePacketTransferService::MessageType::LastDownlinkPartReport);
	report.append<LargeMessageTransactionId>(largeMessageTransactionIdentifier); // large message transaction identifier
	report.append<PartSequenceNum>(partSequenceNumber);                // part sequence number
	report.appendOctetString(string);                       // fixed octet-string
	storeMessage(report);
}

String<ECSSMaxFixedOctetStringSize>
LargePacketTransferService::firstUplinkPart(const String<ECSSMaxFixedOctetStringSize>& string) {
	return string;
}

String<ECSSMaxFixedOctetStringSize>
LargePacketTransferService::intermediateUplinkPart(const String<ECSSMaxFixedOctetStringSize>& string) {
	return string;
}

String<ECSSMaxFixedOctetStringSize>
LargePacketTransferService::lastUplinkPart(const String<ECSSMaxFixedOctetStringSize>& string) {
	return string;
}

void LargePacketTransferService::split(Message& message, LargeMessageTransactionId largeMessageTransactionIdentifier) {
	//TODO: Should this be uint32?
	uint16_t size = message.dataSize;
	uint16_t positionCounter = 0;
	uint16_t parts = (size / ECSSMaxFixedOctetStringSize) + 1;
	String<ECSSMaxFixedOctetStringSize> stringPart("");
	uint8_t dataPart[ECSSMaxFixedOctetStringSize];

	for (uint16_t i = 0; i < ECSSMaxFixedOctetStringSize; i++) {
		dataPart[i] = message.data[positionCounter];
		positionCounter++;
	}
	stringPart = dataPart;
	firstDownlinkPartReport(largeMessageTransactionIdentifier, 0, stringPart);

	for (uint16_t part = 1; part < (parts - 1U); part++) {
		for (uint16_t i = 0; i < ECSSMaxFixedOctetStringSize; i++) {
			dataPart[i] = message.data[positionCounter];
			positionCounter++;
		}
		stringPart = dataPart;
		intermediateDownlinkPartReport(largeMessageTransactionIdentifier, part, stringPart);
	}

	for (uint16_t i = 0; i < ECSSMaxFixedOctetStringSize; i++) {
		if (message.dataSize == positionCounter) {
			dataPart[i] = 0; // To prevent from filling the rest of the String with garbage info
		}
		dataPart[i] = message.data[positionCounter];
		positionCounter++;
	}
	stringPart = dataPart;
	lastDownlinkPartReport(largeMessageTransactionIdentifier, (parts - 1U), stringPart);
}

#endif
