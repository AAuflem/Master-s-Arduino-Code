#include <Wire.h>
#include <Adafruit_DRV2605.h>

Adafruit_DRV2605 drv;

// Declare and initialize BPM globally
float BPM; // Set your desired BPM here

// Calculate BPS (Beats Per Second) from BPM
float BPS = BPM / 60;

int TempStatus;

// 1 = healty, 0 = Hypothermic

//Flag to decide bpm

bool HealtyFlag = false;
bool HypothermFlag = false;

void setup() {
  Serial.begin(115200);
  drv.begin();
  
  drv.selectLibrary(1); // Choose a library of effects
  
  drv.setMode(DRV2605_MODE_INTTRIG); // Use internal trigger mode
}

void loop() {

    // Check if data is available to read.
  if (Serial.available() > 0) {
    // Read the incoming byte:
    char receivedChar = Serial.read();

    // Check the received command
    if (receivedChar == '1') 
    {
      HealtyFlag = true;
      HypothermFlag = false;
    } 
    else if (receivedChar == '0') 
    {
      HealtyFlag = false;
      HypothermFlag = true;
    }
  }

  if (HealtyFlag == true)
  {
    drv.setWaveform(0, 110); // First part of the heartbeat effect
    drv.setWaveform(0, 79);  // Second part of the heartbeat effect, corrected index to 1
    drv.setWaveform(1, 0);   // End of waveform sequence, corrected index to 2
    BPM = 60;
    Serial.print("Healthy, ");
    Serial.print("BPM =");
    Serial.println(BPM);
    drv.go(); // Activate the motor
    delay(1000);
  }

  if (HypothermFlag == true)
    {
      drv.setWaveform(0, 110); // First part of the heartbeat effect
      drv.setWaveform(0, 79);  // Second part of the heartbeat effect, corrected index to 1
      drv.setWaveform(1, 0);   // End of waveform sequence, corrected index to 2
      BPM = 20;
      Serial.print("Hypothermic, ");
      Serial.print("BPM =");
      Serial.println(BPM);
      drv.go(); // Activate the motor
      delay(3000);
    }  

  

 // Adjust timing to match desired heartbeat rate, convert seconds to milliseconds
 //delay(((BPS) * 1000));
  
}
