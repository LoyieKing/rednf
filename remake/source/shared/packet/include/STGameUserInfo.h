//
// Created by loyieking on 24-3-24.
//
#pragma once

// TODO: some field names are not correct, please fix them.

class STGameUserInfo {
public:
    char server_id;                   // offset 0x00
    int user_m_id;                    // offset 0x01
    int what_0x5;                     // offset 0x05
    char channel_no;                  // offset 0x09
    int charac_no;                    // offset 0x0a
    char buddy_n_user_id_what[0x1e];  // offset 0x0e
    char variable_what1;              // offset 0x2c
    char buddy_n_user_what2;          // offset 0x2d
    char variable_what2;              // offset 0x2e
    char buddy_n_user_what3;          // offset 0x2f
    STGameUserInfo();

} __attribute__((packed));
