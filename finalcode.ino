#include <SoftwareSerial.h>
#include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd(0x27, 16, 2);

#define xPin A1
#define yPin A2
#define zPin A3

// Calibrated minimum and maximum Raw Ranges for each axis
// Use calibration.ino file to get these values

int xMin = 263;
int xMax = 393;

int yMin = 264;
int yMax = 395;

int zMin = 267;
int zMax = 297;

SoftwareSerial mySerial(2,3);
//--------------------------------------------------------------
// Take multiple samples to reduce noise
const int samples = 10;
//--------------------------------------------------------------

// Set the threshold value
int threshold = 3;

const int buzzerPin = 7;
const int ledPin = 6;
int flag = 0;

String message = "Earthquake Alert!\r\n"
                   "Safety Guidelines:\r\n"
                   "1. Drop, Cover, and Hold On.\r\n"
                   "2. Stay Indoors and away from hazards.\r\n"
                   "3. Evacuate if necessary to open areas.\r\n"
                   "Listen to authorities for updates.\r\n"
                   "Emergency Contacts:\r\n"
                   "- Medical Emergency: 104\r\n"
                   "- Fire Department: 103\r\n";


void setup() 
{
  // analogReference(EXTERNAL);
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);
  mySerial.println("AT"); // Once the handshake test is successful, it will back to OK
  updateSerial();

  pinMode(buzzerPin, OUTPUT);

  pinMode(ledPin, OUTPUT);

  // lcd.begin(16, 2);
}

void loop() 
{
  //--------------------------------------------------------------
  // Read raw values
  int xRaw=0, yRaw=0, zRaw=0;
  for(int i=0; i<samples; i++)
  {
    xRaw += analogRead(xPin);
    yRaw += analogRead(yPin);
    zRaw += analogRead(zPin);
  }
  // Average 
  xRaw /= samples;
  yRaw /= samples;
  zRaw /= samples;
  //--------------------------------------------------------------
  // Convert raw values to 'milli-Gs"
  // Convert value of RawMin to -1000
  // Convert value of RawMax to 1000
  // Ploating in -1000 to 1000
  long xMilliG = map(xRaw, xMin, xMax, -1000, 1000);
  long yMilliG = map(yRaw, yMin, yMax, -1000, 1000);
  long zMilliG = map(zRaw, zMin, zMax, -1000, 1000);
  //--------------------------------------------------------------
  // Re-scale to fractional Gs
  float x_g_value = xMilliG / 1000.0;
  float y_g_value = yMilliG / 1000.0;
  float z_g_value = zMilliG / 1000.0;
  //--------------------------------------------------------------
  
 // Print the sensor values
  Serial.print(x_g_value, 0);
  Serial.print("G\t");
  Serial.print(y_g_value, 0);
  Serial.print("G\t");
  Serial.print(z_g_value, 0);
  Serial.print("G\t");
  Serial.println();
  //--------------------------------------------------------------
  delay(200);

  // Check if any of the values exceed the threshold
  bool isEarthquake = (abs(x_g_value) > threshold) || (abs(y_g_value) > threshold) || (abs(z_g_value) > threshold);

  // If an earthquake is detected, trigger the GSM module function
  if (isEarthquake) {
    Serial.println("Earthquake Alert!");
    sendSms();
    // buzzer();
    // led();
    // lcdd();
  }


}



void sendSms() {
  if(flag==0){
  // Send the SMS
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+9779814467960\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print(message);
  updateSerial();
  mySerial.write(26); // CTRL+Z to send the SMS
  updateSerial();
  flag = 1;
  }
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());
  }
}

void buzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
  delay(1000);
}

void led() {

 digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}

void lcdd() {

  // lcd.clear();
  // lcd.print("Earthquake detected");
  delay(2000);
}