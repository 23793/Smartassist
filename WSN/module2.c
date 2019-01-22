/**************************************************************************//**
  \file app.c

  \brief Basis-Anwendung.

  \author Markus Krau?e

******************************************************************************/

#include <zdo.h>
#include <app.h>
#include <zcl.h>
#include <appTimer.h>
#include <util/delay.h>
#include <sysTaskManager.h>
#include <zclOnOffCluster.h>
#include <zclIlluminanceMeasurementCluster.h>
#include <zclTemperatureMeasurementCluster.h>
#include <irq.h>
#include <i2cPacket.h>
#include <adc.h>
#include <leds.h>
/********************************PROTOTYPEN************************************************/
 

// Datenstruktur mit allen Infos über das Modul
static Module module;

//aktueller Zustand
static AppState_t appstate = INIT;

// Variable für Helligkeitsmessung
static uint8_t LightData;

//Array fuer den Temperaturwert
static uint8_t lm73Data[2];

// Timer f?r periodische Modulupdate
static HAL_AppTimer_t updateTimer;

// Timer f?r periodische Temperaturmessung
static HAL_AppTimer_t sendeTimer;

// Variable fürs Kommandoschicken zum Server
static ZCL_Request_t toggleLightCommand;

//Uebergabevariable fuer die Funktion ZDO_StartNetworkReq().
static ZDO_StartNetworkReq_t networkParams;

//Endpunkt fuer die Registrierung des Endpunktes zur Datenkommunikation
static ZCL_DeviceEndpoint_t endPointOnOffStatusServer;
static ZCL_DeviceEndpoint_t endPointOnOffMode_lightServer;
static ZCL_DeviceEndpoint_t endPointOnOffMode_climateServer;
static ZCL_DeviceEndpoint_t endPointOnOffLightServer;
static ZCL_DeviceEndpoint_t endPointAppOnOffLightServer;
static ZCL_DeviceEndpoint_t endPointIlluminanceServer;
static ZCL_DeviceEndpoint_t endPointTemperatureMeasurementServer;

static ZCL_DeviceEndpoint_t endPointOnOffLightClient;

// Variablen fürs Binding
APS_BindReq_t bindTemp;
APS_BindReq_t bindOnOffLight;
APS_BindReq_t bindOnOffStatus;
APS_BindReq_t bindOnOffmode_climate;
APS_BindReq_t bindOnOffmode_light;
APS_BindReq_t bindIlluminance;


// Endpoint und Descriptor zum Empfang der Temperature und Illuminance Zielwerte
static SimpleDescriptor_t simpleDescriptorTemp;
static SimpleDescriptor_t simpleDescriptorIlluminance;
static APS_RegisterEndpointReq_t endPointTemperatureZielwert;
static APS_RegisterEndpointReq_t endPointIlluminanceZielwert;

// Variablen fuer manuelles reporten
static uint8_t lightAttrBuff[LIGHT_ATTRIBUTE_BUFFER_SIZE];
static uint8_t statusAttrBuff[STATUS_ATTRIBUTE_BUFFER_SIZE];
static uint8_t modeLightAttrBuff[MODELIGHT_ATTRIBUTE_BUFFER_SIZE];
static uint8_t modeClimateAttrBuff[MODECLIMATE_ATTRIBUTE_BUFFER_SIZE];
ZCL_Report_t *reportLightAttrElement = (ZCL_Report_t*) lightAttrBuff;
ZCL_Report_t *reportStatusAttrElement = (ZCL_Report_t*) statusAttrBuff;
ZCL_Report_t *reportModeLightAttrElement = (ZCL_Report_t*) modeLightAttrBuff;
ZCL_Report_t *reportModeClimateAttrElement = (ZCL_Report_t*) modeClimateAttrBuff;

//CB-Funktion nach Aufruf ZDO_StartNetworkReq().
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* confirmInfo);

// Funktion zum Initialisieren der Variablen im Modul-struct
static void initModule();

//Binding initialisieren
void initBinding(void);

// Funktion fürs Warten
void wait(void);

//Timer für SensorMessungen
static void initTimer(void);

//Funktion wird aufgerufen nach Timerablauf
static void sendeTimerFired(void);

//Funktion wird aufgerufen nach Timerablauf
static void updateTimerFired(void);

// Funktion wird aufgerufen nachdem Temperatursensor gelsen wurde
void readTempSensorDoneCb(bool b);

// Funktion wird aufgerufen nachdem Helligkeitssensor gelsen wurde
void readIlluminanceSensorDoneCb(void);

// Temperaturcluster-Attribute initialisieren
static void clusterInit(void);

