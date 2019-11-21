/*
 Name:		ClientServeurNODEMCU.ino
 Created:	20/11/2019 16:32:56
 Author:	ignat
*/

#include <ESP8266WiFi.h>
#include <HardwareSerial.h>

const char* ssid = "Point4";      // SSID
const char* password = "PointAccess4";      // Password
const char* host = "172.16.8.52";  // IP serveur - Server IP
const char* ID_Carte = "01";
const char* ID_Lam1 = "L01";
const char* ID_Lam2 = "L02";
const int   port = 8888;            // Port serveur - Server Port

void Reconnection(); //If signal to server is lost
void ServerPrint(); //Send to server
void ServerRead(); //Read from server
void Deconnetion(); //Disconect from Server

String Message = (String)NULL;
WiFiClient Server;

// the setup function runs once when you press reset or power the board
void setup() {

	Serial.begin(115200);
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

	Serial.print("Connecting to Server..");

	while (!Server.connected())
	{
		delay(500);
		Serial.print(".");
		Server.connect(host, port);
	}
	Serial.println();
	Serial.println("Connected to Server");

}


void loop() {

	bool a = false;

	Reconnection();

	delay(500);

	ServerRead();

	ServerPrint();

	if (a = true)
	{
		Deconnetion();
	}

}

void Reconnection()
{
	if (!Server.connect(host, port)) {
		Serial.print("Connection To Server Lost,Reconecting..");

		while (!Server.connected())
		{
			delay(500);
			Serial.print(".");
			Server.connect(host, port);
		}

		Serial.println();
		Serial.println("Connected to Server");

	}
}

void ServerPrint()
{
	Server.print(Message);
	delay(500);
}

void ServerRead()
{
	if (Server.connected())
	{
		Message = Server.readStringUntil('\r');
		if (Message != NULL)
		{
			Serial.println("Nous avons Recu: " + Message);
		}
	}
}

void Deconnetion()
{
	if (Server.connected())
	{

		Server.stop();

	}
}
