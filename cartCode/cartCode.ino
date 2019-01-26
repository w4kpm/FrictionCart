
int ledPin = 13;                 // LED connected to digital pin 13
float circumference = 157; // wheel diameter is 50mm round trip = 50*pi = 157
float perTick = 15.7;  // 10 positions on the wheel


bool currentLed = 0;
bool tripped = 0;

int fillCount = 0;
int readingCount = 0;
unsigned long lastTick;
unsigned long currentTick;
unsigned long firstTick;
#define counts 125    // this should be enough for 2 meters
int currPos[counts];
float velocity[counts];
unsigned long time[counts]; 


// copied from https://playground.arduino.cc/Main/PinChangeInterrupt

void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}



void checkPin(int pinno,int distance){
  if ((digitalRead(pinno) == 0)&tripped){
    currPos[fillCount%counts] = fillCount*perTick;
    currentTick = micros();
    if (lastTick ==0){
      firstTick = currentTick;
      velocity[fillCount%counts] = 0;
    }
       
    else
	velocity[fillCount%counts] = perTick/((currentTick-lastTick)/1000000.0); // in mm/s    
    time[fillCount%counts] = currentTick;
    lastTick = currentTick;
    fillCount = (fillCount+1);
  }
}


ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
 {
     // Pins should all be High, if one is low, that should be our 
     // leading edge
     currentLed = !currentLed;
     digitalWrite(ledPin,currentLed);

     
     checkPin(2,150);


 }  





void setup()
{
  Serial.begin(115200);
  fillCount = 0;
  readingCount = 0;
  tripped =0;
  pinMode(ledPin, OUTPUT);      // sets the digital pin as output
  pinMode(2, INPUT_PULLUP);
  pciSetup(2);
  //pciSetup(3);
  //pciSetup(4);
  //pciSetup(5);
  //pciSetup(6);
  //pciSetup(7);

  delay(100);
  tripped=1;
  
  
  Serial.println("----------------"); 
}

void loop()
{

 
  if (fillCount > readingCount){
    Serial.print(readingCount);
    Serial.print(",");
    Serial.print(currPos[readingCount%counts]);
    Serial.print(",");
    Serial.print(velocity[readingCount%counts]);
    Serial.print(",");
    
    Serial.println((time[readingCount%counts]-firstTick)/1000000.0,6);
    readingCount = readingCount+1;
  }
  if (Serial.available()){
    byte incomingByte = Serial.read();
    if (incomingByte == 'X'){
	//reset if we get a capital X
	readingCount = 0;
  lastTick = 0;
  firstTick =0;
	fillCount = 0;
      }
  }

}
