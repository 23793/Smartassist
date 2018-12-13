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
#include <zclIlluminanceMeasurementCluster.h>
#include <zclTemperatureMeasurementCluster.h>
#include <zcl.h>
#include <irq.h>
#include <i2cPacket.h>
#include <adc.h>
#include <appTimer.h>
#include <util/delay.h>
#include <leds.h>
#include <usartManager.h>
/********************************PROTOTYPEN************************************************/
 
// Variablen fürs Binding
APS_BindReq_t bindTemp; 
APS_BindReq_t bindOnOffLight; 
APS_BindReq_t bindOnOffStatus; 
APS_BindReq_t bindOnOffmode_climate; 
APS_BindReq_t bindOnOffmode_light; 
APS_BindReq_t bindOnOffCooling;
APS_BindReq_t bindOnOffHeating;
APS_BindReq_t bindFanControl; 
APS_BindReq_t bindIlluminance; 

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

//?bergabevariable f?r die Funktion ZDO_StartNetworkReq().
static ZDO_StartNetworkReq_t networkParams;

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
void readTempSensorDoneCb(void);

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
//Status
static ZCL_Status_t toggleStatus(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);
// Modus Temperatur
static ZCL_Status_t toggleMode_climate(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);
// Modus Light
static ZCL_Status_t toggleMode_light(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);


//Funktion wird aufgerufen beim Empfang eines On-Kommandos.
static ZCL_Status_t onCooling(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion wird aufgerufen beim Empfang eines Off-Kommandos.
static ZCL_Status_t offCooling(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion wird aufgerufen beim Empfang eines Toggle-Kommandos.
static ZCL_Status_t toggleCooling(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion wird aufgerufen beim Empfang eines On-Kommandos.
static ZCL_Status_t onHeating(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion wird aufgerufen beim Empfang eines Off-Kommandos.
static ZCL_Status_t offHeating(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion wird aufgerufen beim Empfang eines Toggle-Kommandos.
static ZCL_Status_t toggleHeating(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload);

//Funktion zur PWM von FAN/LEDWHITE
static void setPWMOutputDuty(volatile uint16_t* port, uint8_t duty);

//Funktion zum togglen der PWM von LEDWHITE
static void togglePWMOutput(volatile uint16_t* port);

//Funktion zum einschalten der PWM für LEDWHITE
static void onPWMOutput(volatile uint16_t* port);

//Funktion zum ausschalten der PWM für LEDWHITE
static void offPWMOutput(volatile uint16_t* port);

//Funktion zum togglen von LEDBLUE und LEDRED
static void toggle(volatile uint16_t port);

/*Datenstruktur mit allen Variablen des 
OnOff-Serverclusters (hier nur onOff-Attribut) 
Temperaturemeasurement-Serverclusters
Illuminancy-Servercluster
FanControl-Servercluster
*/
//Intervall (min-max) fuer den Report definieren
static ZCL_OnOffClusterServerAttributes_t onOffLightAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};
static ZCL_OnOffClusterServerAttributes_t onOffStatusAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};
static ZCL_OnOffClusterServerAttributes_t onOffMode_climateAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};
static ZCL_OnOffClusterServerAttributes_t onOffMode_lightAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};
				
static ZCL_TemperatureMeasurementClusterAttributes_t temperatureMeasurementAttributes ={ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(0, 2)};
static ZCL_IlluminanceMeasurementClusterServerAttributes_t illuminanceMeasurementAttributes ={ZCL_DEFINE_ILLUMINANCE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(0, 2)};
static ZCL_OnOffClusterServerAttributes_t onOffCoolingAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};
static ZCL_OnOffClusterServerAttributes_t onOffHeatingAttributes = {ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(2,0)};

/*Datenstruktur in der zu jeder OnOff-KommandoId eine Referenz auf die ausf?hrenden Funktionen gespeichert ist.
*/
static ZCL_OnOffClusterCommands_t onOffLightCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onLight, offLight, toggleLight)};
static ZCL_OnOffClusterCommands_t onOffActivityCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onStatus, offStatus, toggleStatus)};
static ZCL_OnOffClusterCommands_t onOffAutomaticTempCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onMode_climate, offMode_climate, toggleMode_climate)};
static ZCL_OnOffClusterCommands_t onOffAutomaticLightCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onMode_light, offMode_light, toggleMode_light)};
static ZCL_OnOffClusterCommands_t onOffCoolingCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onCooling, offCooling, toggleCooling)};
static ZCL_OnOffClusterCommands_t onOffHeatingCommands = {ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onHeating, offHeating, toggleHeating)};

