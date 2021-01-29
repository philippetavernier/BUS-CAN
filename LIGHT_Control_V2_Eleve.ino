/**********************************************************************************
/                    TABLEAU DES IDENTIFIANTS du bus CAN                          *
/                                                                                 *
/                          Carte MASTER       : 1                                 *
/                          Carte LED          : 2                                 *
/                          Carte PUSH_BUTTON  : 3                                 *
/                          Carte POT          : 4                                 *
/                          Carte LIGHT        : 5                                 *
/                          Carte RELAY        : 6                                 *
/                          Carte MOTOR        : 7                                 *
/                          Carte BAROMETRE    : 8                                 *
/                          Carte ULTRASONIC   : 9                                 *
/                          Carte HALL         : 10                                *
/                                                                                 *
/*********************************************************************************/

#include <STI2D.h>
#include "mcp_can.h"
#include "SPI.h"
#include "Adafruit_GFX.h"   // Core graphics library
#include "Adafruit_ILI9341.h" // Hardware-specific library
#include <Ultrasonic.h>

STI2D TP;

unsigned char Flag_Recv = 0;

#define ARDUINO_CAN_Identifiant 5
#define Light_Pin 0 // Choix de l'entrée analogique A0

void setup()
{
  CAN.begin(CAN_500KBPS);                       // init can bus : baudrate = 500k
  attachInterrupt(0, MCP2515_ISR, FALLING);     // start interrupt
  Serial.begin(115200);
    
  Serial.println("*********************************************************************");
  Serial.println("*                     LIGHT_Control_V2_Eleve                        *");
  Serial.println("*********************************************************************");
  Serial.println("");
}

void MCP2515_ISR()
{
  Flag_Recv = 1;
}

void loop()
{  
  // Gestion de la réception d'une trame CAN  
  Flag_Recv=TP.CAN_Light_Control(Light_Pin,Flag_Recv);	
}
