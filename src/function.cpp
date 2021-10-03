/*
 * function.cpp
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#include "config.h"
#include "drcomexception.h"
#include "eap_dealer.h"
#include "function.h"
#include "log.h"
#include <stdio.h>
#include <unistd.h>

void func::online()
{
    std::vector<uint8_t> broadcast_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    std::vector<uint8_t> nearest_mac = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x03};

    try
    {
        eap_dealer eap(conf.local.nic, conf.local.mac, conf.local.ip, conf.general.username, conf.general.password, conf);
        do // auto redial
        {
            bool flag = false;
            if (conf.remote.use_broadcast)
            {
                // eap.logoff(nearest_mac);
                eap.logoff(nearest_mac);
                flag = eap.start(broadcast_mac)
                       && eap.response_identity(broadcast_mac)
                       && eap.response_md5_challenge(broadcast_mac);
            }
            else
            {
                // eap.logoff(conf.remote.mac);
                eap.logoff(conf.remote.mac);
                flag = eap.start(conf.remote.mac)
                       && eap.response_identity(conf.remote.mac)
                       && eap.response_md5_challenge(conf.remote.mac);
            }
            while (flag) // Keep Alive
            {
                eap.alive_identity(broadcast_mac) ? alive_try = 0 : alive_try++;
                if (alive_try > 20)
                {
                    alive_try = 0;
                    SYS_LOG_ERR("Could not connect to server. Start to send connection request again!\n");
                    break;
                }
                sleep(1);
            }
        } while (conf.general.auto_redial);
    }
    catch (drcomException &e)
    {
        SYS_LOG_ERR("Thread Online : " << e.what() << std::endl);
    }
}