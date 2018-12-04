

#include <zdo.h>
#include <app.h>
#include <sysTaskManager.h>
#include <irq.h>
#include <zcl.h>
#include <zclOnOffCluster.h>
#include <usartManager.h>
#include <leds.h>
#include <zclTemperatureMeasurementCluster.h>


static AppState_t appstate = INIT;

static HAL_UsartDescriptor_t usartDesc;
static uint8_t usartRxBuffer[36];

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
	usartDesc.rxCallback	 = NULL;
	usartDesc.txBuffer       = NULL;
	usartDesc.txBufferLength = 0;
	usartDesc.txCallback     = NULL;  // Callback function, confirming data writing
}

static ZDO_StartNetworkReq_t networkParams;

static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);

void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo){
	if(ZDO_SUCCESS_STATUS == confirmInfo->status){
		appstate = NOTHING;
	}
	SYS_PostTask(APL_TASK_ID);
}

static ZCL_DeviceEndpoint_t endPoint;
//static ClusterId_t clientClusterIds[] = {TEMPERATURE_MEASUREMENT_CLUSTER_ID, ONOFF_CLUSTER_ID};
static ClusterId_t clientClusterIds[] = {TEMPERATURE_MEASUREMENT_CLUSTER_ID, ONOFF_CLUSTER_ID};
/*
static ZCL_Cluster_t clientClusters[]={
DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL),
DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)
};
*/
static ZCL_Cluster_t clientClusters[]={
	DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL)
};
static uint8_t temp[] = "Value: XXX.XXXCelsius\n\r";

static void temperatureMeasurementReportInd(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload)
{
  BSP_ToggleLed(LED_GREEN);
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
  char lul[] = "1;1;24.25;2;0;31.75;3;0;27.00\n";
//  appWriteDataToUsart(lul, sizeof(lul));
//	appWriteDataToUsart(temp, sizeof(temp));

  HAL_WriteUsart(&usartDesc, temp, sizeof(temp));

  (void)addressing, (void)reportLength, (void)rep;
	/*
	ZCL_NextElement_t element;

	element.id = ZCL_REPORT_ATTRIBUTES_COMMAND_ID;
	element.payloadLength = reportLength;
	element.payload = reportPayload;
	element.content = NULL;
	
	
	uint32_t value = 2;
	value = *element.payload;
	value = value/100;
	int32_to_str(temp, sizeof(temp), value, 7, 3);
	value = *element.payload - value;
	int32_to_str(temp, sizeof(temp), value, 11, 3);
	
	temp[11]=temp[12];
	temp[12]=temp[13];
	temp[13]=' ';
	appWriteDataToUsart(temp, sizeof(temp));
*/
/*
	ZCL_Report_t *rep = (ZCL_Report_t *)reportPayload;

	uint32_t value = rep->value[0];
	value = value/100;
	int32_to_str(temp, sizeof(temp), value, 7, 3);
	appWriteDataToUsart(temp, sizeof(temp));

	(void)addressing, (void)reportLength, (void)rep;
*/
}

static void initEndpoint(void){
	clientClusters[0].ZCL_ReportInd = temperatureMeasurementReportInd;
	
	endPoint.simpleDescriptor.AppDeviceId = 1;
	endPoint.simpleDescriptor.AppProfileId = 0x0104;
	endPoint.simpleDescriptor.endpoint = 1;
	endPoint.simpleDescriptor.AppDeviceVersion = 1;
	endPoint.simpleDescriptor.AppInClustersCount = 0;
	endPoint.simpleDescriptor.AppInClustersList = NULL;
	endPoint.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterIds);
	endPoint.simpleDescriptor.AppOutClustersList = clientClusterIds;
	endPoint.serverCluster = NULL;
	endPoint.clientCluster = clientClusters;
}

void APL_TaskHandler(void){
	switch(appstate){
	case INIT:
		BSP_OpenLeds();
		//appInitUsartManager();
		initUsart();
		HAL_OpenUsart(&usartDesc);
		initEndpoint();
		appstate = JOIN_NETWORK;
		SYS_PostTask(APL_TASK_ID);
		break;
	case JOIN_NETWORK:
		networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
		ZDO_StartNetworkReq(&networkParams);
		ZCL_RegisterEndpoint(&endPoint);
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
