//
// Created by loyieking on 24-3-24.
//

#pragma once

#include "BuddyManager.h"
#include "NetworkSession.h"
#include "Packet_Monitor_Other_Channel_Chat.h"
#include "Packet_Monitor_Other_Channel_Chat_Hyper_Link.h"
#include "STGameUserInfo.h"
#include "STPvPBuddyDBInfo.h"

// 原始二进制中为命名枚举类型（mangled: 18ENUM_MONITOR_ERROR），
// 重建原为 typedef char，签名不匹配；枚举取值 0/1 为推断（global.cpp 以字面量调用）。
enum ENUM_MONITOR_ERROR {
    ENUM_MONITOR_ERROR_NONE = 0,
    ENUM_MONITOR_ERROR_ONE = 1,
};

class CUser {
    CNetworkSession* networkSession;  // offset 0x00
    STGameUserInfo stGameUserInfo;    // offset 0x04
    CBuddyManager buddyManager;       // offset 0x34

public:
    // 原始二进制中为嵌套枚举类型（mangled: NS_9eLoginoutE）；
    // 0=登录、1=登出（推断自 global.cpp 调用点）。
    enum eLoginout {
        eLoginout_Login = 0,
        eLoginout_Logout = 1,
    };

    CUser(CNetworkSession* networkSession, STGameUserInfo& stGameUserInfo, int buddyCount, STPvPBuddyDBInfo* buddyList);
    CUser(CUser const& other);

    void check_myself();
    bool check_variable_charac_info(char service_id, char const* user_id_what, char variable_what1, char variable_what2);
    void db_delete_buddy(char buddy_server_id, unsigned int buddy_charac_no);
    void db_insert_buddy(STPvPBuddyDBInfo* a, STPvPBuddyDBInfo* b);
    void db_update_buddy();
    CBuddyManager* get_buddy_manager();
    STGameUserInfo* get_user_info();
    void get_user_info_for_buddy(STPvPBuddyDBInfo& output);
    void notice_add_buddy_fail(char reason_what1, unsigned char error_code);
    void notice_add_buddy_success(char reason_what, CUser* user);
    void notice_login_logout(eLoginout loginout);
    void notice_remove_buddy_fail(unsigned char error_code);
    void notice_remove_buddy_success(char server_id, char const* user_id_what);
    void req_add_buddy(CUser* buddy);
    void req_remove_buddy(char param_server_id, char const* param_user_id_what);
    void res_add_buddy(CUser* buddy);
    void send_buddy_list();
    void send_other_channel_chat(Packet_Monitor_Other_Channel_Chat* chat, CUser* user);
    void send_other_channel_chat_hyper_link(Packet_Monitor_Other_Channel_Chat_Hyper_Link* chat, CUser* user);
    void send_other_channel_chat_result(Packet_Monitor_Other_Channel_Chat* chat, ENUM_MONITOR_ERROR error);
    void send_other_channel_chat_result_hyper_link(Packet_Monitor_Other_Channel_Chat_Hyper_Link* chat, ENUM_MONITOR_ERROR error);
    void update_variable_charac_info(char server_id, char const* user_id_what, char variable_what1, char variable_what2);

    friend class CUserManager;
    friend class CFindUser;
};
