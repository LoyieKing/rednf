//
// Created by loyieking on 24-3-30.
//

#include "PacketProc.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"
#include "GuildConstants.h"

#include <csignal>
#include <unistd.h>

#include "ArchiveLog.h"
#include "NetworkSession.h"
#include "Packet_Community_Login.h"
#include "Packet_Community_Logout.h"
#include "Packet_Request_Add_PvP_Buddy.h"
#include "Packet_Request_PvP_Buddy_Conn_List.h"
#include "Packet_Request_Remove_PvP_Buddy.h"
#include "Packet_Response_Add_PvP_Buddy.h"
#include "PacketDispatcher.h"
#include "User.h"
#include "UserManager.h"

// 原始二进制中 g_user_manager 单独位于 PacketProc.cpp TU
//（_GLOBAL__I_g_user_manager 桩，构造 ios_base::Init + CUserManager）。
CUserManager g_user_manager = CUserManager();

namespace packet_proc {

int OnLogin(ISessionManager* sessionManager, CNetworkSession* networkSession, PacketHeader* _packet) {
    Packet_Community_Login* packet = (Packet_Community_Login*)_packet;
    ArchiveLog("packet_proc::OnLogin M_ID(%d)", packet->m_id);
    // 原始：enter_user == false 提前返回（生成 xor eax,1; test al,al; je 主逻辑）
    if (g_user_manager.enter_user(packet->m_id, networkSession, packet->gameUserInfo, packet->buddyCount, packet->buddies) == false) {
        return 0x2a;
    }
    CUser* user = g_user_manager.find_user(packet->m_id);
    if (user != NULL) {
        user->check_myself();
        user->notice_login_logout(CUser::eLoginout_Login);
        user->send_buddy_list();
        ArchiveLog("LOGIN M_ID(%d), CHARAC_INFO(%d, %d, %d, %s), BUDDY_CNT(%d)",
                   packet->m_id,
                   packet->gameUserInfo.server_id,
                   packet->gameUserInfo.channel_no,
                   packet->gameUserInfo.charac_no,
                   packet->gameUserInfo.buddy_n_user_id_what,
                   packet->buddyCount);
    }
    return 0;
}

int OnLogout(ISessionManager* sessionManager, CNetworkSession* networkSession, PacketHeader* packetHeader) {
    Packet_Community_Logout* packet = (Packet_Community_Logout*)packetHeader;
    CUser* user = g_user_manager.find_user(packet->m_id);
    // 原始：条件直接物化（mov eax,1; jmp; mov eax,0; test al,al; je）
    if (user != NULL && user->get_user_info()->charac_no == packet->what_0xe) {
        user->notice_login_logout(CUser::eLoginout_Logout);
    }
    // 原始：leave_user == false 提前返回 0x46（xor eax,1; test/je 形态）
    if (g_user_manager.leave_user(packet->m_id) == false) {
        return 0x46;
    }
    ArchiveLog("packet_proc::OnLogout M_ID(%d)", packet->m_id);
    return 0;
}

int OnReqBuddyList(ISessionManager* sessionManager, CNetworkSession* networkSession, PacketHeader* packetHeader) {
    Packet_Request_PvP_Buddy_Conn_List* packet = (Packet_Request_PvP_Buddy_Conn_List*)packetHeader;
    CUser* user = g_user_manager.find_user(packet->m_id);
    // 原始：条件直接物化（mov eax,1; jmp; mov eax,0; test al,al; je）
    if (user != NULL && user->get_user_info()->charac_no == packet->charac_no) {
        user->send_buddy_list();
    }
    return 0;
}

int OnReqAddBuddy(ISessionManager* sessionManager, CNetworkSession* networkSession, PacketHeader* packetHeader) {
    Packet_Request_Add_PvP_Buddy* packet = (Packet_Request_Add_PvP_Buddy*)packetHeader;
    ArchiveLog("packet_proc::OnReqAddBuddy M_ID(%d)", packet->m_id);
    CUser* user = g_user_manager.find_user(packet->m_id);
    // 原始：user != NULL && charac_no 匹配 复合条件物化一次（mov eax,1/0 + test al,al + je 到 return 0）
    if (user != NULL && user->get_user_info()->charac_no == packet->charac_no) {
        // 原始：find_buddy 调用结果直接入条件（test eax,eax; setne al; test al,al）
        if (user->get_buddy_manager()->find_buddy(packet->server_id, packet->buddy_n_user_id_what) != NULL) {
            // 0x12 means buddy already exists?
            user->notice_add_buddy_fail(0, 0x12);
        } else {
            CUser* buddyUser = g_user_manager.find_user(packet->server_id, packet->buddy_n_user_id_what);
            if (buddyUser == NULL) {
                // 3 means user not found (not online)?
                user->notice_add_buddy_fail(0, 3);
            } else {
                // 原始：self-check 为 && 复合条件（mov eax,1/0 + test al,al 物化）
                if (user->get_user_info()->user_m_id == buddyUser->get_user_info()->user_m_id &&
                    user->get_user_info()->charac_no == buddyUser->get_user_info()->charac_no) {
                    // nope, 3 may mean unreachable error.(user can not add himself as buddy/can not add buddy offline)
                    user->notice_add_buddy_fail(0, 3);
                } else if (user->get_buddy_manager()->get_size() > 0x1f) {
                    // 4 means our buddy list full
                    user->notice_add_buddy_fail(0, 4);
                } else if (buddyUser->get_buddy_manager()->get_size() > 0x1f) {
                    // 0x15 means his buddy list full
                    user->notice_add_buddy_fail(0, 0x15);
                } else {
                    user->req_add_buddy(buddyUser);
                }
            }
        }
    }
    return 0;
}

int OnResAddBuddy(ISessionManager* sessionManager, CNetworkSession* networkSession, PacketHeader* packetHeader) {
    Packet_Response_Add_PvP_Buddy* packet = (Packet_Response_Add_PvP_Buddy*)packetHeader;
    ArchiveLog("packet_proc::OnResAddBuddy M_ID(%d)", packet->m_id);
    CUser* user = g_user_manager.find_user(packet->m_id);
    // 原始：user != NULL && charac_no 匹配 复合条件直接物化（mov eax,1/0 + test al,al）
    if (user != NULL && user->get_user_info()->charac_no == packet->charac_no) {
        if (user->get_buddy_manager()->find_buddy(packet->server_id, packet->buddy_n_user_id_what) != NULL) {
            // 0x12 means buddy already exists?
            user->notice_add_buddy_fail(1, 0x12);
        } else {
            CUser* buddyUser = g_user_manager.find_user(packet->server_id, packet->buddy_n_user_id_what);
            if (buddyUser == NULL) {
                user->notice_add_buddy_fail(1, 3);
            } else {
                if (user->get_buddy_manager()->get_size() > 0x1f) {
                    // my full
                    user->notice_add_buddy_fail(1, 4);
                    buddyUser->notice_add_buddy_fail(0, 0x15);
                } else if (buddyUser->get_buddy_manager()->get_size() > 0x1f) {
                    // my not full, his full.
                    user->notice_add_buddy_fail(1, 0x15);
                    buddyUser->notice_add_buddy_fail(0, 4);
                } else {
                    user->res_add_buddy(buddyUser);
                }
            }
        }
    }
    return 0;
}

int OnReqRemoveBuddy(ISessionManager* sessionManager, CNetworkSession* networkSession, PacketHeader* packetHeader) {
    Packet_Request_Remove_PvP_Buddy* packet = (Packet_Request_Remove_PvP_Buddy*)packetHeader;
    ArchiveLog("packet_proc::OnReqRemoveBuddy M_ID(%d)", packet->m_id);
    CUser* user = g_user_manager.find_user(packet->m_id);
    if (user != NULL && user->get_user_info()->charac_no == packet->charac_no) {
        user->req_remove_buddy(packet->server_id, packet->buddy_n_user_id_what);
    }
    return 0;
}

int OnBuddyChatMsg(ISessionManager* sessionManager, CNetworkSession* networkSession, PacketHeader* packetHeader) {
    Packet_Monitor_Other_Channel_Chat* packet = (Packet_Monitor_Other_Channel_Chat*)packetHeader;
    CUser* user = g_user_manager.find_user(packet->m_id);
    CUser* buddyUser = g_user_manager.find_user(packet->server_id, packet->buddy_n_user_id_what);
    if (buddyUser != NULL) {
        if (user != NULL) {
            buddyUser->send_other_channel_chat(packet, user);
        }
    } else if (user != NULL) {
        user->send_other_channel_chat_result(packet, ENUM_MONITOR_ERROR_ONE);
    }
    return 0;
}

int OnBuddyChatMsgHyperLink(ISessionManager* sessionManager, CNetworkSession* networkSession, PacketHeader* packetHeader) {
    Packet_Monitor_Other_Channel_Chat_Hyper_Link* packet = (Packet_Monitor_Other_Channel_Chat_Hyper_Link*)packetHeader;
    CUser* user = g_user_manager.find_user(packet->m_id);
    CUser* buddyUser = g_user_manager.find_user(packet->server_id, packet->buddy_n_user_id_what);
    if (buddyUser != NULL) {
        if (user != NULL) {
            buddyUser->send_other_channel_chat_hyper_link(packet, user);
        }
    } else if (user != NULL) {
        user->send_other_channel_chat_result_hyper_link(packet, ENUM_MONITOR_ERROR_ONE);
    }
    return 0;
}

}  // namespace packet_proc

packet_table_item g_packet_table[] = {
    {0x1b58, packet_proc::OnLogin},
    {0x1b59, packet_proc::OnLogout},
    {0x1b5a, packet_proc::OnReqBuddyList},
    {0x1b5c, packet_proc::OnReqAddBuddy},
    {0x1b5e, packet_proc::OnResAddBuddy},
    {0x1b60, packet_proc::OnReqRemoveBuddy},
    {0x03f3, packet_proc::OnBuddyChatMsg},
    {0x2719, packet_proc::OnBuddyChatMsgHyperLink},
};

packet_table_item* get_packet_table() {
    return g_packet_table;
}

int get_packet_table_size() {
    return sizeof(g_packet_table) / sizeof(packet_table_item);
}
