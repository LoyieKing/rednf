//
// Created from df_game_r: Packet_Mannerless_User_Accusation::Packet_Mannerless_User_Accusation()
// @ 0x822c1e6（Ghidra 反编译）
//

#ifndef PACKET_MANNERLESS_USER_ACCUSATION_H
#define PACKET_MANNERLESS_USER_ACCUSATION_H

#include <string.h>

#include "PacketHeader.h"

class Packet_Mannerless_User_Accusation : public PacketHeader {
public:
    unsigned int m_uid;             // +0xa（dispatch_sig：CUser::GetUID；ctor = 0xffffffff）
    int m_characNo;                 // +0xe（CUserCharacInfo::getCurCharacNo）
    int m_type;                     // +0x12（dispatch_sig 读入的举报类型；
                                    //      isNeedTypeForChattingMsg(m_type) 决定是否读取聊天文本；
                                    //      同包 0x1b66 另一视图 Check_Overlapped 同名偏移 m_type）
    unsigned int m_serverGroup;     // +0x16（CEnvironment::get_server_group / CUser::GetServerGroup）
    unsigned int m_reportedServerGroup; // +0x1a（client 字节，<0x65；0=视为同服；
                                        //      自举报检查：m_serverGroup==本字段 且 名字相同 -> 拒绝）
    int m_accId;                    // +0x1e（CUser::get_acc_id）
    char m_name1[0x1e];             // +0x22（getCurCharacName 的举报者角色名；
                                    //      同包视图 Check_Overlapped 同名偏移 m_name1）
    char m_name2[0x1e];             // +0x40（client 传来的被举报角色名；
                                    //      同包视图 Check_Overlapped m_name2 起点）
    char m_reportText[0x100];       // +0x5e（isNeedTypeForChattingMsg 时 get_str(...,0x100,...) 的举报文本；
                                    //      MAX_REPORT_MANNERLESS_USER_TEXT_LENGTH=0xff）
    char m_result;                  // +0x15e（game server 置 0x70='p'（库存槽 0x677e 时）；
                                    //      同包视图 Check_Overlapped 同名偏移 m_result，
                                    //      作 AddAccusationCharac 的 type 参数/回写结果码）

    Packet_Mannerless_User_Accusation() : PacketHeader(0x1b66, 0x15f),
        m_uid(0xffffffff), m_characNo(0), m_type(4), m_accId(0)
    {
        memset(m_name1, 0, sizeof(m_name1));
        memset(m_name2, 0, sizeof(m_name2));
        memset(m_reportText, 0, sizeof(m_reportText));
        m_result = 0;
    }
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Mannerless_User_Accusation, 0x15f);

#endif  // PACKET_MANNERLESS_USER_ACCUSATION_H