//Funktion wird aufgerufen beim Empfang eines On-Kommandos.
//Light
static ZCL_Status_t onLight(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);
//Status
static ZCL_Status_t onStatus(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);
//Modus Temperatur
static ZCL_Status_t onMode_climate(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);
//Modus Light
static ZCL_Status_t onMode_light(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion wird aufgerufen beim Empfang eines Off-Kommandos.
//Light
static ZCL_Status_t offLight(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);
//Status
static ZCL_Status_t offStatus(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);
// Modus Temperatur
static ZCL_Status_t offMode_climate(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);
// Modus Light
static ZCL_Status_t offMode_light(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion wird aufgerufen beim Empfang eines Toggle-Kommandos.
//Light
static ZCL_Status_t toggleLight(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion zur PWM von FAN/LEDWHITE
static void setPWMOutputDuty(volatile uint16_t* port, uint8_t duty);

//Funktion zum togglen der PWM von LEDWHITE
static void togglePWMOutput(volatile uint16_t* port);

//Funktion zum einschalten der PWM für LEDWHITE
static void onPWMOutput(volatile uint16_t* port);

//Funktion zum ausschalten der PWM für LEDWHITE
static void offPWMOutput(volatile uint16_t* port);


//Funktion zur Initialisierung des Endpunktes
static void initEndpoints();

//Funktion zur Manipulation des OnOff-Cluster-Attributes OnOff.
static void setOnOffState(ZCL_OnOffClusterServerAttributes_t* attribute, bool state);

//Funktion zur Initialisierung der Outputs (LEDs, Luefter)
static void initOutputs();

// Notifikation nachdem ein Kommando zum Server geschickt wurde
static void ZCL_CommandResp(ZCL_Notify_t *ntfy);

// Initialisierung von Kommandos vom CLient zum Server
static void initKommando(void);

// Funktion zum Initialisieren wenn ein Button gedrückt wurde
static void initButton(void);

// Kommando von Client zu Server schicken, wenn Button gedrückt wurde
static void interruptHandlerINT3(void);

// Funktion die Aufgerufen wird, wenn Zielwerte fuer Temperatur und illuminance empfangen werden
void APS_DataIndTemp(APS_DataInd_t *indData);
void APS_DataIndIlluminance(APS_DataInd_t *indData);

// Notify nachdem manuelle Reports geschickt wurden
void ReportNotify();
/**********************************PROTOTYPEN-ENDE**********************************************/


/**********************************IMPLEMENTIERUNG**********************************************/

/*Datenstruktur mit allen Variablen des
OnOff-Serverclusters (hier nur onOff-Attribut)
Temperaturemeasurement-Serverclusters
Illuminancy-Servercluster
*/
//Intervall (min-max) fuer den Report definieren
static ZCL_OnOffClusterServerAttributes_t onOffLightAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};
static ZCL_OnOffClusterServerAttributes_t onOffStatusAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};
static ZCL_OnOffClusterServerAttributes_t onOffMode_climateAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};
static ZCL_OnOffClusterServerAttributes_t onOffMode_lightAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};
static ZCL_TemperatureMeasurementClusterAttributes_t temperatureMeasurementAttributes ={ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(0, 2)};
static ZCL_IlluminanceMeasurementClusterServerAttributes_t illuminanceMeasurementAttributes ={ZCL_DEFINE_ILLUMINANCE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(0, 2)};

/*Datenstruktur in der zu jeder OnOff-KommandoId eine Referenz auf die ausf?hrenden Funktionen gespeichert ist.*/
static ZCL_OnOffClusterCommands_t onOffLightCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onLight, offLight, toggleLight)};
static ZCL_OnOffClusterCommands_t onOffActivityCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onStatus, offStatus, NULL)};
static ZCL_OnOffClusterCommands_t onOffAutomaticTempCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onMode_climate, offMode_climate, NULL)};
static ZCL_OnOffClusterCommands_t onOffAutomaticLightCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onMode_light, offMode_light, NULL)};

/*Liste mit IDs der unterst?tzend Servercluster.*/
static ClusterId_t serverClusterOnOffStatusId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterOnOffMode_climateId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterOnOffMode_lightId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterOnOffLightId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterIlluminanceId[] = {ILLUMINANCE_MEASUREMENT_CLUSTER_ID};
static ClusterId_t serverClusterTemperatureId[] = {TEMPERATURE_MEASUREMENT_CLUSTER_ID};

/*Liste mit ZCL_Cluster_t Datenstrukturen, der unterst?tzten ServerCluster.*/
static ZCL_Cluster_t serverClustersOnOffStatus[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffStatusAttributes, &onOffActivityCommands)};
static ZCL_Cluster_t serverClustersOnOffMode_climate[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffMode_climateAttributes, &onOffAutomaticTempCommands)};
static ZCL_Cluster_t serverClustersOnOffClimate_light[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffMode_lightAttributes, &onOffAutomaticLightCommands)};
static ZCL_Cluster_t serverClustersOnOffLight[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffLightAttributes, &onOffLightCommands)};
static ZCL_Cluster_t serverClustersIlluminance[] = {DEFINE_ILLUMINANCE_MEASUREMENT_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &illuminanceMeasurementAttributes)};
static ZCL_Cluster_t serverClustersTemperature[] = {DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &temperatureMeasurementAttributes)};

/*Liste mit IDs der unterst?tzend Clientcluster.*/
static ClusterId_t clientClusterOnOffLightIds[] = {ONOFF_CLUSTER_ID};

/*Liste mit ZCL_Cluster_t Datenstrukturen, der unterst?tzten ClientCluster.*/
static ZCL_Cluster_t clientClustersOnOffLight[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};

