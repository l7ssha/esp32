#include "esphome.h"
#include <bitset>

class DeskKeypad : public Component, public UARTDevice, public Sensor
{

public:
    DeskKeypad(UARTComponent *parent) : UARTDevice(parent) {}

    enum Command
    {
        Up = 1,
        Down = 2,
        Preset1 = 3,
        Preset2 = 4,
        Preset3 = 5,
        Preset4 = 9,
        M = 6,
        Alarm = 7,
        Empty = 8
    };

    unsigned long packet[8];

    int msg_len = 0;
    int packet_index = -1;

    float get_setup_priority() const override { return esphome::setup_priority::DATA; }

    void setup() override
    {
        // nothing to do here
    }

    void loop() override
    {
        while (available() > 0)
        {
            packet_index++;

            auto incomingByte = read();

            // First byte, start of a packet
            if (incomingByte == 0x9b)
            {
                msg_len = 0;
                packet_index = 0;
                packet[0] = incomingByte;
                continue;
            }

            if (packet_index == 1)
            {
                msg_len = (int)incomingByte;
                packet[1] = incomingByte;
                continue;
            }

            // Third byte is message type
            if (packet_index == 2)
            {
                if (incomingByte != 0x02) {

                    packet_index = -1;
                    continue;
                }

                packet[2] = incomingByte;
                continue;
            }

            packet[packet_index] = incomingByte;

            if (packet_index < 7)
            {
                continue;
            }

            if (packet[3] == 0x00 && packet[4] == 0x00)
            {   
                publish_state(Command::Empty);
                continue;
            }

            auto mReturnCommand = Command::Empty;

            switch (packet[3])
            {
            case 0x01:
                mReturnCommand = Command::Up;
                break;
            case 0x02:
                mReturnCommand = Command::Down;
                break;
            case 0x04:
                mReturnCommand = Command::Preset1;
                break;
            case 0x08:
                mReturnCommand = Command::Preset2;
                break;
            case 0x10:
                mReturnCommand = Command::Preset3;
                break;
            case 0x00:
                mReturnCommand = Command::Preset4;
                break;
            case 0x20:
                mReturnCommand = Command::M;
                break;
            case 0x40:
                mReturnCommand = Command::Alarm;
                break;
            }

            if (msg_len == 6)
            {
                publish_state(mReturnCommand);
            }

            packet_index = -1;
        }
    }
};