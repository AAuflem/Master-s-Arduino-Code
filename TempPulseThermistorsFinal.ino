//#####################DEFINES########################
#define MOSFET_Pin 3
#define ThermistorPin1 A0
#define ThermistorPin2 A1
#define ThermistorPin3 A2
#define ThermistorPin4 A3   

int V0_1, V0_2, V0_3, V0_4 ;
float logR2_1, R2_1, T_1, Tc_1;
float logR2_2, R2_2, T_2, Tc_2;
float logR2_3, R2_3, T_3, Tc_3;
float logR2_4, R2_4, T_4, Tc_4;
float R1 = 10000;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// flag to control logic
bool printFlag2 = false;
bool ReadSensor = false;

int Heatwire_Signal = 0;

unsigned long SensorPreviousTime = 0;
unsigned long RunTime;
unsigned long CurrentTime;
unsigned long interval = 10;                //100Hz
unsigned long HeatTime = 5*1000;            //5s
unsigned long CoolDownTime = 55*1000;       //55s
unsigned long TestTime = 22*60*1000;        //62min


//#####################FUNCTIONS######################
void ThermistorReading()
{
  V0_1 = analogRead(ThermistorPin1);
  R2_1 = R1 * (1023.0 / (float)V0_1 - 1.0);
  logR2_1 = log(R2_1);
  T_1 = (1.0 / (c1 + c2*logR2_1 + c3*logR2_1*logR2_1*logR2_1));
  Tc_1 = T_1 - 273.15;

  V0_2 = analogRead(ThermistorPin2);
  R2_2 = R1 * (1023.0 / (float)V0_2 - 1.0);
  logR2_2 = log(R2_2);
  T_2 = (1.0 / (c1 + c2*logR2_2 + c3*logR2_2*logR2_2*logR2_2));
  Tc_2 = T_2 - 273.15;

  V0_3 = analogRead(ThermistorPin3);
  R2_3 = R1 * (1023.0 / (float)V0_3 - 1.0);
  logR2_3 = log(R2_3);
  T_3 = (1.0 / (c1 + c2*logR2_3 + c3*logR2_3*logR2_3*logR2_3));
  Tc_3 = T_3 - 273.15;

  V0_4 = analogRead(ThermistorPin4);
  R2_4 = R1 * (1023.0 / (float)V0_4 - 1.0);
  logR2_4 = log(R2_4);
  T_4 = (1.0 / (c1 + c2*logR2_4 + c3*logR2_4*logR2_4*logR2_4));
  Tc_4 = T_4 - 273.15; 
}

void TemperaturePrint()
{
    //Current time
    unsigned long SensorCurrentTime = millis();

    //Check if the given time "interval" have passed since the last sensor reading
    if (SensorCurrentTime - SensorPreviousTime >= interval)
    {
        //Saving the last time the sensor value was read
        SensorPreviousTime = SensorCurrentTime;
        //Print readings to Serial monitor
        Serial.print(Tc_1);
        Serial.print(" , ");
        Serial.print(Tc_2);
        Serial.print(" , ");
        Serial.print(Tc_3);
        Serial.print(" , ");
        Serial.print(Tc_4);
        Serial.print(" , ");
        Serial.println(Heatwire_Signal);
    } 
}

void LED_Blinking()
{
  unsigned long startTime = millis();  // Record the start time
  int delayTime = 1000;                // Initial delay time in milliseconds (1 second)
  int TimeRemaining = 10;

  while (millis() - startTime < 10000) {  // Continue the loop for 10 seconds
    delay(delayTime);              // Wait for 'delayTime' milliseconds
    TimeRemaining -= 1;    
  }
}

//##############################MAIN##############################

enum STATE {INIT, TempPulse, TempOff, TestFinished}; // Enumerator for system states
STATE current_state{INIT};

void setup() 
{
  Serial.begin(115200);
  current_state = INIT;
  pinMode(ThermistorPin1, INPUT);
  pinMode(ThermistorPin2, INPUT);
  pinMode(ThermistorPin3, INPUT);
  pinMode(ThermistorPin4, INPUT);
  //pinMode(AmbientTemp, INPUT);
  pinMode(MOSFET_Pin, OUTPUT);
  digitalWrite(MOSFET_Pin, LOW); //analogWrite values from 0 to 255
  delay(3000);
  Serial.println("Ready!");
}

void loop() 
{
    ThermistorReading();
    if (ReadSensor) 
    {
        TemperaturePrint();
    }

    switch(current_state)
    {
      case (INIT):
        digitalWrite(MOSFET_Pin, LOW);
        Heatwire_Signal = 0;    
        if (Serial.available() > 0) 
        {
          char command = Serial.read();
          if (command == '1') 
          {
            ReadSensor = true;
            LED_Blinking();
            RunTime = millis();
            CurrentTime = millis();
            current_state = TempPulse;
            break;
          } 
        }
        break;
      
      case (TempPulse):
        digitalWrite(MOSFET_Pin, HIGH);
        Heatwire_Signal = 1;
        if(millis() - CurrentTime >= HeatTime)
        {
          current_state = TempOff;
          CurrentTime = millis();
          break;
        }
        break;
      
      case (TempOff):
        digitalWrite(MOSFET_Pin, LOW);
        Heatwire_Signal = 0;
        if (millis() - CurrentTime >= CoolDownTime) 
        {
          if (millis() - RunTime >= TestTime) 
          {
            current_state = TestFinished;
            break;
          } 
          else 
          {
            CurrentTime = millis();
            current_state = TempPulse;
            break;
          }
        }
        break; 
        
      case (TestFinished):
        if(!printFlag2)
        {
          Serial.println("Test Finished!");
          printFlag2 = true;
          ReadSensor = false;
        }  
    }
} 
