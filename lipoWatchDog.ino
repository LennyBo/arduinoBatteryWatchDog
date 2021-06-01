#define voltmeterPIN A0
#define buzzerPIN 3

//Voltage settings
float const refVoltage = 4950; //This is the voltage (mv) between +5 and gnd that the ADC will use
float const analogMax = 1024; //It is a 10 bit ADC so 2^10
float const vStep = refVoltage/analogMax; //Each step mv value
float const Rtot = 55000; // Voltage divider values. Rtot is R1 + R2
float const R2 = 4960;    // R2 is the value of the second Resistance
float const multiplier = Rtot/R2;  // The value of the devider

float const perCellWarning = 3.7; // When the per cell voltage gets under this, the buzzer will beep the voltage
float const perCellAlarm = 3.6;   // When this value is reached, the alarm start and does not stop (pls stop)
float const cellCount = 10; // That would be a 10s lipo. The mesured voltage will be divided by cellCount to get perCellAvg

bool reachedWarning = false; // Will be true once perCellWarning has been reached once

//Beeper settings
int freqAlarm = 1000;
int const freqBeep = 2000;
bool goUp = true;




void setup() {
  Serial.begin(9600);
  analogReference(DEFAULT); //Is not needed but fuck it
  
  pinMode(buzzerPIN,OUTPUT);
  
  beepVoltage(getVoltageAvg()/cellCount); //Will beep the voltage on startup
  
  sei(); //enable interrupts
}



void loop() {
  float mesured = getVoltageAvg(); //Get the mesured voltage. On a 10s lipo the range is between 33-42V
  float avgCellVoltage = mesured / cellCount; // Transform to perCellAvg so 42V->4.2V per cell

  //Debug
  Serial.println(String(avgCellVoltage) + " V cell\t" + String(mesured)+" V total\t" + String(analogRead(voltmeterPIN)));
  Serial.println(vStep);
  
 if(!reachedWarning && avgCellVoltage <= perCellWarning){ //If we get bellow warning for the first time since startup
    beepVoltage(avgCellVoltage);
    reachedWarning = true;
  }else if(avgCellVoltage <= perCellAlarm){ //If we get bellow alarm
    alarm();
  }
  delay(500); //Chill down (could remove that but is easier to debug and also faster is not needed)
}

//Takes the average of 3 mesures and transforms from mV to V
float getVoltageAvg(){
  return (getMVoltage()+getMVoltage()+getMVoltage())/3000;
}

//mv
float getMVoltage(){
  return (analogRead(voltmeterPIN)*vStep * multiplier);
}

//Will beep the alarm forever
void alarm(){
  while(1){
    //Serial.println(String(freqAlarm));
    tone(buzzerPIN,freqAlarm);
    delay(2);
    freqAlarm = goUp ? freqAlarm + 10: freqAlarm-10;
    if(freqAlarm < 1000 || freqAlarm > 5000){
      goUp = !goUp;
    }
  }
}

//Beeps count times with wait delay * 2
void multiBeep(int count,int wait){
  for(int i = 0;i<count;i++){
    tone(buzzerPIN,freqBeep);
    delay(wait);
    noTone(buzzerPIN);
    delay(wait);
  }
}

//With an input of 4.2 this will do
//Beep Beep Beep Beep
//1 sec wait
//Beep Beep
//
//Usefull to know where the cells are at
void beepVoltage(float voltage){
  multiBeep(int(voltage),500);
  delay(1000);
  multiBeep(int((voltage - int(voltage))*10),500);
  
}