/*Liste mit IDs der unterst?tzend Servercluster.*/
static ClusterId_t serverClusterTemperatureId[] = {TEMPERATURE_MEASUREMENT_CLUSTER_ID};
static ClusterId_t serverClusterOnOffLightId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterOnOffStatusId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterOnOffMode_climateId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterOnOffMode_lightId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverAppClusterOnOffLightId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterOnOffCoolingId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterOnOffHeatingId[] = {ONOFF_CLUSTER_ID};
static ClusterId_t serverClusterIlluminanceId[] = {ILLUMINANCE_MEASUREMENT_CLUSTER_ID};
		
/*Liste mit ZCL_Cluster_t Datenstrukturen, der unterst?tzten ServerCluster.*/
static ZCL_Cluster_t serverClustersTemperature[] = {DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &temperatureMeasurementAttributes)};
static ZCL_Cluster_t serverClustersOnOffLight[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffLightAttributes, &onOffLightCommands)};
static ZCL_Cluster_t serverClustersOnOffStatus[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffStatusAttributes, &onOffActivityCommands)};
static ZCL_Cluster_t serverClustersOnOffMode_climate[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffMode_climateAttributes, &onOffAutomaticTempCommands)};
static ZCL_Cluster_t serverClustersOnOffClimate_light[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffMode_lightAttributes, &onOffAutomaticLightCommands)};
static ZCL_Cluster_t serverAppClustersOnOffLight[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffLightAttributes, &onOffLightCommands)};
static ZCL_Cluster_t serverClustersOnOffCooling[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffCoolingAttributes, &onOffCoolingCommands)};
static ZCL_Cluster_t serverClustersOnOffHeating[] = {DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &onOffHeatingAttributes, &onOffHeatingCommands)};
static ZCL_Cluster_t serverClustersIlluminance[] = {DEFINE_ILLUMINANCE_MEASUREMENT_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &illuminanceMeasurementAttributes)};
	
/*Liste mit IDs der unterst?tzend Clientcluster.*/
static ClusterId_t clientClusterOnOffLightIds[] = {ONOFF_CLUSTER_ID};
		
/*Liste mit ZCL_Cluster_t Datenstrukturen, der unterst?tzten ClientCluster.*/
static ZCL_Cluster_t clientClustersOnOffLight[]={DEFINE_ONOFF_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, NULL)};
	
//Endpunkt f?r die Registrierung des Endpunktes zur Datenkommunikation
static ZCL_DeviceEndpoint_t endPointTemperatureMeasurementServer;
static ZCL_DeviceEndpoint_t endPointOnOffLightServer;
static ZCL_DeviceEndpoint_t endPointOnOffStatusServer;
static ZCL_DeviceEndpoint_t endPointOnOffMode_climateServer;
static ZCL_DeviceEndpoint_t endPointOnOffMode_lightServer;
static ZCL_DeviceEndpoint_t endPointAppOnOffLightServer;
static ZCL_DeviceEndpoint_t endPointIlluminanceServer;
static ZCL_DeviceEndpoint_t endPointOnOffLightClient;
static ZCL_DeviceEndpoint_t endPointOnOffCoolingServer;
static ZCL_DeviceEndpoint_t endPointOnOffHeatingServer;

// Endpoint und Descriptor zum Empfang der Zielwerte
static SimpleDescriptor_t simpleDescriptorTemp;
static APS_RegisterEndpointReq_t endPointTemperatureZielwert;
void APS_DataIndTemp(APS_DataInd_t *indData);

static SimpleDescriptor_t simpleDescriptorIlluminance;
static APS_RegisterEndpointReq_t endPointIlluminanceZielwert;
void APS_DataIndIlluminance(APS_DataInd_t *indData);

//Funktion zur Initialisierung des Endpunktes
static void initEndpoints();

//Funktion zur Manipulation des OnOff-Cluster-Attributes OnOff.
static void setOnOffState(ZCL_OnOffClusterServerAttributes_t* attribute, bool state);

//Funktion zur Initialisierung der Outputs (LEDs, L?fter)
static void initOutputs();

// Notifikation nachdem ein Kommando zum Server geschickt wurde
static void ZCL_CommandResp(ZCL_Notify_t *ntfy);

// Initialisierung von Kommandos vom CLient zum Server
static void initKommando(void);

