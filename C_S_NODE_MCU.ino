/*
 Name:		ClientServeurNODEMCU.ino
 Created:	20/11/2019 16:32:56
 Author:	ignat
*/

#include <ESP8266WiFi.h>
#include <HardwareSerial.h>
#include <string>

char* ssid = "Point4";     // SSID
char* password = "PointAccess4";      // Password
int port = 9000;

void Command_Decript_Execute();
void Controle_Proche(); //Controle par les boutons physiques
void WiFi_Connection();
void Serveur_Client();

String Message = (String)NULL;
WiFiServer Server(port);

int pin_led1 = 2;	//D4
int pin_bouton1 = 12;	//D6 
bool AP_1 = LOW;

int pin_led2 = 4;	//D2
int pin_bouton2 = 15;	//D8
bool AP_2 = LOW;

struct
{
	char* ssidUsager;
	char* passwordUsager;
	int   portUsager;
	int pin_led1Usager;
	int pin_bouton1Usager;
	int pin_led2Usager;
	int pin_bouton2Usager;
	bool Y_N_enable = false;
}
ParamUsager;

struct
{
	char* ssidDefault = "Point4";     // SSID
	char* passwordDefault = "PointAccess4";      // Password
	int portDefault = 9000;
	int pin_led1Default = 2;
	int pin_bouton1Default = 12;
	int pin_led2Default = 4;
	int pin_bouton2Default = 15;
}
Default;

void setup() // Initialisation done only one time when you power up the card
{

	Serial.begin(115200);

	pinMode(pin_led1, OUTPUT);
	digitalWrite(pin_led1, AP_1);
	pinMode(pin_bouton1, INPUT);

	pinMode(pin_led2, OUTPUT);
	digitalWrite(pin_led2, AP_2);
	pinMode(pin_bouton2, INPUT);

	WiFi_Connection();

	Server.begin();
	Serial.print("Server available, IP:");
	Serial.print(WiFi.localIP());
	Serial.print("  on port  ");
	Serial.println(port);

}

void loop() //As the name implies this fonction is done in loop avter the setup is done
{
	WiFi_Connection();
	Serveur_Client();
	Controle_Proche();
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

void WiFi_Connection()
{
	if (WiFi.status() != WL_CONNECTED)
	{
		Serial.println();
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
				Message = Client.readString();
			}
			delay(10);
		}
		Command_Decript_Execute(); 
		Client.stop();
		Serial.println("Client disconnected");
	}
}

void SET_Param()
{
	if (ParamUsager.Y_N_enable == true)
	{
		ssid = ParamUsager.ssidUsager;      // SSID
		password = ParamUsager.passwordUsager;      // Password
		port = ParamUsager.portUsager;            // Port serveur - Server Port
	}

	else
	{
		ssid = Default.ssidDefault;	// SSID
		password = Default.passwordDefault;		// Password
		port = Default.portDefault;	// Port serveur - Server Port
	}
}