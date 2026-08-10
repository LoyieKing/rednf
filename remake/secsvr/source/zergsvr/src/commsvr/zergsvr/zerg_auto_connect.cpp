// Reconstructed from zergsvr binary (zerg_auto_connect.cpp DWARF + disassembly).

#include "src/commsvr/zergsvr/zerg_auto_connect.h"
#include "src/commsvr/zergsvr/zerg_tcp_ctrl_handler.h"
#include "output/protocol/common/release/include/comm_conf_zerg.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include <errno.h>
#include <map>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utility>
#include <vector>

// Zerg_Server_Config 由 zerg_configure.cpp 定义；此处按二进制只引用其静态成员与方法。
struct ZERG_SERVICES_INFO {
    SERVICES_ID zerg_svc_info_;
    bool zerg_sessionkey_;
    ZEN_Sockaddr_In zerg_ip_addr_;
};
struct Zerg_Server_Config {
    static ZERG_SERVICES_INFO self_svc_info_;
    static Zerg_Server_Config * instance();
    int GetServicesIPInfo(const SERVICES_ID &svc_info, ZEN_Sockaddr_In &ipaddr);
};

// line 26
SERVICES_ROUTE_INFO::SERVICES_ROUTE_INFO()
    : main_route_info_(0, 0), main_route_ip_(), back_route_valid_(false),
      back_route_info_(0, 0), back_route_ip_() {
}

// line 31
SERVICES_ROUTE_INFO::~SERVICES_ROUTE_INFO() {
}

// line 39
Zerg_Auto_Connector::Zerg_Auto_Connector() : size_of_wantconnect_(0) {
    srand(time(NULL));
}

// line 45
Zerg_Auto_Connector::~Zerg_Auto_Connector() {
}

// line 62
int Zerg_Auto_Connector::get_autoconnect_cfg(const conf_zerg::zerg_config *config) {
    SERVICES_ROUTE_INFO svc_route;
    int ret = 0;
    int numsvr = config->auto_connect_cfg.auto_connect_num;
    if (numsvr < 0) {
        ZEN_Trace_LogMsg::debug_errorex("[framework] Get configure file error. %s.",
                                        "AUTOCONNECT|NUMSVRINFO key error.");
        return 0x1398;
    }
    char tmp[0x40];
    memset(tmp, 0, sizeof(tmp));
    for (int i = 0; i < numsvr; ++i) {
        unsigned short svrtype =
            config->auto_connect_cfg.auto_connect_svrs[i].main_svr.svr_type;
        unsigned int svrid =
            config->auto_connect_cfg.auto_connect_svrs[i].main_svr.svr_id;
        svc_route.main_route_info_.set_serviceid(svrtype, svrid);

        ZEN_Sockaddr_In svr_ip_addr;
        ret = Zerg_Server_Config::instance()->GetServicesIPInfo(
            svc_route.main_route_info_, svr_ip_addr);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[zergsvr] Count find Auto Connect Services Info SvrType=%u,SvrID=%u .Please Check Config file. ",
                svrtype, svrid);
            return 0x11493397;
        }
        svc_route.main_route_ip_ = svr_ip_addr;

        bool back_route_valid = false;
        SERVICES_ID back_svrinfo;
        back_svrinfo.set_serviceid(0, 0);
        ZEN_Sockaddr_In back_addr;
        back_addr.set((unsigned int)0, (unsigned short)0);

        snprintf(tmp, sizeof(tmp) - 1, "BACKSVCTYPE%i", i);
        unsigned short backtype =
            config->auto_connect_cfg.auto_connect_svrs[i].backup_svr.svr_type;
        if (backtype != 0) {
            if (backtype != svc_route.main_route_info_.services_type_) {
                ZEN_Trace_LogMsg::debug_errorex(
                    "[zergsvr] Backup route services type [%u] not equal to main route services id[%u].",
                    backtype, svrtype);
                return 0x11493397;
            }
            snprintf(tmp, sizeof(tmp) - 1, "BACKSVCID%i", i);
            unsigned int backid =
                config->auto_connect_cfg.auto_connect_svrs[i].backup_svr.svr_id;
            if (backid != 0) {
                back_svrinfo.set_serviceid(backtype, backid);
                ret = Zerg_Server_Config::instance()->GetServicesIPInfo(
                    back_svrinfo, svr_ip_addr);
                if (ret != 0) {
                    ZEN_Trace_LogMsg::debug_errorex(
                        "[zergsvr] Count find Backup services Connect Services Info SvrType=%u,SvrID=%u .Please Check Config file. ",
                        backtype, backid);
                    return 0x11493397;
                }
                back_route_valid = true;
                back_addr = svr_ip_addr;
            }
        }

        svc_route.back_route_valid_ = back_route_valid;
        svc_route.back_route_info_ = back_svrinfo;
        svc_route.back_route_ip_ = back_addr;
        ary_want_connect_.push_back(svc_route);
        add_auto_connect_info(svc_route.main_route_info_, back_svrinfo);

        char main_ip[0x20];
        char back_ip[0x20];
        ZEN_Trace_LogMsg::debug_infoex(
            "[zergsvr] Add one auto connect data, main route services id[%u|%u] ip[%s|%u],it have %s backup route services id[%u|%u] ip[%s|%u]",
            svc_route.main_route_info_.services_type_,
            svc_route.main_route_info_.services_id_,
            svc_route.main_route_ip_.get_host_addr(main_ip, 0x20),
            svc_route.main_route_ip_.get_port_number(),
            back_route_valid ? "" : "not",
            back_svrinfo.services_type_, back_svrinfo.services_id_,
            back_addr.get_host_addr(back_ip, 0x20), back_addr.get_port_number());
    }
    size_of_wantconnect_ = ary_want_connect_.size();
    ZEN_Trace_LogMsg::debug_infoex("[zergsvr] Get AutoConnect Config Success.");
    return 0;
}

