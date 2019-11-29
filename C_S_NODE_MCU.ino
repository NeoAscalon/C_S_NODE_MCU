/*
 Name:		ClientServeurNODEMCU.ino
 Created:	20/11/2019 16:32:56
 Author:	ignat
*/

#include <ESP8266WiFi.h>
#include <HardwareSerial.h>
#include <string>

#define SendKey 0

const char* ssid = "Point4";      // SSID
const char* password = "PointAccess4";      // Password
const char* host = "172.16.8.52";  // IP serveur - Server IP
const char* ID_Carte = "01";
const char* ID_Lam1 = "L01";
const char* ID_Lam2 = "L02";
const int   port = 9000;            // Port serveur - Server Port
int pin_led1 = 2; //D4
int pin_led2 = 4; //D2
int pin_bouton1 = 12; //D6
bool AP_1;
int pin_bouton2 = 15; //D8
bool AP_2;

void Reconnection(); //If signal to server is lost
void Server_Read(); //Waiting for client and Read directives from Raspberry server
void Client_Write(); //Respond to server (optional)
void Deconnetion(); //Disconect from Server
void Command_Decript_Execute();
void Controle_Proche(); //Controle par les boutons physiques

String Message = (String)NULL;
WiFiClient Client;
WiFiServer Server(port);

int count = 0;


void setup() // the setup function runs once when you press reset or power the board
{

	Serial.begin(115200);

	pinMode(pin_led1, OUTPUT);
	pinMode(pin_led2, OUTPUT);

	pinMode(pin_bouton1, INPUT);
	pinMode(pin_bouton2, INPUT);

	digitalWrite(pin_led1, LOW);
	AP_1 = LOW;
	digitalWrite(pin_led2, LOW);
	AP_2 = LOW;

	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	Serial.print("Connecting");

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	
	Serial.println();

	Serial.print("Connected To Network, IP address: ");
	Serial.println(WiFi.localIP());

	Server.begin();
	Serial.print("Open Client and connect to IP:");
	Serial.print(WiFi.localIP());
	Serial.print(" on port ");
	Serial.println(port);

}

void loop()
{
	Server_Read();

	Command_Decript_Execute();
	
	Controle_Proche();
	
	delay(500);
}

void Reconnection()
{
	if (!Client.connected()) {

		Serial.print("Client Lost Connection,Reconecting..");
		Client.stop();

		while (!Client.connected())
		{
			Serial.print(".");
			Client.connect(host, port);
			delay(500);
		}

		Serial.println();
		Serial.println("Client Connected");
	}
}

void Server_Read()
{
	Client = Server.available();

	if (Client)
	{
		if (Client.connected() && (Client.available() > 0))
		{
			Serial.println("Client Connected");
			Message = Client.readStringUntil('\n');  // read data from the connected client
			Serial.println(Message);
		}
	}
}

void Client_Write()
{
	if (Client.connected())
	{
		Message = Client.readStringUntil('\n');
	}
}

void Deconnetion()
{
	if (Client.connected())
	{
		Client.stop();
	}
}

void Command_Decript_Execute()
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


	}
	Message = (String)NULL;
}

void Controle_Proche()
{
	if (digitalRead(pin_bouton1) == HIGH)
	{
		Serial.println("Switch1");
		AP_1 = (!AP_1);
		digitalWrite(pin_led1, AP_1);
	}

	if (digitalRead(pin_bouton2) == HIGH)
	{
		Serial.println("Switch2");
		AP_2 = (!AP_2);
		digitalWrite(pin_led2, AP_2);

	}
}