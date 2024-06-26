#include "AK09918.h"
#include "ICM20600.h"
#include <Wire.h>

AK09918_err_type_t err;
int32_t x, y, z;
AK09918 ak09918;
ICM20600 icm20600(true);
int16_t acc_x, acc_y, acc_z;
int32_t offset_x, offset_y, offset_z;
double roll, pitch;
// Find the magnetic declination at your location
// http://www.magnetic-declination.com/
double declination_shenzhen = 4.50;

bool runLoop = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    err = ak09918.initialize();
    icm20600.initialize();
    ak09918.switchMode(AK09918_POWER_DOWN);
    ak09918.switchMode(AK09918_CONTINUOUS_100HZ);
    Serial.begin(115200);

    err = ak09918.isDataReady();
    while (err != AK09918_ERR_OK) {
        Serial.println("Waiting Sensor");
        delay(100);
        err = ak09918.isDataReady();
    }
}

void loop() {
    // Check if serial input is available
    if (Serial.available() > 0) {
        int command = Serial.parseInt();
        if (command == 1) {
            runLoop = true;
        }
    }

    // If runLoop is true, execute the main loop code
    if (runLoop) {
        // get acceleration
        acc_x = icm20600.getAccelerationX();
        acc_y = icm20600.getAccelerationY();
        acc_z = icm20600.getAccelerationZ();

        ak09918.getData(&x, &y, &z);
        x = x - offset_x;
        y = y - offset_y;
        z = z - offset_z;

        // pitch/roll in radian
        pitch = atan2((float)acc_y, (float)acc_z);
        roll = atan2(-(float)acc_x, sqrt((float)acc_y * acc_y + (float)acc_z * acc_z));
        Serial.print(pitch * 57.3);
        Serial.print(" , ");
        Serial.println(roll * 57.3);

        double Xheading = x * cos(roll) + y * sin(pitch) * sin(roll) + z * cos(pitch) * sin(roll);
        double Yheading = y * cos(pitch) - z * sin(pitch);

        double heading = 180 + 57.3 * atan2(Yheading, Xheading) + declination_shenzhen;
        
        delay(10);
    }
}