/*CB-Funktion nach Starten des Netzwerkes (ZDO_StartNetworkReq()).*/
void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* confirmInfo){
	if(ZDO_SUCCESS_STATUS == confirmInfo->status){
		appstate=START_REPORTING;
	}
	SYS_PostTask(APL_TASK_ID);
}

static  void ZCL_CommandResp(ZCL_Notify_t *ntfy){
	(void)ntfy;
}

void interruptHandlerINT3(void){
	if(onOffStatusAttributes.onOff.value){
		ZCL_CommandReq(&toggleLightCommand);
		//	bool newValue = false;
		//	ZCL_WriteAttributeValue(srcOnOff_Mode_Light_Server, ONOFF_CLUSTER_ID,  ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,  ZCL_BOOLEAN_DATA_TYPE_ID, (uint8_t*)(&newValue));
		//	setOnOffState(&onOffMode_lightAttributes, true);
		setOnOffState(&onOffMode_lightAttributes, false);
	}
}

// Pin fuer Temperaturmessung vorbereiten
static HAL_I2cDescriptor_t i2cdescriptor={
	.tty = TWI_CHANNEL_0,
	.clockRate = I2C_CLOCK_RATE_62,
	.f = readTempSensorDoneCb,
	.id = LM73_DEVICE_ADDRESS,
	.data = lm73Data,
	.length = 2,
	.lengthAddr = HAL_NO_INTERNAL_ADDRESS
};

// Pin für Helligkeitsmessung vorbereiten
static HAL_AdcDescriptor_t adcdescriptor={
	.resolution = RESOLUTION_8_BIT,
	.sampleRate = ADC_4800SPS,
	.voltageReference = INTERNAL_1d1V,
	.bufferPointer = &LightData,
	.selectionsAmount = 1,
	.callback = readIlluminanceSensorDoneCb
};

// Bei Timerablauf Temperatur und Helligkeit auslesen
static void sendeTimerFired(){
	HAL_ReadI2cPacket(&i2cdescriptor);
	HAL_ReadAdc(&adcdescriptor, HAL_ADC_CHANNEL1);
}

// Bei Timerablauf Logik-Modulschaltung durchlaufen
static void updateTimerFired(){
	
	if(onOffStatusAttributes.onOff.value){
		if(onOffMode_climateAttributes.onOff.value){
			if((temperatureMeasurementAttributes.measuredValue.value < module.temperatureReference) && (module.LEDRED_status == false)){
				turnOn(LEDRED);
				module.LEDRED_status = true;
				turnOff(FAN);
				module.FAN_status = false;
				turnOff(LEDBLUE);
				module.LEDBLUE_status = false;
			}else if((temperatureMeasurementAttributes.measuredValue.value > module.temperatureReference) && (module.LEDBLUE_status == false)){
				turnOff(LEDRED);
				module.LEDRED_status = false;
				turnOn(FAN);
				module.FAN_status = true;
				turnOn(LEDBLUE);
				module.LEDBLUE_status = true;
			}
		} else {
			turnOff(LEDRED);
			module.LEDRED_status = false;
			turnOff(LEDBLUE);
			module.LEDBLUE_status = false;
			turnOff(FAN);
			module.FAN_status = false;
		}		
		if(onOffMode_lightAttributes.onOff.value){
			if((illuminanceMeasurementAttributes.measuredValue.value > module.illuminanceReference) && (module.LEDWHITE_status == false)){
				setOnOffState(&onOffLightAttributes, true);			
				onPWMOutput(&LEDWHITE);				
			}else if((illuminanceMeasurementAttributes.measuredValue.value < module.illuminanceReference) && (module.LEDWHITE_status == true)){				
				setOnOffState(&onOffLightAttributes, false);
				offPWMOutput(&LEDWHITE);				
			}
		}
	} else {
		turnOff(LEDRED);
		module.LEDRED_status = false;
		turnOff(LEDBLUE);
		module.LEDBLUE_status = false;
		turnOff(FAN);
		module.FAN_status = false;
		
		setOnOffState(&onOffLightAttributes, false);
		offPWMOutput(&LEDWHITE);	
	}
	
}

void readIlluminanceSensorDoneCb(){
	illuminanceMeasurementAttributes.measuredValue.value = LightData;	
}

int16_t calcTemperature(void){
	int16_t i; // Int zum Verarbeiten des Temperaturwertes
	int16_t j; // Int zum Verarbeiten des Temperaturwertes
	i = lm73Data[0];
	i <<= 8;
	i |= lm73Data[1];
	i >>= 7;
	j = i*100;
	i = lm73Data[1] & (0x7F);
	i >>= 5;
	i = i*25;
	j = j+i;
	return j;
}

