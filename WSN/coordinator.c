#include <zdo.h>
#include <app.h>
#include <zcl.h>
#include <leds.h>
#include <sysTaskManager.h>
#include <usartManager.h>
#include <zclOnOffCluster.h>
#include <zclIlluminanceMeasurementCluster.h>
#include <zclTemperatureMeasurementCluster.h>


// Endpunkte fuer Kommunikation mit 3 Clients:
//Temperaturmesswerte, Lichtmesswerte, OnOffLicht, OnOffStatus, OnOffModusLicht, OnOffModusKlima
static ZCL_DeviceEndpoint_t endPointTemperatureMeasurementClient1;
static ZCL_DeviceEndpoint_t endPointTemperatureMeasurementClient2;
static ZCL_DeviceEndpoint_t endPointTemperatureMeasurementClient3;
static ZCL_DeviceEndpoint_t endPointOnOffLightClient1;
static ZCL_DeviceEndpoint_t endPointOnOffLightClient2;
static ZCL_DeviceEndpoint_t endPointOnOffLightClient3;
static ZCL_DeviceEndpoint_t endPointIlluminanceClient1;
static ZCL_DeviceEndpoint_t endPointIlluminanceClient2;
static ZCL_DeviceEndpoint_t endPointIlluminanceClient3;
static ZCL_DeviceEndpoint_t endPointOnOffStatusClient1;
static ZCL_DeviceEndpoint_t endPointOnOffStatusClient2;
static ZCL_DeviceEndpoint_t endPointOnOffStatusClient3;
static ZCL_DeviceEndpoint_t endPointOnOffMode_climateClient1;
static ZCL_DeviceEndpoint_t endPointOnOffMode_climateClient2;
static ZCL_DeviceEndpoint_t endPointOnOffMode_climateClient3;
static ZCL_DeviceEndpoint_t endPointOnOffMode_lightClient1;
static ZCL_DeviceEndpoint_t endPointOnOffMode_lightClient2;
static ZCL_DeviceEndpoint_t endPointOnOffMode_lightClient3;

// Werte von Modulen für Ausgabe speichern 
static Module module1;
static Module module2;
static Module module3;

// Temporaere Werte beim Datenempfang von der App, welche dann an das entsprechende Modul weitergeleitet werden
static Module tempModule;

// Timer fuer periodische Ausgabe an die USART Bruecke
static HAL_AppTimer_t ausgabeTimer;

// Programm Status
static AppState_t appstate = INIT;

// Vorlage für Ausgabe an Bruecke
// ID;Status;Mode_Light;Mode_Climate;LED_Status;Illuminance_Value;Temperature_Value
static uint8_t report[] ="X;X;X;X;X;XXX;XXXX";

// Buffer um Daten zu empfangen
static uint8_t usartRxBuffer[19];
static uint8_t usartBuffer[19];

// String, der beim Handshake mit der App empfangen werden soll
uint8_t handshake[] = "HalloXXXXXXXXXXXXXE";
uint8_t closeUSART[] = "ByeXXXXXXXXXXXXXXXE";

// Netzwerkeigenschaften
static ZDO_StartNetworkReq_t networkParams;

// Callback wenn Daten an der Bruecke empfangen werden
void readBuffer();

// initialisation of the timer
static void initTimer(void);

//initialisation of the modules
static void initModule(void);

// Function gets called after the timer for writing values of modules to bridge is expired
static void ausgabeTimerFired(void);

// Function that gets called if the status of module 1 turns on/off
static void OnOffStatusReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the status of module 2 turns on/off
static void OnOffStatusReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the status of module 3 turns on/off
static void OnOffStatusReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the light turns on/off from module 1
static void OnOffMode_lightReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the light turns on/off from module 2
static void OnOffMode_lightReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the light turns on/off from module 3
static void OnOffMode_lightReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the climate turns on/off from module 1
static void OnOffMode_climateReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the climate turns on/off from module 2
static void OnOffMode_climateReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the climate turns on/off from module 3
static void OnOffMode_climateReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the white LED turns on/off from module 1
static void OnOffLightReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the white LED turns on/off from module 2
static void OnOffLightReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the white LED turns on/off from module 3
static void OnOffLightReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if receiving a TemperatureCluster-Report from module 1
static void temperatureMeasurementReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if receiving a TemperatureCluster-Report from module 2
static void temperatureMeasurementReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if receiving a TemperatureCluster-Report from module 3
static void temperatureMeasurementReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if receiving a IlluminanceCluster-Report from module 1
static void illuminanceMeasurementReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if receiving a IlluminanceCluster-Report from module 2
static void illuminanceMeasurementReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if receiving a IlluminanceCluster-Report from module 3
static void illuminanceMeasurementReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);


// Variable to send mode_light on/off command to module 1
static ZCL_Request_t onMode_LightCommand_Client1;
static ZCL_Request_t offMode_LightCommand_Client1;

// Variable to send mode_light on/off command to module 2
static ZCL_Request_t onMode_LightCommand_Client2;
static ZCL_Request_t offMode_LightCommand_Client2;

// Variable to send mode_light on/off command to module 3
static ZCL_Request_t onMode_LightCommand_Client3;
static ZCL_Request_t offMode_LightCommand_Client3;

// Variable to send mode_climate on/off command to module 1
static ZCL_Request_t onMode_ClimateCommand_Client1;
static ZCL_Request_t offMode_ClimateCommand_Client1;

// Variable to send mode_climate on/off command to module 2
static ZCL_Request_t onMode_ClimateCommand_Client2;
static ZCL_Request_t offMode_ClimateCommand_Client2;

// Variable to send mode_climate on/off command to module 3
static ZCL_Request_t onMode_ClimateCommand_Client3;
static ZCL_Request_t offMode_ClimateCommand_Client3;

// Variable to send status on/off command to module 1
static ZCL_Request_t onStatusCommand_Client1;
static ZCL_Request_t offStatusCommand_Client1;

// Variable to send status on/off command to module 2
static ZCL_Request_t onStatusCommand_Client2;
static ZCL_Request_t offStatusCommand_Client2;

// Variable to send status on/off command to module 3
static ZCL_Request_t onStatusCommand_Client3;
static ZCL_Request_t offStatusCommand_Client3;

// Variable to send light on/off command to module 1
static ZCL_Request_t onLightCommand_Client1;
static ZCL_Request_t offLightCommand_Client1;

// Variable to send light on/off command to module 2
static ZCL_Request_t onLightCommand_Client2;
static ZCL_Request_t offLightCommand_Client2;

// Variable to send light on/off command to module 3
static ZCL_Request_t onLightCommand_Client3;
static ZCL_Request_t offLightCommand_Client3;

// Struct to send data with temperature and illuminance reference to clients
static AppMessage_t_Temperature transmitDataTemperatureClient1;
static AppMessage_t_Temperature transmitDataTemperatureClient2;
static AppMessage_t_Temperature transmitDataTemperatureClient3;
static AppMessage_t_Illuminance transmitDataIlluminanceClient1;
static AppMessage_t_Illuminance transmitDataIlluminanceClient2;
static AppMessage_t_Illuminance transmitDataIlluminanceClient3;

// Command to send Temperature and illuminance reference value to clients
APS_DataReq_t dataReqTemperatureClient1;
APS_DataReq_t dataReqTemperatureClient2;
APS_DataReq_t dataReqTemperatureClient3;
APS_DataReq_t dataReqIlluminanceClient1;
APS_DataReq_t dataReqIlluminanceClient2;
APS_DataReq_t dataReqIlluminanceClient3;

// Characeristics for endpoint to send temperature and illuminance reference to clients
static SimpleDescriptor_t simpleDescriptorTemperatureClient1;
static SimpleDescriptor_t simpleDescriptorTemperatureClient2;
static SimpleDescriptor_t simpleDescriptorTemperatureClient3;
static SimpleDescriptor_t simpleDescriptorIlluminanceClient1;
static SimpleDescriptor_t simpleDescriptorIlluminanceClient2;
static SimpleDescriptor_t simpleDescriptorIlluminanceClient3;

