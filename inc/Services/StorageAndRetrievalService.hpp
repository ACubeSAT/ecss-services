#ifndef ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP
#define ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "PacketStore.hpp"
#include "StorageAndRetrievalConfigurations/ApplicationProcessConfiguration.hpp"
#include "StorageAndRetrievalConfigurations/HousekeepingReportConfiguration.hpp"
#include "StorageAndRetrievalConfigurations/EventReportBlockingConfiguration.hpp"
#include "etl/map.h"

/**
 * Implementation of ST[15] Storage and Retrieval Service, as defined in ECSS-E-ST-70-41C.
 *
 * @brief
 * This Service:
 * 1) provides the capability to select reports generated by other services and store them into packet stores.
 * 2) allows the ground system to manage the reports in the packet stores and request their downlink.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class StorageAndRetrievalService : public Service {
public:
	inline static const uint8_t ServiceType = 15;

	enum MessageType : uint8_t {
		AddReportTypesToAppProcessConfiguration = 3,
		DeleteReportTypesFromAppProcessConfiguration = 4,
		ReportAppConfigurationContent = 5,
		AppConfigurationContentReport = 6,
		AddStructuresToHousekeepingConfiguration = 29,
		DeleteStructuresFromHousekeepingConfiguration = 30,
		DeleteEventDefinitionsFromEventReportConfiguration = 33,
		AddEventDefinitionsToEventReportConfiguration = 34,
		ReportHousekeepingConfigurationContent = 35,
		HousekeepingConfigurationContentReport = 36,
		ReportEventReportConfigurationContent = 39,
		EventReportConfigurationContentReport = 40
	};

	StorageAndRetrievalService();

	typedef String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreKey;
	etl::map<packetStoreKey, PacketStore, ECSS_MAX_PACKET_STORES> packetStores;

	/**
	 * Implementation of the Packet Selection Subservice of the ST[15] Storage and Retrieval Service.
	 *
	 * @brief
	 * This service provides the capability to control the storage of TM messages to the packet stores of the
	 * Storage and Retrieval Service. It contains definitions each one indicating whether a specific TM message
	 * should or should not be stored into the packet stores. For a specific TM message, if the packet selection
	 * includes a definition related to that message, it means that it can be stored into the packet stores.
	 */
	class PacketSelectionSubservice {
	private:
		StorageAndRetrievalService& mainService;

		/**
		 * Reads a packet store ID from a message.
		 */
		static String<ECSS_MAX_PACKET_STORE_ID_SIZE> readPacketStoreId(Message& request);

		/**
		 * Checks if the specified packet store exists in the packet selection sub-service.
		 */
		bool packetStoreExists(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId);

		/**
		 * Checks if the requested application process id is controlled by the packet selection subservice.
		 */
		bool appIsControlled(uint16_t applicationId, Message& request);

		/**
		 * Checks if the maximum number of report type definitions are reached.
		 */
		bool exceedsMaxReportDefinitions(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                 uint16_t applicationId, uint16_t serviceId, Message& request);

		/**
		 * Checks if the maximum number of service type definitions is reached.
		 */
		bool exceedsMaxServiceDefinitions(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId,
		                                  Message& request);

		/**
		 * Checks if there are no report definitions inside a service type definition, so it decides whether to add a
		 * new report type definition.
		 */
		bool noReportDefinitionInService(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId,
		                                 uint16_t serviceId, Message& request);

		/**
		 * Checks if there are no service type definitions inside an application definition.
		 */
		bool noServiceDefinitionInApplication(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                      uint16_t applicationId, Message& request);

		/**
		 * Adds all the report types of an application process, for a specified packet store.
		 */
		void addAllReportDefinitionsOfApplication(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                          uint16_t applicationId);

		/**
		 * Adds all the report types of a service type, for a specified packet store and application process.
		 */
		void addAllReportDefinitionsOfService(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                      uint16_t applicationId, uint16_t serviceId);

		/**
		 * Performs the necessary error checking for a specific application and decides whether the instruction to
		 * add a report type is valid or not.
		 */
		bool checkApplicationForReportTypes(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                    uint16_t applicationId, Message& request);

		/**
		 * Performs the necessary error checking for a specific service type and decides whether the instruction to
		 * add a report type in the application process configuration is valid or not.
		 */
		bool checkService(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId,
		                  uint16_t serviceId, Message& request);

		/**
		 * Creates a report type definition and adds it to the specified service definition.
		 */
		void addReportDefinition(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId,
		                         uint16_t serviceId, uint16_t reportId);

		/**
		 * checks if the requested report type already exists in the service type, to decide whether to add it or not.
		 * Returns the position of the 'reportId' inside the vector if it exists, and -1 if not.
		 */
		int reportExistsInService(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId,
		                          uint16_t serviceId, uint16_t reportId);

		/**
		 * Checks if the requested application ID already exists in the application process configuration, to decide
		 * whether to add it or not.
		 */
		bool appExistsInApplicationConfiguration(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                         uint16_t applicationId);

		/**
		 * Checks if the requested service type already exists in the application, to decide whether to add it or not.
		 */
		bool serviceExistsInApp(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId,
		                        uint16_t serviceId);

		/**
		 * Deletes either specified, or all report type definitions of a specified service type definition.
		 */
		void deleteReportDefinitionsOfService(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                      uint16_t applicationId, uint16_t serviceId, int index);

		/**
		 * Deletes all service type definitions of a specified application process
		 */
		void deleteServiceDefinitionsOfApp(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                   uint16_t applicationId, bool deleteAll, uint16_t serviceId);

		/**
		 * Performs the necessary error checking for a specific application and decides whether the request to add a
		 * new housekeeping structure ID in the housekeeping configuration is valid.
		 */
		bool checkApplicationForHousekeeping(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                     uint16_t applicationId, Message& request);

		/**
		 * Checks if the maximum number of housekeeping structure IDs is reached.
		 */
		bool exceedsMaxHousekeepingStructures(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                      uint16_t applicationId, Message& request);

		/**
		 * Checks if there are no housekeeping structure IDs inside an application.
		 */
		bool noStructureIdsInApplication(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                 uint16_t applicationId, Message& request);

		/**
		 * Adds a new housekeeping structure ID to the specified application. This version takes into consideration
		 * the subsampling rate and adds it to the related vector.
		 */
		void addHousekeepingStructureId(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                uint16_t applicationId, uint8_t structureId, uint16_t subsamplingRate);

		/**
		 * Adds a new housekeeping structure ID to the specified application. This version does not take into
		 * consideration the subsampling rate.
		 */
		void addHousekeepingStructureId(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                uint16_t applicationId, uint8_t structureId);

		/**
		 * Adds all the housekeeping structure IDs, for a specified packet store and a specified application.
		 */
		void addAllHousekeepingStructuresOfApplication(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                               uint16_t applicationId);

		/**
		 * Checks if the requested application ID already exists in the housekeeping configuration, to decide whether
		 * to add it or not.
		 */
		bool appExistsInHousekeepingConfiguration(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                          uint16_t applicationId);

		/**
		 * Checks if the specified housekeeping structure ID exists in the specified application process.
		 */
		int structureIdExistsInApplication(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                   uint16_t applicationId, uint8_t structureId);

		/**
		 * Deletes a housekeeping structure ID from the housekeeping configuration of the packet selection subservice.
		 */
		void deleteHousekeepingStructure(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                 uint16_t applicationId, uint16_t index);

		/**
		 * Performs the necessary error checking for a specific application and decides whether the request to add a
		 * new event report definition in the event report configuration is valid.
		 */
		bool checkApplicationForEventReports(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                     uint16_t applicationId, Message& request);

		/**
		 * Checks if the maximum number of event definitions for a specific application is reached.
		 */
		bool exceedsMaxEventDefinitions(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                uint16_t applicationId, Message& request);

		/**
		 * Checks if there are no event definition IDs for a specific application process.
		 */
		bool noEventDefinitionIdsInApplication(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                       uint16_t applicationId, Message& request);

		/**
		 * Adds a new event definition ID into a specified application process.
		 */
		void addEventDefinitionId(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId,
		                          uint16_t eventDefinitionId);

		/**
		 * Adds all the event definitions of a specified application process, to the event report configuration.
		 */
		void addAllEventDefinitionsOfApplication(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                         uint16_t applicationId);

		/**
		 * Checks if the specified application ID exists in the event report configuration.
		 */
		bool appExistsInEventReportConfiguration(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                         uint16_t applicationId);

		/**
		 * Checks if an event definition ID exists in the specified application process.
		 */
		int eventDefinitionIdExistsInApplication(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId,
		                                         uint16_t applicationId, uint16_t eventDefinitionId);

		/**
		 * Deletes an event definition ID from the specified application process.
		 */
		void deleteEventDefinition(const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId,
		                           uint16_t index);

	public:
		/**
		 * Constructor of the packet selection sub-service.
		 */
		explicit PacketSelectionSubservice(StorageAndRetrievalService& parent, uint16_t numOfControlledAppProcs,
		                                   uint16_t maxEventDefIds, uint16_t maxHousekeepingStructIds,
		                                   uint16_t maxReportTypeDefs, uint16_t maxServiceTypeDefs);

		/**
		 * Vector containing the IDs of the application processes controlled by the packet selection subservice.
		 */
		etl::vector<uint16_t, ECSS_MAX_CONTROLLED_APPLICATION_PROCESSES> controlledApplications;

		/**
		 * Support for storage of housekeeping reports, as per 5.15.4.2.1.a
		 */
		const bool supportsStorageControlOfHousekeepingReports = true;

		/**
		 * Support for storage of event reports, as per 5.15.4.2.1.c
		 */
		const bool supportsStorageControlOfEventReports = true;

		/**
		 * Support to subsample the storage of housekeeping reports as per 5.15.4.2.1.d
		 */
		const bool supportsSubsamplingRate;
		const uint16_t numOfControlledAppProcesses;
		const uint16_t maxServiceTypeDefinitions; // Per Application Process Definition
		const uint16_t maxReportTypeDefinitions; // This is per Service Type Definition
		const uint8_t maxHousekeepingStructureIds; // Per Housekeeping storage-control definition
		const uint16_t maxEventDefinitionIds; // Per Event-Report storage-control definition

		/**
		 * Contains the definitions that the packet selection subservice holds, regarding TM packets coming from
		 * application processes.
		 */
		ApplicationProcessConfiguration applicationProcessConfiguration;
		/**
		 * Contains the definitions that the packet selection subservice holds, regarding TM packets coming from
		 * housekeeping reports.
		 */
		HousekeepingReportConfiguration housekeepingReportConfiguration;
		/**
		 * Contains the definitions that the packet selection subservice holds, regarding TM packets coming from
		 * events.
		 */
		EventReportBlockingConfiguration eventReportConfiguration;

		/**
		 * TC[15,3] 'add report types to an application process storage control configuration'.
		 */
		void addReportTypesToAppProcessConfiguration(Message& request);

		/**
		 * TC[15,4] 'delete report types from an application process storage control configuration'
		 */
		void deleteReportTypesFromAppProcessConfiguration(Message& request);

		/**
		 * This function takes a TC[15,5] 'report the application process storage control configuration content' request
		 * as argument, and performs the necessary error checking.
		 */
		void reportAppConfigurationContent(Message& request);

		/**
		 * Creates and stores a TM[15,6] 'application process storage control configuration content report'.
		 */
		void appConfigurationContentReport(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId);

		/**
		 * TC[15,29] 'add structure identifiers to the housekeeping parameter report storage control configuration'.
		 */
		void addStructuresToHousekeepingConfiguration(Message& request);

		/**
		 * TC[15,30] 'delete structure identifiers from the housekeeping parameter report storage control
		 * configuration'.
		 */
		void deleteStructuresFromHousekeepingConfiguration(Message& request);

		/**
		 * This function takes a TC[15,35] 'report the housekeeping parameter report storage control configuration
		 * content' request as argument, and performs the necessary error checking.
		 */
		void reportHousekeepingConfigurationContent(Message& request);

		/**
		 * Creates and stores a TM[15,36] 'housekeeping parameter report storage control configuration content
		 * report'.
		 */
		void housekeepingConfigurationContentReport(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId);

		/**
		 * TC[15,34] 'add event definition identifiers to the event report blocking storage-control configuration'.
		 */
		void addEventDefinitionsToEventReportConfiguration(Message& request);

		/**
		 * TC[15,33] 'delete event definition identifiers from the event report blocking storage-control configuration'.
		 */
		void deleteEventDefinitionsFromEventReportConfiguration(Message& request);

		/**
		 * This function takes a TC[15,39] 'report the event report blocking storage control configuration
		 * content' request as argument, and performs the necessary error checking.
		 */
		void reportEventReportConfigurationContent(Message& request);

		/**
		 * Creates and stores a TM[15,40] 'event report blocking configuration content report'.
		 */
		void eventReportConfigurationContentReport(String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId);

	} packetSelectionSubservice;

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param param Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& request);
};

#endif