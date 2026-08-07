//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PVPBUDDYINFO_H
#define REDNF_PVPBUDDYINFO_H

#include "ReverseEngineerLib.h"
#include <cstring>

#pragma pack(push, 1)
class PvPBuddyInfo {
public:
    // 原始二进制有构造函数（清零全部字段，mangled _ZN12PvPBuddyInfoC1Ev）
    PvPBuddyInfo() {
        server_id = 0;
        channel_no = 0;
        isOnline = false;
        charac_no = 0;
        variable_what1 = 0;
        buddy_n_user_what2 = 0;
        variable_what2 = 0;
        buddy_n_user_what3 = 0;
        // 原始：memset 在字段清零之后
        memset(buddy_n_user_id_what, 0, sizeof(buddy_n_user_id_what));
    }
    char server_id;        // offset 0x00
    char channel_no;       // offset 0x01
    bool isOnline;         // offset 0x02
    int charac_no;         // offset 0x03
    char buddy_n_user_id_what[0x1e];  // offset 0x07
    char variable_what1;        // offset 0x25
    char buddy_n_user_what2;        // offset 0x26
    char variable_what2;        // offset 0x27
    char buddy_n_user_what3;        // offset 0x28
};
#pragma pack(pop)

TEST_CLASS_SIZE(PvPBuddyInfo, 0x29);

#endif  // REDNF_PVPBUDDYINFO_H