// Temperturwert konvertieren und in Temperaturclusterattribut schreiben. Die ersten 9 bits sind der vorkommawert und bit 10+11 der Nachkommawert
void readTempSensorDoneCb(bool b){	
	temperatureMeasurementAttributes.measuredValue.value = calcTemperature();
	
}
// Temperaturclusterinit
void clusterInit(void){
	temperatureMeasurementAttributes.measuredValue.value = TEMPERATURE_MEASUREMENT_MEASURED_VALUE_ATTRIBUTE_VALUE;
	temperatureMeasurementAttributes.minMeasuredValue.value = TEMPERATURE_MEASUREMENT_MIN_MEASURED_VALUE_ATTRIBUTE_VALUE;
	temperatureMeasurementAttributes.maxMeasuredValue.value = TEMPERATURE_MEASUREMENT_MAX_MEASURED_VALUE_ATTRIBUTE_VALUE;
	temperatureMeasurementAttributes.tolerance.value = TEMPERATURE_MEASUREMENT_TOLERANCE_ATTRIBUTE_VALUE;
// Illuminanceclusterinit
	illuminanceMeasurementAttributes.measuredValue.value = ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_ATTRIBUTE_VALUE;
	illuminanceMeasurementAttributes.minMeasuredValue.value = ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_ATTRIBUTE_VALUE;
	illuminanceMeasurementAttributes.maxMeasuredValue.value = ILLUMINANCE_MEASUREMENT_MAX_MEASURED_VALUE_ATTRIBUTE_VALUE;
	illuminanceMeasurementAttributes.tolerance.value = ILLUMINANCE_MEASUREMENT_TOLERANCE_ATTRIBUTE_VALUE;
	onOffLightAttributes.onOff.reportableChange = true;
	onOffStatusAttributes.onOff.reportableChange = true;
	onOffMode_climateAttributes.onOff.reportableChange = true;
	onOffMode_lightAttributes.onOff.reportableChange = true;
}


static void initButton(void){
	HAL_RegisterIrq(IRQ_3, IRQ_FALLING_EDGE, interruptHandlerINT3);
	HAL_EnableIrq(IRQ_3);
}

// Timer fuer periodische Sensormessungen und Logik-Modulschaltung initialisieren
static void initTimer(){
	sendeTimer.interval = 2000;
	sendeTimer.mode = TIMER_REPEAT_MODE;
	sendeTimer.callback = sendeTimerFired;
	HAL_StartAppTimer(&sendeTimer);
	
	updateTimer.interval = 2000;
	updateTimer.mode = TIMER_REPEAT_MODE;
	updateTimer.callback = updateTimerFired;
	HAL_StartAppTimer(&updateTimer);
}

void initReport(){
	reportLightAttrElement->id = ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID;
	reportLightAttrElement->type = ZCL_BOOLEAN_DATA_TYPE_ID;
	reportLightAttrElement->value[0] = onOffLightAttributes.onOff.value;

	reportStatusAttrElement->id = ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID;
	reportStatusAttrElement->type = ZCL_BOOLEAN_DATA_TYPE_ID;
	reportStatusAttrElement->value[0] = onOffStatusAttributes.onOff.value;
	
	reportModeLightAttrElement->id = ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID;
	reportModeLightAttrElement->type = ZCL_BOOLEAN_DATA_TYPE_ID;
	reportModeLightAttrElement->value[0] = onOffMode_lightAttributes.onOff.value;
	
	reportModeClimateAttrElement->id = ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID;
	reportModeClimateAttrElement->type = ZCL_BOOLEAN_DATA_TYPE_ID;
	reportModeClimateAttrElement->value[0] = onOffMode_climateAttributes.onOff.value;
}


/* Initialisierung der Outputs (LEDS, Luefter) */
/* PINS: PE7 = rote LED , PE2 = blaue LED ,PE4 = weisse LED, PE3 = Luefter */
static void initOutputs(){
	
	/* PINS als Ausgaenge deklarieren */
	DDRE |= (1<<PE2);
	DDRE |= (1<<PE3);
	DDRE |= (1<<PE4);
	DDRE |= (1<<PE7);
	
	/* init Timer3 in fast PWM mode 
	   Timer Clock = CPU Clock (No Prescaleing)
	   Compare Output Mode = Clear OC3B and OC3A on compare match with TCNT3 (Value = 2)
	   Mode        = Fast PWM (Value = 3)
	   PWM Output  = Non Inverted                */
	TCCR3A |= (1<<WGM00)|(1<<WGM01)|(1<<COM3B1);
	TCCR3B |= (1<<CS30);
	
	/* Alle Ausgaenge im Ausgangszustand ausschalten */
	turnOff(LEDBLUE);
	turnOff(LEDRED);
}

static void initModule(){
	module.ID = MODULE_ID;
	setOnOffState(&onOffLightAttributes, false);
	onOffStatusAttributes.onOff.value = false;
	onOffMode_climateAttributes.onOff.value = false;
	onOffMode_lightAttributes.onOff.value = false;
	module.illuminanceReference = 100;
	module.temperatureReference = 3000; // Wert in °C: /100
	module.LEDWHITE_status = false;
	module.LEDWHITE_power = 0;
	LEDWHITE = 0;
	setPWMOutputDuty(&LEDWHITE, 255);		// Wert zwischen 0 und 255 (0 = 0%, 255 = 100%)
	module.LEDBLUE_status = false;
	module.LEDRED_status = false;
	module.FAN_status = false;
	offPWMOutput(&LEDWHITE);
}

