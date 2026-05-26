/*
 * This program is written for the ESP32 DevkitV1 development board. 
 * The project is designed for remote monitoring of a palm wine tapping setup. 
 * Drilling a hole into the tree's trunk and monitoring the status of the liquid collected inside a container
 * The code is scalable for including a flow sensor to monitor the flow rate and communicate with the harvester remotel on the ground
 * A smartphone is used for sending and receiving information to and from the setup
 * Even when the harvester has been away, upon close proximity with the clamp-tree setup, the bluetooth connection can be re-established by the harvester
 */
#include "BluetoothSerial.h" //include bluetooth library

#define DRILL_MOTOR_TRIGGER_PIN 4
#define TRIG_PIN 16
#define ECHO_PIN 17
#define CONTAINER_FULL_LEVEL 10
#define SPEED_OF_SOUND_IN_AIR 0.034f 
#define FILLED_MESSAGE_ARRAY_LENGTH 16

char filledMessage[] = {'T', 'a', 'n', 'k', ' ', 'i', 's', ' ', 'f', 'i', 'l', 'l', 'e', 'd', '!', '\n'};
String message;

BluetoothSerial BluetoothInstance; //create an instance of the BluetoothSerial class

void setup() 
{
 //configure the drill motor's driving transistor gate as output
 pinMode (DRILL_MOTOR_TRIGGER_PIN, OUTPUT);
 //switch off the gate 
 digitalWrite (DRILL_MOTOR_TRIGGER_PIN, LOW);

 //configure ultrasonic sensor pins
 pinMode(TRIG_PIN, OUTPUT);
 pinMode(ECHO_PIN, INPUT);

 /*flow sensor pins configuration here*/

Serial.begin (115200); //initialize Serial debugging
 //initialize bluetooth for the ESP32 device
 BluetoothInstance.begin("Clamp Extractor 1"); //assign a bluetooth name for this clamp unit
  Serial.println ("Waiting for Bluetooth Connection....."); //optional debug messsage
  while (!BluetoothInstance.hasClient()) //waiting for harvester to connect via bluetooth
  {
    
  }
 
Serial.println ("Bluetooth Connected");

//start drilling to make the incision on the tree trunk
digitalWrite (DRILL_MOTOR_TRIGGER_PIN, HIGH);
Serial.println ("Drill started");

while (!BluetoothInstance.available()) {} //wait until a bluetooth message is received from the harvester
    //turn off drill
    digitalWrite (DRILL_MOTOR_TRIGGER_PIN, LOW);
    Serial.println ("Drill stopped");
  
}

void loop() {

  /*commence sampling ultrasonic sensor for liquid level*/
  digitalWrite (TRIG_PIN, LOW);
  delayMicroseconds(5);

  /*10 microsecond pulse*/
  digitalWrite (TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite (TRIG_PIN, LOW);

  //calculate time of ultrasonic echo
  long duration = pulseIn(ECHO_PIN, HIGH);

  //calculate distance from time of flight
  int distance = ((duration * SPEED_OF_SOUND_IN_AIR) / 2);
   

  /*read a bluetooth message if it is available*/
  while (BluetoothInstance.available())
  {
    message = BluetoothInstance.readStringUntil('\n');
    message.trim();
    
    if (message == "CHECK LEVEL") //if the harvester wants to check the liquid level,
  {
    Serial.println ("Level check requested");
    BluetoothInstance.write(char(distance)); //send the level to the harvester
     message = ""; //empty message string
  }

  /*for flow sensor interfacing
   * if (message == "CHECK FLOW RATE") //if the harvester wants to check the flow rate,
  {
    Serial.println ("Flow Rate requested");
    BluetoothInstance.write(char(flowRate)); //send the level to the harvester
     message = ""; //empty message string
  }
   */
      
    
  }


  if (distance >= CONTAINER_FULL_LEVEL)
  {
    Serial.println ("Container is full!");
   
    //alert the harvester that the tank is full
    unsigned char i;
    for (i = 0; i<FILLED_MESSAGE_ARRAY_LENGTH; i++)
    {
    BluetoothInstance.write(filledMessage[i]); //send the Full Tank alert to the harvester
    }
    
  }
  
  delay(50);
}
 
  