// line 189
void Zerg_Auto_Connector::reconnect_allserver(size_t &szvalid, size_t &szsucc,
                                              size_t &szfail) {
    szvalid = 0;
    szsucc = 0;
    szfail = 0;
    for (size_t i = 0; i < size_of_wantconnect_; ++i) {
        TCP_Svc_Handler *svchandle = NULL;
        int ret = TCP_Svc_Handler::find_services_peer(
            ary_want_connect_[i].main_route_info_, svchandle);
        if (ret != 0) {
            ret = connect_server_bysvcid(ary_want_connect_[i].main_route_info_,
                                         ary_want_connect_[i].main_route_ip_);
            if (ret == 0) {
                ++szsucc;
            } else {
                ++szfail;
            }
        } else {
            ++szvalid;
            if (svchandle->get_peer_status() == TCP_Svc_Handler::PEER_STATUS_ACTIVE) {
                svchandle->send_zergheatbeat_reg();
            }
        }

        if (ary_want_connect_[i].back_route_valid_) {
            svchandle = NULL;
            ret = TCP_Svc_Handler::find_services_peer(
                ary_want_connect_[i].back_route_info_, svchandle);
            if (ret != 0) {
                ret = connect_server_bysvcid(ary_want_connect_[i].back_route_info_,
                                             ary_want_connect_[i].back_route_ip_);
                if (ret == 0) {
                    ++szsucc;
                } else {
                    ++szfail;
                }
            } else {
                ++szvalid;
                if (svchandle->get_peer_status() == TCP_Svc_Handler::PEER_STATUS_ACTIVE) {
                    svchandle->send_zergheatbeat_reg();
                }
            }
        }
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] Auto NONBLOCK connect server,vaild number:%d ,success Number :%d,fail number:%d .",
        szvalid, szsucc, szfail);
}

// line 280
int Zerg_Auto_Connector::reconnect_server(const SERVICES_ID &reconnect_svrinfo) {
    for (size_t i = 0; i < size_of_wantconnect_; ++i) {
        if (ary_want_connect_[i].main_route_info_ == reconnect_svrinfo ||
            (ary_want_connect_[i].back_route_valid_ &&
             ary_want_connect_[i].back_route_info_ == reconnect_svrinfo)) {
            ZEN_Sockaddr_In inetaddr(ary_want_connect_[i].main_route_ip_);
            return connect_server_bysvcid(reconnect_svrinfo, inetaddr);
        }
    }
    return 0x114933b5;
}

