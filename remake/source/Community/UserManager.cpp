//
// Created by loyieking on 24-3-24.
//

#include "UserManager.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"
#include "GuildConstants.h"

#include <algorithm>
#include <cstring>

bool CUserManager::enter_user(unsigned int m_id, CNetworkSession *networkSession, STGameUserInfo &gameUserInfo, int buddyCount, STPvPBuddyDBInfo *buddyList) {
    std::pair<std::map<unsigned int, CUser>::iterator, bool> insert_result =
        users.insert(std::make_pair(m_id, CUser(networkSession, gameUserInfo, buddyCount, buddyList)));
    // 原始：second == false 提前返回（xor eax,1; test/je 形态）
    if (insert_result.second == false) {
        return 0;
    }
    return 1;
}

CUser *CUserManager::find_user(unsigned int m_id) {
    // 原始：const_iterator（反编译 _Rb_tree_const_iterator 构造 + operator!= + operator->）
    std::map<unsigned int, CUser>::const_iterator user = users.find(m_id);
    if (user != users.end()) {
        CUser* pUser = (CUser *)&user->second;  // 原始：先存局部变量再返回（mov [ebp-X],eax; mov eax,[ebp-X]）
        return pUser;
    }
    return NULL;
}

CUser *CUserManager::find_user(char service_id, char const *user_id_what) {
    // 原始：CFindUser 局部对象 + end/begin 先求值（值传递副本 [ebp-0x38]）
    CFindUser finder(service_id, user_id_what);
    std::map<unsigned int, CUser>::iterator end = users.end();
    std::map<unsigned int, CUser>::iterator begin = users.begin();
    std::map<unsigned int, CUser>::iterator user = std::find_if(begin, end, finder);
    // 原始：!= end 分支在前（调用 ne + test al,al; je）
    if (user != users.end()) {
        return &user->second;
    }
    return NULL;
}
bool CUserManager::leave_user(unsigned int m_id) {
    std::map<unsigned int, CUser>::iterator user = users.find(m_id);
    if (user != users.end()) {
        users.erase(user);
        return true;
    }
    return false;
}
bool CUserManager::leave_user(CNetworkSession *networkSession) {
    std::map<unsigned int, CUser>::iterator p = users.begin();
    std::map<unsigned int, CUser>::iterator end = users.end();
    while (p != end) {
        // 原始：先取 CUser* 临时指针（mov [ebp-X],eax; mov eax,[eax]; cmp; jne）
        CUser* user = &p->second;
        if (user->networkSession == networkSession) {
            users.erase(p++);
        } else {
            ++p;
        }
    }
    return true;
}

// ===== CFindUser（原始位于 UserManager.cpp TU）=====

CFindUser::CFindUser(char server_id, char const *user_id_what) {
    this->server_id = server_id;
    memset(this->user_id_what, 0, sizeof(this->user_id_what));
    memcpy(this->user_id_what, user_id_what, sizeof(this->user_id_what) - 1);
}

bool CFindUser::operator()(std::pair<unsigned int, CUser> user) const {
    if (this->server_id == user.second.stGameUserInfo.server_id && strcmp(this->user_id_what, user.second.stGameUserInfo.buddy_n_user_id_what) == 0) {
        return true;
    }
    return false;
}
