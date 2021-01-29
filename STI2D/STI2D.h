#ifndef STI2D_h
#define STI2D_h

#include "Arduino.h"
#include "mcp_can.h"
#include "SPI.h"
#include "Adafruit_GFX.h"   // Core graphics library
#include "Adafruit_ILI9341.h" // Hardware-specific library

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define ARDUINO_CAN_MASTER_Identifiant 		1  // Identifiant de la carte MASTER
#define ARDUINO_CAN_LED_Identifiant 		2  // Identifiant de la carte LED
#define ARDUINO_CAN_PUSH_BUTTON_Identifiant 3  // Identifiant de la carte PUSH_BUTTON
#define ARDUINO_CAN_POT_Identifiant 		4  // Identifiant de la carte POT
#define ARDUINO_CAN_LIGHT_Identifiant 		5  // Identifiant de la carte LIGHT
#define ARDUINO_CAN_RELAY_Identifiant 		6  // Identifiant de la carte RELAY
#define ARDUINO_CAN_MOTOR_Identifiant 		7  // Identifiant de la carte MOTOR
#define ARDUINO_CAN_BAROMETRE_Identifiant 	8  // Identifiant de la carte BAROMETRE
#define ARDUINO_CAN_ULTRASONIC_Identifiant  9  // Identifiant de la carte ULTRASONIC
#define ARDUINO_CAN_HALL_Identifiant 		10 // Identifiant de la carte HALL

#define MAX_CHAR_IN_MESSAGE 21 // Maximum de caractères dans un message de commande du moniteur

#define Delay_Time  5 // Delai de 5 ms

class STI2D
{
  public:
	
	float acquisition_monitor();
	int Analog_Average_Acquire(int input, int average_size);
	boolean CAN_Monitor_Control(char *buf,boolean Monitor_String_Complete);
	unsigned char CAN_Receive(unsigned char Flag, unsigned char *Receive_data, unsigned char *Receive_len);
	unsigned char CAN_Light_Control(int pin, unsigned char Flag);

  private:
	// Variables de la fonction 'acquisition_monitor'
	float receive_data;
	String receive_string;
	char buffer[20];
	float return_data;
	int flag;
	
	// Variable de la fonction 'Analog_Average_Acquire'
	int Analog_Value_Average;

	// Variables de la fonction 'CAN_Monitor_Control' & 'light_control
	String buf_data_string;
	
	int Serial_Pos_T;
	int Serial_Pos_Id;
	int Serial_Pos_Data;
	int Serial_Pos_End;
	
	String Type_string;
	String Id_string;
	String Data_string;
	
	char Speed_array[4]; 
	String Speed_string;
	int Speed;
	boolean String_Receive_RAZ;
	
	// Variables de la fonction 'CAN_Receive'
	unsigned char len;
	unsigned char buf[8];
	char str[20];
	int CAN_Id;
	
	// Variable de la fonction 'light_control'
	int Sensor_Value;
	float R_LIGHT_Value;
	long R_LIGHT_Value_2;
	long R_LIGHT_Value_1;
	long R_LIGHT_Value_0;
	
};

#endif
