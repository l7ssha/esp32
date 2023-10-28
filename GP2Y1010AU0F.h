#include "esphome.h"

static const char *TAG = "GP2Y1010AU0F.sensor";

#define VOLTAGE_VREF 3.33

#define PIN_ADC 33
#define PIN_LED 18

#define SAMPLING_TIME 280

#define COV_RATIO 151
#define NO_DUST_VOLTAGE 0.4

#define UPDATE_INTERVAL 60000

class GP2Y1010AU0F : public PollingComponent, public Sensor
{
public:
    GP2Y1010AU0F() : PollingComponent(UPDATE_INTERVAL) {}

    float get_setup_priority() const override { return esphome::setup_priority::DATA; }

    void setup() override
    {
        ESP_LOGCONFIG(TAG, "Setting up sensor...");

        pinMode(PIN_LED, OUTPUT);
        digitalWrite(PIN_LED, LOW);
    }

    void update() override
    {
        digitalWrite(PIN_LED, HIGH);
        delayMicroseconds(SAMPLING_TIME);
        float adcvalue = float(analogRead(PIN_ADC));
        digitalWrite(PIN_LED, LOW);

        float voltage = (VOLTAGE_VREF / 1024.0) * adcvalue;
        
        float density = 0.0;
        if (voltage > NO_DUST_VOLTAGE) {
            density = (voltage - NO_DUST_VOLTAGE) * COV_RATIO;
        }

        ESP_LOGCONFIG(TAG, "Measurements done, VRef: %f, ADC Value: %f, Final Voltage: %f, Density: %f", VOLTAGE_VREF, adcvalue, voltage, density);
        publish_state(density);
    }
};