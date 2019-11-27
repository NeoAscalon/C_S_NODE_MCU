/*
 Name:		ClientServeurNODEMCU.ino
 Created:	20/11/2019 16:32:56
 Author:	ignat
*/

#include <ESP8266WiFi.h>
#include <HardwareSerial.h>
#include <string>


const char* ssid = "Point4";      // SSID
const char* password = "PointAccess4";      // Password
const char* host = "172.16.8.52";  // IP serveur - Server IP
const char* ID_Carte = "1";
const char* ID_Lam1 = "L1";
const char* ID_Lam2 = "L2";
const int   port = 9000;            // Port serveur - Server Port
int pin_led1 = 5;
int pin_led2 = 4;

void Reconnection(); //If signal to server is lost
void ServerPrint(); //Send to server
void ServerRead(); //Read from server
void Deconnetion(); //Disconect from Server


String Message = (String)NULL;
WiFiClient Server;
WiFiClient ServerPresenceTest;


void setup() // the setup function runs once when you press reset or power the board
{

	Serial.begin(115200);

	pinMode(pin_led1, OUTPUT);
	pinMode(pin_led2, OUTPUT);
	digitalWrite(pin_led1, LOW);
	digitalWrite(pin_led2, LOW);

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

void loop()
{
	Reconnection();
	ServerRead();
	delay(500);
	Command_Decript_Execute();
	//ServerPrint();
	delay(4500);
}

void Reconnection()
{
	if (!ServerPresenceTest.connect(host, port)) {
		Serial.print("Connection To Server Lost,Reconecting..");
		Server.stop();
		while (!Server.connected())
		{
			Serial.print(".");
			Server.connect(host, port);
			delay(500);
		}

		Serial.println();
		Serial.println("Connected to Server");

	}
	ServerPresenceTest.stop();
}

void ServerPrint()
{
	Message = Serial.readString();
	if (Message == (String)NULL) {
		Serial.println(Message);
		Server.print("Le Message recu est: " + Message );
	}
}

void ServerRead()
{
	if (Server.available())
	{
		Message = Server.readStringUntil('\r');
	}
}

void Deconnetion()
{
	if (Server.connected())
	{

		Server.stop();

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


		if (Message == "1A")
		{
			digitalWrite(pin_led1, HIGH);
		}
		else if(Message == "1E")
		{
			digitalWrite(pin_led1, LOW);
		}


		if (Message == "2A")
		{
			digitalWrite(pin_led2, HIGH);
		}
		else if (Message == "2E")
		{
			digitalWrite(pin_led2, LOW);
		}


	}
	Message = (String)NULL;
}
