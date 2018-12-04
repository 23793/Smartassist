/**************************************************************************//**
  \file app.c

  \brief Basis-Anwendung.

  \author Markus Krau?e

******************************************************************************/

#include <zdo.h>
#include <app.h>
#include <sysTaskManager.h>
#include <zclOnOffCluster.h>
#include <zclFanControlCluster.h>
#include <zcl.h>
#include <irq.h>
//****************TemperatureSensor************************//
#include <zclTemperatureMeasurementCluster.h>
#include <i2cPacket.h>
#include <appTimer.h>
#include <util/delay.h>


static void readSensorDoneCb(); //Funktion nach Temperatursensormessung
static void sendeTimerFired(); // Funktion wenn Timer abgelaufen ist
static uint8_t lm73Data[2]; //Array f?r Temperaturwert
static HAL_AppTimer_t sendeTimer; // Timer f?r periodische Temperaturmessung
static uint8_t temp[] = "Value: XXX.XXXCelsius\n\r";
static int16_t i; // Int zum Verarbeiten des Temperaturwertes
static int16_t j; // Int zum Verarbeiten des Temperaturwertes

ZCL_TemperatureMeasurementClusterAttributes_t temperatureMeasurementAttributes ={ 
	ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(0, 3)
	}; //Reportable Temperaturattribut
	
APS_BindReq_t bindTemp; //Binding Variable
APS_BindReq_t bindOnOff; //Binding Variable
APS_BindReq_t bindFanControl; //Binding Variable

// Pin f?r Temperaturmessung vorbereiten
static HAL_I2cDescriptor_t i2cdescriptor={
	.tty = TWI_CHANNEL_0,
	.clockRate = I2C_CLOCK_RATE_62,
	.f = readSensorDoneCb,
	.id = LM73_DEVICE_ADDRESS,
	.data = lm73Data,
	.length = 2,
	.lengthAddr = HAL_NO_INTERNAL_ADDRESS
};
// Timer f?r periodische Temperaturmessung initialisieren
static void initTimer(){
	sendeTimer.interval = 2000;
	sendeTimer.mode = TIMER_REPEAT_MODE;
	sendeTimer.callback = sendeTimerFired;
	HAL_StartAppTimer(&sendeTimer);
}

// Bei Timerablauf Temperatur auslesen
static void sendeTimerFired(){
	HAL_ReadI2cPacket(&i2cdescriptor);

}

// Temperturwert konvertieren und in Temperaturclusterattribut schreiben
void readSensorDoneCb(){
	i = lm73Data[0];
	i <<= 8;
	i |= lm73Data[1];
	i >>= 7;
	j = i*100;
	i = lm73Data[1] & (0x7F);
	i >>= 5;
	i = i*25;
	j = j+i;
//	temperatureMeasurementAttributes.measuredValue.value = j;
	ZCL_WriteAttributeValue(1,
	TEMPERATURE_MEASUREMENT_CLUSTER_ID,
	ZCL_CLUSTER_SIDE_SERVER,
	ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MEASURED_VALUE_ATTRIBUTE_ID,
	ZCL_S16BIT_DATA_TYPE_ID,
	(uint8_t*)(& j));
	
}
// Temperaturclusterinit
void temperatureMeasurementClusterInit(void)
{
	temperatureMeasurementAttributes.measuredValue.value = APP_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_ATTRIBUTE_VALUE;
	temperatureMeasurementAttributes.minMeasuredValue.value = APP_TEMPERATURE_MEASUREMENT_MIN_MEASURED_VALUE_ATTRIBUTE_VALUE;
	temperatureMeasurementAttributes.maxMeasuredValue.value = APP_TEMPERATURE_MEASUREMENT_MAX_MEASURED_VALUE_ATTRIBUTE_VALUE;
	temperatureMeasurementAttributes.tolerance.value = APP_TEMPERATURE_MEASUREMENT_TOLERANCE_ATTRIBUTE_VALUE;
}

//****************ENDTemperatureSensor************************//

//******************Binding*****************//

