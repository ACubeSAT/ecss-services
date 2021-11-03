#ifndef ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP
#define ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "PacketStore.hpp"
#include "etl/map.h"

/**
 * Implementation of ST[15] Storage and Retrieval Service, as defined in ECSS-E-ST-70-41C.
 *
 * @brief This Service:
 * 1) provides the capability to select reports generated by other services and store them into packet stores.
 * 2) allows the ground system to manage the reports in the packet stores and request their downlink.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class StorageAndRetrievalService : public Service {
public:

	inline static const uint8_t ServiceType = 15;

	enum MessageType : uint8_t {
		EnableStorageFunction = 1,
		DisableStorageFunction = 2,
		StartByTimeRangeRetrieval = 9,
		DeletePacketStoreContent = 11,
		ChangeOpenRetrievalStartTimeTag = 14,
		ResumeOpenRetrievalOfPacketStores = 15,
		SuspendOpenRetrievalOfPacketStores = 16,
		AbortByTimeRangeRetrieval = 17,
		ReportStatusOfPacketStores = 18,
		PacketStoresStatusReport = 19,
		CreatePacketStores = 20,
		DeletePacketStores = 21,
		ReportConfigurationOfPacketStores = 22,
		PacketStoreConfigurationReport = 23,
		CopyPacketsInTimeWindow = 24,
		ResizePacketStores = 25,
		ChangeTypeToCircular = 26,
		ChangeTypeToBounded = 27,

	};

	enum VirtualChannels : uint8_t {

	};

	enum TimeStamping {
	    StorageBased = 0,
		PacketBased = 1
	};

	static const uint16_t maxPacketStores = 20;

	etl::map <uint16_t, PacketStore, maxPacketStores> packetStores;

	const bool supportsCircularType = true;
	const bool supportsBoundedType = true;

	/**
	* @brief Support for the capability to handle multiple retrieval requests in parallel as per 6.15.3.1(i)
 	*/
	const bool supportsConcurrentRetrievalRequests = false;

	/**
	* @brief Support for the capability to queue requests pending their execution as per 6.15.3.1(k)
	*/
	const bool supportsQueuingRetrievalRequests = true;

	/**
	* @brief Support for the capability to prioritize packet retrieval as per 6.15.3.1(m)
	*/
	const bool supportsPrioritizingRetrievals = true;
	const TimeStamping timeStamping = PacketBased;

	/**
	* TC[15,1] request to enable the packet stores' storage function
	*/
	void enableStorageFunction(Message& request);

	/**
	* TC[15,2] request to disable the packet stores' storage function
	*/
	void disableStorageFunction(Message& request);

	/**
	* TC[15,14] change the open retrieval start time tag
	*/
	void changeOpenRetrievalStartTimeTag(Message& request);

	/**
	* TC[15,15] resume the open retrieval of packet stores
	*/
    void resumeOpenRetrievalOfPacketStores(Message& request);

  	/**
	* TC[15,16] suspend the open retrieval of packet stores
	*/
    void suspendOpenRetrievalOfPacketStores(Message& request);

	/**
	* TC[15,9] start the by-time-range retrieval of packet stores
	*/
	void startByTimeRangeRetrieval(Message& request);

	/**
	* TC[15,17] abort the by-time-range retrieval of packet stores
	*/
	void abortByTimeRangeRetrieval(Message& request);

	/**
	* This function takes a TC[15,18] 'report the status of packet stores' request as argument and responds with a
	* TM[15,19] 'packet stores status' report message.
	*/
	void packetStoresStatusReport(Message& request);

	/**
	* TC[15,11] delete the packet store content up to the specified time
	*/
	void deletePacketStoreContent(Message& request);

	/**
	* TC[15,20] create packet stores
	*/
	void createPacketStores(Message& request);

	/**
	 * TC[15,21] delete packet stores
	 */
	void deletePacketStores(Message& request);

	/**
	 * This function takes a TC[15,22] 'report the packet store configuration' as argument and responds with a TM[15,
	 * 23] 'packet store configuration report' report message.
	 */
	void packetStoreConfigurationReport(Message& request);

	/**
	 * TC[15,24] copy the packets contained into a packet store, selected by the time window
	 */
	void copyPacketsInTimeWindow(Message& request);

	/**
	 * TC[15,25] resize packet stores
	 */
	void resizePacketStores(Message& request);

	/**
	 * TC[15,26] change the packet store type to circular
	 */
	void changeTypeToCircular(Message& request);

	/**
	 * TC[15,27] change the packet store type to bounded
	 */
	void changeTypeToBounded(Message& request);
};

#endif