// Funktion zum Initialisieren wenn ein Button gedrückt wurde
static void initButton(void);

// Kommando von Client zu Server schicken, wenn Button gedrückt wurde
static void interruptHandlerINT3(void);


/**********************************PROTOTYPEN-ENDE**********************************************/

/**********************************IMPLEMENTIERUNG**********************************************/
static void initKommando(void){
	toggleLightCommand.dstAddressing.addrMode=APS_EXT_ADDRESS;
	toggleLightCommand.dstAddressing.addr.extAddress = 0x50000000A01LL;
	toggleLightCommand.dstAddressing.profileId=0x0104;
	toggleLightCommand.dstAddressing.endpointId=srcOnOff_Light_Server;
	toggleLightCommand.dstAddressing.clusterId=ONOFF_CLUSTER_ID;
	toggleLightCommand.dstAddressing.clusterSide=ZCL_CLUSTER_SIDE_SERVER;

	toggleLightCommand.endpointId=srcOnOff_Light_Client;
	toggleLightCommand.id=ZCL_ONOFF_CLUSTER_TOGGLE_COMMAND_ID;
	toggleLightCommand.ZCL_Notify=ZCL_CommandResp;
}
static void ZCL_CommandResp(ZCL_Notify_t *ntfy){
	(void)ntfy;
}
static void initButton(void){
	HAL_RegisterIrq(IRQ_3, IRQ_FALLING_EDGE, interruptHandlerINT3);
	HAL_EnableIrq(IRQ_3);
}

void interruptHandlerINT3(void){
	ZCL_CommandReq(&toggleLightCommand);
}

// Pin f?r Temperaturmessung vorbereiten
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

// Timer f?r periodische Sensormessungen initialisieren
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

// Bei Timerablauf Temperatur und Helligkeit auslesen
static void sendeTimerFired(){
	HAL_ReadI2cPacket(&i2cdescriptor);
	HAL_ReadAdc(&adcdescriptor, HAL_ADC_CHANNEL1);
}

