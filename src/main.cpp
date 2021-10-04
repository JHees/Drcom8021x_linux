/*
 * main.cpp
 *
 *  Created on: May 11, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#include "clipp.h"
#include "function.h"
using namespace clipp;

int main(int argc, char **argv)
{
    drcom_config conf;
    enum class mode
    {
        conf,
        cmd,
        help
    };
    mode selectedMode = mode::help;
    std::string confpath = "";
    std::string mac_buf;

    auto cli = group(
        (required("-c", "--conf").set(selectedMode, mode::conf) & value("path", confpath)) % "path to config file"
        | ((option("-d", "--device") & value("interface", conf.local.nic)) % "name of network interface (default: eht0)",
           (required("-s", "--server-ip").set(selectedMode, mode::cmd).if_missing([]() { std::cout << "need to set the ip address of authentication server\n"; }).call([&]() { conf.local.ip = config::get_ip_address(conf.local.nic), conf.local.mac = config::get_mac_address(conf.local.nic); }) & value("host", conf.remote.ip)) % "ip address of authentication server",
           (option("-p", "--port") & integer("port", conf.remote.port)) % "port of authentication server (default port: 61440)",
           option("--no-redial").set(conf.general.auto_redial, false) % "do not reconnect after disconnection ",
           (option("--no-use-broadcast").set(conf.remote.use_broadcast, false) & value("mac", mac_buf).call([&]() { conf.remote.mac = config::str_mac_to_vec(mac_buf); })) % "do not broadcast and specify authentication server <mac> ",
           (option("--eap-timeout") & integer("time", conf.local.eap_timeout)) % "timeout for 802.1x authentication (ms)(default: 1000)",
           (option("--udp-timeout") & integer("time", conf.local.udp_timeout)) % "timeout for udp (ms)(default: 2000)",
           value("username", conf.general.username),
           value("password", conf.general.password))
        | (option("--help", "-h").set(selectedMode, mode::help) % "show this message" | option("--version", "-v").call([]() { std::cout << "version 1.0\n\n"; }) % "show version"));
    auto fmt = doc_formatting{}
                   .first_column(2)                                   //left border column for text body
                   .doc_column(28)                                    //column where parameter docstring starts
                   .last_column(100)                                  //right border column for text body
                   .indent_size(4)                                    //indent of documentation lines for children of a documented group
                   .line_spacing(0)                                   //number of empty lines after single documentation lines
                   .paragraph_spacing(1)                              //number of empty lines before and after paragraphs
                   .flag_separator(", ")                              //between flags of the same parameter
                   .param_separator(" ")                              //between parameters
                   .group_separator(" ")                              //between groups (in usage)
                   .alternative_param_separator("|")                  //between alternative flags
                   .alternative_group_separator(" | ")                //between alternative groups
                   .surround_group("(", ")")                          //surround groups with these
                   .surround_alternatives("(", ")")                   //surround group of alternatives with these
                   .surround_alternative_flags("", "")                //surround alternative flags with these
                   .surround_joinable("(", ")")                       //surround group of joinable flags with these
                   .surround_optional("[", "]")                       //surround optional parameters with these
                   .surround_repeat("", "...")                        //surround repeatable parameters with these
                   .surround_labels("<", ">")                         //surround values with these
                   .empty_label("")                                   //used if parameter has no flags and no label
                   .max_flags_per_param_in_usage(1)                   //max. # of flags per parameter in usage
                   .max_flags_per_param_in_doc(32)                    //max. # of flags per parameter in detailed documentation
                   .split_alternatives(true)                          //split usage into several lines for large alternatives
                   .alternatives_min_split_size(3)                    //min. # of parameters for separate usage line
                   .merge_alternative_flags_with_common_prefix(false) //-ab(cdxy|xy) instead of -abcdxy|-abxy
                   .merge_joinable_with_common_prefix(true)           //-abc instead of -a -b -c
                   .ignore_newline_chars(false)                       //ignore '\n' in docstrings
        ;
    if (!parse(argc, argv, cli) || selectedMode == mode::help)
    {
        std::cout << make_man_page(cli, argv[0], fmt);
        return 0;
    }

    func operate;
    if (selectedMode == mode::conf)
        operate.get_conf(confpath);
    if (selectedMode == mode::cmd)
        operate.get_conf(conf);
    operate.online();
    return 0;
}
