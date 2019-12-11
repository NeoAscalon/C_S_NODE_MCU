/*
 Name:		ClientServeurNODEMCU.ino
 Created:	20/11/2019 16:32:56
 Author:	ignat
*/

#include <ESP8266WiFi.h>
#include <DNSServer.h> 
#include <ESP8266WebServer.h>
#include <IotWebConf.h>

#define CONFIG_PIN 0
#define STATUS_PIN LED_BUILTIN

const char thingName[] = "NodeMCUConfig";
const char wifiInitialApPassword[] = "MotDePasse";



int port = 9000;

int pin_led1 = 5;	//D3
int pin_bouton1 = 14;	//D6 
bool AP_1 = LOW;

int pin_led2 = 4;	//D2
int pin_bouton2 = 12;	//D8
bool AP_2 = LOW;


void Command_Decript_Execute();
void Controle_Proche(); //Controle par les boutons physiques
void Serveur_Client();
void WebConf();
void handleRoot();

DNSServer dnsServer;
WebServer server(80);

String Message = (String)NULL; 
WiFiServer Server(port);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);

void setup() // Initialisation done only one time when you power up the card
{

	Serial.begin(115200);

	WebConf();

	/*
	Serial.print("Port: ");
	Serial.println(port);
	Serial.print("Pin Led1  " );
	Serial.print(pin_led1);
	Serial.print(" et pin bouton1  ");
	Serial.println(pin_bouton1);
	Serial.print("Pin Led2  ");
	Serial.print(pin_led2);
	Serial.print("  et pin bouton2  ");
	Serial.println(pin_bouton2);
	*/

	pinMode(pin_led1, OUTPUT);
	digitalWrite(pin_led1, AP_1);
	pinMode(pin_bouton1, INPUT);

	pinMode(pin_led2, OUTPUT);
	digitalWrite(pin_led2, AP_2);
	pinMode(pin_bouton2, INPUT);

	Server.begin(port);
	Serial.print("Server available, IP:");
	Serial.print(WiFi.localIP());
	Serial.print("  on port  ");
	Serial.println(port);

}

void loop() //As the name implies this fonction is done in loop avter the setup is done
{
	iotWebConf.doLoop();
	Serveur_Client();
	Controle_Proche();
}

void WebConf()
{
	Serial.println();
	Serial.println("Starting up...");

	// -- Initializing the configuration.
	iotWebConf.setStatusPin(STATUS_PIN);
	iotWebConf.setConfigPin(CONFIG_PIN);
	iotWebConf.init();

	// -- Set up required URL handlers on the web server.
	server.on("/", handleRoot);
	server.on("/config", [] { iotWebConf.handleConfig(); });
	server.onNotFound([]() { iotWebConf.handleNotFound(); });

	Serial.println("Access Point Ready.");
}

void handleRoot()
{
	// -- Let IotWebConf test and handle captive portal requests.
	if (iotWebConf.handleCaptivePortal())
	{
		// -- Captive portal request were already served.
		return;
	}
	String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
	s += "<title>IotWebConf 01 Minimal</title></head><body>Hello world!";
	s += "Go to <a href='config'>configure page</a> to change settings.";
	s += "</body></html>\n";

	server.send(200, "text/html", s);
}

void Command_Decript_Execute() //Analisis of the data recieved and execution of the directives sent from the server
{
	if (Message != (String)NULL)
	{
		Serial.println("Sortie en chaine Char[] :");
		Message.c_str();
		int i = 0;
		do
		{
			Serial.print(Message[i]);
			i++;
		} while (Message[i] != NULL);
		Serial.println("");


		if (Message == "L01A")
		{
			digitalWrite(pin_led1, HIGH);
			AP_1 = HIGH;
		}
		else if(Message == "L01E")
		{
			digitalWrite(pin_led1, LOW);
			AP_1 = LOW;
		}


		if (Message == "L02A")
		{
			digitalWrite(pin_led2, HIGH);
			AP_2 = HIGH;
		}
		else if (Message == "L02E")
		{
			digitalWrite(pin_led2, LOW);
			AP_2 = LOW;
		}
		Message = (String)NULL; //usefull to restart the conditioning of the fonction
	}
}

void Controle_Proche()
{
	if (digitalRead(pin_bouton1) == HIGH)
	{
		Serial.println("Bouton 1 Enclanche");
		AP_1 = (!AP_1);
		digitalWrite(pin_led1, AP_1);
		while (digitalRead(pin_bouton1) == HIGH);
	}

	if (digitalRead(pin_bouton2) == HIGH)
	{
		Serial.println("Bouton 2 Enclanche");
		AP_2 = (!AP_2);
		digitalWrite(pin_led2, AP_2);
		while (digitalRead(pin_bouton2) == HIGH);
	}
}

void Serveur_Client()
{
	WiFiClient Client = Server.available();

	if (Client)
	{

		if (Client.connected())
		{
			Serial.println("Client Connecte!");
		}
		while (Client.connected())
		{

			while (Client.available() > 0)
			{
				Message = Client.readStringUntil('\n');
			}
			delay(10);
		}
		Command_Decript_Execute(); 
		Client.stop();
		Serial.println("Client disconnected");
	}
}
