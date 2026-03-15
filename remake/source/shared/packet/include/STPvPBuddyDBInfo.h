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
    char buddy_n_user_id_what[0x1e];  // offset 0x09
    char variable_what1;              // offset 0x27
    char buddy_n_user_what2;          // offset 0x28
    char variable_what2;              // offset 0x29
    char buddy_n_user_what3;          // offset 0x2a
    STPvPBuddyDBInfo();
    ~STPvPBuddyDBInfo();
} __attribute__((packed));

TEST_CLASS_SIZE(STPvPBuddyDBInfo, 0x2b);