// Binding f?r Tempertur und OnOff initialisieren
void initBinding(void){
	CS_ReadParameter(CS_UID_ID, &bindTemp.srcAddr); //eigene Adresse lesen und schreiben

	bindTemp.srcEndpoint = 1; 
	bindTemp.clusterId   = TEMPERATURE_MEASUREMENT_CLUSTER_ID; 
	bindTemp.dstAddrMode = APS_EXT_ADDRESS;
	bindTemp.dst.unicast.extAddr  =   0x50000000A04LL; 
	bindTemp.dst.unicast.endpoint = 1; 

	APS_BindReq(&bindTemp); //local binding ausf?hren
	
	CS_ReadParameter(CS_UID_ID, &bindOnOff.srcAddr); //eigene Adresse lesen und schreiben

	bindOnOff.srcEndpoint = 2; 
	bindOnOff.clusterId   = ONOFF_CLUSTER_ID; 
	bindOnOff.dstAddrMode = APS_EXT_ADDRESS;
	bindOnOff.dst.unicast.extAddr  =  0x50000000A04LL;  
	bindOnOff.dst.unicast.endpoint = 2; 

	APS_BindReq(&bindOnOff); //local binding ausf?hren
	
	CS_ReadParameter(CS_UID_ID, &bindFanControl.srcAddr);
	
	bindFanControl.srcEndpoint = 3;
	bindFanControl.clusterId = FAN_CONTROL_CLUSTER_ID;
	bindFanControl.dstAddrMode = APS_EXT_ADDRESS;
	bindFanControl.dst.unicast.extAddr  =  0x50000000A04LL;
	bindFanControl.dst.unicast.endpoint = 3;
	
	APS_BindReq(&bindFanControl);
}
//*****************ENDBinding********************************//



/********************************PROTOTYPEN************************************************/

//aktueller Zustand
static AppState_t appstate = INIT;

//?bergabevariable f?r die Funktion ZDO_StartNetworkReq().
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
//Intervall (min-max) f?r den Report definieren
static ZCL_OnOffClusterServerAttributes_t onOffAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(0, 3)};

/*Datenstruktur in der zu jeder OnOff-KommandoId eine Referenz auf die ausf?hrenden Funktionen gespeichert ist.
*/
static ZCL_OnOffClusterCommands_t onOffCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onInd, offInd, toggleInd)};

static ZCL_FanControlClusterServerAttributes_t fanControlAttribute = {ZCL_DEFINE_FAN_CONTROL_CLUSTER_SERVER_ATTRIBUTES()};

/*Liste mit IDs der unterst?tzend Servercluster.*/
static ClusterId_t serverClusterIds[] = {
//	ONOFF_CLUSTER_ID,
	TEMPERATURE_MEASUREMENT_CLUSTER_ID,
//	FAN_CONTROL_CLUSTER_ID
	};

/*Liste mit ZCL_Cluster_t Datenstrukturen, der unterst?tzten Cluster.*/
static ZCL_Cluster_t serverClusters[] = {
//DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffAttributes, &onOffCommands),
DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &temperatureMeasurementAttributes),
//DEFINE_FAN_CONTROL_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &fanControlAttribute, NULL),
};

/*Clientcluster*/
static ClusterId_t clientClusterIds[] = {ONOFF_CLUSTER_ID, FAN_CONTROL_CLUSTER_ID};
static ZCL_Cluster_t clientClusters[]={
DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL),
DEFINE_FAN_CONTROL_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};

//Endpunkt f?r die Registrierung des Endpunktes zur Datenkommunikation
static ZCL_DeviceEndpoint_t endPoint;

//Funktion zur Initialisierung des Endpunktes
static void initEndpoint();

//Funktion zur Manipulation des OnOff-Cluster-Attributes OnOff.
static void setOnOffState(bool state);

//Funktion zur Initialisierung der Outputs (LEDs, L?fter)
static void initOutputs();

static ZCL_Request_t toggleCommand;
static void ZCL_CommandResp(ZCL_Notify_t *ntfy);
static void initKommando(void);

static void initButton(void);
static void interruptHandlerINT3(void);

