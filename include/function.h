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
#include <iostream>

class func
{
public:
    func() : alive_try(0){}; // do nothing and initialize a empty class
    void get_conf(const drcom_config &c) { conf = c; };
    void get_conf(const std::string &path)
    {
        config(path, conf);
    }
    void online();

private:
    drcom_config conf;
    int alive_try;
};

#endif /* SRC_FUNCTION_H_ */
