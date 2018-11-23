/**************************************************************************//**
  \file app.c

  \brief Basis-Anwendung.

  \author Markus Krauﬂe

******************************************************************************/


#include <zdo.h>
#include <app.h>
#include <sysTaskManager.h>
#include <bspLeds.h>
#include <zclOnOffCluster.h>
#include <zcl.h>
#include <irq.h>
/********************************PROTOTYPEN************************************************/

//aktueller Zustand
static AppState_t appstate = INIT;

//‹bergabevariable f¸r die Funktion ZDO_StartNetworkReq().
static ZDO_StartNetworkReq_t networkParams;

//CB-Funktion nach Aufruf ZDO_StartNetworkReq().
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* confirmInfo);

//Funktion wird aufgerufen beim Empfang eines On-Kommandos.
static ZCL_Status_t onInd(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion wird aufgerufen beim Empfang eines Off-Kommandos.
static ZCL_Status_t offInd(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion wird aufgerufen beim Empfang eines Toggle-Kommandos.
static ZCL_Status_t toggleInd(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);


/*Datenstruktur mit allen Variablen des OnOff-Serverclusters (hier nur onOff-Attribut).*/
static ZCL_OnOffClusterServerAttributes_t onOffAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(30, 60)};

/*Datenstruktur in der zu jeder OnOff-KommandoId eine Referenz auf die ausf¸hrenden Funktionen gespeichert ist.
*/
static ZCL_OnOffClusterCommands_t onOffCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onInd, offInd, toggleInd)};

/*Liste mit IDs der unterst¸tzend Servercluster (hier nur OnOff-Cluster).*/
static ClusterId_t serverClusterIds[] = {ONOFF_CLUSTER_ID}; // 0x0006

/*Liste mit ZCL_Cluster_t Datenstrukturen, der unterst¸tzten Cluster (hier nur OnOff-Cluster).*/
static ZCL_Cluster_t serverClusters[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffAttributes, &onOffCommands)};

/*Clientcluster*/
static ClusterId_t clientClusterIds[] = {ONOFF_CLUSTER_ID};
static ZCL_Cluster_t clientClusters[]={
DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};

//Endpunkt f¸r die Registrierung des Endpunktes zur Datenkommunikation
static ZCL_DeviceEndpoint_t endPoint;

//Funktion zur Initialisierung des Endpunktes
static void initEndpoint();

//Funktion zur Manipulation des OnOff-Cluster-Attributes OnOff.
static void setOnOffState(bool state);

static ZCL_Request_t toggleCommand;
static void ZCL_CommandResp(ZCL_Notify_t *ntfy);
static void initKommando(void);

static void initButton(void);
static void interruptHandlerINT3(void);

static void initKommando(void){
	toggleCommand.dstAddressing.addrMode=APS_EXT_ADDRESS;
	toggleCommand.dstAddressing.addr.extAddress = 0x50000000A03LL;
	toggleCommand.dstAddressing.profileId=0x0104;
	toggleCommand.dstAddressing.endpointId=5;
	toggleCommand.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	toggleCommand.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;

	toggleCommand.endpointId=5;
	toggleCommand.id=ZCL_ONOFF_CLUSTER_TOGGLE_COMMAND_ID;
	toggleCommand.ZCL_Notify=ZCL_CommandResp;
}
static void ZCL_CommandResp(ZCL_Notify_t *ntfy){
	(void)ntfy;
}
static void initButton(void){
	HAL_RegisterIrq(IRQ_3, IRQ_FALLING_EDGE, interruptHandlerINT3);
	HAL_EnableIrq(IRQ_3);
}

void interruptHandlerINT3(void){
	ZCL_CommandReq(&toggleCommand);
}
/**********************************PROTOTYPEN-ENDE**********************************************/

/**********************************IMPLEMENTIERUNG**********************************************/

/*Initialisierung des Endpunktes zur Datenkommunikation*/
static void initEndpoint(){
	endPoint.simpleDescriptor.AppDeviceId =1;
	endPoint.simpleDescriptor.AppProfileId = 0x0104;
	endPoint.simpleDescriptor.endpoint = 5;
	endPoint.simpleDescriptor.AppDeviceVersion = 1;
	endPoint.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterIds);
	endPoint.simpleDescriptor.AppInClustersList = serverClusterIds;
	endPoint.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterIds);
	endPoint.simpleDescriptor.AppOutClustersList = clientClusterIds;
	endPoint.serverCluster = serverClusters;
	endPoint.clientCluster = clientClusters;
}

/* On-Kommando erhalten. OnOff-Attribut auf On setzen und LED anschalten. */
ZCL_Status_t onInd(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(true);
	BSP_OnLed(LED_FIRST);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Off-Kommando erhalten. OnOff-Attribut auf Off setzen und LED ausschalten.*/
ZCL_Status_t offInd(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(false);
	BSP_OffLed(false);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Toggle-Kommando erhalten. Zustand des OnOff-Attribut und der LED wechseln. */
ZCL_Status_t toggleInd(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(!onOffAttributes.onOff.value);
	BSP_ToggleLed(LED_FIRST);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Manipulation des OnOff-Cluster-Attributes OnOff. */
static void setOnOffState(bool state){
	onOffAttributes.onOff.value = state;
}

/*CB-Funktion nach Starten des Netzwerkes (ZDO_StartNetworkReq()).*/
void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* confirmInfo){
	if(ZDO_SUCCESS_STATUS == confirmInfo->status){
		appstate=REG_ENDPOINT;
	}
	SYS_PostTask(APL_TASK_ID);
}

void APL_TaskHandler(){
	
	switch(appstate){
		case INIT:
			initKommando();
			BSP_OpenLeds();
			initEndpoint();
			appstate = START_NETWORK;
			SYS_PostTask(APL_TASK_ID);
			break;
			
		case START_NETWORK:
			networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
			ZDO_StartNetworkReq(&networkParams);
			break;
			
		case REG_ENDPOINT:
			ZCL_RegisterEndpoint(&endPoint);
			initButton();
			appstate = NOTHING;
			break;
		
		case NOTHING:
			break;
	}
}





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
