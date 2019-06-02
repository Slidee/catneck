#include <SoftwareSerial.h>
#include <TinyGPS.h>

const byte HC12RxdPin = 4;                  // Recieve Pin on HC12
const byte HC12TxdPin = 5;                  // Transmit Pin on HC12
SoftwareSerial HC12Serial(HC12TxdPin,HC12RxdPin); // Create Software Serial Port

const byte GPSRxdPin = 6;                  // Recieve Pin on GPS
const byte GPSTxdPin = 7;  // Transmit Pin on GPS
SoftwareSerial GPSSerial(GPSTxdPin, GPSRxdPin);

TinyGPS gps;

void gpsdump(TinyGPS &gps);

long startingMillis = 0;
int waitingTimeSeconds = 10;

void setup() {
  Serial.begin(1200);                       // Open serial port to computer
  HC12Serial.begin(1200);                         // Open serial port to HC12
  GPSSerial.begin(9600);                // set the data rate for the GPS SoftwareSerial port

  startingMillis = millis();
}

bool pendingData = false;
void loop() {
  
  if(HC12Serial.available()){                     // If Arduino's HC12 rx buffer has data
    Serial.write(HC12Serial.read());              // Send the data to the computer
    }
  if(Serial.available()){                   // If Arduino's computer rx buffer has data
    HC12Serial.write(Serial.read());              // Send that data to serial
  }

  if (GPSSerial.available()) 
    {
      char c = GPSSerial.read();
      if (gps.encode(c)) 
      {
        pendingData=true;
        
      }
    }

    if(pendingData && millis() > startingMillis + waitingTimeSeconds*1000){
      Serial.println("Acquired Data");
      Serial.println("-------------");
      gpsdump(gps);
      Serial.println("-------------");
      Serial.println();
        
      pendingData=false;
      startingMillis = millis();
    }
}

void gpsdump(TinyGPS &gps)
{
  float flat, flon;
  unsigned long age, date, time, chars;
  int year;
  unsigned short sentences, failed;
  Serial.println(" Print HC12 header line");
  HC12Serial.println("lat,lng,time,cat");
  
  // On Arduino, GPS characters may be lost during lengthy Serial.print()
  // On Teensy, Serial prints to USB, which has large output buffering and
  //   runs very fast, so it's not necessary to worry about missing 4800
  //   baud GPS characters.

  gps.f_get_position(&flat, &flon, &age);
  Serial.print("Lat/Long(float): "); Serial.print(flat, 5); Serial.print(", "); Serial.print(flon, 5);
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");
  
  gps.get_datetime(&date, &time, &age);
  Serial.print("Date(ddmmyy): "); Serial.print(date); Serial.print(" Time(hhmmsscc): ");Serial.print(time);
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  gps.stats(&chars, &sentences, &failed);
  Serial.print("Stats: characters: "); Serial.print(chars); Serial.print(" sentences: ");
  Serial.print(sentences); Serial.print(" failed checksum: "); Serial.println(failed);

  HC12Serial.print(flat,5);HC12Serial.print(",");
  HC12Serial.print(flon,5);HC12Serial.print(",");
  HC12Serial.print(date);HC12Serial.print(":");HC12Serial.print(time);HC12Serial.print(",");
  HC12Serial.println("ippy");
}