/*Initialisierung der Endpunkte zur Datenkommunikation*/
static void initEndpoints(){

	endPointOnOffStatusServer.simpleDescriptor.AppDeviceId =1;
	endPointOnOffStatusServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffStatusServer.simpleDescriptor.endpoint = srcOnOff_Status_Server;
	endPointOnOffStatusServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffStatusServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterOnOffStatusId);
	endPointOnOffStatusServer.simpleDescriptor.AppInClustersList = serverClusterOnOffStatusId;
	endPointOnOffStatusServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointOnOffStatusServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointOnOffStatusServer.serverCluster = serverClustersOnOffStatus;
	endPointOnOffStatusServer.clientCluster = NULL;

	endPointOnOffMode_climateServer.simpleDescriptor.AppDeviceId =1;
	endPointOnOffMode_climateServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffMode_climateServer.simpleDescriptor.endpoint = srcOnOff_Mode_Climate_Server;
	endPointOnOffMode_climateServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffMode_climateServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterOnOffMode_climateId);
	endPointOnOffMode_climateServer.simpleDescriptor.AppInClustersList = serverClusterOnOffMode_climateId;
	endPointOnOffMode_climateServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointOnOffMode_climateServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointOnOffMode_climateServer.serverCluster = serverClustersOnOffMode_climate;
	endPointOnOffMode_climateServer.clientCluster = NULL;

	endPointOnOffMode_lightServer.simpleDescriptor.AppDeviceId =1;
	endPointOnOffMode_lightServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffMode_lightServer.simpleDescriptor.endpoint = srcOnOff_Mode_Light_Server;
	endPointOnOffMode_lightServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffMode_lightServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterOnOffMode_lightId);
	endPointOnOffMode_lightServer.simpleDescriptor.AppInClustersList = serverClusterOnOffMode_lightId;
	endPointOnOffMode_lightServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointOnOffMode_lightServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointOnOffMode_lightServer.serverCluster = serverClustersOnOffClimate_light;
	endPointOnOffMode_lightServer.clientCluster = NULL;
	
	endPointAppOnOffLightServer.simpleDescriptor.AppDeviceId =1;
	endPointAppOnOffLightServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointAppOnOffLightServer.simpleDescriptor.endpoint = srcApp_OnOff_Light_Server;
	endPointAppOnOffLightServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointAppOnOffLightServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterOnOffLightId);
	endPointAppOnOffLightServer.simpleDescriptor.AppInClustersList = serverClusterOnOffLightId;
	endPointAppOnOffLightServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointAppOnOffLightServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointAppOnOffLightServer.serverCluster = serverClustersOnOffLight;
	endPointAppOnOffLightServer.clientCluster = NULL;
	
	endPointOnOffLightServer.simpleDescriptor.AppDeviceId =1;
	endPointOnOffLightServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffLightServer.simpleDescriptor.endpoint = srcOnOff_Light_Server;
	endPointOnOffLightServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffLightServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterOnOffLightId);
	endPointOnOffLightServer.simpleDescriptor.AppInClustersList = serverClusterOnOffLightId;
	endPointOnOffLightServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointOnOffLightServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointOnOffLightServer.serverCluster = serverClustersOnOffLight;
	endPointOnOffLightServer.clientCluster = NULL;
	
	endPointIlluminanceServer.simpleDescriptor.AppDeviceId =1;
	endPointIlluminanceServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointIlluminanceServer.simpleDescriptor.endpoint = srcIlluminance_Measurement_Server;
	endPointIlluminanceServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointIlluminanceServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterIlluminanceId);
	endPointIlluminanceServer.simpleDescriptor.AppInClustersList = serverClusterIlluminanceId;
	endPointIlluminanceServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointIlluminanceServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointIlluminanceServer.serverCluster = serverClustersIlluminance;
	endPointIlluminanceServer.clientCluster = NULL;

	endPointTemperatureMeasurementServer.simpleDescriptor.AppDeviceId =1;
	endPointTemperatureMeasurementServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointTemperatureMeasurementServer.simpleDescriptor.endpoint = srcTemperature_Measurement_Server;
	endPointTemperatureMeasurementServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointTemperatureMeasurementServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterTemperatureId);
	endPointTemperatureMeasurementServer.simpleDescriptor.AppInClustersList = serverClusterTemperatureId;
	endPointTemperatureMeasurementServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointTemperatureMeasurementServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointTemperatureMeasurementServer.serverCluster = serverClustersTemperature;
	endPointTemperatureMeasurementServer.clientCluster = NULL;
	
	endPointOnOffLightClient.simpleDescriptor.AppDeviceId =1;
	endPointOnOffLightClient.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffLightClient.simpleDescriptor.endpoint = srcOnOff_Light_Client;
	endPointOnOffLightClient.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffLightClient.simpleDescriptor.AppInClustersCount = 0;
	endPointOnOffLightClient.simpleDescriptor.AppInClustersList = NULL;
	endPointOnOffLightClient.simpleDescriptor.AppOutClustersCount = ARRAY_SIZE(clientClusterOnOffLightIds);
	endPointOnOffLightClient.simpleDescriptor.AppOutClustersList = clientClusterOnOffLightIds;
	endPointOnOffLightClient.serverCluster = NULL;
	endPointOnOffLightClient.clientCluster = clientClustersOnOffLight;
	
	// Endpoints for receiving values for controlling temperature / illuminance
	simpleDescriptorTemp.AppDeviceId = 1;
	simpleDescriptorTemp.AppProfileId = 1;
	simpleDescriptorTemp.endpoint = srcTemperature_Zielwert;
	simpleDescriptorTemp.AppDeviceVersion = 1;
	endPointTemperatureZielwert.simpleDescriptor = &simpleDescriptorTemp;
	endPointTemperatureZielwert.APS_DataInd = APS_DataIndTemp;
	APS_RegisterEndpointReq(&endPointTemperatureZielwert);

	simpleDescriptorIlluminance.AppDeviceId = 1;
	simpleDescriptorIlluminance.AppProfileId = 1;
	simpleDescriptorIlluminance.endpoint = srcIlluminance_Zielwert;
	simpleDescriptorIlluminance.AppDeviceVersion = 1;
	endPointIlluminanceZielwert.simpleDescriptor = &simpleDescriptorIlluminance;
	endPointIlluminanceZielwert.APS_DataInd = APS_DataIndIlluminance;
	APS_RegisterEndpointReq(&endPointIlluminanceZielwert);
	
	ZCL_RegisterEndpoint(&endPointTemperatureMeasurementServer);
	ZCL_RegisterEndpoint(&endPointOnOffLightServer);
	ZCL_RegisterEndpoint(&endPointOnOffStatusServer);
	ZCL_RegisterEndpoint(&endPointOnOffMode_climateServer);
	ZCL_RegisterEndpoint(&endPointOnOffMode_lightServer);
	ZCL_RegisterEndpoint(&endPointIlluminanceServer);
	ZCL_RegisterEndpoint(&endPointOnOffLightClient);
	ZCL_RegisterEndpoint(&endPointAppOnOffLightServer);
}