// Bei Timerablauf Temperatur und Helligkeit auslesen
static void updateTimerFired(){
	
	if(module.status){
		if(module.mode_climate){
			double vorkomma;
			vorkomma = temperatureMeasurementAttributes.measuredValue.value/100;
			double nachkomma;
			nachkomma = temperatureMeasurementAttributes.measuredValue.value%100;
	
			module.temperatureValue = vorkomma + nachkomma/100;
	
			if((module.temperatureValue < module.temperatureReference) && (module.LEDRED_status == false)){
				setOnOffState(&onOffHeatingAttributes, true);
				setOnOffState(&onOffCoolingAttributes, false);
				turnOn(LEDRED);
				module.LEDRED_status = true;
				turnOff(FAN);
				module.FAN_status = false;
				turnOff(LEDBLUE);
				module.LEDBLUE_status = false;
			}else if((module.temperatureValue > module.temperatureReference) && (module.LEDRED_status == true)){
				setOnOffState(&onOffHeatingAttributes, false);
				setOnOffState(&onOffCoolingAttributes, true);
				turnOff(LEDRED);
				module.LEDRED_status = false;
				turnOn(FAN);
				module.FAN_status = true;
				turnOn(LEDBLUE);
				module.LEDBLUE_status = true;
			}
		} else {
			setOnOffState(&onOffHeatingAttributes, false);
			setOnOffState(&onOffCoolingAttributes, false);
			turnOff(LEDRED);
			module.LEDRED_status = false;
			turnOff(LEDBLUE);
			module.LEDBLUE_status = false;
			turnOff(FAN);
			module.FAN_status = false;
		}
		
		if(module.mode_light){
			module.illuminanceValue = illuminanceMeasurementAttributes.measuredValue.value;
			if((illuminanceMeasurementAttributes.measuredValue.value > module.illuminanceReference) && (module.LEDWHITE_status == false)){
				setOnOffState(&onOffLightAttributes, true);
				onPWMOutput(&LEDWHITE);
				}else if((illuminanceMeasurementAttributes.measuredValue.value < module.illuminanceReference) && (module.LEDWHITE_status == true)){				
				setOnOffState(&onOffLightAttributes, false);
				offPWMOutput(&LEDWHITE);
			}
		}
	} else {
		setOnOffState(&onOffHeatingAttributes, false);
		setOnOffState(&onOffCoolingAttributes, false);
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
	BSP_ToggleLed(LED_RED);
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
void readTempSensorDoneCb(){
	
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
	onOffCoolingAttributes.onOff.reportableChange = true;
	onOffHeatingAttributes.onOff.reportableChange = true;
}
//Initial, first Report
void statusReportNotify(){
	
}

static uint8_t statusAttrBuff[STATUS_ATTRIBUTE_BUFFER_SIZE];
ZCL_Report_t *reportAttrElement = (ZCL_Report_t*) statusAttrBuff;
void initReport(){
	reportAttrElement->id = ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID;
	reportAttrElement->type = ZCL_BOOLEAN_DATA_TYPE_ID;
	reportAttrElement->value[0] = onOffStatusAttributes.onOff.value;
}
static ZCL_Request_t statusAttrReq = {
	.id = ZCL_REPORT_ATTRIBUTES_COMMAND_ID,
	.ZCL_Notify = statusReportNotify,
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

// Binding f?r Tempertur und OnOff initialisieren
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
	
	CS_ReadParameter(CS_UID_ID, &bindOnOffCooling.srcAddr); //eigene Adresse lesen und schreiben

	bindOnOffCooling.srcEndpoint = srcOnOff_COOLING_Server;
	bindOnOffCooling.clusterId   = ONOFF_CLUSTER_ID;
	bindOnOffCooling.dstAddrMode = APS_EXT_ADDRESS;
	bindOnOffCooling.dst.unicast.extAddr  =  0x50000000A04LL;
	bindOnOffCooling.dst.unicast.endpoint = 2;

	APS_BindReq(&bindOnOffCooling); //local binding ausf?hren
	
	CS_ReadParameter(CS_UID_ID, &bindOnOffHeating.srcAddr); //eigene Adresse lesen und schreiben

	bindOnOffHeating.srcEndpoint = srcOnOff_HEATING_Server;
	bindOnOffHeating.clusterId   = ONOFF_CLUSTER_ID;
	bindOnOffHeating.dstAddrMode = APS_EXT_ADDRESS;
	bindOnOffHeating.dst.unicast.extAddr  =  0x50000000A04LL;
	bindOnOffHeating.dst.unicast.endpoint = 2;

	APS_BindReq(&bindOnOffHeating); //local binding ausf?hren
///	
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
	bindOnOffmode_climate.dst.unicast.endpoint = dstOnOff_Mode_Climate;

	APS_BindReq(&bindOnOffmode_climate); //local binding ausf?hren
	
	CS_ReadParameter(CS_UID_ID, &bindOnOffmode_light.srcAddr); //eigene Adresse lesen und schreiben

	bindOnOffmode_light.srcEndpoint = srcOnOff_Mode_Light_Server;
	bindOnOffmode_light.clusterId   = ONOFF_CLUSTER_ID;
	bindOnOffmode_light.dstAddrMode = APS_EXT_ADDRESS;
	bindOnOffmode_light.dst.unicast.extAddr  =  0x50000000A04LL;
	bindOnOffmode_light.dst.unicast.endpoint = dstOnOff_Mode_Light_Client;

	APS_BindReq(&bindOnOffmode_light); //local binding ausf?hren
}

static void initModule(){
	module.ID = MODULE_ID;
	module.status = true;
	onOffStatusAttributes.onOff.value = true;
	module.mode_climate = true;
	module.mode_light = true;
	module.illuminanceReference = 100;
	module.illuminanceValue = temperatureMeasurementAttributes.measuredValue.value;
	module.temperatureReference = 30.00;
	module.temperatureValue = illuminanceMeasurementAttributes.measuredValue.value;
	module.LEDWHITE_status = false;
	module.LEDWHITE_power = OCR3B = 0;
	setPWMOutputDuty(&LEDWHITE, 255);		// Wert zwischen 0 und 255 (0 = 0%, 255 = 100%)
	module.LEDBLUE_status = false;
	module.LEDRED_status = false;
	module.FAN_status = false;
}

/*Initialisierung der Endpunkte zur Datenkommunikation*/
static void initEndpoints(){
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
	
	endPointAppOnOffLightServer.simpleDescriptor.AppDeviceId =1;
	endPointAppOnOffLightServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointAppOnOffLightServer.simpleDescriptor.endpoint = srcApp_OnOff_Light_Server;
	endPointAppOnOffLightServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointAppOnOffLightServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverAppClusterOnOffLightId);
	endPointAppOnOffLightServer.simpleDescriptor.AppInClustersList = serverAppClusterOnOffLightId;
	endPointAppOnOffLightServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointAppOnOffLightServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointAppOnOffLightServer.serverCluster = serverAppClustersOnOffLight;
	endPointAppOnOffLightServer.clientCluster = NULL;
	
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


	
	endPointOnOffCoolingServer.simpleDescriptor.AppDeviceId =1;
	endPointOnOffCoolingServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffCoolingServer.simpleDescriptor.endpoint = srcOnOff_COOLING_Server;
	endPointOnOffCoolingServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffCoolingServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterOnOffCoolingId);
	endPointOnOffCoolingServer.simpleDescriptor.AppInClustersList = serverClusterOnOffCoolingId;
	endPointOnOffCoolingServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointOnOffCoolingServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointOnOffCoolingServer.serverCluster = serverClustersOnOffCooling;
	endPointOnOffCoolingServer.clientCluster = NULL;
	
	endPointOnOffHeatingServer.simpleDescriptor.AppDeviceId =1;
	endPointOnOffHeatingServer.simpleDescriptor.AppProfileId = 0x0104;
	endPointOnOffHeatingServer.simpleDescriptor.endpoint = srcOnOff_HEATING_Server;
	endPointOnOffHeatingServer.simpleDescriptor.AppDeviceVersion = 1;
	endPointOnOffHeatingServer.simpleDescriptor.AppInClustersCount = ARRAY_SIZE(serverClusterOnOffHeatingId);
	endPointOnOffHeatingServer.simpleDescriptor.AppInClustersList = serverClusterOnOffHeatingId;
	endPointOnOffHeatingServer.simpleDescriptor.AppOutClustersCount = 0;
	endPointOnOffHeatingServer.simpleDescriptor.AppOutClustersList = NULL;
	endPointOnOffHeatingServer.serverCluster = serverClustersOnOffHeating;
	endPointOnOffHeatingServer.clientCluster = NULL;
	
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
}

