//
// Created by loyieking on 24-3-24.
//

#pragma once
#include "ReverseEngineerLib.h"
// TODO: some field names are not correct, please fix them.

class STPvPBuddyDBInfo {
public:
    char server_id;                   // offset 0x00
    int user_m_id;                    // offset 0x01
    int charac_no;                    // offset 0x05
    char m_name[0x1e];                // offset 0x09（好友名：Community get_user_info_for_buddy memcpy
                                      //          stGameUserInfo.m_name 0x1d；DB charac_name）
    char m_level;                     // offset 0x27（get_user_info_for_buddy = stGameUserInfo.m_level；SQL lev）
    char m_job;                       // offset 0x28（get_user_info_for_buddy = stGameUserInfo.m_job；SQL job）
    char m_growType;                  // offset 0x29（get_user_info_for_buddy = stGameUserInfo.m_growType；SQL grow_type）
    char m_sex;                       // offset 0x2a（get_user_info_for_buddy = stGameUserInfo.m_sex；SQL sex）
    STPvPBuddyDBInfo();
} __attribute__((packed));

TEST_CLASS_SIZE(STPvPBuddyDBInfo, 0x2b);