// line 301
int Zerg_Auto_Connector::get_backupsvcinfo(const SERVICES_ID &main_svrinfo,
                                           bool &backroute_valid,
                                           SERVICES_ID &backroute_svrinfo) {
    backroute_valid = false;
    for (size_t i = 0; i < size_of_wantconnect_; ++i) {
        if (ary_want_connect_[i].main_route_info_ == main_svrinfo) {
            ZEN_Sockaddr_In inetaddr(ary_want_connect_[i].main_route_ip_);
            backroute_valid = ary_want_connect_[i].back_route_valid_;
            backroute_svrinfo = ary_want_connect_[i].back_route_info_;
            return 0;
        }
    }
    return 0x11493397;
}

// line 322
int Zerg_Auto_Connector::connect_server_bysvcid(const SERVICES_ID &svrinfo,
                                                const ZEN_Sockaddr_In &inetaddr) {
    ZEN_Trace_LogMsg::debug_debugEx(
        "[zergsvr] Try NONBLOCK connect services[%u|%u] IP|Port :[%s|%u] .",
        svrinfo.services_type_, svrinfo.services_id_, inetaddr.get_host_addr(),
        inetaddr.get_port_number());

    ZEN_Socket_Stream tcpscoket;
    const socklen_t opval = 0x50000;
    socklen_t opvallen = sizeof(opval);
    tcpscoket.open(2, 0, true);
    tcpscoket.setsockopt(SOL_SOCKET, SO_RCVBUF, &opval, opvallen);
    tcpscoket.setsockopt(SOL_SOCKET, SO_SNDBUF, &opval, opvallen);

    int ret = zerg_connector_.connect(tcpscoket, &inetaddr, true, false, 0, NULL);
    if (ret >= 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zergsvr] My God! NonBlock Socket Connect Success , ACE is a cheat.");
        return 0;
    }
    if (errno != EAGAIN && errno != EINPROGRESS) {
        tcpscoket.close();
        return 0x11493391;
    }

    TCP_Svc_Handler *p_handler =
        TCP_Svc_Handler::AllocSvcHandlerFromPool(TCP_Svc_Handler::HANDLER_MODE_CONNECT);
    Zerg_Server_Config::instance();
    p_handler->init_tcpsvr_handler(Zerg_Server_Config::self_svc_info_.zerg_svc_info_,
                                   svrinfo, tcpscoket, inetaddr);
    return 0;
}

// line 374
size_t Zerg_Auto_Connector::numsvr_connect() {
    return size_of_wantconnect_;
}

// line 380
int Zerg_Auto_Connector::get_server(unsigned short svr_type, SERVICES_ID *svrinfo) {
    RouteType2Index::iterator iter = want_connect_type_2_index_.find(svr_type);
    if (iter == want_connect_type_2_index_.end()) {
        return 0x114933b6;
    }
    unsigned int index = iter->second;
    if (index >= list_of_want_connect_main_id_.size()) {
        return 0x114933b6;
    }
    std::vector<unsigned int> &list_id = list_of_want_connect_main_id_[index];
    unsigned int id_num = list_id.size();
    if (id_num == 0) {
        return 0x114933b6;
    }
    for (unsigned int i = 0; i < id_num; ++i) {
        unsigned int rand_num = rand() % id_num;
        svrinfo->set_serviceid(svr_type, list_id[rand_num]);
        if (is_connected(*svrinfo)) {
            return 0;
        }
    }
    return 0x114933b6;
}

// line 423
bool Zerg_Auto_Connector::is_connected(const SERVICES_ID &svrinfo) {
    TCP_Svc_Handler *svchandle = NULL;
    int ret = TCP_Svc_Handler::find_services_peer(svrinfo, svchandle);
    return ret == 0;
}

