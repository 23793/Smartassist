

#include <zdo.h>
#include <app.h>
#include <sysTaskManager.h>
#include <irq.h>
#include <zcl.h>
#include <zclOnOffCluster.h>

static AppState_t appstate = INIT;

static ZDO_StartNetworkReq_t networkParams;

static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);

static ClusterId_t clientClusterIds[] = {ONOFF_CLUSTER_ID};
static ZCL_Cluster_t clientClusters[]={
		DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};

static ZCL_DeviceEndpoint_t endPoint;
static void initEndpoint(void);

static ZCL_Request_t toggleCommand;
static void ZCL_CommandResp(ZCL_Notify_t *ntfy);
static void initKommando(void);

static void initButton(void);
static void interruptHandlerINT4(void);


static void initEndpoint(void){
	endPoint.simpleDescriptor.AppDeviceId = 1;
	endPoint.simpleDescriptor.AppProfileId = 0x0104;
	endPoint.simpleDescriptor.endpoint = 1;
	endPoint.simpleDescriptor.AppDeviceVersion = 1;
	endPoint.simpleDescriptor.AppInClustersCount =0;
	endPoint.simpleDescriptor.AppInClustersList = NULL;
	endPoint.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterIds);
	endPoint.simpleDescriptor.AppOutClustersList = clientClusterIds;
	endPoint.serverCluster = NULL;
	endPoint.clientCluster = clientClusters;
}

static void initKommando(void){
	toggleCommand.dstAddressing.addrMode=APS_SHORT_ADDRESS;
	toggleCommand.dstAddressing.addr.shortAddress = CPU_TO_LE16(0x0000);
	toggleCommand.dstAddressing.profileId=0x0104;
	toggleCommand.dstAddressing.endpointId=5;
	toggleCommand.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	toggleCommand.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;

	toggleCommand.endpointId=1;
	toggleCommand.id=ZCL_ONOFF_CLUSTER_TOGGLE_COMMAND_ID;
	toggleCommand.ZCL_Notify=ZCL_CommandResp;
}

static void ZCL_CommandResp(ZCL_Notify_t *ntfy){
	(void)ntfy;
}



void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo){
	if(ZDO_SUCCESS_STATUS == confirmInfo->status){
		appstate = REG_ENDPOINT;
	}
	SYS_PostTask(APL_TASK_ID);
}

static void initButton(void){
	HAL_RegisterIrq(IRQ_4, IRQ_FALLING_EDGE, interruptHandlerINT4);
	HAL_EnableIrq(IRQ_4);
}
void interruptHandlerINT4(void){
	ZCL_CommandReq(&toggleCommand);
}

void APL_TaskHandler(void){
	switch(appstate){
	case INIT:
		initEndpoint();
		initKommando();
		appstate = JOIN_NETWORK;
		SYS_PostTask(APL_TASK_ID);
		break;
	case JOIN_NETWORK:
		networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
		ZDO_StartNetworkReq(&networkParams);
		break;
	case REG_ENDPOINT:
		ZCL_RegisterEndpoint(&endPoint);
		appstate = REG_IRQ;
		SYS_PostTask(APL_TASK_ID);
		break;
	case REG_IRQ:
		initButton();
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
