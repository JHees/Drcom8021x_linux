/*
 * eapdealer.h
 *
 *  Created on: May 14, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#ifndef SRC_EAPDEALER_H_
#define SRC_EAPDEALER_H_

#include "md5.h"
#include "pcap_dealer.h"
#include <iostream>
#include <vector>
#define DRCOM_EAP_FRAME_SIZE (0x60)
#define EAP_MD5_VALUE_SIZE   (0x10)
#define MAX_RETRY_TIME       2

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

#define EAP_SHOW_PACKET_TYPE(step)                \
    EAP_LOG_DBG("Recevied after " << step << ", " \
                                  << "eapol_type = 0x" << std::hex << (int)eap_header->eapol_type << ", eap_id = 0x" << std::hex << (int)eap_header->eap_id << ", eap_type = 0x" << std::hex << (int)eap_header->eap_type << ", eap_length = " << (int)eap_header->eap_length << std::endl);

#define EAP_HANDLE_ERROR(step) EAP_LOG_ERR(step << ": " << error << std::endl)

class eap_dealer
{
public:
    eap_dealer(string nic, vector<uint8_t> local_mac, std::string local_ip, std::string identity, std::string key, drcom_config conf);

    struct ether_header get_eth_header(std::vector<uint8_t> gateway_mac, std::vector<uint8_t> local_mac);
    int start(std::vector<uint8_t> gateway_mac);
    int logoff(std::vector<uint8_t> gateway_mac);
    int response_identity(std::vector<uint8_t> gateway_mac);
    int alive_identity(std::vector<uint8_t> gateway_mac);
    int response_md5_challenge(std::vector<uint8_t> gateway_mac);

private:
    pcap_dealer pcap;
    //int rec;
    int resp_eap_id;
    int resp_md5_eap_id;   // Recved from Request, MD5-Challenge EAP
    uint8_t response[128]; //数据包
    int begintime;

    std::vector<uint8_t> local_mac; // Const
    std::vector<uint8_t> resp_id, resp_md5_id, key;
    std::vector<uint8_t> alive_data;
    std::vector<uint8_t> resp_md5_attach_key; // Recved from Request, Identity
private:
    std::vector<uint8_t> get_md5_digest(std::vector<uint8_t> &data)
    {
        md5_byte_t digest[16];
        md5_state_t state;

        md5_init(&state);
        md5_append(&state, &data[0], (int)data.size());
        md5_finish(&state, digest);

        return std::vector<uint8_t>(digest, digest + 16);
    }

    std::vector<std::string> split_string(std::string src, char delimiter = ' ', bool append_last = true)
    {
        std::string::size_type pos = 0;
        std::vector<std::string> ret;

        while ((pos = src.find(delimiter)) != std::string::npos)
        {
            ret.push_back(src.substr(0, pos));
            src = src.substr(pos + 1);
        }

        // the last element
        if (append_last) ret.push_back(src);

        return ret;
    }

    std::vector<uint8_t> str_ip_to_vec(std::string ip)
    {
        std::vector<uint8_t> ret(4, 0);

        auto vec_addr = split_string(ip, '.');
        if (vec_addr.size() < 4)
            return ret;

        unsigned long addr = (atol(vec_addr[0].c_str()) << 24) + (atol(vec_addr[1].c_str()) << 16) + (atol(vec_addr[2].c_str()) << 8) + atol(vec_addr[3].c_str());
        addr = ntohl(addr);

        memcpy(&ret[0], &addr, 4);

        return ret;
    }

    std::vector<uint8_t> str_to_vec(std::string str)
    {
        std::vector<uint8_t> ret(str.length(), 0);
        memcpy(&ret[0], &str[0], str.length());
        return ret;
    }
};

#endif /* SRC_EAPDEALER_H_ */
