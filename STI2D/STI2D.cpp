#include "STI2D.h"
#include "mcp_can.h"
#include "SPI.h"
#include "Adafruit_GFX.h"   // Core graphics library
#include "Adafruit_ILI9341.h" // Hardware-specific library

///////////////////////////////////////////////////////////////////////
//               Fonction "acquisition_monitor"                      //
//                                                                   //
//       Acquisition des données saisies sur le monitor              //
///////////////////////////////////////////////////////////////////////

float STI2D::acquisition_monitor()
{
  receive_data = 0;
  receive_string = "";
  return_data = 0;
  flag = 1;
  
  while (flag == 1) 
  {
    // Si une liaison série est disponible, il faut la lire
    while (Serial.available())
    {
      receive_data = Serial.read();
    
      // Si donnée reçue DIFFERENTE de la touche ENTER, on l'ajoute à la chaîne receive_string
      if (receive_data !=10) 
      {
        receive_string=receive_string+(char)receive_data;
      }
      
      // Sinon conversion de la chaîne de caractères en décimal
      else 
      {    
        receive_string.toCharArray(buffer, sizeof(buffer));
        return_data = atof(buffer);
      
        flag = 0;
        return return_data;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
//             Fonction "Analog_Average_Acquire"                     //
//                                                                   //
//   Calcul de la valeur moyenne des conversions faites sur le CAN   //
//                                                                   //
//         average_size : nombre de points du moyenneur              //
//         input        : entrée analogique sur laquelle             //
//                               faire l'acquisition                 //
///////////////////////////////////////////////////////////////////////

int STI2D::Analog_Average_Acquire(int input, int average_size)
{
  int Analog_Value_Story[average_size];
  Analog_Value_Average=0;
  
  // Initialisation du tableau Analog_Value_Story[]
  for(int a=0; a<average_size; a++)
    Analog_Value_Story[a]=analogRead(input);
  
  // Décalage vers la gauche des cases du tableau Analog_Value_Story[]
  for(int a=0; a<(average_size-1); a++)
  {
    Analog_Value_Story[0+a]=Analog_Value_Story[1+a];
    /*
	Serial.print("Analog_Value_Story [");
    Serial.print(a);
    Serial.print("] = ");
    Serial.print(Analog_Value_Story[a]);
    Serial.print("\n");
	*/
  }
  
  /* 
  Lecture de l’entrée analogique choisie et 
  Stockage de la valeur lue dans la case n-1 du tableau Analog_Value_Story[]
  */
  Analog_Value_Story[average_size-1]=analogRead(input);
  /*
  Serial.print("Analog_Value_Story [");
  Serial.print(average_size-1);
  Serial.print("] = ");
  Serial.print(Analog_Value_Story[average_size-1]);
  Serial.print("\n");
  */
  
  // Calcul de la moyenne
  for(int a=0; a<average_size; a++)
  {
    Analog_Value_Average += Analog_Value_Story[a];
  }
  Analog_Value_Average /= average_size;
  
  return Analog_Value_Average;
}

///////////////////////////////////////////////////////////////////////////////////////
//   						    Fonction "CAN_Receive"			    				 //
																					 //
//						Gestion de la réception d'une trame CAN                      //
///////////////////////////////////////////////////////////////////////////////////////

unsigned char STI2D::CAN_Receive(unsigned char Flag, unsigned char *Receive_data, unsigned char *Receive_len)
{  
  if(Flag)                           // check if get data
  {   	  
	// Lecture des données contenues dans la trame
	CAN.readMsgBuf(&len, buf);
	
	// Utilisation du contenu de la trame
	if (buf[0]==1) // Si la trame est destinée à MASTER_Control
	{
	  if (buf[1]==0) // S'il s'agit d'une missive
	  {
		// Affichage du contenu de la trame
		Serial.print("Missive recue avec Data = "); 
		
		for(int i = 0; i<=2; i++)        // print the data
		Serial.print(buf[i]);
	
		for(int i = 3; i<len; i++)       // print the data char
		Serial.print(char(buf[i]));
	
		Serial.println("");
		
		// Renseignement des zones memoire pointées
		*Receive_len = len;
		for (int n=0; n<8; n++)
		{
			*Receive_data++ = buf[n];
		}
	  }
	}
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////
//               Fonction "CAN_Monitor_Control"                      //
//                                                                   //
//             Gestion du bus CAN par le moniteur                    //
///////////////////////////////////////////////////////////////////////

boolean STI2D::CAN_Monitor_Control(char *buf,boolean Monitor_String_Complete)
{
	  unsigned char Data_LED_On[3] = {2, 0, 1};            // Data[0] = 2 = Id_destinataire // Data[1] = 0 = Missive // Data[2] = 1 = information
	  unsigned char Data_LED_Off[3] = {2, 0, 0};           // Data[0] = 2 = Id_destinataire // Data[1] = 0 = Missive // Data[2] = 0 = information
	  unsigned char Data_PB_State[2] = {3, 1};             // Data[0] = 3 = Id_destinataire // Data[1] = 1 = Requête // demande l'état du bouton
	  unsigned char Data_POT_Val[2] = {4, 1};              // Data[0] = 4 = Id_destinataire // Data[1] = 1 = Requête // demande la valeur issue de la conversion sur A0
	  unsigned char Data_LIGHT_Val[2] = {5, 1};            // Data[0] = 5 = Id_destinataire // Data[1] = 1 = Requête // demande la valeur de la résistance du capteur de luminosité
	  unsigned char Data_RELAY_NC[3] = {6, 0, 1};          // Data[0] = 6 = Id_destinataire // Data[1] = 0 = Missive // Data[2] = 1 = information
	  unsigned char Data_RELAY_NO[3] = {6, 0, 0};          // Data[0] = 6 = Id_destinataire // Data[1] = 0 = Missive // Data[2] = 0 = information
	  unsigned char Data_MOTOR_Control[4] = {7, 0, 0, 0};  // Data[0] = 7 = Id_destinataire // Data[1] = 0 = Missive // Data[2] = 0 = sens de rotation // Data[3] = 0 = vitesse de rotation 
	  unsigned char Data_BAROMETRE_Val[2] = {8, 1};        // Data[0] = 8 = Id_destinataire // Data[1] = 1 = Requête // demande la valeur de la température, de la pression atmosphérique, de la pression relative et de l'altitude
	  unsigned char Data_ULTRA_Val[2] = {9, 1};            // Data[0] = 9 = Id_destinataire // Data[1] = 1 = Requête // demande la valeur de la distance séparant le capteur de l'obstacle
	  unsigned char Data_HALL_State[2] = {10, 1};          // Data[0] = 3 = Id_destinataire // Data[1] = 1 = Requête // demande l'état du bouton
	  
	  buf_data_string = String(22);
	  buf_data_string = "";
	  
	  for(int i = 0; i<MAX_CHAR_IN_MESSAGE; i++)
	  {
		buf_data_string += *(buf+i);
	  }
	  
	  if (Monitor_String_Complete && buf_data_string.indexOf("T") >0) 
	  {
		 //Serial.println(buf_data_string);
		 
		 Serial_Pos_T = buf_data_string.indexOf("T_");
		 Serial_Pos_Id = buf_data_string.indexOf("_Id_");
		 Serial_Pos_Data = buf_data_string.indexOf("_Data_");
		 Serial_Pos_End = buf_data_string.indexOf("_End_");
		 
		 Type_string = buf_data_string.substring(Serial_Pos_T+2,Serial_Pos_Id);
		 //Serial.print("Type = "); Serial.println(Type_string);
		 Id_string = buf_data_string.substring(Serial_Pos_Id+4,Serial_Pos_Data);
		 //Serial.print("Id_destinataire = "); Serial.println(Id_string);
		 Data_string = buf_data_string.substring(Serial_Pos_Data+6,Serial_Pos_End);
		 //Serial.print("Data = "); Serial.println(Data_string);
					  
		 // Si c'est une missive
		 if (Type_string == "!")
		 {
		   //Si à destination de la carte LED (Id=2)
		   if (Id_string == "2")
		   {
			 // Si Data = 0, on souhaite éteindre la LED
			 if (Data_string == "0")
			 {
			   // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 3, stmp: data buf
			   CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_LED_Off), Data_LED_Off);
			 }
			 // Si Data = 1, on souhaite allumer la LED
			 else if (Data_string == "1")
			 {
			   // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 3, stmp: data buf
			   CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_LED_On), Data_LED_On);
			 }
			 // Sinon
			 else
			   Serial.println("Donnees non geree ... Dommage!!");
		   }
			 
		   //Si à destination de la carte RELAY (Id=6)
		   else if (Id_string == "6")
		   {
			 // Si Data = 0, on souhaite faire le contact entre NO et COM
			 if (Data_string == "0")
			 {
			   // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 3, stmp: data buf
			   CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_RELAY_NO), Data_RELAY_NO);
			 }
			 // Si Data = 1, on souhaite faire le contact entre NC et COM
			 else if (Data_string == "1")
			 {
			   // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 3, stmp: data buf
			   CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_RELAY_NC), Data_RELAY_NC);
			 }
			 // Sinon
			 else
			   Serial.println("Donnees non geree ... Dommage!!");
		   }
		   
		   //Si à destination de la carte MOTOR (Id=7)
		   else if (Id_string == "7")
		   {
			 /********************************* SENS DE ROTATION *******************************/
			 // Si le 1er caractère de Data_String est à "0", on tourne dans un sens
			 if (Data_string.substring(0,1) == "1")
			 {
			   Data_MOTOR_Control[2] = 1;
			 }
			 // Si le 1er caractère de Data_String est à "1", on tourne dans l'autre sens
			 else if (Data_string.substring(0,1) == "0")
			 {
			   Data_MOTOR_Control[2] = 0;
			 }
			 
			 /******************************** VITESSE DE ROTATION ******************************/
			 
			 Speed_string = Data_string.substring(1); // Speed_string prend le contenu de Data_string (du 2ème caractère à la fin)
			 Speed_string.toCharArray(Speed_array, sizeof(Speed_array));
			 Speed = atoi(Speed_array);
			 
			 Data_MOTOR_Control[3] = (unsigned char)Speed;
			 
			 // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 4, stmp: data buf
			 CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_MOTOR_Control), Data_MOTOR_Control);                 
		   }

		   // Sinon, l'identifiant n'est pas géré
		   else
			 Serial.println("Identifiant non geree ... Dommage!!");
		 }
		 
		 // Si c'est une requete
		 if (Type_string == "?")
		 {
		   //Si à destination de la carte PUSH_BUTTON (Id=3)
		   if (Id_string == "3")
		   {
			 // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 2, stmp: data buf
			 CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_PB_State), Data_PB_State);
		   }
		   
		   //Si à destination de la carte POT (Id=4)
		   else if (Id_string == "4")
		   {
			 // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 2, stmp: data buf
			 CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_POT_Val), Data_POT_Val);
		   }
		   
		   //Si à destination de la carte LIGHT (Id=5)
		   else if (Id_string == "5")
		   {
			 // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 2, stmp: data buf
			 CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_LIGHT_Val), Data_LIGHT_Val);
		   }
		   
		   //Si à destination de la carte BAROMETRE (Id=8)
		   else if (Id_string == "8")
		   {
			 // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 2, stmp: data buf
			 CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_BAROMETRE_Val), Data_BAROMETRE_Val);
		   }
		   
		   //Si à destination de la carte ULTRASONIC RANGER (Id=9)
		   else if (Id_string == "9")
		   {
			 // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 2, stmp: data buf
			 CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_ULTRA_Val), Data_ULTRA_Val);
		   }
		   
		   //Si à destination de la carte HALL (Id=10)
		   else if (Id_string == "10")
		   {
			 // send data:  id_emetteur = 1 = MASTER_Control, standard flame, data len = 2, stmp: data buf
			 CAN.sendMsgBuf(ARDUINO_CAN_MASTER_Identifiant, 0, sizeof(Data_HALL_State), Data_HALL_State);
		   }
					
		   // Sinon
		   else
			Serial.println("Identifiant non geree ... Dommage!!");
		 }
		  
		 buf_data_string = "";
		 String_Receive_RAZ = true;
		 		 
		 return String_Receive_RAZ;
	   }
	}
	
