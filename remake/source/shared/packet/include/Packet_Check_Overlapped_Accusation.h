#ifndef PACKET_CHECK_OVERLAPPED_ACCUSATION_H
#define PACKET_CHECK_OVERLAPPED_ACCUSATION_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Check_Overlapped_Accusation : public PacketHeader {
public:
    // 与 Packet_Mannerless_User_Accusation 同包 0x1b66/0x15f，
    // 布局以 df_game_r Dispatcher_ReportMannerlessUser::dispatch_sig 为准。
    unsigned int m_uid;             // +0xa（CUser::GetUID；ORIG ctor = 0xffffffff；
                                    //        monitor OnCheckOverlappedAccusation 按连接号读取该值）
    int m_characNo;                 // +0xe（CUserCharacInfo::getCurCharacNo）
    int m_type;                     // +0x12（举报类型；isNeedTypeForChattingMsg；ORIG ctor = 4）
    unsigned int m_serverGroup;     // +0x16（CEnvironment::get_server_group / CUser::GetServerGroup）
    unsigned int m_reportedServerGroup; // +0x1a（client 字节 <0x65；0=视为同服）
    int m_accId;                    // +0x1e（CUser::get_acc_id）
    char m_name1[0x1e];             // +0x22（举报者角色名 getCurCharacName）
    char m_name2[0x1e];             // +0x40（被举报角色名）
    char m_reportText[0x100];       // +0x5e（isNeedTypeForChattingMsg 时读取的举报文本）
    char m_result;                  // +0x15e（game server 置 0x70='p'；monitor 回写结果码）
    Packet_Check_Overlapped_Accusation() : PacketHeader(0x1b66, 0x15f),
        m_uid(0xffffffff), m_characNo(0), m_type(4), m_accId(0)
    {
        memset(m_name1, 0, sizeof(m_name1));
        memset(m_name2, 0, sizeof(m_name2));
        memset(m_reportText, 0, sizeof(m_reportText));
        m_result = 0;
    }
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Check_Overlapped_Accusation, 0x15f);

#endif  // PACKET_CHECK_OVERLAPPED_ACCUSATION_H
