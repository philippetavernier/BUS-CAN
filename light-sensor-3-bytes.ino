/* Grove - Light Sensor demo v1.0
* 
* Signal wire to A0.
* By: http://www.seeedstudio.com
*/
#include <math.h>

#define LIGHT_SENSOR A0//Grove - Light Sensor is connected to A0 of Arduino
const int ledPin=12;                 //Connect the LED Grove module to Pin12, Digital 12
const int thresholdvalue=10;         //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
float Rsensor; //Resistance of sensor in K

float R_LIGHT_Value;
long R_LIGHT_Value_2;
long R_LIGHT_Value_1;
long R_LIGHT_Value_0;


void setup() 
{
    Serial.begin(9600);                //Start the Serial connection
    pinMode(ledPin,OUTPUT);            //Set the LED on Digital 12 as an OUTPUT
  
  R_LIGHT_Value = 0;
  R_LIGHT_Value_2 = 0;
  R_LIGHT_Value_1 = 0;
  R_LIGHT_Value_0 = 0;

}
void loop() 
{
    int sensorValue = analogRead(LIGHT_SENSOR); 
    Rsensor = (float)(1023-sensorValue)*10/sensorValue;
    Serial.println("the analog read data is ");
    Serial.println(sensorValue);
    Serial.println("the sensor resistance in kohm is ");
    Serial.println(Rsensor,DEC);//show the ligth intensity on the serial monitor;

    R_LIGHT_Value = (float)(1023 - sensorValue ) * 10000 / sensorValue ;
        Serial.print("R_LIGHT_Value = ");  Serial.println(R_LIGHT_Value);
        
        R_LIGHT_Value_0 = (long)R_LIGHT_Value & 0b11111111; // masque sur l'octet de poids faible
        //Data_R_LIGHT_Val_0[2] = (unsigned char)(R_LIGHT_Value_0);
        Serial.print("R_LIGHT_Value_0 = ");  Serial.println(R_LIGHT_Value_0);
    
        R_LIGHT_Value_1 = (long)R_LIGHT_Value >>8; // récupération des bits de l'octet du milieu
        R_LIGHT_Value_1 = R_LIGHT_Value_1 & 0b11111111; // masque sur l'octet du milieu
        //Data_R_LIGHT_Val_1[2] = (unsigned char)(R_LIGHT_Value_1);
        Serial.print("R_LIGHT_Value_1 = ");  Serial.println(R_LIGHT_Value_1);
      
        R_LIGHT_Value_2 = (long)R_LIGHT_Value >>16; // récupération des bits de l'octet de poids fort
        R_LIGHT_Value_2 = R_LIGHT_Value_2 & 0b11111111; // masque sur l'octet de poids fort
        //Data_R_LIGHT_Val_2[2] = (unsigned char)(R_LIGHT_Value_2);
        Serial.print("R_LIGHT_Value_2 = ");  Serial.println(R_LIGHT_Value_2);
        
        // R_LIGHT_Value = 0bxxxxxxxx xxxxxxxx xxxxxxxx
        //                   Octet 2   Octet 1  Octet 0
        delay(1000);
}
