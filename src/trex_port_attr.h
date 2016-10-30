/*
Copyright (c) 2015-2015 Cisco Systems, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __TREX_PORT_ATTR_H__
#define __TREX_PORT_ATTR_H__

#include <string>
#include <vector>
#include "rte_ethdev_includes.h"
#include "trex_defs.h"


class TRexPortAttr {
public:
    virtual ~TRexPortAttr(){}

/*    UPDATES    */
    virtual void update_link_status() = 0;
    virtual bool update_link_status_nowait() = 0; // returns true if the status was changed
    virtual void update_device_info() = 0;
    virtual void reset_xstats() = 0;
    virtual void update_description() = 0;

/*    GETTERS    */
    virtual bool get_promiscuous() = 0;
    virtual void macaddr_get(struct ether_addr *mac_addr) = 0;
    virtual uint32_t get_link_speed() { return m_link.link_speed; } // L1 Mbps
    virtual bool is_link_duplex() { return (m_link.link_duplex ? true : false); }
    virtual bool is_link_autoneg() { return (m_link.link_autoneg ? true : false); }
    virtual bool is_link_up() { return (m_link.link_status ? true : false); }
    virtual int get_xstats_values(xstats_values_t &xstats_values) = 0;
    virtual int get_xstats_names(xstats_names_t &xstats_names) = 0;
    virtual int get_flow_ctrl(int &mode) = 0;
    virtual bool is_virtual() { return flag_is_virtual; }
    virtual bool is_fc_change_supported() { return flag_is_fc_change_supported; }
    virtual bool is_led_change_supported() { return flag_is_led_change_supported; }
    virtual bool is_link_change_supported() { return flag_is_link_change_supported; }
    virtual void get_description(std::string &description) { description = intf_info_st.description; }
    virtual void get_supported_speeds(supp_speeds_t &supp_speeds) = 0;

/*    SETTERS    */
    virtual int set_promiscuous(bool enabled) = 0;
    virtual int add_mac(char * mac) = 0;
    virtual int set_link_up(bool up) = 0;
    virtual int set_flow_ctrl(int mode) = 0;
    virtual int set_led(bool on) = 0;

/*    DUMPS    */
    virtual void dump_link(FILE *fd) = 0;

protected:
    uint8_t m_port_id;
    rte_eth_link m_link;
    struct rte_eth_dev_info dev_info;
    bool flag_is_virtual;
    bool flag_is_fc_change_supported;
    bool flag_is_led_change_supported;
    bool flag_is_link_change_supported;

    struct intf_info_st {
        std::string     pci_addr;
        std::string     description;
        int             numa_node;
    }intf_info_st;

};

class DpdkTRexPortAttr : public TRexPortAttr {
public:

    DpdkTRexPortAttr(uint8_t port_id, bool is_virtual, bool fc_change_allowed) {
        m_port_id = port_id;
        flag_is_virtual = is_virtual;
        int tmp;
        flag_is_fc_change_supported = fc_change_allowed && (get_flow_ctrl(tmp) != -ENOTSUP);
        flag_is_led_change_supported = (set_led(true) != -ENOTSUP);
        flag_is_link_change_supported = (set_link_up(true) != -ENOTSUP);
        update_description();
        update_device_info();
    }

/*    UPDATES    */
    virtual void update_link_status();
    virtual bool update_link_status_nowait(); // returns true if the status was changed
    virtual void update_device_info();
    virtual void reset_xstats();
    virtual void update_description();

/*    GETTERS    */
    virtual bool get_promiscuous();
    virtual void macaddr_get(struct ether_addr *mac_addr);
    virtual int get_xstats_values(xstats_values_t &xstats_values);
    virtual int get_xstats_names(xstats_names_t &xstats_names);
    virtual int get_flow_ctrl(int &mode);
    virtual void get_supported_speeds(supp_speeds_t &supp_speeds);

/*    SETTERS    */
    virtual int set_promiscuous(bool enabled);
    virtual int add_mac(char * mac);
    virtual int set_link_up(bool up);
    virtual int set_flow_ctrl(int mode);
    virtual int set_led(bool on);


/*    DUMPS    */
    virtual void dump_link(FILE *fd);

private:
    rte_eth_fc_conf fc_conf_tmp;
    std::vector <struct rte_eth_xstat> xstats_values_tmp;
    std::vector <struct rte_eth_xstat_name> xstats_names_tmp;

};


class SimTRexPortAttr : public TRexPortAttr {
public:
    SimTRexPortAttr() {
        m_link.link_speed   = 10000;
        m_link.link_duplex  = 1;
        m_link.link_autoneg = 0;
        m_link.link_status  = 1;
        flag_is_virtual = true;
        flag_is_fc_change_supported = false;
        flag_is_led_change_supported = false;
        flag_is_link_change_supported = false;
    }

    /* DUMMY */
    void update_link_status() {}
    bool update_link_status_nowait() { return false; }
    void update_device_info() {}
    void reset_xstats() {}
    void update_description() {}
    bool get_promiscuous() { return false; }
    void macaddr_get(struct ether_addr *mac_addr) {}
    int get_xstats_values(xstats_values_t &xstats_values) { return -ENOTSUP; }
    int get_xstats_names(xstats_names_t &xstats_names) { return -ENOTSUP; }
    int get_flow_ctrl(int &mode) { return -ENOTSUP; }
    void get_description(std::string &description) {}
    void get_supported_speeds(supp_speeds_t &supp_speeds) {}
    int set_promiscuous(bool enabled) { return -ENOTSUP; }
    int add_mac(char * mac) { return -ENOTSUP; }
    int set_link_up(bool up) { return -ENOTSUP; }
    int set_flow_ctrl(int mode) { return -ENOTSUP; }
    int set_led(bool on) { return -ENOTSUP; }
    void dump_link(FILE *fd) {}
};


#endif /* __TREX_PORT_ATTR_H__ */