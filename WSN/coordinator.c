#include <zdo.h>
#include <app.h>
#include <sysTaskManager.h>
#include <irq.h>
#include <zcl.h>
#include <zclOnOffCluster.h>
#include <zclIlluminanceMeasurementCluster.h>
#include <usartManager.h>
#include <leds.h>
#include <zclTemperatureMeasurementCluster.h>
#include <stdlib.h>

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

static Module module1;
static Module module2;
static Module module3;

static struct{
	uint8_t nextOutput[20];
	uint8_t nextOutputPos;
	uint8_t output[22];
	uint8_t outputSize;
} usart;

// Timer fuer periodische Ausgabe an die bridge
static HAL_AppTimer_t ausgabeTimer;

static AppState_t appstate = INIT;
static uint8_t temp[] = "Value: XXX.XXXCelsius\n\r";
static uint8_t test[] = "XXXXX\n\r";
static uint8_t report[] ="X;X;XX.XX;XXX;X;X;X\n\r";
static HAL_UsartDescriptor_t usartDesc;
static uint8_t usartRxBuffer[20];

static ZDO_StartNetworkReq_t networkParams;

void interruptHandlerINT4();

void readBuffer(uint16_t bytesReceived);

static void copyTextToAusgabe(uint8_t* text, uint8_t size);

// initialisation of the timer
static void initTimer(void);

// Function gets called after the timer is expired
static void ausgabeTimerFired(void);

// Function that gets called if the module 1 turns on/off
static void OnOffStatusReportInd1(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the module 2 turns on/off
static void OnOffStatusReportInd2(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

// Function that gets called if the module 3 turns on/off
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


// Variable to send mode_light toggle command to module 1
static ZCL_Request_t onMode_LightCommand_Client1;
static ZCL_Request_t offMode_LightCommand_Client1;

// Variable to send mode_light toggle command to module 2
static ZCL_Request_t onMode_LightCommand_Client2;
static ZCL_Request_t offMode_LightCommand_Client2;

// Variable to send mode_light toggle command to module 3
static ZCL_Request_t onMode_LightCommand_Client3;
static ZCL_Request_t offMode_LightCommand_Client3;

// Variable to send mode_climate toggle command to module 1
static ZCL_Request_t onMode_ClimateCommand_Client1;
static ZCL_Request_t offMode_ClimateCommand_Client1;

// Variable to send mode_climate toggle command to module 2
static ZCL_Request_t onMode_ClimateCommand_Client2;
static ZCL_Request_t offMode_ClimateCommand_Client2;

// Variable to send mode_climate toggle command to module 3
static ZCL_Request_t onMode_ClimateCommand_Client3;
static ZCL_Request_t offMode_ClimateCommand_Client3;

// Variable to send status toggle command to module 1
static ZCL_Request_t onStatusCommand_Client1;
static ZCL_Request_t offStatusCommand_Client1;

// Variable to send status toggle command to module 2
static ZCL_Request_t onStatusCommand_Client2;
static ZCL_Request_t offStatusCommand_Client2;

// Variable to send status toggle command to module 3
static ZCL_Request_t onStatusCommand_Client3;
static ZCL_Request_t offStatusCommand_Client3;

// Variable to send light toggle command to module 1
static ZCL_Request_t onLightCommand_Client1;
static ZCL_Request_t offLightCommand_Client1;
// Variable to send light toggle command to module 2
static ZCL_Request_t onLightCommand_Client2;
static ZCL_Request_t offLightCommand_Client2;
// Variable to send light toggle command to module 3
static ZCL_Request_t onLightCommand_Client3;
static ZCL_Request_t offLightCommand_Client3;


// Function used to calculate the temperature out of a TemperatureCluster-Report
static double calcTemperature(uint8_t *reportPayload);

// Function used to calculate the illuminance out of a IlluminanceCluster-Report
static uint16_t calcIlluminance(uint8_t *reportPayload);

// Function used to extract the boolean out of a Report
static bool calcBoolean(uint8_t *reportPayload);

// Function to write to bridge
static void ausgabe(Module module);

static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);

// Function when data arrived from App
static void dataAppReceived(void);

void initUsart(){
	usartDesc.tty            = USART_CHANNEL_1;
	usartDesc.mode           = USART_MODE_ASYNC;        // USART synchronization mode
	usartDesc.baudrate       = USART_BAUDRATE_38400;    // USART baud rate
	usartDesc.dataLength     = USART_DATA8;             // USART data length
	usartDesc.parity         = USART_PARITY_NONE;       // USART parity mode.
	usartDesc.stopbits       = USART_STOPBIT_1;         // USART stop bit
	usartDesc.flowControl    = USART_FLOW_CONTROL_NONE; // Flow control
	usartDesc.rxBuffer       = usartRxBuffer;
	usartDesc.rxBufferLength = sizeof(usartRxBuffer);
	usartDesc.rxCallback	 = readBuffer;
	usartDesc.txBuffer       = NULL;
	usartDesc.txBufferLength = 0;
	usartDesc.txCallback     = NULL;  
}


void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo){
	if(ZDO_SUCCESS_STATUS == confirmInfo->status){
		appstate = NOTHING;
	}
	SYS_PostTask(APL_TASK_ID);
}