// endpoint to send temperature and illuminance reference to clients
static APS_RegisterEndpointReq_t endPointTemperatureZielwertClient1;
static APS_RegisterEndpointReq_t endPointTemperatureZielwertClient2;
static APS_RegisterEndpointReq_t endPointTemperatureZielwertClient3;
static APS_RegisterEndpointReq_t endPointIlluminanceZielwertClient1;
static APS_RegisterEndpointReq_t endPointIlluminanceZielwertClient2;
static APS_RegisterEndpointReq_t endPointIlluminanceZielwertClient3;

// Function used to calculate the temperature out of a TemperatureCluster-Report
static uint16_t calcTemperature(uint8_t *reportPayload);

// Function used to calculate the illuminance out of a IlluminanceCluster-Report
static uint16_t calcIlluminance(uint8_t *reportPayload);

// Function used to extract the boolean out of a Report
static bool calcBoolean(uint8_t *reportPayload);

// Function to write the values of the modules to the bridge
static void ausgabe(Module module);

// Function to start the network 
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);

// Function when data arrived from App at the bridge
static void dataAppReceived(void);

// Configure Reports to send temperature and illuminance reference data to clients
static void initTransmitData(void);

// Enpoints initialisieren
static void initEndpoint(void);

//////////////////START IMPLEMENTATION//////////////////////

// ClusterIDs fuer Temperaturmessung, Lichtmessung und onOff
static ClusterId_t clientClusterTemperatureId[] = {TEMPERATURE_MEASUREMENT_CLUSTER_ID};
static ClusterId_t clientClusterOnOffLightId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t clientClusterIlluminanceId[] = {ILLUMINANCE_MEASUREMENT_CLUSTER_ID};
static ClusterId_t clientClusterOnOffStatusId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t clientClusterOnOffMode_climateId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t clientClusterOnOffMode_lightId[] = {ONOFF_CLUSTER_ID};