// Binding fuer Tempertur und OnOff initialisieren
void initBinding(void){
	CS_ReadParameter(CS_UID_ID, &bindIlluminance.srcAddr);
	
	bindIlluminance.srcEndpoint = srcIlluminance_Measurement_Server;
	bindIlluminance.clusterId = ILLUMINANCE_MEASUREMENT_CLUSTER_ID;
	bindIlluminance.dstAddrMode = APS_EXT_ADDRESS;
	bindIlluminance.dst.unicast.extAddr  =  0x50000000A04LL;
	bindIlluminance.dst.unicast.endpoint = dstIlluminance_Measurement_Client;
	
	APS_BindReq(&bindIlluminance);
	
	CS_ReadParameter(CS_UID_ID, &bindTemp.srcAddr); //eigene Adresse lesen und schreiben

	bindTemp.srcEndpoint = srcTemperature_Measurement_Server;
	bindTemp.clusterId   = TEMPERATURE_MEASUREMENT_CLUSTER_ID;
	bindTemp.dstAddrMode = APS_EXT_ADDRESS;
	bindTemp.dst.unicast.extAddr  =   0x50000000A04LL;
	bindTemp.dst.unicast.endpoint = dstTemperature_Measurement_Client;

	APS_BindReq(&bindTemp); //local binding ausf?hren
	
	CS_ReadParameter(CS_UID_ID, &bindOnOffLight.srcAddr); //eigene Adresse lesen und schreiben

	bindOnOffLight.srcEndpoint = srcOnOff_Light_Server;
	bindOnOffLight.clusterId   = ONOFF_CLUSTER_ID;
	bindOnOffLight.dstAddrMode = APS_EXT_ADDRESS;
	bindOnOffLight.dst.unicast.extAddr  =  0x50000000A04LL;
	bindOnOffLight.dst.unicast.endpoint = dstOnOff_Light_Client;

	APS_BindReq(&bindOnOffLight); //local binding ausf?hren
	

	CS_ReadParameter(CS_UID_ID, &bindOnOffStatus.srcAddr); //eigene Adresse lesen und schreiben

	bindOnOffStatus.srcEndpoint = srcOnOff_Status_Server;
	bindOnOffStatus.clusterId   = ONOFF_CLUSTER_ID;
	bindOnOffStatus.dstAddrMode = APS_EXT_ADDRESS;
	bindOnOffStatus.dst.unicast.extAddr  =  0x50000000A04LL;
	bindOnOffStatus.dst.unicast.endpoint = dstOnOff_Status_Client;

	APS_BindReq(&bindOnOffStatus); //local binding ausf?hren
	
	CS_ReadParameter(CS_UID_ID, &bindOnOffmode_climate.srcAddr); //eigene Adresse lesen und schreiben

	bindOnOffmode_climate.srcEndpoint = srcOnOff_Mode_Climate_Server;
	bindOnOffmode_climate.clusterId   = ONOFF_CLUSTER_ID;
	bindOnOffmode_climate.dstAddrMode = APS_EXT_ADDRESS;
	bindOnOffmode_climate.dst.unicast.extAddr  =  0x50000000A04LL;
	bindOnOffmode_climate.dst.unicast.endpoint = dstOnOff_Mode_Climate_Client;

	APS_BindReq(&bindOnOffmode_climate); //local binding ausf?hren
	
	CS_ReadParameter(CS_UID_ID, &bindOnOffmode_light.srcAddr); //eigene Adresse lesen und schreiben

	bindOnOffmode_light.srcEndpoint = srcOnOff_Mode_Light_Server;
	bindOnOffmode_light.clusterId   = ONOFF_CLUSTER_ID;
	bindOnOffmode_light.dstAddrMode = APS_EXT_ADDRESS;
	bindOnOffmode_light.dst.unicast.extAddr  =  0x50000000A04LL;
	bindOnOffmode_light.dst.unicast.endpoint = dstOnOff_Mode_Light_Client;

	APS_BindReq(&bindOnOffmode_light); //local binding ausf?hren
}

