//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_NOTICE_ADD_PVP_BUDDY_RESULT_H
#define REDNF_PACKET_NOTICE_ADD_PVP_BUDDY_RESULT_H

#include "PacketHeader.h"
#include <cstring>

#pragma pack(push, 1)
class Packet_Notice_Add_PvP_Buddy_Result : public PacketHeader {
public:
    int charac_no;                          // offset 10
    int m_uid;                              // offset 14（notice_add_buddy_fail/success：
                                            //          = stGameUserInfo.m_uid（STGameUserInfo +0x05））
    char m_resultType;                      // offset 0x12（notice_add_buddy_fail/success 首参：
                                            //          0/1 结果类别；ReqAdd 直接请求流程=0，
                                            //          ResAdd 同意流程请求方=1、被加方=0。
                                            //          client 展示分支用，原始符号名不可考）
    char server_id;                         // offset 0x13
    char m_name[0x1e];                      // offset 0x14（notice_add_buddy_success：
                                            //          = user->stGameUserInfo.m_name，memcpy 0x1d）
    unsigned char m_errorCode;              // offset 0x32（notice_add_buddy_fail 第二参 error_code；
                                            //          0=成功，0x12=已存在，3=不在线/不能加自己，
                                            //          4=自己列表满，0x15=对方列表满）

    Packet_Notice_Add_PvP_Buddy_Result() : PacketHeader(0x1b5f, 0x33) {
        // 初始化行为对齐原始二进制（2026-08-06 反汇编验证）
        charac_no = 0;
        m_uid = 0;
        m_resultType = 0;
        server_id = 0;
        m_errorCode = 0;
        memset(m_name, 0, sizeof(m_name));
    }
} __attribute__((packed));

#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Notice_Add_PvP_Buddy_Result, 0x33);

#endif  // REDNF_PACKET_NOTICE_ADD_PVP_BUDDY_RESULT_H
