//
// Created by loyieking on 24-3-24.
//

#ifndef DF_PLAYGROUND_USERMANAGER_H
#define DF_PLAYGROUND_USERMANAGER_H

#include <map>
#include <utility>

#include "User.h"
#include "STGameUserInfo.h"
#include "STPvPBuddyDBInfo.h"

class CNetworkSession;

// 原始：CFindUser 位于 UserManager.cpp TU（ORIG 地址紧邻 CUserManager 方法）。
class CFindUser {
    char server_id;
    char user_id_what[0x1e];

public:
    CFindUser(char server_id, char const* user_id_what);
    bool operator()(std::pair<unsigned int, CUser> user) const;
};

class CUserManager {
    std::map<unsigned int, CUser> users;

public:
    bool enter_user(unsigned int m_id, CNetworkSession* networkSession, STGameUserInfo& gameUserInfo, int buddyCount, STPvPBuddyDBInfo* buddyList);
    CUser* find_user(unsigned int m_id);
    CUser* find_user(char server_id, char const* user_id_what);
    bool leave_user(unsigned int m_id);
    bool leave_user(CNetworkSession* networkSession);
};

// 原始：g_user_manager 定义位于 PacketProc.cpp TU（_GLOBAL__I_g_user_manager 桩）。
extern CUserManager g_user_manager;

#endif  // DF_PLAYGROUND_USERMANAGER_H
