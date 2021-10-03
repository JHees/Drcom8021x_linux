/*
 * function.h
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#ifndef SRC_FUNCTION_H_
#define SRC_FUNCTION_H_

#include "config.h"
#include "drcomexception.h"
#include "eap_dealer.h"
#include "log.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
class func
{
public:
    func() : alive_try(0){}; // do nothing and initialize a empty class
    void get_conf(const drcom_config &c) { conf = c; };
    void get_conf(const std::string &path)
    {
        config(path, conf);
    }
    void online()
    {
        try
        {
            eap_dealer eap(conf.local.nic, conf.local.mac, conf.local.ip, conf.general.username, conf.general.password, conf);
            do // auto redial
            {
                bool flag = false;
                flag = eap.start(conf.remote.mac)
                       && eap.response_identity(conf.remote.mac)
                       && eap.response_md5_challenge(conf.remote.mac);

                while (flag) // Keep Alive
                {
                    eap.alive_identity(conf.remote.mac) ? alive_try = 0 : alive_try++;
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

private:
    drcom_config conf;
    int alive_try;
};

#endif /* SRC_FUNCTION_H_ */
