#pragma once

#include <vector>
#include <array>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/twai.h>

namespace aqua {

enum class VESCSimpleCommandID : uint32_t {
    CAN_PACKET_SET_DUTY = 0,
	CAN_PACKET_SET_CURRENT = 1,
	CAN_PACKET_SET_CURRENT_BRAKE = 2,
	CAN_PACKET_SET_RPM = 3,
	CAN_PACKET_SET_POS = 4,
	CAN_PACKET_SET_CURRENT_REL = 10,
	CAN_PACKET_SET_CURRENT_BRAKE_REL = 11,
	CAN_PACKET_SET_CURRENT_HANDBRAKE = 12,
	CAN_PACKET_SET_CURRENT_HANDBRAKE_REL = 13,
    CAN_PACKET_SET_SERVO = 69,
};

enum class VESCStatusCommandID : uint32_t {
    CAN_PACKET_STATUS = 9,
    CAN_PACKET_STATUS_2 = 14,	
    CAN_PACKET_STATUS_3 = 15,	
    CAN_PACKET_STATUS_4 = 16,	
    CAN_PACKET_STATUS_5 = 27,
    CAN_PACKET_STATUS_6 = 58,
};

enum class ADCChannel : uint8_t {
    ADC1, 
    ADC2,
    ADC3
};


void statusCommandPeriodicTransmission(void* pvParameters);

class VESC {
    public:
        static constexpr int STATUS_COMMAND_NUM = 6;
        explicit VESC(twai_general_config_t g_config, twai_timing_config_t t_config, uint32_t vesc_id);
        void setDuty(double duty); // これをメインで使う
        void setCurrent(double current);
        void setCurrentBrake(double current);
        void setRPM(double rpm);
        void setPOS(double pos);
        void setCurrentRel(double current);
        void setCurrentBrakeRel(double current);
        void setCurrentHandBrake(double current);
        void setCurrentHandBrakeRel(double current);
        void setServo(double value); // lispBMにより実装。0 ~ 1.0 
        void setServoAngle(double angle);

        double getERPM(void);
        double getCurrent(void);
        double getDuty(void);

        double getAmpHours(void); // Cheap FOCerには充電機能がない
        double getAmpHoursChg(void);

        double getWattHours(void);
        double getWattHoursChg(void);

        double getTempFet(void);
        double getTempMotor(void);
        double getCurrentIn(void);
        double getPIDPos(void);

        double getTachometer(void);
        double getVoltIn(void);

        double getADCData(ADCChannel adc);
        double getPPM(void);



    private:
        const uint8_t _vesc_id = {0x00};
        std::array<bool, STATUS_COMMAND_NUM> _transmit_status_command;

        static constexpr double duty_scale = 100000;
        static constexpr double current_scale = 1000;
        static constexpr double current_brake_scale = 1000;
        static constexpr double rpm_scale = 1;
        static constexpr double pos_scale = 1000000;
        static constexpr double current_rel_scale = 100000;
        static constexpr double current_brake_rel_scale = 100000;
        static constexpr double current_handbrake_scale = 1000;
        static constexpr double current_handbrake_rel_scale = 100000;

        static constexpr double servo_scale = 10000; //lispBMにより実装

        //CAN_PACKET_STATUS
        static constexpr double erpm_scale = 1;
        static constexpr double current_scale = 10;
        static constexpr double duty_scale = 1000;

        //CAN_PACKET_STATUS2
        static constexpr double amp_hour_scale = 10000;
        static constexpr double amp_hour_charge_scale = 10000;

        //CAN_PACKET_STATUS3
        static constexpr double watt_hour_scale = 10000;
        static constexpr double watt_hour_charge = 10000;

        //CAN_PACKET_STATUS4
        static constexpr double temp_fet_scale = 10;
        static constexpr double temp_motor_scale = 10;
        static constexpr double current_in_scale = 10;
        static constexpr double pid_pos_scale = 50;

        //CAN_PACKET_STATUS5
        static constexpr double tacometer_scale = 6;
        static constexpr double volts_in_scale = 10;

        //CAN_PACKET_STATUS6
        static constexpr double adc1_scale = 1000;
        static constexpr double adc2_scale = 1000;
        static constexpr double adc3_scale = 1000;
        static constexpr double ppm_scale = 1000;

        std::array<uint8_t, 4> bufferAppendInt32(int32_t value);
        void sendCommandPacket(VESCSimpleCommandID command_id, std::array<uint8_t, 4>data_array);
        double convertPos0To360(double pos);
};


}