// Clusterdefinition fuer Temperaturmessung, Lichtmessung und onOff als Clients (Senden Kommandos / Erhalten Daten)
static ZCL_Cluster_t clientClustersTemperatureMeasurement1[]={DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL)};
static ZCL_Cluster_t clientClustersTemperatureMeasurement2[]={DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL)};
static ZCL_Cluster_t clientClustersTemperatureMeasurement3[]={DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL)};
static ZCL_Cluster_t clientClustersOnOffLight1[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffLight2[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffLight3[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersIlluminanceMeasurement1[]={DEFINE_ILLUMINANCE_MEASUREMENT_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL)};
static ZCL_Cluster_t clientClustersIlluminanceMeasurement2[]={DEFINE_ILLUMINANCE_MEASUREMENT_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL)};
static ZCL_Cluster_t clientClustersIlluminanceMeasurement3[]={DEFINE_ILLUMINANCE_MEASUREMENT_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL)};
static ZCL_Cluster_t clientClustersOnOffStatus1[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffStatus2[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffStatus3[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffMode_climate1[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffMode_climate2[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffMode_climate3[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffMode_light1[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffMode_light2[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
static ZCL_Cluster_t clientClustersOnOffMode_light3[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
	
void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo){
	if(ZDO_SUCCESS_STATUS == confirmInfo->status){
		appstate = NOTHING;
	}
	SYS_PostTask(APL_TASK_ID);
}


static void initTransmitData(){
	dataReqTemperatureClient1.profileId = 1;
	dataReqTemperatureClient1.dstAddrMode = APS_EXT_ADDRESS;
	dataReqTemperatureClient1.dstAddress.extAddress = 0x50000000A01LL;
	dataReqTemperatureClient1.dstEndpoint = dstTemperature_Zielwert_Server1;
	dataReqTemperatureClient1.asdu = transmitDataTemperatureClient1.data;
	dataReqTemperatureClient1.asduLength = sizeof(transmitDataTemperatureClient1.data);
	dataReqTemperatureClient1.srcEndpoint = srcTemperature_Zielwert_Client1;
	dataReqTemperatureClient1.APS_DataConf = NULL;
	
	dataReqTemperatureClient2.profileId = 1;
	dataReqTemperatureClient2.dstAddrMode = APS_EXT_ADDRESS;
	dataReqTemperatureClient2.dstAddress.extAddress = 0x50000000A02LL;
	dataReqTemperatureClient2.dstEndpoint = dstTemperature_Zielwert_Server2;
	dataReqTemperatureClient2.asdu = transmitDataTemperatureClient2.data;
	dataReqTemperatureClient2.asduLength = sizeof(transmitDataTemperatureClient2.data);
	dataReqTemperatureClient2.srcEndpoint = srcTemperature_Zielwert_Client2;
	dataReqTemperatureClient2.APS_DataConf = NULL;
	
	dataReqTemperatureClient3.profileId = 1;
	dataReqTemperatureClient3.dstAddrMode = APS_EXT_ADDRESS;
	dataReqTemperatureClient3.dstAddress.extAddress = 0x50000000A03LL;
	dataReqTemperatureClient3.dstEndpoint = dstTemperature_Zielwert_Server3;
	dataReqTemperatureClient3.asdu = transmitDataTemperatureClient3.data;
	dataReqTemperatureClient3.asduLength = sizeof(transmitDataTemperatureClient3.data);
	dataReqTemperatureClient3.srcEndpoint = srcTemperature_Zielwert_Client3;
	dataReqTemperatureClient3.APS_DataConf = NULL;
	
	dataReqIlluminanceClient1.profileId = 1;
	dataReqIlluminanceClient1.dstAddrMode = APS_EXT_ADDRESS;
	dataReqIlluminanceClient1.dstAddress.extAddress = 0x50000000A01LL;
	dataReqIlluminanceClient1.dstEndpoint = dstIlluminance_Zielwert_Server1;
	dataReqIlluminanceClient1.asdu = transmitDataIlluminanceClient1.data;
	dataReqIlluminanceClient1.asduLength = sizeof(transmitDataIlluminanceClient1.data);
	dataReqIlluminanceClient1.srcEndpoint = srcIlluminance_Zielwert_Client1;
	dataReqIlluminanceClient1.APS_DataConf = NULL;
	
	dataReqIlluminanceClient2.profileId = 1;
	dataReqIlluminanceClient2.dstAddrMode = APS_EXT_ADDRESS;
	dataReqIlluminanceClient2.dstAddress.extAddress = 0x50000000A02LL;
	dataReqIlluminanceClient2.dstEndpoint = dstIlluminance_Zielwert_Server2;
	dataReqIlluminanceClient2.asdu = transmitDataIlluminanceClient2.data;
	dataReqIlluminanceClient2.asduLength = sizeof(transmitDataIlluminanceClient2.data);
	dataReqIlluminanceClient2.srcEndpoint = srcIlluminance_Zielwert_Client2;
	dataReqIlluminanceClient2.APS_DataConf = NULL;
	
	dataReqIlluminanceClient3.profileId = 1;
	dataReqIlluminanceClient3.dstAddrMode = APS_EXT_ADDRESS;
	dataReqIlluminanceClient3.dstAddress.extAddress = 0x50000000A03LL;
	dataReqIlluminanceClient3.dstEndpoint = dstIlluminance_Zielwert_Server3;
	dataReqIlluminanceClient3.asdu = transmitDataIlluminanceClient3.data;
	dataReqIlluminanceClient3.asduLength = sizeof(transmitDataIlluminanceClient3.data);
	dataReqIlluminanceClient3.srcEndpoint = srcIlluminance_Zielwert_Client3;
	dataReqIlluminanceClient3.APS_DataConf = NULL;	
}

static void ZCL_CommandResp(ZCL_Notify_t *ntfy){
	(void)ntfy;
}

static void initCommands(void){
	onMode_LightCommand_Client1.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onMode_LightCommand_Client1.dstAddressing.addr.extAddress = 0x50000000A01LL;
	onMode_LightCommand_Client1.dstAddressing.profileId=0x0104;
	onMode_LightCommand_Client1.dstAddressing.endpointId=dstOnOff_Mode_Light_Server1;
	onMode_LightCommand_Client1.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onMode_LightCommand_Client1.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onMode_LightCommand_Client1.endpointId=srcOnOff_Mode_Light_Client1;
	onMode_LightCommand_Client1.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onMode_LightCommand_Client1.ZCL_Notify=ZCL_CommandResp;
	
	onMode_LightCommand_Client2.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onMode_LightCommand_Client2.dstAddressing.addr.extAddress = 0x50000000A02LL;
	onMode_LightCommand_Client2.dstAddressing.profileId=0x0104;
	onMode_LightCommand_Client2.dstAddressing.endpointId=dstOnOff_Mode_Light_Server2;
	onMode_LightCommand_Client2.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onMode_LightCommand_Client2.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onMode_LightCommand_Client2.endpointId=srcOnOff_Mode_Light_Client2;
	onMode_LightCommand_Client2.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onMode_LightCommand_Client2.ZCL_Notify=ZCL_CommandResp;
	
	onMode_LightCommand_Client3.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onMode_LightCommand_Client3.dstAddressing.addr.extAddress = 0x50000000A03LL;
	onMode_LightCommand_Client3.dstAddressing.profileId=0x0104;
	onMode_LightCommand_Client3.dstAddressing.endpointId=dstOnOff_Mode_Light_Server3;
	onMode_LightCommand_Client3.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onMode_LightCommand_Client3.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onMode_LightCommand_Client3.endpointId=srcOnOff_Mode_Light_Client3;
	onMode_LightCommand_Client3.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onMode_LightCommand_Client3.ZCL_Notify=ZCL_CommandResp;
	
	
	onMode_ClimateCommand_Client1.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onMode_ClimateCommand_Client1.dstAddressing.addr.extAddress = 0x50000000A01LL;
	onMode_ClimateCommand_Client1.dstAddressing.profileId=0x0104;
	onMode_ClimateCommand_Client1.dstAddressing.endpointId=dstOnOff_Mode_Climate_Server1;
	onMode_ClimateCommand_Client1.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onMode_ClimateCommand_Client1.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onMode_ClimateCommand_Client1.endpointId=srcOnOff_Mode_Climate_Client1;
	onMode_ClimateCommand_Client1.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onMode_ClimateCommand_Client1.ZCL_Notify=ZCL_CommandResp;
	
	onMode_ClimateCommand_Client2.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onMode_ClimateCommand_Client2.dstAddressing.addr.extAddress = 0x50000000A02LL;
	onMode_ClimateCommand_Client2.dstAddressing.profileId=0x0104;
	onMode_ClimateCommand_Client2.dstAddressing.endpointId=dstOnOff_Mode_Climate_Server2;
	onMode_ClimateCommand_Client2.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onMode_ClimateCommand_Client2.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onMode_ClimateCommand_Client2.endpointId=srcOnOff_Mode_Climate_Client2;
	onMode_ClimateCommand_Client2.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onMode_ClimateCommand_Client2.ZCL_Notify=ZCL_CommandResp;
	
	onMode_ClimateCommand_Client3.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onMode_ClimateCommand_Client3.dstAddressing.addr.extAddress = 0x50000000A03LL;
	onMode_ClimateCommand_Client3.dstAddressing.profileId=0x0104;
	onMode_ClimateCommand_Client3.dstAddressing.endpointId=dstOnOff_Mode_Climate_Server3;
	onMode_ClimateCommand_Client3.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onMode_ClimateCommand_Client3.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onMode_ClimateCommand_Client3.endpointId=srcOnOff_Mode_Climate_Client3;
	onMode_ClimateCommand_Client3.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onMode_ClimateCommand_Client3.ZCL_Notify=ZCL_CommandResp;
	
	onStatusCommand_Client1.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onStatusCommand_Client1.dstAddressing.addr.extAddress = 0x50000000A01LL;
	onStatusCommand_Client1.dstAddressing.profileId=0x0104;
	onStatusCommand_Client1.dstAddressing.endpointId=dstOnOff_Status_Server1;
	onStatusCommand_Client1.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onStatusCommand_Client1.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onStatusCommand_Client1.endpointId=srcOnOff_Status_Client1;
	onStatusCommand_Client1.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onStatusCommand_Client1.ZCL_Notify=ZCL_CommandResp;
	
	onStatusCommand_Client2.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onStatusCommand_Client2.dstAddressing.addr.extAddress = 0x50000000A02LL;
	onStatusCommand_Client2.dstAddressing.profileId=0x0104;
	onStatusCommand_Client2.dstAddressing.endpointId=dstOnOff_Status_Server2;
	onStatusCommand_Client2.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onStatusCommand_Client2.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onStatusCommand_Client2.endpointId=srcOnOff_Status_Client2;
	onStatusCommand_Client2.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onStatusCommand_Client2.ZCL_Notify=ZCL_CommandResp;
	
	onStatusCommand_Client3.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onStatusCommand_Client3.dstAddressing.addr.extAddress = 0x50000000A03LL;
	onStatusCommand_Client3.dstAddressing.profileId=0x0104;
	onStatusCommand_Client3.dstAddressing.endpointId=dstOnOff_Status_Server3;
	onStatusCommand_Client3.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onStatusCommand_Client3.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onStatusCommand_Client3.endpointId=srcOnOff_Status_Client3;
	onStatusCommand_Client3.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onStatusCommand_Client3.ZCL_Notify=ZCL_CommandResp;
	
	onLightCommand_Client1.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onLightCommand_Client1.dstAddressing.addr.extAddress = 0x50000000A01LL;
	onLightCommand_Client1.dstAddressing.profileId=0x0104;
	onLightCommand_Client1.dstAddressing.endpointId=dstApp_OnOff_Light_Server1;
	onLightCommand_Client1.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onLightCommand_Client1.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onLightCommand_Client1.endpointId=srcOnOff_Light_Client1;
	onLightCommand_Client1.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onLightCommand_Client1.ZCL_Notify=ZCL_CommandResp;
	
	onLightCommand_Client2.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onLightCommand_Client2.dstAddressing.addr.extAddress = 0x50000000A02LL;
	onLightCommand_Client2.dstAddressing.profileId=0x0104;
	onLightCommand_Client2.dstAddressing.endpointId=dstApp_OnOff_Light_Server2;
	onLightCommand_Client2.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onLightCommand_Client2.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onLightCommand_Client2.endpointId=srcOnOff_Light_Client2;
	onLightCommand_Client2.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onLightCommand_Client2.ZCL_Notify=ZCL_CommandResp;
	
	onLightCommand_Client3.dstAddressing.addrMode=APS_EXT_ADDRESS;
	onLightCommand_Client3.dstAddressing.addr.extAddress = 0x50000000A03LL;
	onLightCommand_Client3.dstAddressing.profileId=0x0104;
	onLightCommand_Client3.dstAddressing.endpointId=dstApp_OnOff_Light_Server3;
	onLightCommand_Client3.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	onLightCommand_Client3.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	onLightCommand_Client3.endpointId=srcOnOff_Light_Client3;
	onLightCommand_Client3.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
	onLightCommand_Client3.ZCL_Notify=ZCL_CommandResp;
	
	offMode_LightCommand_Client1.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offMode_LightCommand_Client1.dstAddressing.addr.extAddress = 0x50000000A01LL;
	offMode_LightCommand_Client1.dstAddressing.profileId=0x0104;
	offMode_LightCommand_Client1.dstAddressing.endpointId=dstOnOff_Mode_Light_Server1;
	offMode_LightCommand_Client1.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offMode_LightCommand_Client1.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offMode_LightCommand_Client1.endpointId=srcOnOff_Mode_Light_Client1;
	offMode_LightCommand_Client1.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offMode_LightCommand_Client1.ZCL_Notify=ZCL_CommandResp;
	
	offMode_LightCommand_Client2.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offMode_LightCommand_Client2.dstAddressing.addr.extAddress = 0x50000000A02LL;
	offMode_LightCommand_Client2.dstAddressing.profileId=0x0104;
	offMode_LightCommand_Client2.dstAddressing.endpointId=dstOnOff_Mode_Light_Server2;
	offMode_LightCommand_Client2.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offMode_LightCommand_Client2.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offMode_LightCommand_Client2.endpointId=srcOnOff_Mode_Light_Client2;
	offMode_LightCommand_Client2.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offMode_LightCommand_Client2.ZCL_Notify=ZCL_CommandResp;
	
	offMode_LightCommand_Client3.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offMode_LightCommand_Client3.dstAddressing.addr.extAddress = 0x50000000A03LL;
	offMode_LightCommand_Client3.dstAddressing.profileId=0x0104;
	offMode_LightCommand_Client3.dstAddressing.endpointId=dstOnOff_Mode_Light_Server3;
	offMode_LightCommand_Client3.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offMode_LightCommand_Client3.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offMode_LightCommand_Client3.endpointId=srcOnOff_Mode_Light_Client3;
	offMode_LightCommand_Client3.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offMode_LightCommand_Client3.ZCL_Notify=ZCL_CommandResp;
	
	
	offMode_ClimateCommand_Client1.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offMode_ClimateCommand_Client1.dstAddressing.addr.extAddress = 0x50000000A01LL;
	offMode_ClimateCommand_Client1.dstAddressing.profileId=0x0104;
	offMode_ClimateCommand_Client1.dstAddressing.endpointId=dstOnOff_Mode_Climate_Server1;
	offMode_ClimateCommand_Client1.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offMode_ClimateCommand_Client1.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offMode_ClimateCommand_Client1.endpointId=srcOnOff_Mode_Climate_Client1;
	offMode_ClimateCommand_Client1.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offMode_ClimateCommand_Client1.ZCL_Notify=ZCL_CommandResp;
	
	offMode_ClimateCommand_Client2.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offMode_ClimateCommand_Client2.dstAddressing.addr.extAddress = 0x50000000A02LL;
	offMode_ClimateCommand_Client2.dstAddressing.profileId=0x0104;
	offMode_ClimateCommand_Client2.dstAddressing.endpointId=dstOnOff_Mode_Climate_Server2;
	offMode_ClimateCommand_Client2.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offMode_ClimateCommand_Client2.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offMode_ClimateCommand_Client2.endpointId=srcOnOff_Mode_Climate_Client2;
	offMode_ClimateCommand_Client2.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offMode_ClimateCommand_Client2.ZCL_Notify=ZCL_CommandResp;
	
	offMode_ClimateCommand_Client3.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offMode_ClimateCommand_Client3.dstAddressing.addr.extAddress = 0x50000000A03LL;
	offMode_ClimateCommand_Client3.dstAddressing.profileId=0x0104;
	offMode_ClimateCommand_Client3.dstAddressing.endpointId=dstOnOff_Mode_Climate_Server3;
	offMode_ClimateCommand_Client3.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offMode_ClimateCommand_Client3.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offMode_ClimateCommand_Client3.endpointId=srcOnOff_Mode_Climate_Client3;
	offMode_ClimateCommand_Client3.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offMode_ClimateCommand_Client3.ZCL_Notify=ZCL_CommandResp;
	
	offStatusCommand_Client1.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offStatusCommand_Client1.dstAddressing.addr.extAddress = 0x50000000A01LL;
	offStatusCommand_Client1.dstAddressing.profileId=0x0104;
	offStatusCommand_Client1.dstAddressing.endpointId=dstOnOff_Status_Server1;
	offStatusCommand_Client1.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offStatusCommand_Client1.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offStatusCommand_Client1.endpointId=srcOnOff_Status_Client1;
	offStatusCommand_Client1.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offStatusCommand_Client1.ZCL_Notify=ZCL_CommandResp;
	
	offStatusCommand_Client2.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offStatusCommand_Client2.dstAddressing.addr.extAddress = 0x50000000A02LL;
	offStatusCommand_Client2.dstAddressing.profileId=0x0104;
	offStatusCommand_Client2.dstAddressing.endpointId=dstOnOff_Status_Server2;
	offStatusCommand_Client2.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offStatusCommand_Client2.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offStatusCommand_Client2.endpointId=srcOnOff_Status_Client2;
	offStatusCommand_Client2.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offStatusCommand_Client2.ZCL_Notify=ZCL_CommandResp;
	
	offStatusCommand_Client3.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offStatusCommand_Client3.dstAddressing.addr.extAddress = 0x50000000A03LL;
	offStatusCommand_Client3.dstAddressing.profileId=0x0104;
	offStatusCommand_Client3.dstAddressing.endpointId=dstOnOff_Status_Server3;
	offStatusCommand_Client3.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offStatusCommand_Client3.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offStatusCommand_Client3.endpointId=srcOnOff_Status_Client3;
	offStatusCommand_Client3.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offStatusCommand_Client3.ZCL_Notify=ZCL_CommandResp;
	
	offLightCommand_Client1.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offLightCommand_Client1.dstAddressing.addr.extAddress = 0x50000000A01LL;
	offLightCommand_Client1.dstAddressing.profileId=0x0104;
	offLightCommand_Client1.dstAddressing.endpointId=dstApp_OnOff_Light_Server1;
	offLightCommand_Client1.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offLightCommand_Client1.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offLightCommand_Client1.endpointId=srcOnOff_Light_Client1;
	offLightCommand_Client1.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offLightCommand_Client1.ZCL_Notify=ZCL_CommandResp;
	
	offLightCommand_Client2.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offLightCommand_Client2.dstAddressing.addr.extAddress = 0x50000000A02LL;
	offLightCommand_Client2.dstAddressing.profileId=0x0104;
	offLightCommand_Client2.dstAddressing.endpointId=dstApp_OnOff_Light_Server2;
	offLightCommand_Client2.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offLightCommand_Client2.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offLightCommand_Client2.endpointId=srcOnOff_Light_Client2;
	offLightCommand_Client2.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offLightCommand_Client2.ZCL_Notify=ZCL_CommandResp;
	
	offLightCommand_Client3.dstAddressing.addrMode=APS_EXT_ADDRESS;
	offLightCommand_Client3.dstAddressing.addr.extAddress = 0x50000000A03LL;
	offLightCommand_Client3.dstAddressing.profileId=0x0104;
	offLightCommand_Client3.dstAddressing.endpointId=dstApp_OnOff_Light_Server3;
	offLightCommand_Client3.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	offLightCommand_Client3.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;
	offLightCommand_Client3.endpointId=srcOnOff_Light_Client3;
	offLightCommand_Client3.id=ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID;
	offLightCommand_Client3.ZCL_Notify=ZCL_CommandResp;
}



static void initModule(){
	module1.ID = 1;
	module2.ID = 2;
	module3.ID = 3;
}

static void initTimer(){
	ausgabeTimer.interval = 2000;
	ausgabeTimer.mode = TIMER_REPEAT_MODE;
	ausgabeTimer.callback = ausgabeTimerFired;
	HAL_StartAppTimer(&ausgabeTimer);
}

static void ausgabeTimerFired(){
	ausgabe(module1);
	ausgabe(module2);
	ausgabe(module3);
}

// Die Callback wird schon direkt nach dem ersten empfangenen Byte aufgerufen, deshalb muessen wir solange warten, bis der komplette String übertragen ist
// Der String wird mit einem "E" abgeschlossen. Wir testen dies mit usartRxBuffer[18]== 69 und verändern das "E" in ein "F", damit wir auch die folgenden Strings abgrenzen können
void readBuffer(){
		HAL_ReadUsart(&usartDescriptor, usartBuffer, sizeof(usartBuffer));
		if(usartRxBuffer[18]== 69){
				usartRxBuffer[18]= 70;
			// Testen ob es sich um ein handshake handelt
			uint8_t k=0;
			for(uint8_t i=0;i<19;i++){
				if(handshake[i]==usartRxBuffer[i]){
					k++;
				}
			}
			//Wenn kein Handshake, normaler Datenempfang
			if(k != 18){
				uint8_t j=0;
				for(uint8_t i=0;i<19;i++){
					if(closeUSART[i]==usartRxBuffer[i]){
						j++;
					}
				}
				if(j == 18){
					HAL_StopAppTimer(&ausgabeTimer);
				} else {
					appstate = DATA_PROCESS;
					SYS_PostTask(APL_TASK_ID);
				}
			//Wenn Handshake, dann "Hallo" zurückschicken und Timer fuer die Ausgabe initialisieren
			} else {
				initTimer(); 
				appWriteDataToUsart(usartRxBuffer, sizeof(usartRxBuffer));
			}
		}
}

static void dataAppReceived(){
		
	tempModule.ID = usartRxBuffer[0]-48;
	tempModule.status = usartRxBuffer[2]-48;
	tempModule.mode_light = usartRxBuffer[4]-48;
	tempModule.mode_climate = usartRxBuffer[6]-48;
	tempModule.LEDWHITE_status = usartRxBuffer[8]-48;
	tempModule.illuminanceReference = (usartRxBuffer[10]-48)*100 + (usartRxBuffer[11]-48)*10 + (usartRxBuffer[12]-48);
	tempModule.temperatureReference = (usartRxBuffer[14]-48)*1000 + (usartRxBuffer[15]-48)*100 + (usartRxBuffer[16]-48)*10 + (usartRxBuffer[17]-48);

	switch(tempModule.ID){
	case 1:	
		module1.status = tempModule.status;
		if(tempModule.status){
			ZCL_CommandReq(&onStatusCommand_Client1);
			
			module1.mode_climate = tempModule.mode_climate;
			if(tempModule.mode_climate){
				ZCL_CommandReq(&onMode_ClimateCommand_Client1);
			}else{
				ZCL_CommandReq(&offMode_ClimateCommand_Client1);
			}
			
			module1.mode_light = tempModule.mode_light;
			if(tempModule.mode_light){
				ZCL_CommandReq(&onMode_LightCommand_Client1);
			}else{
				ZCL_CommandReq(&offMode_LightCommand_Client1);
				
				module1.LEDWHITE_status = tempModule.LEDWHITE_status;
				if(tempModule.LEDWHITE_status){
					ZCL_CommandReq(&onLightCommand_Client1);
				}else{
					ZCL_CommandReq(&offLightCommand_Client1);
				}
			}
			
		transmitDataIlluminanceClient1.data[0] = tempModule.illuminanceReference/100;
		transmitDataIlluminanceClient1.data[1] = (tempModule.illuminanceReference/10)%10;
		transmitDataIlluminanceClient1.data[2] = tempModule.illuminanceReference%10;
				
		APS_DataReq(&dataReqIlluminanceClient1);

		transmitDataTemperatureClient1.data[0] = tempModule.temperatureReference/1000;
		transmitDataTemperatureClient1.data[1] = (tempModule.temperatureReference/100)%10;
		transmitDataTemperatureClient1.data[2] = (tempModule.temperatureReference/10)%10;
		transmitDataTemperatureClient1.data[3] = tempModule.temperatureReference%10;	
			
		APS_DataReq(&dataReqTemperatureClient1);	
			
		}else{
			ZCL_CommandReq(&offStatusCommand_Client1);
	}
	
	break;
	
	case 2:
	module2.status = tempModule.status;
	if(tempModule.status){
		ZCL_CommandReq(&onStatusCommand_Client2);
		
		module2.mode_climate = tempModule.mode_climate;
		if(tempModule.mode_climate){
			ZCL_CommandReq(&onMode_ClimateCommand_Client2);
			}else{
			ZCL_CommandReq(&offMode_ClimateCommand_Client2);
		}
		
		module2.mode_light = tempModule.mode_light;
		if(tempModule.mode_light){
			ZCL_CommandReq(&onMode_LightCommand_Client2);
			}else{
			ZCL_CommandReq(&offMode_LightCommand_Client2);
			
			module2.LEDWHITE_status = tempModule.LEDWHITE_status;
			if(tempModule.LEDWHITE_status){
				ZCL_CommandReq(&onLightCommand_Client2);
				}else{
				ZCL_CommandReq(&offLightCommand_Client2);
			}
		}
		
		transmitDataIlluminanceClient2.data[0] = tempModule.illuminanceReference/100;
		transmitDataIlluminanceClient2.data[1] = (tempModule.illuminanceReference/10)%10;
		transmitDataIlluminanceClient2.data[2] = tempModule.illuminanceReference%10;
		
		APS_DataReq(&dataReqIlluminanceClient2);

		transmitDataTemperatureClient2.data[0] = tempModule.temperatureReference/1000;
		transmitDataTemperatureClient2.data[1] = (tempModule.temperatureReference/100)%10;
		transmitDataTemperatureClient2.data[2] = (tempModule.temperatureReference/10)%10;
		transmitDataTemperatureClient2.data[3] = tempModule.temperatureReference%10;
		
		APS_DataReq(&dataReqTemperatureClient2);
		
	}else{
		ZCL_CommandReq(&offStatusCommand_Client2);
	}
	
	break;
	
	case 3:
	module3.status = tempModule.status;
	if(tempModule.status){
		ZCL_CommandReq(&onStatusCommand_Client3);
		
		module3.mode_climate = tempModule.mode_climate;
		if(tempModule.mode_climate){
			ZCL_CommandReq(&onMode_ClimateCommand_Client3);
			}else{
			ZCL_CommandReq(&offMode_ClimateCommand_Client3);
		}
		
		module3.mode_light = tempModule.mode_light;
		if(tempModule.mode_light){
			ZCL_CommandReq(&onMode_LightCommand_Client3);
			}else{
			ZCL_CommandReq(&offMode_LightCommand_Client3);
			
			module3.LEDWHITE_status = tempModule.LEDWHITE_status;
			if(tempModule.LEDWHITE_status){
				ZCL_CommandReq(&onLightCommand_Client3);
				}else{
				ZCL_CommandReq(&offLightCommand_Client3);
			}
		}
		
		transmitDataIlluminanceClient3.data[0] = tempModule.illuminanceReference/100;
		transmitDataIlluminanceClient3.data[1] = (tempModule.illuminanceReference/10)%10;
		transmitDataIlluminanceClient3.data[2] = tempModule.illuminanceReference%10;
		
		APS_DataReq(&dataReqIlluminanceClient3);

		transmitDataTemperatureClient3.data[0] = tempModule.temperatureReference/1000;
		transmitDataTemperatureClient3.data[1] = (tempModule.temperatureReference/100)%10;
		transmitDataTemperatureClient3.data[2] = (tempModule.temperatureReference/10)%10;
		transmitDataTemperatureClient3.data[3] = tempModule.temperatureReference%10;
		
		APS_DataReq(&dataReqTemperatureClient3);
		
		}else{
		ZCL_CommandReq(&offStatusCommand_Client3);
		}
	
	break;
	}
		
	appstate = NOTHING;
	SYS_PostTask(APL_TASK_ID);
}

static void ausgabe(Module module){
	uint32_to_str(report, sizeof(report), module.ID, 0, 1);
	uint32_to_str(report, sizeof(report), module.status, 2, 1);
	uint32_to_str(report, sizeof(report), module.mode_light, 4, 1);
	uint32_to_str(report, sizeof(report), module.mode_climate, 6, 1);
	uint32_to_str(report, sizeof(report), module.LEDWHITE_status, 8, 1);
	uint32_to_str(report, sizeof(report), module.illuminanceValue, 10, 3);
	uint32_to_str(report, sizeof(report), module.temperatureValue, 14, 4);

	appWriteDataToUsart(report, sizeof(report)-1);
}

static uint16_t calcTemperature(uint8_t *reportPayload){
	  int16_t reportValue;
	  ZCL_Report_t *rep = (ZCL_Report_t *)reportPayload;
	  memcpy(&reportValue, &rep->value[0], sizeof(int16_t));	  
	  return (uint16_t) reportValue;
}

static uint16_t calcIlluminance(uint8_t *reportPayload){
		uint16_t reportValue;
		ZCL_Report_t *rep = (ZCL_Report_t *)reportPayload;
		memcpy(&reportValue, &rep->value[0], sizeof(uint16_t));
		return reportValue;
}

static bool calcBoolean(uint8_t *reportPayload){
	uint16_t reportValue;
	ZCL_Report_t* rep = (ZCL_Report_t*)reportPayload;
	memcpy(&reportValue, &rep->value[0], sizeof(uint16_t));	
	return reportValue;
}

static void OnOffMode_lightReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module1.mode_light = calcBoolean(reportPayload);
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffMode_lightReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module2.mode_light = calcBoolean(reportPayload);	
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffMode_lightReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module3.mode_light = calcBoolean(reportPayload);	
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffMode_climateReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module1.mode_climate = calcBoolean(reportPayload);	
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffMode_climateReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module2.mode_climate = calcBoolean(reportPayload);	
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffMode_climateReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module3.mode_climate = calcBoolean(reportPayload);	
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffStatusReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module1.status = calcBoolean(reportPayload);
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffStatusReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module2.status = calcBoolean(reportPayload);	
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffStatusReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module3.status = calcBoolean(reportPayload);	
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffLightReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module1.LEDWHITE_status = calcBoolean(reportPayload);
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffLightReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module2.LEDWHITE_status = calcBoolean(reportPayload);	
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void OnOffLightReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module3.LEDWHITE_status = calcBoolean(reportPayload);	
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void temperatureMeasurementReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){	
	module1.temperatureValue = calcTemperature(reportPayload);  
   (void)addressing, (void)reportLength, (void)reportPayload;
}

static void temperatureMeasurementReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module2.temperatureValue = calcTemperature(reportPayload);
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void temperatureMeasurementReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module3.temperatureValue = calcTemperature(reportPayload);
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void illuminanceMeasurementReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module1.illuminanceValue = calcIlluminance(reportPayload);
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void illuminanceMeasurementReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module2.illuminanceValue = calcIlluminance(reportPayload);
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void illuminanceMeasurementReportInd3(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload){
	module3.illuminanceValue = calcIlluminance(reportPayload);
	(void)addressing, (void)reportLength, (void)reportPayload;
}

static void initEndpoint(void){
	// Festlegen, welche Funktion beim Datenempfang aufgerufen werden soll
	clientClustersTemperatureMeasurement1[0].ZCL_ReportInd = temperatureMeasurementReportInd1;
	clientClustersTemperatureMeasurement2[0].ZCL_ReportInd = temperatureMeasurementReportInd2;
	clientClustersTemperatureMeasurement3[0].ZCL_ReportInd = temperatureMeasurementReportInd3;
	
	clientClustersIlluminanceMeasurement1[0].ZCL_ReportInd = illuminanceMeasurementReportInd1;
	clientClustersIlluminanceMeasurement2[0].ZCL_ReportInd = illuminanceMeasurementReportInd2;	
	clientClustersIlluminanceMeasurement3[0].ZCL_ReportInd = illuminanceMeasurementReportInd3;	
	
	clientClustersOnOffLight1[0].ZCL_ReportInd = OnOffLightReportInd1;
	clientClustersOnOffLight2[0].ZCL_ReportInd = OnOffLightReportInd2;
	clientClustersOnOffLight3[0].ZCL_ReportInd = OnOffLightReportInd3;
	
	clientClustersOnOffStatus1[0].ZCL_ReportInd = OnOffStatusReportInd1;
	clientClustersOnOffStatus2[0].ZCL_ReportInd = OnOffStatusReportInd2;
	clientClustersOnOffStatus3[0].ZCL_ReportInd = OnOffStatusReportInd3;
	
	clientClustersOnOffMode_climate1[0].ZCL_ReportInd = OnOffMode_climateReportInd1;
	clientClustersOnOffMode_climate2[0].ZCL_ReportInd = OnOffMode_climateReportInd2;
	clientClustersOnOffMode_climate3[0].ZCL_ReportInd = OnOffMode_climateReportInd3;
		
	clientClustersOnOffMode_light1[0].ZCL_ReportInd = OnOffMode_lightReportInd1;
	clientClustersOnOffMode_light2[0].ZCL_ReportInd = OnOffMode_lightReportInd2;
	clientClustersOnOffMode_light3[0].ZCL_ReportInd = OnOffMode_lightReportInd3;
		
	endPointTemperatureMeasurementClient1.simpleDescriptor.AppDeviceId = 1;
	endPointTemperatureMeasurementClient1.simpleDescriptor.AppProfileId = 0x0104;
	endPointTemperatureMeasurementClient1.simpleDescriptor.endpoint = srcTemperature_Measurement_Client1;
	endPointTemperatureMeasurementClient1.simpleDescriptor.AppDeviceVersion = 1;
	endPointTemperatureMeasurementClient1.simpleDescriptor.AppInClustersCount = 0;
	endPointTemperatureMeasurementClient1.simpleDescriptor.AppInClustersList = NULL;
	endPointTemperatureMeasurementClient1.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterTemperatureId);
	endPointTemperatureMeasurementClient1.simpleDescriptor.AppOutClustersList = clientClusterTemperatureId;
	endPointTemperatureMeasurementClient1.serverCluster = NULL;
	endPointTemperatureMeasurementClient1.clientCluster = clientClustersTemperatureMeasurement1;
	
	endPointTemperatureMeasurementClient2.simpleDescriptor.AppDeviceId = 1;
	endPointTemperatureMeasurementClient2.simpleDescriptor.AppProfileId = 0x0104;
	endPointTemperatureMeasurementClient2.simpleDescriptor.endpoint = srcTemperature_Measurement_Client2;
	endPointTemperatureMeasurementClient2.simpleDescriptor.AppDeviceVersion = 1;
	endPointTemperatureMeasurementClient2.simpleDescriptor.AppInClustersCount = 0;
	endPointTemperatureMeasurementClient2.simpleDescriptor.AppInClustersList = NULL;
	endPointTemperatureMeasurementClient2.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterTemperatureId);
	endPointTemperatureMeasurementClient2.simpleDescriptor.AppOutClustersList = clientClusterTemperatureId;
	endPointTemperatureMeasurementClient2.serverCluster = NULL;
	endPointTemperatureMeasurementClient2.clientCluster = clientClustersTemperatureMeasurement2;
		
	endPointTemperatureMeasurementClient3.simpleDescriptor.AppDeviceId = 1;
	endPointTemperatureMeasurementClient3.simpleDescriptor.AppProfileId = 0x0104;
	endPointTemperatureMeasurementClient3.simpleDescriptor.endpoint = srcTemperature_Measurement_Client3;
	endPointTemperatureMeasurementClient3.simpleDescriptor.AppDeviceVersion = 1;
	endPointTemperatureMeasurementClient3.simpleDescriptor.AppInClustersCount = 0;
	endPointTemperatureMeasurementClient3.simpleDescriptor.AppInClustersList = NULL;
	endPointTemperatureMeasurementClient3.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterTemperatureId);
	endPointTemperatureMeasurementClient3.simpleDescriptor.AppOutClustersList = clientClusterTemperatureId;
	endPointTemperatureMeasurementClient3.serverCluster = NULL;
	endPointTemperatureMeasurementClient3.clientCluster = clientClustersTemperatureMeasurement3;
	
	endPointOnOffLightClient1.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffLightClient1.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffLightClient1.simpleDescriptor.endpoint = srcOnOff_Light_Client1;
	endPointOnOffLightClient1.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffLightClient1.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffLightClient1.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffLightClient1.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffLightId);
	endPointOnOffLightClient1.simpleDescriptor.AppOutClustersList = clientClusterOnOffLightId;
	endPointOnOffLightClient1.serverCluster = NULL;
	endPointOnOffLightClient1.clientCluster = clientClustersOnOffLight1;
	
	endPointOnOffLightClient2.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffLightClient2.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffLightClient2.simpleDescriptor.endpoint = srcOnOff_Light_Client2;
	endPointOnOffLightClient2.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffLightClient2.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffLightClient2.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffLightClient2.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffLightId);
	endPointOnOffLightClient2.simpleDescriptor.AppOutClustersList = clientClusterOnOffLightId;
	endPointOnOffLightClient2.serverCluster = NULL;
	endPointOnOffLightClient2.clientCluster = clientClustersOnOffLight2;
	
	endPointOnOffLightClient3.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffLightClient3.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffLightClient3.simpleDescriptor.endpoint = srcOnOff_Light_Client3;
	endPointOnOffLightClient3.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffLightClient3.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffLightClient3.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffLightClient3.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffLightId);
	endPointOnOffLightClient3.simpleDescriptor.AppOutClustersList = clientClusterOnOffLightId;
	endPointOnOffLightClient3.serverCluster = NULL;
	endPointOnOffLightClient3.clientCluster = clientClustersOnOffLight3;
	
	endPointOnOffStatusClient1.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffStatusClient1.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffStatusClient1.simpleDescriptor.endpoint = srcOnOff_Status_Client1;
	endPointOnOffStatusClient1.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffStatusClient1.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffStatusClient1.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffStatusClient1.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffStatusId);
	endPointOnOffStatusClient1.simpleDescriptor.AppOutClustersList = clientClusterOnOffStatusId;
	endPointOnOffStatusClient1.serverCluster = NULL;
	endPointOnOffStatusClient1.clientCluster = clientClustersOnOffStatus1;
	
	endPointOnOffStatusClient2.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffStatusClient2.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffStatusClient2.simpleDescriptor.endpoint = srcOnOff_Status_Client2;
	endPointOnOffStatusClient2.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffStatusClient2.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffStatusClient2.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffStatusClient2.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffStatusId);
	endPointOnOffStatusClient2.simpleDescriptor.AppOutClustersList = clientClusterOnOffStatusId;
	endPointOnOffStatusClient2.serverCluster = NULL;
	endPointOnOffStatusClient2.clientCluster = clientClustersOnOffStatus2;
	
	endPointOnOffStatusClient3.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffStatusClient3.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffStatusClient3.simpleDescriptor.endpoint = srcOnOff_Status_Client3;
	endPointOnOffStatusClient3.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffStatusClient3.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffStatusClient3.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffStatusClient3.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffStatusId);
	endPointOnOffStatusClient3.simpleDescriptor.AppOutClustersList = clientClusterOnOffStatusId;
	endPointOnOffStatusClient3.serverCluster = NULL;
	endPointOnOffStatusClient3.clientCluster = clientClustersOnOffStatus3;
	
	endPointOnOffMode_climateClient1.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffMode_climateClient1.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffMode_climateClient1.simpleDescriptor.endpoint = srcOnOff_Mode_Climate_Client1;
	endPointOnOffMode_climateClient1.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffMode_climateClient1.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffMode_climateClient1.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffMode_climateClient1.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffMode_climateId);
	endPointOnOffMode_climateClient1.simpleDescriptor.AppOutClustersList = clientClusterOnOffMode_climateId;
	endPointOnOffMode_climateClient1.serverCluster = NULL;
	endPointOnOffMode_climateClient1.clientCluster = clientClustersOnOffMode_climate1;
	
	endPointOnOffMode_climateClient2.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffMode_climateClient2.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffMode_climateClient2.simpleDescriptor.endpoint = srcOnOff_Mode_Climate_Client2;
	endPointOnOffMode_climateClient2.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffMode_climateClient2.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffMode_climateClient2.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffMode_climateClient2.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffMode_climateId);
	endPointOnOffMode_climateClient2.simpleDescriptor.AppOutClustersList = clientClusterOnOffMode_climateId;
	endPointOnOffMode_climateClient2.serverCluster = NULL;
	endPointOnOffMode_climateClient2.clientCluster = clientClustersOnOffMode_climate2;
	
	endPointOnOffMode_climateClient3.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffMode_climateClient3.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffMode_climateClient3.simpleDescriptor.endpoint = srcOnOff_Mode_Climate_Client3;
	endPointOnOffMode_climateClient3.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffMode_climateClient3.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffMode_climateClient3.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffMode_climateClient3.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffMode_climateId);
	endPointOnOffMode_climateClient3.simpleDescriptor.AppOutClustersList = clientClusterOnOffMode_climateId;
	endPointOnOffMode_climateClient3.serverCluster = NULL;
	endPointOnOffMode_climateClient3.clientCluster = clientClustersOnOffMode_climate3;
	
	endPointOnOffMode_lightClient1.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffMode_lightClient1.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffMode_lightClient1.simpleDescriptor.endpoint = srcOnOff_Mode_Light_Client1;
	endPointOnOffMode_lightClient1.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffMode_lightClient1.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffMode_lightClient1.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffMode_lightClient1.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffMode_lightId);
	endPointOnOffMode_lightClient1.simpleDescriptor.AppOutClustersList = clientClusterOnOffMode_lightId;
	endPointOnOffMode_lightClient1.serverCluster = NULL;
	endPointOnOffMode_lightClient1.clientCluster = clientClustersOnOffMode_light1;
	
	endPointOnOffMode_lightClient2.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffMode_lightClient2.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffMode_lightClient2.simpleDescriptor.endpoint = srcOnOff_Mode_Light_Client2;
	endPointOnOffMode_lightClient2.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffMode_lightClient2.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffMode_lightClient2.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffMode_lightClient2.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffMode_lightId);
	endPointOnOffMode_lightClient2.simpleDescriptor.AppOutClustersList = clientClusterOnOffMode_lightId;
	endPointOnOffMode_lightClient2.serverCluster = NULL;
	endPointOnOffMode_lightClient2.clientCluster = clientClustersOnOffMode_light2;
	
	endPointOnOffMode_lightClient3.simpleDescriptor.AppDeviceId = 1;
	endPointOnOffMode_lightClient3.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffMode_lightClient3.simpleDescriptor.endpoint = srcOnOff_Mode_Light_Client3;
	endPointOnOffMode_lightClient3.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffMode_lightClient3.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffMode_lightClient3.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffMode_lightClient3.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffMode_lightId);
	endPointOnOffMode_lightClient3.simpleDescriptor.AppOutClustersList = clientClusterOnOffMode_lightId;
	endPointOnOffMode_lightClient3.serverCluster = NULL;
	endPointOnOffMode_lightClient3.clientCluster = clientClustersOnOffMode_light3;
	
	endPointIlluminanceClient1.simpleDescriptor.AppDeviceId = 1;
	endPointIlluminanceClient1.simpleDescriptor.AppProfileId = 0x0104;
	endPointIlluminanceClient1.simpleDescriptor.endpoint = srcIlluminance_Measurement_Client1;
	endPointIlluminanceClient1.simpleDescriptor.AppDeviceVersion = 1;
	endPointIlluminanceClient1.simpleDescriptor.AppInClustersCount = 0;
	endPointIlluminanceClient1.simpleDescriptor.AppInClustersList = NULL;
	endPointIlluminanceClient1.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterIlluminanceId);
	endPointIlluminanceClient1.simpleDescriptor.AppOutClustersList = clientClusterIlluminanceId;
	endPointIlluminanceClient1.serverCluster = NULL;
	endPointIlluminanceClient1.clientCluster = clientClustersIlluminanceMeasurement1;
	
	endPointIlluminanceClient2.simpleDescriptor.AppDeviceId = 1;
	endPointIlluminanceClient2.simpleDescriptor.AppProfileId = 0x0104;
	endPointIlluminanceClient2.simpleDescriptor.endpoint = srcIlluminance_Measurement_Client2;
	endPointIlluminanceClient2.simpleDescriptor.AppDeviceVersion = 1;
	endPointIlluminanceClient2.simpleDescriptor.AppInClustersCount = 0;
	endPointIlluminanceClient2.simpleDescriptor.AppInClustersList = NULL;
	endPointIlluminanceClient2.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterIlluminanceId);
	endPointIlluminanceClient2.simpleDescriptor.AppOutClustersList = clientClusterIlluminanceId;
	endPointIlluminanceClient2.serverCluster = NULL;
	endPointIlluminanceClient2.clientCluster = clientClustersIlluminanceMeasurement2;
	
	endPointIlluminanceClient3.simpleDescriptor.AppDeviceId = 1;
	endPointIlluminanceClient3.simpleDescriptor.AppProfileId = 0x0104;
	endPointIlluminanceClient3.simpleDescriptor.endpoint = srcIlluminance_Measurement_Client3;
	endPointIlluminanceClient3.simpleDescriptor.AppDeviceVersion = 1;
	endPointIlluminanceClient3.simpleDescriptor.AppInClustersCount = 0;
	endPointIlluminanceClient3.simpleDescriptor.AppInClustersList = NULL;
	endPointIlluminanceClient3.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterIlluminanceId);
	endPointIlluminanceClient3.simpleDescriptor.AppOutClustersList = clientClusterIlluminanceId;
	endPointIlluminanceClient3.serverCluster = NULL;
	endPointIlluminanceClient3.clientCluster = clientClustersIlluminanceMeasurement3;
	
	// Endpoints for reference values for temperature / illuminance
	simpleDescriptorTemperatureClient1.AppDeviceId = 1;
	simpleDescriptorTemperatureClient1.AppProfileId = 1;
	simpleDescriptorTemperatureClient1.endpoint = dstTemperature_Zielwert_Server1;
	simpleDescriptorTemperatureClient1.AppDeviceVersion = 1;
	endPointTemperatureZielwertClient1.simpleDescriptor = &simpleDescriptorTemperatureClient1;
	endPointTemperatureZielwertClient1.APS_DataInd = NULL;
	APS_RegisterEndpointReq(&endPointTemperatureZielwertClient1);
	
	simpleDescriptorTemperatureClient2.AppDeviceId = 1;
	simpleDescriptorTemperatureClient2.AppProfileId = 1;
	simpleDescriptorTemperatureClient2.endpoint = dstTemperature_Zielwert_Server2;
	simpleDescriptorTemperatureClient2.AppDeviceVersion = 1;
	endPointTemperatureZielwertClient2.simpleDescriptor = &simpleDescriptorTemperatureClient2;
	endPointTemperatureZielwertClient2.APS_DataInd = NULL;
	APS_RegisterEndpointReq(&endPointTemperatureZielwertClient2);
	
	simpleDescriptorTemperatureClient3.AppDeviceId = 1;
	simpleDescriptorTemperatureClient3.AppProfileId = 1;
	simpleDescriptorTemperatureClient3.endpoint = dstTemperature_Zielwert_Server3;
	simpleDescriptorTemperatureClient3.AppDeviceVersion = 1;
	endPointTemperatureZielwertClient3.simpleDescriptor = &simpleDescriptorTemperatureClient3;
	endPointTemperatureZielwertClient3.APS_DataInd = NULL;
	APS_RegisterEndpointReq(&endPointTemperatureZielwertClient3);	
	
	simpleDescriptorIlluminanceClient1.AppDeviceId = 1;
	simpleDescriptorIlluminanceClient1.AppProfileId = 1;
	simpleDescriptorIlluminanceClient1.endpoint = dstIlluminance_Zielwert_Server1;
	simpleDescriptorIlluminanceClient1.AppDeviceVersion = 1;
	endPointIlluminanceZielwertClient1.simpleDescriptor = &simpleDescriptorIlluminanceClient1;
	endPointIlluminanceZielwertClient1.APS_DataInd = NULL;
	APS_RegisterEndpointReq(&endPointIlluminanceZielwertClient1);
	
	simpleDescriptorIlluminanceClient2.AppDeviceId = 1;
	simpleDescriptorIlluminanceClient2.AppProfileId = 1;
	simpleDescriptorIlluminanceClient2.endpoint = dstIlluminance_Zielwert_Server2;
	simpleDescriptorIlluminanceClient2.AppDeviceVersion = 1;
	endPointIlluminanceZielwertClient2.simpleDescriptor = &simpleDescriptorIlluminanceClient2;
	endPointIlluminanceZielwertClient2.APS_DataInd = NULL;
	APS_RegisterEndpointReq(&endPointIlluminanceZielwertClient2);
	
	simpleDescriptorIlluminanceClient3.AppDeviceId = 1;
	simpleDescriptorIlluminanceClient3.AppProfileId = 1;
	simpleDescriptorIlluminanceClient3.endpoint = dstIlluminance_Zielwert_Server3;
	simpleDescriptorIlluminanceClient3.AppDeviceVersion = 1;
	endPointIlluminanceZielwertClient3.simpleDescriptor = &simpleDescriptorIlluminanceClient3;
	endPointIlluminanceZielwertClient3.APS_DataInd = NULL;
	APS_RegisterEndpointReq(&endPointIlluminanceZielwertClient3);
	
	ZCL_RegisterEndpoint(&endPointTemperatureMeasurementClient1);
	ZCL_RegisterEndpoint(&endPointTemperatureMeasurementClient2);
	ZCL_RegisterEndpoint(&endPointTemperatureMeasurementClient3);
	ZCL_RegisterEndpoint(&endPointOnOffStatusClient1);
	ZCL_RegisterEndpoint(&endPointOnOffStatusClient2);
	ZCL_RegisterEndpoint(&endPointOnOffStatusClient3);
	ZCL_RegisterEndpoint(&endPointOnOffMode_climateClient1);
	ZCL_RegisterEndpoint(&endPointOnOffMode_climateClient2);
	ZCL_RegisterEndpoint(&endPointOnOffMode_climateClient3);
	ZCL_RegisterEndpoint(&endPointOnOffMode_lightClient1);
	ZCL_RegisterEndpoint(&endPointOnOffMode_lightClient2);
	ZCL_RegisterEndpoint(&endPointOnOffMode_lightClient3);
	ZCL_RegisterEndpoint(&endPointOnOffLightClient1);
	ZCL_RegisterEndpoint(&endPointOnOffLightClient2);
	ZCL_RegisterEndpoint(&endPointOnOffLightClient3);
	ZCL_RegisterEndpoint(&endPointIlluminanceClient1);
	ZCL_RegisterEndpoint(&endPointIlluminanceClient2);
	ZCL_RegisterEndpoint(&endPointIlluminanceClient3);
}