static void initKommando(void){
	toggleCommand.dstAddressing.addrMode=APS_EXT_ADDRESS;
	toggleCommand.dstAddressing.addr.extAddress = 0x50000000A01LL;
	toggleCommand.dstAddressing.profileId=0x0104;
	toggleCommand.dstAddressing.endpointId=2;
	toggleCommand.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	toggleCommand.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;

	toggleCommand.endpointId=2;
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
	endPoint.simpleDescriptor.endpoint = 1;
	endPoint.simpleDescriptor.AppDeviceVersion = 1;
	endPoint.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterIds);
	endPoint.simpleDescriptor.AppInClustersList = serverClusterIds;
	endPoint.simpleDescriptor.AppOutClustersCount = 0;
	endPoint.simpleDescriptor.AppOutClustersList = NULL;
	endPoint.serverCluster = serverClusters;
	endPoint.clientCluster = NULL;
}

static void setPWMOutput(uint8_t duty)
{
	OCR3B=duty;
}

static uint8_t brightness;

void Wait()
{
	_delay_loop_2(10000);
}

/* Initialisierung der Outputs (LEDS, Luefter) */
/* PE2 = rote LED , PE3 = blaue LED ,PE4 = weisse LED, PE7 = Luefter */
static void initOutputs(){
	
	/* PINS als Ausgang deklarieren */
	DDRE |= (1<<PE2);
	DDRE |= (1<<PE3);
	DDRE |= (1<<PE4);
	DDRE |= (1<<PE7);
	
	/* init Timer3 in fast PWM mode 
	   Timer Clock = CPU Clock (No Prescaleing)
	   Compare Output Mode = Clear OC3B on compare match with TCNT3 (Value = 2)
	   Mode        = Fast PWM (Value = 3)
	   PWM Output  = Non Inverted                */
	TCCR3A |= (1<<WGM00)|(1<<WGM01)|(1<<COM3B1);
	TCCR3B |= (1<<CS30);
	
	OCR3B = 0;

	/* Alle Ausgaenge im Ausgangszustand ausschalten */
	turnOff(LEDBLUE);
	turnOff(LEDRED);
	turnOff(FAN);
}
;
/* On-Kommando erhalten. OnOff-Attribut auf On setzen und LED anschalten. */
ZCL_Status_t onInd(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(true);
	turnOn(LEDBLUE);
	turnOn(LEDRED);
	turnOn(FAN);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Off-Kommando erhalten. OnOff-Attribut auf Off setzen und LED ausschalten.*/
ZCL_Status_t offInd(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(false);
	turnOff(LEDBLUE);
	turnOff(LEDRED);
	turnOff(FAN);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Toggle-Kommando erhalten. Zustand des OnOff-Attribut und der LED wechseln. */
ZCL_Status_t toggleInd(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(!onOffAttributes.onOff.value);

	toggle(FAN);
	toggle(LEDBLUE);
	toggle(LEDRED);
	
	if(OCR3B == 0)
	{
		for(brightness=0;brightness<255;brightness++)
		{
			//Increase the Brightness using PWM

			setPWMOutput(brightness);

			//Now Wait For Some Time
			Wait();
		}
	}
	else
	{
		for(brightness=OCR3B;brightness>0;brightness--)
		{
			//Decrease The Brightness using PWM

			setPWMOutput(brightness);

			//Now Wait For Some Time
			Wait();
		}
		OCR3B = 0;
	}
	
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
			initOutputs();
			initKommando();
			initEndpoint();
            HAL_OpenI2cPacket(&i2cdescriptor);
			appstate = START_NETWORK;
			SYS_PostTask(APL_TASK_ID);
			break;
			
		case START_NETWORK:
			networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
			ZDO_StartNetworkReq(&networkParams);
			break;
			
		case REG_ENDPOINT:
			ZCL_RegisterEndpoint(&endPoint);
            temperatureMeasurementClusterInit();
			initTimer();
			initButton();
            initBinding(); //Binding initialisieren
			ZCL_StartReporting(); //Automatisches Reporting starten
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