void APS_DataIndTemp(APS_DataInd_t *indData){
	BSP_ToggleLed(LED_YELLOW);	
	module.temperatureReference = ((int16_t) indData->asdu/100 + (int16_t)indData->asdu%100)/100;
}
void APS_DataIndIlluminance(APS_DataInd_t *indData){
	BSP_ToggleLed(LED_YELLOW);
	module.illuminanceReference = (uint16_t) indData->asdu;
}
void wait()
{
	_delay_loop_2(10000);
}

// Funktion mit der man von einem PWM Pin den Output erhöht
static void setPWMOutputDuty(volatile uint16_t* port, uint8_t duty)
{
	if(port == &LEDWHITE){
		module.LEDWHITE_duty = duty;
	}
}

static void onPWMOutput(volatile uint16_t* port){
	
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
	}
}

static void offPWMOutput(volatile uint16_t* port){
	
	if(port == &LEDWHITE){
		if(*port == module.LEDWHITE_power){
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

static void toggle(volatile uint16_t port){
	
	if(port == LEDBLUE){
		if(PINE & (1<<LEDBLUE)){			// if LEDBLUE in PINE is set
			PORTE &= ~(1<<LEDBLUE);
			module.LEDBLUE_status = false;	// turn off
		}
		else{
			PORTE |= (1<<LEDBLUE);		// tun on
			module.LEDBLUE_status = true;
		}
	}
	else if(port == LEDRED){
		if(PINE & (1<<LEDRED)){			// if LEDBLUE in PINE is set
			PORTE &= ~(1<<LEDRED);
			module.LEDRED_status = false;	// turn off
		}
		else{
			PORTE |= (1<<LEDRED);		// tun on
			module.LEDRED_status = true;
		}
	}
	else{
		if(PINE & (1<<FAN)){			// if LEDBLUE in PINE is set
			PORTE &= ~(1<<FAN);
			module.FAN_status = false;	// turn off
		}
		else{
			PORTE |= (1<<FAN);		// tun on
			module.FAN_status = true;
		}
	}
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

/* On-Kommando erhalten. OnOff-Attribut auf On setzen und LED anschalten. */
ZCL_Status_t onLight(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffLightAttributes, true);
	onPWMOutput(&LEDWHITE);
	module.mode_light = false;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t onStatus(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	module.status = true;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t onMode_climate(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	module.mode_climate = true;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t onMode_light(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	module.mode_light = true;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
/* On-Kommando erhalten. OnOff-Attribut auf On setzen und LED anschalten. */
ZCL_Status_t onCooling(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffCoolingAttributes, true);
	turnOn(LEDBLUE);
	turnOn(FAN);
	module.LEDBLUE_status = true;
	module.FAN_status = true;
	
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* On-Kommando erhalten. OnOff-Attribut auf On setzen und LED anschalten. */
ZCL_Status_t onHeating(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffHeatingAttributes, true);
	turnOn(LEDRED);
	module.LEDRED_status = true;
	
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Off-Kommando erhalten. OnOff-Attribut auf Off setzen und LED ausschalten.*/
ZCL_Status_t offLight(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffLightAttributes, false);
	offPWMOutput(&LEDWHITE);
	module.mode_light = false;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t offStatus(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	module.status = false;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t offMode_climate(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	module.mode_climate = false;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t offMode_light(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	module.mode_light = false;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
/* Off-Kommando erhalten. OnOff-Attribut auf Off setzen und LED ausschalten.*/
ZCL_Status_t offHeating(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffHeatingAttributes, false);
	module.LEDRED_status = false;
	turnOff(LEDRED);
	
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Off-Kommando erhalten. OnOff-Attribut auf Off setzen und LED ausschalten.*/
ZCL_Status_t offCooling(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffCoolingAttributes, false);
	module.LEDBLUE_status = false;
	module.FAN_status = false;
	turnOff(LEDBLUE);
	turnOff(FAN);
	
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Toggle-Kommando erhalten. Zustand des OnOff-Attribut und der LED wechseln. */
ZCL_Status_t toggleHeating(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffHeatingAttributes, !onOffHeatingAttributes.onOff.value);
	toggle(LEDRED);
	
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Toggle-Kommando erhalten. Zustand des OnOff-Attribut und der LED wechseln. */
ZCL_Status_t toggleCooling(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffCoolingAttributes, !onOffCoolingAttributes.onOff.value);
	toggle(LEDBLUE);
	toggle(FAN);
	
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Toggle-Kommando erhalten. Zustand des OnOff-Attribut und der LED wechseln. */
ZCL_Status_t toggleLight(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	setOnOffState(&onOffLightAttributes, !onOffLightAttributes.onOff.value);	
	togglePWMOutput(&LEDWHITE);	
	module.mode_light = false;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t toggleStatus(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	module.status = !module.status;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t toggleMode_climate(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	module.mode_climate = !module.mode_climate;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}
ZCL_Status_t toggleMode_light(ZCL_Addressing_t* addressing, uint8_t payloadLength, uint8_t* payload){
	module.mode_light = !module.mode_light;
	(void)addressing, (void)payloadLength, (void)payload;
	return ZCL_SUCCESS_STATUS;
}

/* Manipulation des OnOff-Cluster-Attributes OnOff. */
static void setOnOffState(ZCL_OnOffClusterServerAttributes_t* attribute, bool state){
	attribute->onOff.value = state;
	ZCL_ReportOnChangeIfNeeded(attribute);
	
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
			appInitUsartManager();
			BSP_OpenLeds();
			initOutputs();
			initKommando();
			initEndpoints();
            HAL_OpenI2cPacket(&i2cdescriptor);
			HAL_OpenAdc(&adcdescriptor);
			appstate = START_NETWORK;
			SYS_PostTask(APL_TASK_ID);
			break;
			
		case START_NETWORK:
			networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
			ZDO_StartNetworkReq(&networkParams);
			break;
			
		case REG_ENDPOINT:
			ZCL_RegisterEndpoint(&endPointTemperatureMeasurementServer);
			ZCL_RegisterEndpoint(&endPointOnOffLightServer);
			ZCL_RegisterEndpoint(&endPointOnOffStatusServer);
			ZCL_RegisterEndpoint(&endPointOnOffMode_climateServer);
			ZCL_RegisterEndpoint(&endPointOnOffMode_lightServer);		
			ZCL_RegisterEndpoint(&endPointIlluminanceServer);	
			ZCL_RegisterEndpoint(&endPointOnOffLightClient);
			ZCL_RegisterEndpoint(&endPointOnOffCoolingServer);
			ZCL_RegisterEndpoint(&endPointOnOffHeatingServer);
			ZCL_RegisterEndpoint(&endPointAppOnOffLightServer);
			clusterInit();
			initModule();
			initTimer();
			initButton();
            initBinding(); //Binding initialisieren
			ZCL_StartReporting(); //Automatisches Reporting starten
			initReport();
			ZCL_AttributeReq(&statusAttrReq);
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
