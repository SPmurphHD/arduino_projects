/*
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/7ce5686a-0aba-49c7-9f46-9d831495eb75

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  float humidity;
  float packVoltage;
  float temperature;
  float usbVolt;
  int moisture;
  bool setRelayState;
  bool useSensor;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/
#include "thingProperties.h"
#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;


#define Sensor A6 // defining the voltage sensor for the 12v battery pack
float vOUT = 0.0; // setting vOUT to zero at start
float vIN = 0.0; // setting vIN to zero at start
float R1=30000.0;//30kohms resistor
float R2=7500.0;//7.5kohms resistor

int valveRelay; // defining the valveRelay variable - boolean

int moistPin = A5; // defining the moisture sensor variable and assigning it to pin A5

// calibrate moisture sensor
const int AIR_ANALOG = 4095; // calibrate moisture sensor dry state
const int WATER_ANALOG = 3000; // calibrate moisture sensor wet state


void setup() {
  
  
  Serial.begin(9600); // Initialize serial and wait for port to open:
  //delay(1500);

  
  initProperties(); // Defined in thingProperties.h

  
  ArduinoCloud.begin(ArduinoIoTPreferredConnection); // Connect to Arduino IoT Cloud
  //Get Cloud Info/errors , 0 (only errors) up to 4
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  //Wait to get cloud connection to init the carrier
  while (ArduinoCloud.connected() != 1) {
    ArduinoCloud.update();
    delay(500);
  }
  delay(500);
  CARRIER_CASE = true; //setting the case use parameter since the box is like the case for button sensitivity
  carrier.begin();
  carrier.display.setRotation(0); //setting the display rotation
  delay(1500);
  
  pinMode(valveRelay, OUTPUT); // setting the valve relay variable as an output
  digitalWrite(valveRelay, LOW); // starting it at low or closed
  pinMode(LED_BUILTIN, OUTPUT);   // Set the built-in LED as an output 
  pinMode(3, OUTPUT); // Set pin 3 the screen backlight as an output
  digitalWrite(3, LOW); // setting it to low or off
  
  delay(1500);
  
}

void loop() {
  ArduinoCloud.update();
  
  carrier.Buttons.update();


  while (!carrier.Light.colorAvailable()) {
    delay(5);
  }
  int none;
  
  temperature = carrier.Env.readTemperature(FAHRENHEIT) - 10; //temp calibration
  
  humidity = carrier.Env.readHumidity(); // humidity

  
  // if useSensor is true, read the sensor value and turn on/off the relay based on the threshold
  if (useSensor) {
    int raw_moisture = analogRead(moistPin); // read raw moisture value  
    // map raw moisture value to a scale of 0 to 100 from set constants
    moisture = map(raw_moisture, AIR_ANALOG, WATER_ANALOG, 0, 100);
    // the moisture relay settings
    // open the relay - hose valve when it goes below 40%
    if (moisture < 40) {
      Serial.println("RELAY ON"); // print a message to serial 
      digitalWrite(valveRelay, HIGH); // Turn on the relay
      digitalWrite(LED_BUILTIN, HIGH); // Turn on the carrier LED
      carrier.Relay2.open(); //set the relay open
    }
    // turn the relay - valve off at 70% moisture
    else if (moisture >= 70) {
      Serial.println("Relay OFF"); // print a message to serial
      digitalWrite(valveRelay, LOW); // Turn off the relay
      digitalWrite(LED_BUILTIN, LOW); // turn off the carrier LED
      carrier.Relay2.close();
    }   
  }
  // If useSensor is false, read the button state and turn on/off the relay based on it
  else {
    if(setRelayState == HIGH) {
    
      digitalWrite(valveRelay, HIGH); // Turn on the relay with the web button
    }
    else {
      digitalWrite(valveRelay, LOW); // Turn off the relay with the web button
    }
  }
  
 
    analogReadResolution(12); // set the sensors analog read resolution to 12 bit - 4095
    Serial.print(analogRead(A0)); // print to serial the raw voltage value on pin A0 - the USB voltage on the arduino
    
  int sensorValue = analogRead(A0); // set sensorValue to the voltage on pin A0
  // Convert the analog reading (which goes from 0 4095) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 4095.0);
  
  usbVolt = voltage;

  int value=analogRead (Sensor); // set the analog read of the 12v battery to int value
  vOUT = (value *3.3) / 4095.0; // set voltage out to value multiplied the board voltage of 3.3v
  vIN = vOUT / (R2/(R1 + R2)); // convert the vOUT to the actual pack voltage
  
  packVoltage = vIN; // set vIN to a cloud variable packVoltage
  
// Carrier button 0 configuration
  if (carrier.Buttons.onTouchDown(TOUCH0)) {
    carrier.display.fillScreen(ST77XX_BLACK); // background color
    carrier.display.setTextColor(ST77XX_WHITE); // text color
    carrier.display.setTextSize(4); // text size

    carrier.display.setCursor(30, 110); // set where the cursor begins printing characters line 1
    carrier.display.print("Humidity"); // print Humidity to the carrier screen
    carrier.display.setCursor(50, 160); // set where the cursor begins printing characters line 2
    carrier.display.print(humidity); // print the humidity variable current value

  }
// Carrier button 1 configuration
  if (carrier.Buttons.onTouchDown(TOUCH1)) {
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(4);

    carrier.display.setCursor(30, 110);
    carrier.display.print("Voltage ");
    carrier.display.setCursor(50, 160);
    carrier.display.print(packVoltage);
    carrier.display.print("v");
  }
  
// Carrier button 2 configuration
  if (carrier.Buttons.onTouchDown(TOUCH2)) {
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(4);

    carrier.display.setCursor(30, 110);
    carrier.display.print("temp");
    carrier.display.setCursor(50, 160);
    carrier.display.print(temperature);
  }
  
// Carrier button 4 configuration
  if (carrier.Buttons.onTouchDown(TOUCH4)) {
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(4);

    carrier.display.setCursor(10, 110);
    carrier.display.print("moisture% ");
    carrier.display.setCursor(100, 160);
    carrier.display.print(moisture);
    carrier.display.print("%");
  }
  
}

/*
  Since SetRelayState is READ_WRITE variable, onSetRelayStateChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onSetRelayStateChange()  {
  // Relay state change on web button change on the dashboard
  if(setRelayState)
  {
    digitalWrite(valveRelay, HIGH);
    carrier.Relay2.open();
  }
  else
  {
    digitalWrite(valveRelay, LOW);
    carrier.Relay2.close();
  }
}


/*
  Since UseSensor is READ_WRITE variable, onUseSensorChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onUseSensorChange()  {
  // Add your code here to act upon UseSensor change
  if (useSensor)
  {
    
  }
}