unsigned char STI2D::CAN_Light_Control(int pin, unsigned char Flag)
{
	unsigned char Data_R_LIGHT_Val_2[7] = {1, 0, 0, 'R', 'L', 'V', '2'}; // Data[0] = 1 = Id_destinataire // Data[1] = 0 = Missive // Data[2] = R_LIGHT_Value_2 (valeur renseignée après) // Data = RLV2: = Information en ohm octet de poids 2
	unsigned char Data_R_LIGHT_Val_1[7] = {1, 0, 0, 'R', 'L', 'V', '1'}; // Data[0] = 1 = Id_destinataire // Data[1] = 0 = Missive // Data[2] = R_LIGHT_Value_1 (valeur renseignée après) // Data = RLV1: = Information en ohm octet de poids 1
	unsigned char Data_R_LIGHT_Val_0[7] = {1, 0, 0, 'R', 'L', 'V', '0'}; // Data[0] = 1 = Id_destinataire // Data[1] = 0 = Missive // Data[2] = R_LIGHT_Value_0 (valeur renseignée après) // Data = RLV0: = Information en ohm octet de poids 0
	
	Sensor_Value = 0;
	R_LIGHT_Value = 0;
	R_LIGHT_Value_2 = 0;
	R_LIGHT_Value_1 = 0;
	R_LIGHT_Value_0 = 0;
  
	//Serial.print("Flag = ");  	Serial.println(Flag);
	
	if(Flag)                           // check if get data
	{   	  
		// Lecture des données contenues dans la trame
		CAN.readMsgBuf(&len, buf);

		//for(int i = 0; i<=sizeof(buf); i++)        // print the data
		//	Serial.print(buf[i]);
			
		//Serial.println("");
		//Serial.println("");
	  		
		// Utilisation du contenu de la trame
		if (buf[0]==5) // Si la trame est destinée à LIGHT_Control
		{
			if (buf[1]==1) // S'il s'agit d'une requête
			{
				// lecture de la valeur issue du CAN sur lequel est branché le capteur de luminosité
				Sensor_Value = analogRead(pin);
				//Serial.print("Sensor_Value = ");  Serial.println(Sensor_Value);
					
				// Calcul de la résistance de la sonde de luminosité
				// Calcul réalisé à partir du schéma électronique
				// Rlight = [R1 x (VCC - VD1)] / VD1
				// Si on remplace R1 par sa valeur en Ohm (10000), VCC par la valeur max de conversion (1023) et VD1 par la valeur issue de la conversion (SensorValue), on obtient
				// Rlight = [10 x (1023 - SensorValue)] / SensorValue
				R_LIGHT_Value = (float)(1023 - Sensor_Value) * 10000 / Sensor_Value;
				//Serial.print("R_LIGHT_Value = ");  Serial.println(R_LIGHT_Value);
				
				R_LIGHT_Value_0 = (long)R_LIGHT_Value & 0b11111111; // masque sur l'octet de poids faible
				Data_R_LIGHT_Val_0[2] = (unsigned char)(R_LIGHT_Value_0);
				//Serial.print("R_LIGHT_Value_0 = ");  Serial.println(R_LIGHT_Value_0);
		
				R_LIGHT_Value_1 = (long)R_LIGHT_Value >>8; // récupération des bits de l'octet du milieu
				R_LIGHT_Value_1 = R_LIGHT_Value_1 & 0b11111111; // masque sur l'octet du milieu
				Data_R_LIGHT_Val_1[2] = (unsigned char)(R_LIGHT_Value_1);
				//Serial.print("R_LIGHT_Value_1 = ");  Serial.println(R_LIGHT_Value_1);
		  
				R_LIGHT_Value_2 = (long)R_LIGHT_Value >>16; // récupération des bits de l'octet de poids fort
				R_LIGHT_Value_2 = R_LIGHT_Value_2 & 0b11111111; // masque sur l'octet de poids fort
				Data_R_LIGHT_Val_2[2] = (unsigned char)(R_LIGHT_Value_2);
				//Serial.print("R_LIGHT_Value_2 = ");  Serial.println(R_LIGHT_Value_2);
				
				// R_LIGHT_Value = 0bxxxxxxxx xxxxxxxx xxxxxxxx
				//                   Octet 2   Octet 1  Octet 0
		
				// Envoi d'une trame de réponse contenant la valeur de la résistance du capteur en Ohm (Octet 2)
				CAN.sendMsgBuf(ARDUINO_CAN_LIGHT_Identifiant, 0, sizeof(Data_R_LIGHT_Val_2), Data_R_LIGHT_Val_2);
				delay(Delay_Time);
		
				// Envoi d'une trame de réponse contenant la valeur de la résistance du capteur en Ohm (Octet 1)
				CAN.sendMsgBuf(ARDUINO_CAN_LIGHT_Identifiant, 0, sizeof(Data_R_LIGHT_Val_1), Data_R_LIGHT_Val_1);
				delay(Delay_Time);
		
				// Envoi d'une trame de réponse contenant la valeur de la résistance du capteur en Ohm (Octet 0)
				CAN.sendMsgBuf(ARDUINO_CAN_LIGHT_Identifiant, 0, sizeof(Data_R_LIGHT_Val_0), Data_R_LIGHT_Val_0);
				delay(Delay_Time);
				
				Serial.println("CAN_Light_Control send data");
			}	
		}	
	 return 0; 
	}
	return 1;
}