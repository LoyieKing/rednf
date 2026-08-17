//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_REMOVE_PVP_BUDDY_H
#define PACKET_REQUEST_REMOVE_PVP_BUDDY_H

#include "PacketHeader.h"
#include <string.h>

#pragma pack(push, 1)
class Packet_Request_Remove_PvP_Buddy : public PacketHeader {
public:
    int m_id;                         // offset 10（Community OnReqRemoveBuddy：find_user(m_id) 用户查找键
                                      //          = acc_id；df_game_r SendRemovePvPBuddy param_1 =
                                      //          CUser::get_acc_id；原始日志串 "M_ID(%d)"）
    int charac_no;                    // offset 14（OnReqRemoveBuddy：user->charac_no == packet->charac_no；
                                      //          df_game_r param_2 = CUserCharacInfo::getCurCharacNo）
    char server_id;                   // offset 0x12（OnReqRemoveBuddy：req_remove_buddy(server_id, name)；
                                      //          df_game_r param_3 服务区字节）
    char m_name[0x1e];                // offset 0x13（OnReqRemoveBuddy：被删好友角色名，CFindBuddy 按
                                      //          STPvPBuddyDBInfo.m_name strcmp；df_game_r param_4 字符串，
                                      //          原占位 buddy_n_user_id_what）
    Packet_Request_Remove_PvP_Buddy() : PacketHeader(0x1b60, 0x31),
        m_id(0), charac_no(0), server_id(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Request_Remove_PvP_Buddy, 0x31);

#endif  // PACKET_REQUEST_REMOVE_PVP_BUDDY_H
