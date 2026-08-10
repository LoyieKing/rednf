//
// Created by loyieking on 24-3-24.
//

#ifndef DF_PLAYGROUND_BUDDYMANAGER_H
#define DF_PLAYGROUND_BUDDYMANAGER_H

#include <vector>

#include "STPvPBuddyDBInfo.h"

// 原始：CFindBuddy 位于 BuddyManager.cpp TU（ORIG 地址紧邻 CBuddyManager 方法之后）。
class CFindBuddy {
    char server_id;
    char buddy_n_user_id_what[0x1e];

public:
    CFindBuddy(char server_id, char const* buddy_n_user_id_what);
    bool operator()(STPvPBuddyDBInfo & buddy) const;
};

class CBuddyManager {
public:
    std::vector<STPvPBuddyDBInfo> buddies;
    CBuddyManager();
    CBuddyManager(const CBuddyManager &buddyManager);
    ~CBuddyManager();  // 原始：显式析构，先 clear 再析构 vector
    void add_buddy(STPvPBuddyDBInfo &buddy);
    bool del_buddy(char server_id, char const* user_id_what);
    STPvPBuddyDBInfo* find_buddy(char server_id, char const* user_id_what);
    unsigned int get_size();  // 原始：直通 vector::size()（size_t），调用处 seta 无符号比较


};

#endif  // DF_PLAYGROUND_BUDDYMANAGER_H
