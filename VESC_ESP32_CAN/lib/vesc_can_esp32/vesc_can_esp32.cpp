#include "vesc_can_esp32.hpp"
#include <Arduino.h>
#include <esp_log.h>

namespace aqua {

VESC::VESC(twai_general_config_t g_config, twai_timing_config_t t_config, uint32_t vesc_id) : _vesc_id(vesc_id) 
{
    const char* TAG = "constructor";
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    if (ESP_OK == twai_driver_install(&g_config, &t_config, &f_config)) {
        ESP_LOGI(TAG, "Installed driver.");
    }
    else {
        ESP_LOGE(TAG, "Failed to Installed driver.");
    }
    if (twai_start() == ESP_OK) {
        ESP_LOGI(TAG, "CAN driver started.");
    }
    else {
        ESP_LOGI(TAG, "Failed to start driver.");
    }
}


void VESC::setDuty(double duty) {
    duty = constrain(duty, -1.0, 1.0);
    int32_t duty_scaled = static_cast<int32_t>(duty * duty_scale);
    std::array<uint8_t, 4>data_array = bufferAppendInt32(duty_scaled);
    sendCommandPacket(VESCSimpleCommandID::CAN_PACKET_SET_DUTY, data_array);
}

void VESC::setCurrent(double current) {
    int32_t current_scaled = static_cast<int32_t>(current * current_scale);
    std::array<uint8_t, 4>data_array = bufferAppendInt32(current_scaled);
    sendCommandPacket(VESCSimpleCommandID::CAN_PACKET_SET_CURRENT, data_array);
}

void VESC::setCurrentBrake(double current) {
    int32_t current_brake_scaled = static_cast<int32_t>(current * current_brake_scale);
    std::array<uint8_t, 4>data_array = bufferAppendInt32(current_brake_scaled);
    sendCommandPacket(VESCSimpleCommandID::CAN_PACKET_SET_CURRENT_BRAKE, data_array);
}

void VESC::setRPM(double rpm) {
    int32_t rpm_scaled = static_cast<int32_t>(rpm * rpm_scale);
    std::array<uint8_t, 4>data_array = bufferAppendInt32(rpm_scaled);
    sendCommandPacket(VESCSimpleCommandID::CAN_PACKET_SET_RPM, data_array);
}

void VESC::setPOS(double pos) {
    pos = convertPos0To360(pos);
    int32_t pos_scaled = static_cast<int32_t>(pos * pos_scale);
    std::array<uint8_t, 4>data_array = bufferAppendInt32(pos_scaled);
    sendCommandPacket(VESCSimpleCommandID::CAN_PACKET_SET_POS, data_array);
}

void VESC::setCurrentRel(double current_rel) {
    int32_t current_rel_scaled = static_cast<int32_t>(current * current_rel_scale);
    std::array<uint8_t, 4>data_array = bufferAppendInt32(current_rel_scaled);
    sendCommandPacket(VESCSimpleCommandID::CAN_PACKET_SET_CURRENT_REL, data_array);
}

void VESC::setCurrentBrakeRel(double ratio) {

}

void VESC::setCurrentHandBrake(double current) {

}

void VESC::setCurrentHandBrakeRel(double ratio) {

}

void VESC::setServo(double value) {
    value = constrain(value, 0, 1.0);
    int32_t value_scaled = static_cast<int32_t>(value * servo_scale);
    std::array<uint8_t, 4>data_array = bufferAppendInt32(value_scaled);
    sendCommandPacket(VESCSimpleCommandID::CAN_PACKET_SET_SERVO, data_array);
}

std::array<uint8_t, 4> VESC::bufferAppendInt32(int32_t value) {
    std::array<uint8_t, 4>data_array;
    data_array.at(0) = static_cast<uint8_t>(value >> 24);
    data_array.at(1) = static_cast<uint8_t>(value >> 16 & 0xFF);
    data_array.at(2) = static_cast<uint8_t>(value >> 8 & 0xFF);
    data_array.at(3) = static_cast<uint8_t>(value & 0xFF);
    return data_array;
}

void VESC::sendCommandPacket(VESCSimpleCommandID command_id, std::array<uint8_t, 4>data_array) {
    const char* TAG = "send";
    twai_message_t message;
    std::copy(data_array.begin(), data_array.end(), message.data);
    message.data_length_code = static_cast<uint8_t>(data_array.size());
    message.dlc_non_comp = 0;
    message.extd = 1;
    message.identifier = (static_cast<uint32_t>(command_id) & 0xFF) << 8 | (_vesc_id & 0xFF);
    message.rtr = 0;
    message.self = 0;
    message.ss = 0;
    Serial.println(message.identifier);
    if (twai_transmit(&message, 10) == ESP_OK) {
        ESP_LOGI(TAG, "Message queued for transmission");
    }
    else {
        ESP_LOGE(TAG, "Failed to queue message for transmission\n");
    }
}


double convertPos0To360(double pos) {
    while (1) {
        if (pos > 360) {
            pos -= 360;
        }
        else if (pos < 0) {
            pos += 360
        }
        else {
            return pos;
        }
    }
}

}