//
// Created by loyieking on 24-3-24.
//
#pragma once

// TODO: some field names are not correct, please fix them.

class STGameUserInfo {
public:
    char server_id;                   // offset 0x00
    int user_m_id;                    // offset 0x01
    int m_uid;                        // offset 0x05  SendLogin: CUser::GetUID
    unsigned char channel_no;         // offset 0x09（原始：movzx 零扩展，unsigned char）
    int charac_no;                    // offset 0x0a
    char m_name[0x1e];                // offset 0x0e  getCurCharacName（memcpy 0x1d）
    char m_level;                     // offset 0x2c  get_charac_level
    char m_job;                       // offset 0x2d  get_charac_job
    char m_growType;                  // offset 0x2e  getCurCharacGrowType
    char m_sex;                       // offset 0x2f  CUser::getSex
    STGameUserInfo();

} __attribute__((packed));