static AppMessage_t_Temperature transmitDataTemperatureClient1;
static AppMessage_t_Temperature transmitDataTemperatureClient2;
static AppMessage_t_Temperature transmitDataTemperatureClient3;
static AppMessage_t_Illuminance transmitDataIlluminanceClient1;
static AppMessage_t_Illuminance transmitDataIlluminanceClient2;
static AppMessage_t_Illuminance transmitDataIlluminanceClient3;
APS_DataReq_t dataReqTemperatureClient1;
APS_DataReq_t dataReqTemperatureClient2;
APS_DataReq_t dataReqTemperatureClient3;
APS_DataReq_t dataReqIlluminanceClient1;
APS_DataReq_t dataReqIlluminanceClient2;
APS_DataReq_t dataReqIlluminanceClient3;

static void APS_DataConf(APS_DataConf_t *confInfo);
static SimpleDescriptor_t simpleDescriptorTemperatureClient1;
static SimpleDescriptor_t simpleDescriptorTemperatureClient2;
static SimpleDescriptor_t simpleDescriptorTemperatureClient3;
static SimpleDescriptor_t simpleDescriptorIlluminanceClient1;
static SimpleDescriptor_t simpleDescriptorIlluminanceClient2;
static SimpleDescriptor_t simpleDescriptorIlluminanceClient3;
static APS_RegisterEndpointReq_t endPointTemperatureZielwertClient1;
static APS_RegisterEndpointReq_t endPointTemperatureZielwertClient2;
static APS_RegisterEndpointReq_t endPointTemperatureZielwertClient3;
static APS_RegisterEndpointReq_t endPointIlluminanceZielwertClient1;
static APS_RegisterEndpointReq_t endPointIlluminanceZielwertClient2;
static APS_RegisterEndpointReq_t endPointIlluminanceZielwertClient3;
void APS_DataIndTemperatureClient1(APS_DataInd_t *indData);
void APS_DataIndTemperatureClient2(APS_DataInd_t *indData);
void APS_DataIndTemperatureClient3(APS_DataInd_t *indData);
void APS_DataIndIlluminanceClient1(APS_DataInd_t *indData);
void APS_DataIndIlluminanceClient2(APS_DataInd_t *indData);
void APS_DataIndIlluminanceClient3(APS_DataInd_t *indData);

