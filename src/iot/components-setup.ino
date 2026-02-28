#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

constexpr uint8_t MOTOR_PIN = 9;
constexpr uint8_t DT_PIN = 2;
constexpr uint8_t SDA_PIN = 3;
constexpr uint8_t SCL_PIN = 4;
constexpr uint8_t INT_PIN = 10;

class VibrationMotor {
    private:
        const uint8_t _pin;
    
    public:
        VibrationMotor(uint8_t pin) : _pin(pin) {}
        
        void begin() const {
            pinMode(_pin, OUTPUT);
            stop();
        }
        
        void vibrate(uint8_t intensity = 255) const { analogWrite(_pin, intensity); }
        
        void stop() const { analogWrite(_pin, 0); }
};

class IMUSensor {
    private:
        Adafruit_MPU6050 _mpu;
        const uint8_t _sdaPin;
        const uint8_t _sclPin;

    public:
        IMUSensor(uint8_t sdaPin, uint8_t sclPin)
            : _sdaPin(sdaPin), _sclPin(sclPin) {}
        
        bool begin() {
            Wire.begin(_sdaPin, _sclPin);
        
            if (!_mpu.begin()) {
            return false;
            }
        
            _mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
            _mpu.setGyroRange(MPU6050_RANGE_500_DEG);
            _mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        
            return true;
        }

        void printData() {
            sensors_event_t accel, gyro, temp;
            _mpu.getEvent(&accel, &gyro, &temp);
        
            Serial.print("Gyro (X, Y, Z): ");
            Serial.print(gyro.gyro.x);
            Serial.print(", ");
            Serial.print(gyro.gyro.y);
            Serial.print(", ");
            Serial.print(gyro.gyro.z);
            Serial.println(" rad/s");
        
            Serial.print("Accel (X, Y, Z): ");
            Serial.print(accel.acceleration.x);
            Serial.print(", ");
            Serial.print(accel.acceleration.y);
            Serial.print(", ");
            Serial.print(accel.acceleration.z);
            Serial.println(" m/s^2");
        }
};

VibrationMotor hapticMotor(MOTOR_PIN);
IMUSensor gyroscope(SDA_PIN, SCL_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  hapticMotor.begin();

  while (!gyroscope.begin())
    delay(1000);
}

void loop() {
  hapticMotor.vibrate();

  gyroscope.printData();
  Serial.println("-------------------------");

  delay(500);
}
