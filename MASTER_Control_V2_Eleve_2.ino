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

/**********************************************************************************
/                    Exemples de commandes pour le moniteur                       *
/                                                                                 *
/                    Carte LED à 1      : _T_!_Id_2_Data_1_End_                   *
/                    Carte LED à 0      : _T_!_Id_2_Data_0_End_                   *
/                    Carte PUSH_BUTTON  : _T_?_Id_3_Data__End_                    *
/                    Carte POT          : _T_?_Id_4_Data__End_                    *
/                    Carte LIGHT        : _T_?_Id_5_Data__End_                    *
/                    Carte RELAY à 1    : _T_!_Id_6_Data_1_End_                   *
/                    Carte RELAY à 0    : _T_!_Id_6_Data_0_End_                   *
/                    Carte MOTOR pour sens positif et vitesse 234/255             *
                                        : _T_!_Id_7_Data_1234_End_                *
/                    Carte BAROMETRE    : _T_?_Id_8_Data__End_                    *
/                    Carte ULTRASONIC   : _T_?_Id_9_Data__End_                    *
/                    Carte HALL         : _T_?_Id_10_Data__End_                   *
/                                                                                 *
/*********************************************************************************/

#include <STI2D.h>
#include "mcp_can.h"
#include "SPI.h"
#include "Adafruit_GFX.h"   // Core graphics library
#include "Adafruit_ILI9341.h" // Hardware-specific library
#include <Ultrasonic.h>

STI2D TP;

boolean Serial_String_RAZ = false;
String Serial_String = String(21);
char Serial_String_Data[22];
boolean Serial_String_Complete = false;

unsigned char Flag_Recv = 0;
unsigned char len;
unsigned char buf[8]={};
unsigned char buf_mem[8]={};
unsigned char same_buf=1;

unsigned char octet_0=0;
unsigned char octet_1=0;
unsigned char octet_2=0;
unsigned long light_resistance=0;
unsigned long light_resistance_mem=0;

// demande la valeur de la résistance du capteur de luminosité
// Data[0] = 5 = Id_destinataire // Data[1] = 1 = Requête
unsigned char Data_LIGHT_Val[2] = {5, 1};


void setup()
{
  CAN.begin(CAN_500KBPS);                       // init can bus : baudrate = 500k
  attachInterrupt(0, MCP2515_ISR, FALLING);     // start interrupt
  Serial.begin(115200);
  
  Serial.println("*********************************************************************");
  Serial.println("*                     MASTER_Control_V2_Eleve                       *");
  Serial.println("*********************************************************************");
  Serial.println("");
 
  Serial_String =""; 
  
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
}

void MCP2515_ISR()
{
  Flag_Recv = 1;
}

void loop()
{  
  // Commande du bus CAN grâce au Moniteur
  //Serial_String_RAZ=TP.CAN_Monitor_Control(Serial_String_Data,Serial_String_Complete);
  
  // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 2, stmp: data buf
  CAN.sendMsgBuf(1, 0, sizeof(Data_LIGHT_Val), Data_LIGHT_Val);
  /*
  // Copie du tableau buf[8] dans buf_mem[8]
  for (int n=0; n<8; n++)
  {
    buf_mem[n] = buf[n];
  }
  */
  // Gestion de la réception d'une trame CAN  
  Flag_Recv=TP.CAN_Receive(Flag_Recv, &buf[0], &len);
  /*  
  // Comparaison de buf et buf_mem
  for (int i=0; i<8; i++)
  {
    if (buf[i] != buf_mem[i])
      same_buf = 0;    
  }
  
  // S'il y a une différence entre buf et buf_mem
  if(same_buf == 0)
  {
    Serial.print("len = ");  Serial.println(len);
    for (int n=0; n<8; n++)
    {
      Serial.print("buf["); Serial.print(n); Serial.print("] = "); Serial.println(buf[n]);
    }
    
    same_buf = 1;    
  }
  */
  if (buf[6] == 48) octet_0 = buf[2]; // récupération de l'octet 0
  if (buf[6] == 49) octet_1 = buf[2]; // récupération de l'octet 1
  if (buf[6] == 50) octet_2 = buf[2]; // récupération de l'octet 2

  //light_resistance_mem = light_resistance; 
  light_resistance = 65536*octet_2 + 256*octet_1 + octet_0;
  /*
  if (light_resistance != light_resistance_mem)
  {
    Serial.print("light_resistance =  ");  Serial.println(light_resistance);
  }
  */
  if (light_resistance > 50000)
    digitalWrite(A0, HIGH);
  else
    digitalWrite(A0, LOW);
    
  // Si commande moniteur gérée, alors effacement de la chaîne de commande
  if (Serial_String_RAZ)
  {
    Serial_String_Complete = false;
  }
  
}

/*********************************************************************
         SerialEvent occurs whenever a new data comes in the
         hardware serial RX.  This routine is run between each
         time loop() runs, so using delay inside loop can delay
         response.  Multiple bytes of data may be available.
**********************************************************************/

void serialEvent()
{
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    
    // add it to the inputString:
    Serial_String += inChar;
    Serial_String.toCharArray(Serial_String_Data, sizeof(Serial_String_Data));
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') 
    {
      Serial_String_Complete = true;
      //Serial.print("Serial_String_Data = "); Serial.println(Serial_String_Data);
      Serial_String ="";
    } 
  }
}