static void initTransmitData(){
	dataReqTemperatureClient1.profileId = 1;
	dataReqTemperatureClient1.dstAddrMode = APS_EXT_ADDRESS;
	dataReqTemperatureClient1.dstAddress.extAddress = 0x50000000A01LL;
	dataReqTemperatureClient1.dstEndpoint = dstTemperature_Zielwert_Server1;
	dataReqTemperatureClient1.asdu = transmitDataTemperatureClient1.data;
	dataReqTemperatureClient1.asduLength = sizeof(transmitDataTemperatureClient1.data);
	dataReqTemperatureClient1.srcEndpoint = srcTemperature_Zielwert_Client1;
	dataReqTemperatureClient1.APS_DataConf = APS_DataConf;
	
	dataReqTemperatureClient2.profileId = 1;
	dataReqTemperatureClient2.dstAddrMode = APS_EXT_ADDRESS;
	dataReqTemperatureClient2.dstAddress.extAddress = 0x50000000A02LL;
	dataReqTemperatureClient2.dstEndpoint = dstTemperature_Zielwert_Server2;
	dataReqTemperatureClient2.asdu = transmitDataTemperatureClient2.data;
	dataReqTemperatureClient2.asduLength = sizeof(transmitDataTemperatureClient2.data);
	dataReqTemperatureClient2.srcEndpoint = srcTemperature_Zielwert_Client2;
	dataReqTemperatureClient2.APS_DataConf = APS_DataConf;
	
	dataReqTemperatureClient3.profileId = 1;
	dataReqTemperatureClient3.dstAddrMode = APS_EXT_ADDRESS;
	dataReqTemperatureClient3.dstAddress.extAddress = 0x50000000A03LL;
	dataReqTemperatureClient3.dstEndpoint = dstTemperature_Zielwert_Server3;
	dataReqTemperatureClient3.asdu = transmitDataTemperatureClient3.data;
	dataReqTemperatureClient3.asduLength = sizeof(transmitDataTemperatureClient3.data);
	dataReqTemperatureClient3.srcEndpoint = srcTemperature_Zielwert_Client3;
	dataReqTemperatureClient3.APS_DataConf = APS_DataConf;
	
	dataReqIlluminanceClient1.profileId = 1;
	dataReqIlluminanceClient1.dstAddrMode = APS_EXT_ADDRESS;
	dataReqIlluminanceClient1.dstAddress.extAddress = 0x50000000A01LL;
	dataReqIlluminanceClient1.dstEndpoint = dstIlluminance_Zielwert_Server1;
	dataReqIlluminanceClient1.asdu = transmitDataIlluminanceClient1.data;
	dataReqIlluminanceClient1.asduLength = sizeof(transmitDataIlluminanceClient1.data);
	dataReqIlluminanceClient1.srcEndpoint = srcIlluminance_Zielwert_Client1;
	dataReqIlluminanceClient1.APS_DataConf = APS_DataConf;
	
	dataReqIlluminanceClient2.profileId = 1;
	dataReqIlluminanceClient2.dstAddrMode = APS_EXT_ADDRESS;
	dataReqIlluminanceClient2.dstAddress.extAddress = 0x50000000A02LL;
	dataReqIlluminanceClient2.dstEndpoint = dstIlluminance_Zielwert_Server2;
	dataReqIlluminanceClient2.asdu = transmitDataIlluminanceClient2.data;
	dataReqIlluminanceClient2.asduLength = sizeof(transmitDataIlluminanceClient2.data);
	dataReqIlluminanceClient2.srcEndpoint = srcIlluminance_Zielwert_Client2;
	dataReqIlluminanceClient2.APS_DataConf = APS_DataConf;
	
	dataReqIlluminanceClient3.profileId = 1;
	dataReqIlluminanceClient3.dstAddrMode = APS_EXT_ADDRESS;
	dataReqIlluminanceClient3.dstAddress.extAddress = 0x50000000A03LL;
	dataReqIlluminanceClient3.dstEndpoint = dstIlluminance_Zielwert_Server3;
	dataReqIlluminanceClient3.asdu = transmitDataIlluminanceClient3.data;
	dataReqIlluminanceClient3.asduLength = sizeof(transmitDataIlluminanceClient3.data);
	dataReqIlluminanceClient3.srcEndpoint = srcIlluminance_Zielwert_Client3;
	dataReqIlluminanceClient3.APS_DataConf = APS_DataConf;
}