void APL_TaskHandler(void){
	switch(appstate){
	case INIT:
		BSP_OpenLeds();
		appInitUsartManager();
		// Weitere Spezifikationen für die Bridge (Buffer, Bufferlaenge und Callback Funktion spezifizieren)
		usartDescriptor.rxBuffer       = usartRxBuffer;
		usartDescriptor.rxBufferLength = sizeof(usartRxBuffer);
		usartDescriptor.rxCallback	 = readBuffer;
		initEndpoint();
		initModule();
		initCommands();
		initTransmitData();
		appstate = JOIN_NETWORK;
		SYS_PostTask(APL_TASK_ID);
		break;
	case JOIN_NETWORK:
		networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
		ZDO_StartNetworkReq(&networkParams);
		break;
	case DATA_PROCESS:
		dataAppReceived();
		break;
	case NOTHING:
		break;
	}

};






/*******************************************************************************
  \brief The function is called by the stack to notify the application about
  various network-related events. See detailed description in API Reference.

  Mandatory function: must be present in any application.

  \param[in] nwkParams - contains notification type and additional data varying
             an event
  \return none
*******************************************************************************/
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams)
{
  nwkParams = nwkParams;  // Unused parameter warning prevention
}

/*******************************************************************************
  \brief The function is called by the stack when the node wakes up by timer.

  When the device starts after hardware reset the stack posts an application
  task (via SYS_PostTask()) once, giving control to the application, while
  upon wake up the stack only calls this indication function. So, to provide
  control to the application on wake up, change the application state and post
  an application task via SYS_PostTask(APL_TASK_ID) from this function.

  Mandatory function: must be present in any application.

  \return none
*******************************************************************************/
void ZDO_WakeUpInd(void)
{
}

#ifdef _BINDING_
/***********************************************************************************
  \brief The function is called by the stack to notify the application that a
  binding request has been received from a remote node.

  Mandatory function: must be present in any application.

  \param[in] bindInd - information about the bound device
  \return none
 ***********************************************************************************/
void ZDO_BindIndication(ZDO_BindInd_t *bindInd)
{
  (void)bindInd;
}

/***********************************************************************************
  \brief The function is called by the stack to notify the application that a
  binding request has been received from a remote node.

  Mandatory function: must be present in any application.

  \param[in] unbindInd - information about the unbound device
  \return none
 ***********************************************************************************/
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd)
{
  (void)unbindInd;
}
#endif //_BINDING_

/**********************************************************************//**
  \brief The entry point of the program. This function should not be
  changed by the user without necessity and must always include an
  invocation of the SYS_SysInit() function and an infinite loop with
  SYS_RunTask() function called on each step.

  \return none
**************************************************************************/
int main(void)
{
  //Initialization of the System Environment
  SYS_SysInit();

  //The infinite loop maintaing task management
  for(;;)
  {
    //Each time this function is called, the task
    //scheduler processes the next task posted by one
    //of the BitCloud components or the application
    SYS_RunTask();
  }
}

//eof app.c