static ZCL_Request_t lightAttrReq = {
	.id = ZCL_REPORT_ATTRIBUTES_COMMAND_ID,
	.ZCL_Notify = ReportNotify,
	.dstAddressing.addrMode = APS_EXT_ADDRESS,
	.dstAddressing.addr.extAddress = 0x50000000A04LL,
	.endpointId = srcOnOff_Light_Server,
	.dstAddressing.clusterId = ONOFF_CLUSTER_ID,
	.dstAddressing.profileId = PROFILE_ID_HOME_AUTOMATION,
	.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_CLIENT,
	.dstAddressing.endpointId = dstOnOff_Light_Client,
	.requestLength = sizeof(ZCL_Report_t),
	.requestPayload = lightAttrBuff,
	.defaultResponse = ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE,
};

static ZCL_Request_t statusAttrReq = {
	.id = ZCL_REPORT_ATTRIBUTES_COMMAND_ID,
	.ZCL_Notify = ReportNotify,
	.dstAddressing.addrMode = APS_EXT_ADDRESS,
	.dstAddressing.addr.extAddress = 0x50000000A04LL,
	.endpointId = srcOnOff_Status_Server,
	.dstAddressing.clusterId = ONOFF_CLUSTER_ID,
	.dstAddressing.profileId = PROFILE_ID_HOME_AUTOMATION,
	.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_CLIENT,
	.dstAddressing.endpointId = dstOnOff_Status_Client,
	.requestLength = sizeof(ZCL_Report_t),
	.requestPayload = statusAttrBuff,
	.defaultResponse = ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE,
};

static ZCL_Request_t modeLightAttrReq = {
	.id = ZCL_REPORT_ATTRIBUTES_COMMAND_ID,
	.ZCL_Notify = ReportNotify,
	.dstAddressing.addrMode = APS_EXT_ADDRESS,
	.dstAddressing.addr.extAddress = 0x50000000A04LL,
	.endpointId = srcOnOff_Mode_Light_Server,
	.dstAddressing.clusterId = ONOFF_CLUSTER_ID,
	.dstAddressing.profileId = PROFILE_ID_HOME_AUTOMATION,
	.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_CLIENT,
	.dstAddressing.endpointId = dstOnOff_Mode_Light_Client,
	.requestLength = sizeof(ZCL_Report_t),
	.requestPayload = modeLightAttrBuff,
	.defaultResponse = ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE,
};


static ZCL_Request_t modeClimateAttrReq = {
	.id = ZCL_REPORT_ATTRIBUTES_COMMAND_ID,
	.ZCL_Notify = ReportNotify,
	.dstAddressing.addrMode = APS_EXT_ADDRESS,
	.dstAddressing.addr.extAddress = 0x50000000A04LL,
	.endpointId = srcOnOff_Mode_Climate_Server,
	.dstAddressing.clusterId = ONOFF_CLUSTER_ID,
	.dstAddressing.profileId = PROFILE_ID_HOME_AUTOMATION,
	.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_CLIENT,
	.dstAddressing.endpointId = dstOnOff_Mode_Climate_Client,
	.requestLength = sizeof(ZCL_Report_t),
	.requestPayload = modeClimateAttrBuff,
	.defaultResponse = ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE,
};

//Initial, first Report for light, status and modus because of onChange reports
void ReportNotify(){
	
}

void APS_DataIndTemp(APS_DataInd_t *indData){
	int16_t sum;	
	sum = indData->asdu[0]*1000;
	sum = sum + indData->asdu[1]*100;
	sum = sum + indData->asdu[2]*10;
	sum = sum + indData->asdu[3];
	module.temperatureReference = sum;
}

void APS_DataIndIlluminance(APS_DataInd_t *indData){
	int16_t sum;
	sum = indData->asdu[0]*100;
	sum = sum + indData->asdu[1]*10;
	sum = sum + indData->asdu[2];		
	module.illuminanceReference = sum;
}
void wait()
{
	_delay_loop_2(10000);
}