static void APS_DataConf(APS_DataConf_t *confInfo){
	
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
	offStatusCommand_Client2.id=ZCL_ONOFF_CLUSTER_ON_COMMAND_ID;
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

static ClusterId_t clientClusterIds[] = {TEMPERATURE_MEASUREMENT_CLUSTER_ID, ONOFF_CLUSTER_ID};
static ClusterId_t clientClusterTemperatureId[] = {TEMPERATURE_MEASUREMENT_CLUSTER_ID};
static ClusterId_t clientClusterOnOffLightId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t clientClusterIlluminanceId[] = {ILLUMINANCE_MEASUREMENT_CLUSTER_ID};
static ClusterId_t clientClusterOnOffStatusId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t clientClusterOnOffMode_climateId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t clientClusterOnOffMode_lightId[] = {ONOFF_CLUSTER_ID};

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


static void initTimer(){
	ausgabeTimer.interval = 2000;
	ausgabeTimer.mode = TIMER_REPEAT_MODE;
	ausgabeTimer.callback = ausgabeTimerFired;
	HAL_StartAppTimer(&ausgabeTimer);
}

static void ausgabeTimerFired(){
	/*ausgabe(module1);
	ausgabe(module2);
	ausgabe(module3);*/
}

static void copyTextToAusgabe(uint8_t* text, uint8_t size){
	for(int i=0;i<size;i++){
		usart.output[i]=text[i];
	}
	usart.output[size]='\r';
	usart.output[size+1]='\n';
	usart.outputSize=size+2;
}

static uint8_t counter = 0;

void readBuffer(uint16_t bytesReceived){
	
	counter++;
	uint8_t byte;
	BSP_ToggleLed(LED_GREEN);
	HAL_ReadUsart(&usartDesc, &byte, bytesReceived);
	if(byte!='\r' && usart.nextOutputPos<20){
		usart.nextOutput[usart.nextOutputPos] = byte;
		usart.nextOutputPos++;
	}else{
		copyTextToAusgabe(usart.nextOutput, usart.nextOutputPos);
		usart.nextOutputPos = 0;
		usart.output[0] = counter;
		HAL_WriteUsart(&usartDesc, usart.output, usart.outputSize);
		//appWriteDataToUsart(usart.output, usart.outputSize);
	}
}

void interruptHandlerINT4(){
	
	//HAL_WriteUsart(&usartDesc, (uint8_t*)usartRxBuffer, 36);
	//appWriteDataToUsart(usartRxBuffer, sizeof(usartRxBuffer));
	
	transmitDataTemperatureClient1.data[0] = usartRxBuffer[14]-48;
	transmitDataTemperatureClient1.data[1] = usartRxBuffer[15]-48;
	transmitDataTemperatureClient1.data[2] = usartRxBuffer[16]-48;
	transmitDataTemperatureClient1.data[3] = usartRxBuffer[17]-48;
		
	transmitDataIlluminanceClient1.data[0] = 2;
	transmitDataIlluminanceClient1.data[1] = 3;
	transmitDataIlluminanceClient1.data[2] = 3;
	
	APS_DataReq(&dataReqIlluminanceClient1);
	APS_DataReq(&dataReqTemperatureClient1);
	
	// 	ZCL_CommandReq(&toggleMode_LightCommand_Client1);
	// 	ZCL_CommandReq(&toggleMode_ClimateCommand_Client1);
	// 	ZCL_CommandReq(&toggleStatusCommand_Client1);
	// 	ZCL_CommandReq(&toggleLightCommand_Client1);

}
static void initButton(void){
	HAL_RegisterIrq(IRQ_4, IRQ_FALLING_EDGE, interruptHandlerINT4);
	HAL_EnableIrq(IRQ_4);
}

static void dataAppReceived(){
	
	Module tempModule;
	tempModule.ID = usartRxBuffer[0];
	tempModule.status = usartRxBuffer[2];
	tempModule.mode_light = usartRxBuffer[4];
	tempModule.mode_climate = usartRxBuffer[6];
	tempModule.LEDWHITE_status = usartRxBuffer[8];
	tempModule.illuminanceReference = usartRxBuffer[10]*100 + usartRxBuffer[11]*10 + usartRxBuffer[12];
	tempModule.temperatureReference = usartRxBuffer[14]*1000 + usartRxBuffer[15]*100 + usartRxBuffer[16]*10 + usartRxBuffer[17];
	
	switch(tempModule.ID){
		
		case 1:	if(tempModule.status != module1.status){
			
					module1.status = tempModule.status;
					
					if(module1.status == 1)
					{
						ZCL_CommandReq(&onStatusCommand_Client1);
					}else
						ZCL_CommandReq(&offStatusCommand_Client1);
				}
				
				if(tempModule.mode_light != module1.mode_light){
					
					module1.mode_light = tempModule.mode_light;
					
					if(module1.mode_light == 1)
					{ 	
						ZCL_CommandReq(&onMode_LightCommand_Client1);
					}else
						ZCL_CommandReq(&offMode_LightCommand_Client1);
				}
				
				if(tempModule.mode_climate != module1.mode_climate){
					
					module1.mode_climate = tempModule.mode_climate;
					
					if(module1.mode_climate == 1)
					{
						ZCL_CommandReq(&onMode_ClimateCommand_Client1);
					}else
						ZCL_CommandReq(&offMode_ClimateCommand_Client1);
				}
				
				if(tempModule.LEDWHITE_status != module1.LEDWHITE_status){
					
					module1.LEDWHITE_status = tempModule.LEDWHITE_status;
					
					if(module1.LEDWHITE_status == 1){
						
						ZCL_CommandReq(&onLightCommand_Client1);
					}else
						ZCL_CommandReq(&offLightCommand_Client1);
				}
				
				if(tempModule.illuminanceReference != module1.illuminanceReference){
					
					module1.illuminanceReference = tempModule.illuminanceReference;
					
					transmitDataIlluminanceClient1.data[0] = tempModule.illuminanceReference/100;
					transmitDataIlluminanceClient1.data[1] = (tempModule.illuminanceReference/10)%10;
					transmitDataIlluminanceClient1.data[2] = tempModule.illuminanceReference%10;
					
					APS_DataReq(&dataReqIlluminanceClient1);
				}
				
				if(tempModule.temperatureReference != module1.temperatureReference){
					
					module1.temperatureReference = tempModule.temperatureReference;
					
					transmitDataTemperatureClient1.data[0] = tempModule.temperatureReference/1000;
					transmitDataTemperatureClient1.data[1] = (tempModule.temperatureReference/100)%10;
					transmitDataTemperatureClient1.data[2] = (tempModule.temperatureReference/10)%10;
					transmitDataTemperatureClient1.data[3] = tempModule.temperatureReference%10;
					
					APS_DataReq(&dataReqTemperatureClient1);
				}
		
		case 2: if(tempModule.status != module2.status){
			
					module2.status = tempModule.status;
			
					if(module2.status == 1)
					{
						ZCL_CommandReq(&onStatusCommand_Client2);
					}else
						ZCL_CommandReq(&offStatusCommand_Client2);
				}
		
				if(tempModule.mode_light != module2.mode_light){
			
					module2.mode_light = tempModule.mode_light;
			
					if(module2.mode_light == 1)
					{
						ZCL_CommandReq(&onMode_LightCommand_Client2);
					}else
						ZCL_CommandReq(&offMode_LightCommand_Client2);
				}
		
				if(tempModule.mode_climate != module2.mode_climate){
			
					module2.mode_climate = tempModule.mode_climate;
			
					if(module2.mode_climate == 1)
					{
						ZCL_CommandReq(&onMode_ClimateCommand_Client2);
					}else
						ZCL_CommandReq(&offMode_ClimateCommand_Client2);
				}
		
				if(tempModule.LEDWHITE_status != module2.LEDWHITE_status){
			
					module2.LEDWHITE_status = tempModule.LEDWHITE_status;
			
					if(module2.LEDWHITE_status == 1){
				
						ZCL_CommandReq(&onLightCommand_Client2);
					}else
						ZCL_CommandReq(&offLightCommand_Client2);
				}
		
				if(tempModule.illuminanceReference != module2.illuminanceReference){
			
					module2.illuminanceReference = tempModule.illuminanceReference;
			
					transmitDataIlluminanceClient2.data[0] = tempModule.illuminanceReference/100;
					transmitDataIlluminanceClient2.data[1] = (tempModule.illuminanceReference/10)%10;
					transmitDataIlluminanceClient2.data[2] = tempModule.illuminanceReference%10;
			
					APS_DataReq(&dataReqIlluminanceClient2);
				}
		
				if(tempModule.temperatureReference != module2.temperatureReference){
			
					module2.temperatureReference = tempModule.temperatureReference;
			
					transmitDataTemperatureClient2.data[0] = tempModule.temperatureReference/1000;
					transmitDataTemperatureClient2.data[1] = (tempModule.temperatureReference/100)%10;
					transmitDataTemperatureClient2.data[2] = (tempModule.temperatureReference/10)%10;
					transmitDataTemperatureClient2.data[3] = tempModule.temperatureReference%10;
			
					APS_DataReq(&dataReqTemperatureClient2);
				}
		
		case 3:	if(tempModule.status != module3.status){
			
					module3.status = tempModule.status;
			
					if(module3.status == 1)
					{
						ZCL_CommandReq(&onStatusCommand_Client3);
					}else
						ZCL_CommandReq(&offStatusCommand_Client3);
				}
		
				if(tempModule.mode_light != module3.mode_light){
			
					module3.mode_light = tempModule.mode_light;
			
					if(module3.mode_light == 1)
					{
						ZCL_CommandReq(&onMode_LightCommand_Client3);
					}else
						ZCL_CommandReq(&offMode_LightCommand_Client3);
				}
		
				if(tempModule.mode_climate != module3.mode_climate){
			
					module3.mode_climate = tempModule.mode_climate;
			
					if(module3.mode_climate == 1)
					{
						ZCL_CommandReq(&onMode_ClimateCommand_Client3);
					}else
						ZCL_CommandReq(&offMode_ClimateCommand_Client3);
				}
		
				if(tempModule.LEDWHITE_status != module3.LEDWHITE_status){
			
					module3.LEDWHITE_status = tempModule.LEDWHITE_status;
			
					if(module3.LEDWHITE_status == 1){
				
						ZCL_CommandReq(&onLightCommand_Client3);
					}else
						ZCL_CommandReq(&offLightCommand_Client3);
				}
		
				if(tempModule.illuminanceReference != module3.illuminanceReference){
			
					module3.illuminanceReference = tempModule.illuminanceReference;
			
					transmitDataIlluminanceClient3.data[0] = tempModule.illuminanceReference/100;
					transmitDataIlluminanceClient3.data[1] = (tempModule.illuminanceReference/10)%10;
					transmitDataIlluminanceClient3.data[2] = tempModule.illuminanceReference%10;
			
					APS_DataReq(&dataReqIlluminanceClient3);
				}
		
				if(tempModule.temperatureReference != module3.temperatureReference){
			
					module3.temperatureReference = tempModule.temperatureReference;
			
					transmitDataTemperatureClient3.data[0] = tempModule.temperatureReference/1000;
					transmitDataTemperatureClient3.data[1] = (tempModule.temperatureReference/100)%10;
					transmitDataTemperatureClient3.data[2] = (tempModule.temperatureReference/10)%10;
					transmitDataTemperatureClient3.data[3] = tempModule.temperatureReference%10;
			
					APS_DataReq(&dataReqTemperatureClient3);
				}
			}
	
	appWriteDataToUsart(usartRxBuffer, sizeof(usartRxBuffer));
}


static void ausgabe(Module module){
	uint32_to_str(report, sizeof(report), module.ID, 0, 1);
	uint32_to_str(report, sizeof(report), module.status, 2, 1);
	uint32_to_str(report, sizeof(report), module.temperatureValue, 4, 2);
	uint32_to_str(report, sizeof(report), (module.temperatureValue - (int16_t)module.temperatureValue)*100, 7, 2);
	uint32_to_str(report, sizeof(report), module.illuminanceValue, 10, 3);
	uint32_to_str(report, sizeof(report), module.LEDWHITE_status, 14, 1);
	uint32_to_str(report, sizeof(report), module.mode_climate, 16, 1);
	uint32_to_str(report, sizeof(report), module.mode_light, 18, 1);
	//appWriteDataToUsart(report, sizeof(report));
}

static double calcTemperature(uint8_t *reportPayload){
	  int16_t reportValue;
	  int16_t tmpValue;
	  ZCL_Report_t *rep = (ZCL_Report_t *)reportPayload;
	  memcpy(&reportValue, &rep->value[0], sizeof(int16_t));
	  tmpValue = reportValue/100;
	  int32_to_str(temp, sizeof(temp), tmpValue, 7, 3);
	  reportValue = reportValue%100;
	  int32_to_str(temp, sizeof(temp), reportValue, 11, 3);
	  temp[11]=temp[12];
	  temp[12]=temp[13];
	  temp[13]=' ';
	  return (double)reportValue/100+(double)tmpValue;
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
	
	//uint32_to_str(test, sizeof(test), module1.LEDWHITE_status, 0, 1);
	//appWriteDataToUsart(test, sizeof(test));
	
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
	//uint32_to_str(test, sizeof(test), module1.LEDWHITE_status, 0, 1);
	//appWriteDataToUsart(test, sizeof(test));
	
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
  //int32_to_str(test, sizeof(test), module1.temperatureValue, 0, 6);
  //appWriteDataToUsart(test, sizeof(test));
  
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
	
	//uint32_to_str(test, sizeof(test), module1.illuminanceValue, 0, 3);
	//appWriteDataToUsart(test, sizeof(test));
	
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
	
	// Endpoints for receiving values for controlling temperature / illuminance
	simpleDescriptorTemperatureClient1.AppDeviceId = 1;
	simpleDescriptorTemperatureClient1.AppProfileId = 1;
	simpleDescriptorTemperatureClient1.endpoint = dstTemperature_Zielwert_Server1;
	simpleDescriptorTemperatureClient1.AppDeviceVersion = 1;
	endPointTemperatureZielwertClient1.simpleDescriptor = &simpleDescriptorTemperatureClient1;
	endPointTemperatureZielwertClient1.APS_DataInd = APS_DataIndTemperatureClient1;
	APS_RegisterEndpointReq(&endPointTemperatureZielwertClient1);
	
	simpleDescriptorTemperatureClient2.AppDeviceId = 1;
	simpleDescriptorTemperatureClient2.AppProfileId = 1;
	simpleDescriptorTemperatureClient2.endpoint = dstTemperature_Zielwert_Server2;
	simpleDescriptorTemperatureClient2.AppDeviceVersion = 1;
	endPointTemperatureZielwertClient2.simpleDescriptor = &simpleDescriptorTemperatureClient2;
	endPointTemperatureZielwertClient2.APS_DataInd = APS_DataIndTemperatureClient2;
	APS_RegisterEndpointReq(&endPointTemperatureZielwertClient2);
	
	simpleDescriptorTemperatureClient3.AppDeviceId = 1;
	simpleDescriptorTemperatureClient3.AppProfileId = 1;
	simpleDescriptorTemperatureClient3.endpoint = dstTemperature_Zielwert_Server3;
	simpleDescriptorTemperatureClient3.AppDeviceVersion = 1;
	endPointTemperatureZielwertClient3.simpleDescriptor = &simpleDescriptorTemperatureClient3;
	endPointTemperatureZielwertClient3.APS_DataInd = APS_DataIndTemperatureClient3;
	APS_RegisterEndpointReq(&endPointTemperatureZielwertClient3);	
	
	simpleDescriptorIlluminanceClient1.AppDeviceId = 1;
	simpleDescriptorIlluminanceClient1.AppProfileId = 1;
	simpleDescriptorIlluminanceClient1.endpoint = dstIlluminance_Zielwert_Server1;
	simpleDescriptorIlluminanceClient1.AppDeviceVersion = 1;
	endPointIlluminanceZielwertClient1.simpleDescriptor = &simpleDescriptorIlluminanceClient1;
	endPointIlluminanceZielwertClient1.APS_DataInd = APS_DataIndIlluminanceClient1;
	APS_RegisterEndpointReq(&endPointIlluminanceZielwertClient1);
	
	simpleDescriptorIlluminanceClient2.AppDeviceId = 1;
	simpleDescriptorIlluminanceClient2.AppProfileId = 1;
	simpleDescriptorIlluminanceClient2.endpoint = dstIlluminance_Zielwert_Server2;
	simpleDescriptorIlluminanceClient2.AppDeviceVersion = 1;
	endPointIlluminanceZielwertClient2.simpleDescriptor = &simpleDescriptorIlluminanceClient2;
	endPointIlluminanceZielwertClient2.APS_DataInd = APS_DataIndIlluminanceClient2;
	APS_RegisterEndpointReq(&endPointIlluminanceZielwertClient2);
	
	simpleDescriptorIlluminanceClient3.AppDeviceId = 1;
	simpleDescriptorIlluminanceClient3.AppProfileId = 1;
	simpleDescriptorIlluminanceClient3.endpoint = dstIlluminance_Zielwert_Server1;
	simpleDescriptorIlluminanceClient3.AppDeviceVersion = 1;
	endPointIlluminanceZielwertClient3.simpleDescriptor = &simpleDescriptorIlluminanceClient3;
	endPointIlluminanceZielwertClient3.APS_DataInd = APS_DataIndIlluminanceClient3;
	APS_RegisterEndpointReq(&endPointIlluminanceZielwertClient3);
}
void APS_DataIndTemperatureClient1(APS_DataInd_t *indData){
	
}
void APS_DataIndTemperatureClient2(APS_DataInd_t *indData){
	
}
void APS_DataIndTemperatureClient3(APS_DataInd_t *indData){
	
}
void APS_DataIndIlluminanceClient1(APS_DataInd_t *indData){
	
}
void APS_DataIndIlluminanceClient2(APS_DataInd_t *indData){
	
}
void APS_DataIndIlluminanceClient3(APS_DataInd_t *indData){
	
}
void APL_TaskHandler(void){
	switch(appstate){
	case INIT:
		BSP_OpenLeds();
		initUsart();
		//appInitUsartManager();
		HAL_OpenUsart(&usartDesc);
		initEndpoint();
		appstate = JOIN_NETWORK;
		SYS_PostTask(APL_TASK_ID);
		break;
	case JOIN_NETWORK:
		networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
		ZDO_StartNetworkReq(&networkParams);
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
		initTimer();
		initButton();
		initCommands();
		initTransmitData();
		appstate = NOTHING;
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
