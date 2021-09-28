/*
 * function.cpp
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#include "config.h"
#include "def.h"
#include "drcomexception.h"
#include "eap_dealer.h"
#include "function.h"
#include "log.h"
#include <stdio.h>
#include <unistd.h>


void func::online()
{
    if (!readflag)
    {
        return;
        cout << "read config file error, program stopped" << endl;
    }
    std::vector<uint8_t> broadcast_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    std::vector<uint8_t> nearest_mac = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x03};

    ONLINE_STATE state = ONLINE_PROCESSING;
    bool flag = false;

    try
    {
        eap_dealer eap(conf.local.nic, conf.local.mac, conf.local.ip, conf.general.username, conf.general.password, conf);
        while (state != OFFLINE_PROCESSING) // auto redial
        {
            if (conf.remote.use_broadcast)
            {
                eap.logoff(nearest_mac);
                eap.logoff(nearest_mac);
                flag = eap.start(broadcast_mac)
                       && eap.response_identity(broadcast_mac)
                       && eap.response_md5_challenge(broadcast_mac);
            }
            else
            {
                eap.logoff(conf.remote.mac);
                eap.logoff(conf.remote.mac);
                flag = eap.start(conf.remote.mac)
                       && eap.response_identity(conf.remote.mac)
                       && eap.response_md5_challenge(conf.remote.mac);
            }
            while (flag && state != OFFLINE_PROCESSING) // Keep Alive
            {
                //std::unique_lock<std::mutex> lock(mtx);
                if (eap.alive_identity(broadcast_mac))
                {
                    state = ONLINE;
                    alive_try = 0;
                }
                else
                {
                    alive_try++;
                }
                if (alive_try > 20)
                {
                    flag = false;
                    alive_try = 0;
                    SYS_LOG_ERR("Could not connect to server. Start to send connection requqest again!\n");
                    break;
                }
                sleep(1);
                //cv.wait_for(lock, std::chrono::seconds(1));
            }
            if (!conf.general.auto_redial)
            {
                break; //如果设置不自动重播，则退出，否则自动重播。
            }
        } /*##############  while   ################*/

        //	std::unique_lock<std::mutex> lock(mtx);
        //	state = OFFLINE_NOTIFY;
        //	cv.notify_one();
    }
    catch (drcomexception &e)
    {
        SYS_LOG_ERR("Thread Online : " << e.what() << std::endl);
    }
}

void func::offline()
{
}

func::~func()
{
    // TODO Auto-generated destructor stub
}
