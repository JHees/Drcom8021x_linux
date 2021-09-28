/*
 * typedef.h
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 *     www.shareideas.net
 */

#ifndef HEADER_DEF_H_
#define HEADER_DEF_H_

#include <iostream>
#include <stdint.h>
#include <vector>

struct drcom_config
{
    struct config_general
    {
        std::string username;
        std::string password;
        bool auto_redial = true;
    } general;

    struct config_remote
    {
        std::string ip;
        uint32_t port = 61440;

        bool use_broadcast = true;
        std::vector<uint8_t> mac{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // broadcast
    } remote;

    struct config_local
    {
        std::string nic = "eth0";
        // ip and mac will load from nic
        std::string ip;
        std::vector<uint8_t> mac;

        uint32_t eap_timeout = 1000;
        uint32_t udp_timeout = 2000;
    } local;
};

struct eap_header
{
    uint8_t eapol_version;
    uint8_t eapol_type;    // 0x01 - Start, 0x02 - Logoff, 0x00 - EAP Packet
    uint16_t eapol_length; // equal to eap_length
    uint8_t eap_code;
    uint8_t eap_id;
    uint16_t eap_length;
    uint8_t eap_type;
    uint8_t eap_md5_value_size;
    uint8_t eap_md5_value[16];
};

enum ONLINE_STATE
{
    OFFLINE_PROCESSING,
    OFFLINE_NOTIFY,
    OFFLINE,
    ONLINE_PROCESSING,
    ONLINE,
};

#endif /* HEADER_DEF_H_ */
