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
        m_level = 0;
        m_job = 0;
        m_growType = 0;
        m_sex = 0;
        // 原始：memset 在字段清零之后
        memset(m_name, 0, sizeof(m_name));
    }
    char server_id;        // offset 0x00
    unsigned char channel_no;       // offset 0x01（原始：ORIG movzbl 直接装入存储寄存器 %cl，
                                    // 无 char 强转的 eax 往返；unsigned char 目标在 4.4.6 -O0
                                    // 下折叠为直接装载，2026-08-11 二进制实测）
    bool isOnline;         // offset 0x02
    int charac_no;         // offset 0x03
    char m_name[0x1e];      // offset 0x07（角色名；CUser::get_user_info_for_buddy:
                            //          memcpy(..., stGameUserInfo.m_name, 0x1d)；buddy 列表按
                            //          server_id+m_name 匹配（CFindBuddy），DB 列 buddy_charac_name）
    char m_level;           // offset 0x25（get_user_info_for_buddy: = stGameUserInfo.m_level；
                            //          check_variable_charac_info 用其与 m_level 比对）
    char m_job;             // offset 0x26（get_user_info_for_buddy: = stGameUserInfo.m_job）
    char m_growType;        // offset 0x27（get_user_info_for_buddy: = stGameUserInfo.m_growType；
                            //          update_variable_charac_info 只更新 level/growType）
    char m_sex;             // offset 0x28（get_user_info_for_buddy: = stGameUserInfo.m_sex）
};
#pragma pack(pop)

TEST_CLASS_SIZE(PvPBuddyInfo, 0x29);

#endif  // REDNF_PVPBUDDYINFO_H