static void initKommando(void){
	toggleLightCommand.dstAddressing.addrMode=APS_EXT_ADDRESS;
	toggleLightCommand.dstAddressing.addr.extAddress = 0x50000000A02LL;
	toggleLightCommand.dstAddressing.profileId=0x0104;
	toggleLightCommand.dstAddressing.endpointId=srcOnOff_Light_Server;
	toggleLightCommand.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	toggleLightCommand.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;

	toggleLightCommand.endpointId=srcOnOff_Light_Client;
	toggleLightCommand.id=ZCL_ONOFF_CLUSTER_TOGGLE_COMMAND_ID;
	toggleLightCommand.ZCL_Notify=ZCL_CommandResp;
}

// Funktion mit der man von einem PWM Pin den Output erhöht
static void setPWMOutputDuty(volatile uint16_t* port, uint8_t duty)
{
	if(port == &LEDWHITE){
		module.LEDWHITE_duty = duty;
	}
}

static void onPWMOutput(volatile uint16_t* port){
	for(uint8_t power=*port;power<module.LEDWHITE_duty;power++){
		//Increase the Brightness/Speed using PWM
		*port = power;
		module.LEDWHITE_power = *port;
		//Now Wait For Some Time
		wait();
	}
	*port = 255;
	module.LEDWHITE_status = true;
	module.LEDWHITE_power = 255;
	
}

static void offPWMOutput(volatile uint16_t* port){
	for(uint8_t power=*port;power>0;power--){
		//Decrease The Brightness/Speed using PWM
		*port = power;
		module.LEDWHITE_power = *port;
		//Now Wait For Some Time
		wait();
	}
	*port = 0;
	module.LEDWHITE_power = *port;
	module.LEDWHITE_status = false;
		
	
}

static void togglePWMOutput(volatile uint16_t* port){
	
	if(port == &LEDWHITE){
		if(*port == 0){
			for(uint8_t power=0;power<module.LEDWHITE_duty;power++){
				//Increase the Brightness/Speed using PWM
				*port = power;
				module.LEDWHITE_power = *port;
				//Now Wait For Some Time
				wait();
			}
			module.LEDWHITE_status = true;
		}
		else{
			for(uint8_t power=module.LEDWHITE_duty;power>0;power--){
				//Decrease The Brightness/Speed using PWM
				*port = power;
				module.LEDWHITE_power = *port;
				//Now Wait For Some Time
				wait();
			}
			*port = 0;
			module.LEDWHITE_power = *port;
			module.LEDWHITE_status = false;
		}
	}
	
}


/* On-Kommando erhalten. OnOff-Attribut auf On setzen und LED anschalten. */
ZCL_Status_t onLight(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){	

	setOnOffState(&onOffLightAttributes, true);
	onPWMOutput(&LEDWHITE);

	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

ZCL_Status_t onStatus(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){

	setOnOffState(&onOffStatusAttributes, true);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t onMode_climate(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	
	setOnOffState(&onOffMode_climateAttributes, true);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t onMode_light(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){

	setOnOffState(&onOffMode_lightAttributes, true);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Off-Kommando erhalten. OnOff-Attribut auf Off setzen und LED ausschalten.*/
ZCL_Status_t offLight(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	
	setOnOffState(&onOffLightAttributes, false);
	offPWMOutput(&LEDWHITE);

	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

ZCL_Status_t offStatus(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffStatusAttributes, false);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t offMode_climate(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffMode_climateAttributes, false);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t offMode_light(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffMode_lightAttributes, false);
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Toggle-Kommando erhalten. Zustand des OnOff-Attribut und der LED wechseln. */
ZCL_Status_t toggleLight(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffLightAttributes, !onOffLightAttributes.onOff.value);	
// 	bool newValue = !onOffLightAttributes.onOff.value;
// 	ZCL_WriteAttributeValue(srcOnOff_Light_Server, ONOFF_CLUSTER_ID,  ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,  ZCL_BOOLEAN_DATA_TYPE_ID, (uint8_t*)(&newValue));
	togglePWMOutput(&LEDWHITE);	
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Manipulation des OnOff-Cluster-Attributes OnOff. */
static void setOnOffState(ZCL_OnOffClusterServerAttributes_t* attribute, bool state){
	attribute->onOff.value = state;
	ZCL_ReportOnChangeIfNeeded(attribute);
	
}


void APL_TaskHandler(){
	
	switch(appstate){
		case INIT:
			HAL_OpenI2cPacket(&i2cdescriptor);
			HAL_OpenAdc(&adcdescriptor);
			BSP_OpenLeds();
			initOutputs();
			initKommando();
			initEndpoints();
			clusterInit();
			initModule();
			initTimer();
			initButton();
            initBinding(); 
			initReport();
   			appstate = JOIN_NETWORK;
			SYS_PostTask(APL_TASK_ID);
			break;
			
		case JOIN_NETWORK:
			networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
			ZDO_StartNetworkReq(&networkParams);
			break;
			
		case START_REPORTING:
			ZCL_StartReporting(); //Automatisches Reporting starten
			ZCL_AttributeReq(&lightAttrReq);
			ZCL_AttributeReq(&statusAttrReq);
			ZCL_AttributeReq(&modeLightAttrReq);
			ZCL_AttributeReq(&modeClimateAttrReq);
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