// line 452
void Zerg_Auto_Connector::add_auto_connect_info(const SERVICES_ID &main_svrinfo,
                                                const SERVICES_ID &back_svrinfo) {
    RouteType2Index::iterator iter =
        want_connect_type_2_index_.find(main_svrinfo.services_type_);
    if (iter == want_connect_type_2_index_.end()) {
        std::vector<unsigned int> main_id_list;
        main_id_list.push_back(main_svrinfo.services_id_);
        std::vector<unsigned int> back_id_list;
        back_id_list.push_back(back_svrinfo.services_id_);
        list_of_want_connect_main_id_.push_back(main_id_list);
        list_of_want_connect_back_id_.push_back(back_id_list);
        want_connect_type_2_index_.insert(std::make_pair(
            main_svrinfo.services_type_, list_of_want_connect_main_id_.size() - 1));
    } else {
        unsigned int index = iter->second;
        std::vector<unsigned int> main_id_list = list_of_want_connect_main_id_[index];
        main_id_list.push_back(main_svrinfo.services_id_);
        std::vector<unsigned int> back_id_list = list_of_want_connect_back_id_[index];
        back_id_list.push_back(back_svrinfo.services_id_);
        list_of_want_connect_main_id_[index] = main_id_list;
        list_of_want_connect_back_id_[index] = back_id_list;
    }
}

// line 488
int Zerg_Auto_Connector::reload_cfg(const conf_zerg::zerg_config *config) {
    RouteType2Index old_want_connect_type_2_index = want_connect_type_2_index_;
    ListOfMainRouteId old_list_of_want_connect_main_id =
        list_of_want_connect_main_id_;
    ListOfBackRouteId old_list_of_want_connect_back_id =
        list_of_want_connect_back_id_;

    want_connect_type_2_index_.clear();
    list_of_want_connect_main_id_.clear();
    list_of_want_connect_back_id_.clear();
    ary_want_connect_.clear();

    int ret = get_autoconnect_cfg(config);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("zergsvr reload autoconnect fail. ret=%d",
                                        ret);
        want_connect_type_2_index_ = old_want_connect_type_2_index;
        list_of_want_connect_main_id_ = old_list_of_want_connect_main_id;
        list_of_want_connect_back_id_ = old_list_of_want_connect_back_id;
    } else {
        for (RouteType2Index::iterator iter = old_want_connect_type_2_index.begin();
             iter != old_want_connect_type_2_index.end(); ++iter) {
            const std::vector<unsigned int> &old_main_id_list =
                old_list_of_want_connect_main_id[iter->second];
            const std::vector<unsigned int> &old_back_id_list =
                old_list_of_want_connect_back_id[iter->second];
            for (size_t i = 0; i < old_main_id_list.size(); ++i) {
                SERVICES_ID service(iter->first, old_main_id_list[i]);
                if (!is_current_auto_connect(service, true)) {
                    ZEN_Trace_LogMsg::debug_infoex(
                        "zergsvr close old auto connect. svr_type=%u svr_id=%u",
                        service.services_type_, service.services_id_);
                    TCP_Svc_Handler::close_services_peer(service);
                }
            }
            for (size_t i = 0; i < old_back_id_list.size(); ++i) {
                if (old_back_id_list[i] != 0) {
                    SERVICES_ID service(iter->first, old_back_id_list[i]);
                    if (!is_current_auto_connect(service, false)) {
                        TCP_Svc_Handler::close_services_peer(service);
                    }
                }
            }
        }
    }
    return ret;
}

// line 551
bool Zerg_Auto_Connector::is_current_auto_connect(const SERVICES_ID &service,
                                                  bool is_main_service) {
    RouteType2Index::iterator iter =
        want_connect_type_2_index_.find(service.services_type_);
    if (iter == want_connect_type_2_index_.end()) {
        return false;
    }
    std::vector<unsigned int> *list_id = is_main_service
                                        ? &list_of_want_connect_main_id_[iter->second]
                                        : &list_of_want_connect_back_id_[iter->second];
    for (size_t i = 0; i < list_id->size(); ++i) {
        if ((*list_id)[i] == service.services_id_) {
            TCP_Svc_Handler *svchandle = NULL;
            int ret = TCP_Svc_Handler::find_services_peer(service, svchandle);
            if (ret != 0) {
                return true;
            }
            ZEN_Sockaddr_In svr_ip_addr;
            ret = Zerg_Server_Config::instance()->GetServicesIPInfo(service,
                                                                    svr_ip_addr);
            if (ret != 0) {
                return false;
            }
            if (svr_ip_addr != svchandle->get_peer_sockaddr()) {
                return false;
            }
            return true;
        }
    }
    return false;
}
