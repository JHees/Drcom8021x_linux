/*
 * config.h
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include "drcomexception.h"
#include <boost/core/noncopyable.hpp>
#include <boost/exception/exception.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <cstdlib>
#include <errno.h>
#include <exception>
#include <iostream>
#include <string.h>
#include <string>
#include <type_traits>
#include <vector>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "log.h"
// #include "utils.h"
using namespace std;

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

class config : boost::noncopyable
{
public:
    config(const std::string &path, drcom_config &conf)
    {
        try
        {
            boost::property_tree::ini_parser::read_ini(path, pt);
        }
        catch (std::exception &e)
        {
            SYS_LOG_ERR("Failed to read '" << path << "' - " << e.what() << std::endl);
            throw e;
        }

        try
        {
            conf.general.username = pt.get<std::string>("General.UserName");
            conf.general.password = pt.get<std::string>("General.PassWord");
            conf.remote.ip = pt.get<std::string>("Remote.IP");
        }
        catch (std::exception &e)
        {
            SYS_LOG_ERR("Failed to fetch necessary info at " << path << "' - " << e.what() << std::endl);
            throw e;
        }
        get4pt(pt, conf.local.nic, "Local.NIC");
        try
        {
            conf.local.ip = get_ip_address(conf.local.nic);
        }
        catch (drcomException &e)
        {
            SYS_LOG_ERR("Failed on" << e.what() << std::endl);
        }

        conf.local.mac = get_mac_address(conf.local.nic);
        get4pt(pt, conf.general.auto_redial, "General.AutoRedial");
        get4pt(pt, conf.remote.port, "Remote.Port");
        get4pt(pt, conf.remote.use_broadcast, "Remote.UseBroadcast");
        if (conf.remote.use_broadcast)
            get4pt(pt, conf.remote.mac, "Remote.MAC", str_mac_to_vec);
        get4pt(pt, conf.local.eap_timeout, "Local.EAPTimeout");
        get4pt(pt, conf.local.udp_timeout, "Local.UDPTimeout");

        SYS_LOG_INFO("Fetch NIC IP & MAC successfully." << std::endl);
        SYS_LOG_DBG("Local.NIC = " << conf.local.nic << endl);
        SYS_LOG_INFO("Local.IP = " << conf.local.ip << ", Local.MAC = " << vec_mac_to_str(conf.local.mac) << endl);
        SYS_LOG_DBG("General.UserName = " << conf.general.username << ", General.PassWord = " << conf.general.password << endl);
        SYS_LOG_DBG("General.AutoRedial = " << (conf.general.auto_redial ? "True" : "False") << endl);
        SYS_LOG_DBG("Remote.IP:Port = " << conf.remote.ip << ":" << conf.remote.port << ", Remote.UseBroadcast = " << (conf.remote.use_broadcast ? "True" : "False") << endl);
        SYS_LOG_DBG("Remote.MAC = " << vec_mac_to_str(conf.remote.mac) << endl);
        SYS_LOG_DBG("Local.EAPTimeout = " << conf.local.eap_timeout << ", Local.UDPTimeout = " << conf.local.udp_timeout << endl);
        SYS_LOG_INFO("Loaded config successfully." << std::endl);
    };

public:
    static std::vector<uint8_t> get_mac_address(std::string nic)
    {
        int sock;
        struct ifreq dev;

        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            throw drcomException("get_mac_address: socket failed"); //  errno;  //##
        }

        strncpy(dev.ifr_name, nic.c_str(), sizeof(dev.ifr_name));
        dev.ifr_name[sizeof(dev.ifr_name) - 1] = '\0';

        if (ioctl(sock, SIOCGIFHWADDR, &dev) < 0)
        {
            throw drcomException("get_mac_address: ioctl failed"); //, errno);  //##
        }

        std::vector<uint8_t> ret(6, 0);
        memcpy(&ret[0], dev.ifr_hwaddr.sa_data, 6);
        return ret;
    }

    static std::string get_ip_address(std::string nic)
    {
        struct ifaddrs *ifaddr = NULL;
        std::string ip;

        if (getifaddrs(&ifaddr) < 0)
        {
            throw drcomException("get_ip_address: getifaddrs failed");
        }
        for (; ifaddr != NULL; ifaddr = ifaddr->ifa_next)
        {
            if (!strcmp(ifaddr->ifa_name, nic.c_str())
                && ifaddr->ifa_addr->sa_family == AF_INET) // only deal with IPv4
            {
                ip = inet_ntoa(((struct sockaddr_in *)ifaddr->ifa_addr)->sin_addr);
                break;
            }
        }

        if (ip.empty())
        {
            throw drcomException("get_ip_address: NIC '" + nic + "' not found."); //##
        }
        return ip;
    }
    static std::vector<uint8_t> str_mac_to_vec(const std::string &mac)
    {
        std::vector<uint8_t> ret;
        auto p = mac.c_str();
        for (; p < mac.end().base(); p += 3)
            ret.push_back(std::strtol(p, nullptr, 16));
        return ret;
    }
    static std::string vec_mac_to_str(std::vector<uint8_t> mac)
    {
        constexpr const static char table[] = "0123456789ABCDEF";
        std::string ret;
        for (auto i : mac)
        {
            ret += table[(i & 0xF0) >> 4];
            ret += table[i & 0x0F];
            ret += ':';
        }
        ret.pop_back();
        return ret;
    }

private:
    template <class U, class T = U>
    static bool get4pt(
        const boost::property_tree::ptree &pt,
        U &conf,
        const std::string &name,
        U (*call)(T) = [](T v) -> U { return v; }) noexcept
    {
        auto opt = pt.get_optional<
            typename std::remove_const<
                typename std::remove_reference<
                    T>::type>::type>(name);

        return opt.is_initialized() ? (conf = call(opt.value())), 1 : 0;
    }

    boost::property_tree::ptree pt;
};

#endif /* SRC_CONFIG_H_ */
