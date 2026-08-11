//
// Created by loyieking on 24-3-24.
//


#include "User.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"
#include "GuildConstants.h"

#include <cstring>

#include "ArchiveLog.h"
#include "BuddyManager.h"
#include "Packet_Delete_PvP_Buddy.h"
#include "Packet_Insert_PvP_Buddy.h"
#include "Packet_Monitor_Other_Channel_Chat_ToUser.h"
#include "Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link.h"
#include "Packet_Notice_Add_PvP_Buddy_Result.h"
#include "Packet_Notice_PvP_Buddy_In_Out.h"
#include "Packet_Notice_Request_Add_PvP_Buddy.h"
#include "Packet_Response_PvP_Buddy_Conn_List.h"
#include "Packet_Response_Remove_PvP_Buddy.h"
#include "Packet_Update_PvP_Buddy.h"
#include "UserManager.h"

void CUser::check_myself() {
    bool flag = false;
    for (std::vector<STPvPBuddyDBInfo>::iterator _it_buddy = buddyManager.buddies.begin(); _it_buddy != buddyManager.buddies.end(); ++_it_buddy) {
        CUser* user = g_user_manager.find_user((*_it_buddy).user_m_id);
        // 原始：user != NULL && !check_variable_charac_info(...) 复合条件直接物化（mov eax,1/0 + test al,al）
        if (user != NULL && !user->check_variable_charac_info(stGameUserInfo.server_id,
                                                         stGameUserInfo.buddy_n_user_id_what,
                                                         stGameUserInfo.variable_what1,
                                                         stGameUserInfo.variable_what2)) {
            flag = true;
            // 来自反编译/DWARF 的推断：应调用 user->update_variable_charac_info，在buddy用户的buddy列表中更新我的信息
            user->update_variable_charac_info(stGameUserInfo.server_id,
                                        stGameUserInfo.buddy_n_user_id_what,
                                        stGameUserInfo.variable_what1,
                                        stGameUserInfo.variable_what2);
        }
    }
    // 来自反编译/DWARF 的推断：有修改时才写库，原重gou if (!flag) 显然反了
    if (flag) {
        db_update_buddy();
    }
}

CUser::CUser(CNetworkSession *networkSession, STGameUserInfo &stGameUserInfo, int buddyCount, STPvPBuddyDBInfo *buddyList) {
    this->networkSession = networkSession;
    this->stGameUserInfo = stGameUserInfo;
    for (int i = 0; i < buddyCount; i++) {
        buddyManager.add_buddy(buddyList[i]);
    }
}
CUser::CUser(const CUser &other)
    : networkSession(other.networkSession),
      stGameUserInfo(other.stGameUserInfo),
      buddyManager(other.buddyManager) {  // 原始：成员初始化列表直接复制，无临时对象
}
bool CUser::check_variable_charac_info(char service_id, char const *user_id_what, char variable_what1, char variable_what2) {
    STPvPBuddyDBInfo* buddy = buddyManager.find_buddy(service_id, user_id_what);
    if (buddy == NULL) {
        return true;
    }
    if (buddy->variable_what1 == variable_what1 && buddy->variable_what2 == variable_what2) {
        return true;
    }
    return false;
}
void CUser::db_delete_buddy(char buddy_server_id, unsigned int buddy_charac_no) {
    Packet_Delete_PvP_Buddy packet;
    packet.user_server_id = stGameUserInfo.server_id;
    packet.user_charac_no = stGameUserInfo.charac_no;
    // 原始：buddy_server_id 先于 buddy_charac_no 赋值（[ebp-0xd] 在 [ebp-0xc] 之前）
    packet.buddy_server_id = buddy_server_id;
    packet.buddy_charac_no = buddy_charac_no;
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::db_insert_buddy(STPvPBuddyDBInfo *a, STPvPBuddyDBInfo *b) {
    Packet_Insert_PvP_Buddy packet;
    memcpy(&packet.a, a, sizeof(STPvPBuddyDBInfo));
    memcpy(&packet.b, b, sizeof(STPvPBuddyDBInfo));
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::db_update_buddy() {
    Packet_Update_PvP_Buddy packet;
    packet.server_id = stGameUserInfo.server_id;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.variable_what1 = stGameUserInfo.variable_what1;
    packet.variable_what2 = stGameUserInfo.variable_what2;
    networkSession->Send((char *)&packet, packet.packetSize);
}
CBuddyManager *CUser::get_buddy_manager() {
    return &buddyManager;
}
STGameUserInfo *CUser::get_user_info() {
    return &stGameUserInfo;
}
void CUser::get_user_info_for_buddy(STPvPBuddyDBInfo &output) {
    output.server_id = stGameUserInfo.server_id;
    output.user_m_id = stGameUserInfo.user_m_id;
    output.charac_no = stGameUserInfo.charac_no;
    output.variable_what1 = stGameUserInfo.variable_what1;
    output.buddy_n_user_what2 = stGameUserInfo.buddy_n_user_what2;
    output.variable_what2 = stGameUserInfo.variable_what2;
    output.buddy_n_user_what3 = stGameUserInfo.buddy_n_user_what3;
    memcpy(output.buddy_n_user_id_what, stGameUserInfo.buddy_n_user_id_what, 0x1d);
}
void CUser::notice_add_buddy_fail(char reason_what1, unsigned char error_code) {
    Packet_Notice_Add_PvP_Buddy_Result packet;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.sTGameUserInfo_what3_0x05 = stGameUserInfo.what_0x5;
    packet.reason_what_18_0x12 = reason_what1;
    packet.error_code_what_50_0x32 = error_code;
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::notice_add_buddy_success(char reason_what, CUser *user) {
    Packet_Notice_Add_PvP_Buddy_Result packet;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.sTGameUserInfo_what3_0x05 = stGameUserInfo.what_0x5;
    // 来自反编译/DWARF 的推断：server_id和buddy_n_user_id_what来自参数user，不是来自this
    // 原始顺序：reason_what_18_0x12 在 error_code_what_50_0x32 之前赋值
    packet.reason_what_18_0x12 = reason_what;
    packet.error_code_what_50_0x32 = 0;
    packet.server_id = user->stGameUserInfo.server_id;
    memcpy(packet.buddy_n_user_id_what, user->stGameUserInfo.buddy_n_user_id_what, 0x1d);
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::notice_login_logout(CUser::eLoginout loginout) {
    Packet_Notice_PvP_Buddy_In_Out packet;
    packet.what_0x12 = loginout;
    packet.channel_no = stGameUserInfo.channel_no;
    packet.server_id = stGameUserInfo.server_id;
    memcpy(packet.buddy_n_user_id_what, stGameUserInfo.buddy_n_user_id_what, 0x1d);
    for (std::vector<STPvPBuddyDBInfo>::iterator _it_buddie = buddyManager.buddies.begin(); _it_buddie != buddyManager.buddies.end(); ++_it_buddie) {
        CUser* user = g_user_manager.find_user((*_it_buddie).server_id, (*_it_buddie).buddy_n_user_id_what);
        if (user != NULL) {
            packet.what_0xa = user->stGameUserInfo.charac_no;
            packet.what_0xe = user->stGameUserInfo.what_0x5;
            user->networkSession->Send((char *)&packet, packet.packetSize);
        }
    }
}
void CUser::notice_remove_buddy_fail(unsigned char error_code) {
    Packet_Response_Remove_PvP_Buddy packet;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.sTGameUserInfo_what3_0x05 = stGameUserInfo.what_0x5;
    packet.error_code_what_0x31 = error_code;
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::notice_remove_buddy_success(char server_id, const char *user_id_what) {
    Packet_Response_Remove_PvP_Buddy packet;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.sTGameUserInfo_what3_0x05 = stGameUserInfo.what_0x5;
    packet.error_code_what_0x31 = 0;
    packet.server_id = server_id;
    memcpy(packet.buddy_n_user_id_what, user_id_what, 0x1d);
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::req_add_buddy(CUser *buddy) {
    Packet_Notice_Request_Add_PvP_Buddy packet;
    // 来自反编译/DWARF 的推断：packet内charac_no和what_0x5来自buddy，发送目标是buddy的网络会话
    packet.charac_no = buddy->stGameUserInfo.charac_no;
    packet.sTGameUserInfo_what3_0x05 = buddy->stGameUserInfo.what_0x5;
    packet.server_id = stGameUserInfo.server_id;
    memcpy(packet.buddy_n_user_id_what, stGameUserInfo.buddy_n_user_id_what, 0x1d);
    buddy->networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::req_remove_buddy(char param_server_id, char const *param_user_id_what) {
    STPvPBuddyDBInfo* removingBuddy = buddyManager.find_buddy(param_server_id, param_user_id_what);
    if (removingBuddy == NULL) {
        // 3 means buddy not found?
        notice_remove_buddy_fail(3);
        return;
    }
    // copied from server, never used
    __attribute__((unused)) int user_m_id = removingBuddy->user_m_id;
    char server_id = removingBuddy->server_id;
    // 原始：unsigned int 局部（ORIG db_delete_buddy 实参先求值 server_id 再 charac_no，
    // 2026-08-11 实测 int 类型会翻转求值顺序）
    unsigned int charac_no = removingBuddy->charac_no;
    if (!buddyManager.del_buddy(param_server_id, param_user_id_what)) {
        // 3 not means buddy not found???
        notice_remove_buddy_fail(3);
    } else {
        CUser* user = g_user_manager.find_user(param_server_id, param_user_id_what);
        if (user != NULL) {
            // 原始（ORIG 二进制实测）：del_buddy 第一实参为 this->stGameUserInfo.server_id
            // （movzbl 0x4(%eax),eax 中 eax=this），不是 user->...（2026-08-11 修正）
            user->get_buddy_manager()->del_buddy(stGameUserInfo.server_id, stGameUserInfo.buddy_n_user_id_what);
        }
        db_delete_buddy(server_id, charac_no);
        notice_remove_buddy_success(param_server_id, param_user_id_what);
        ArchiveLog("CUser::req_remove_buddy REQ(server_id:%d, charac_no:%d), RES(server_id:%d, charac_no:%d)", stGameUserInfo.server_id, stGameUserInfo.charac_no, server_id, charac_no);
    }
}
void CUser::res_add_buddy(CUser *buddy) {
    STPvPBuddyDBInfo buddyUserInfo;
    buddy->get_user_info_for_buddy(buddyUserInfo);
    buddyManager.add_buddy(buddyUserInfo);

    STPvPBuddyDBInfo myUserInfo;
    get_user_info_for_buddy(myUserInfo);
    buddy->buddyManager.add_buddy(myUserInfo);

    db_insert_buddy(&buddyUserInfo, &myUserInfo);

    notice_add_buddy_success(1, buddy);
    buddy->notice_add_buddy_success(0, this);

    ArchiveLog("CUser::res_add_buddy REQ(server_id:%d, charac_no:%d, channel_no:%d), RES(server_id:%d, charac_no:%d, channel_no:%d)", stGameUserInfo.server_id, stGameUserInfo.charac_no,
               stGameUserInfo.channel_no, buddy->stGameUserInfo.server_id, buddy->stGameUserInfo.charac_no, buddy->stGameUserInfo.channel_no);
}
void CUser::send_buddy_list() {
    Packet_Response_PvP_Buddy_Conn_List packet;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.sTGameUserInfo_what3_0x05 = stGameUserInfo.what_0x5;
    packet.buddyCount = buddyManager.get_size();
    // 原始：i = 0 初始化在 begin() 调用之前（ORIG movl $0x0,-0x14 先于 begin call）；
    // iBuddy 为 copy-init（iterator iBuddy = begin()），避免默认构造调用。
    int i = 0;
    std::vector<STPvPBuddyDBInfo>::iterator iBuddy = buddyManager.buddies.begin();
    while (iBuddy != buddyManager.buddies.end()) {
        if (i > 31) {
            break;
        }
        CUser* user = g_user_manager.find_user((*iBuddy).server_id, (*iBuddy).buddy_n_user_id_what);
        // 原始：user != NULL 分支内联在前（je 跳 NULL 分支），极性以 ORIG 反汇编为准
        if (user != NULL) {
            packet.buddies[i].isOnline = true;
            packet.buddies[i].channel_no = user->stGameUserInfo.channel_no;
        } else {
            packet.buddies[i].isOnline = false;
            packet.buddies[i].channel_no = -1;
        }
        STPvPBuddyDBInfo buddy = *iBuddy;
        // 原始：Ghidra 还原出 local_10 = &local_43 的指针形态（-0xc 槽），
        // 字段访问经指针重载（mov -0xc,%eax; movzbl (%eax)），非直接栈槽偏移。
        STPvPBuddyDBInfo *buddyPtr = &buddy;
        packet.buddies[i].server_id = buddyPtr->server_id;
        packet.buddies[i].charac_no = buddyPtr->charac_no;
        packet.buddies[i].variable_what1 = buddyPtr->variable_what1;
        packet.buddies[i].buddy_n_user_what2 = buddyPtr->buddy_n_user_what2;
        packet.buddies[i].variable_what2 = buddyPtr->variable_what2;
        packet.buddies[i].buddy_n_user_what3 = buddyPtr->buddy_n_user_what3;
        memcpy(packet.buddies[i].buddy_n_user_id_what, buddyPtr->buddy_n_user_id_what, 0x1d);
        i++;
        ++iBuddy;
    }
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::send_other_channel_chat(Packet_Monitor_Other_Channel_Chat *chat, CUser *user) {
    Packet_Monitor_Other_Channel_Chat_ToUser packet;
    packet.what_0x0a = chat->what_0x0a;
    packet.sTGameUserInfo_what3_0x05 = stGameUserInfo.what_0x5;
    packet.charac_no = stGameUserInfo.charac_no;
    // 原始：server_id 与 buddy id 来自发送者 user 参数（param_2），而非 this
    packet.server_id = user->stGameUserInfo.server_id;
    memcpy(packet.buddy_n_user_id_what, user->stGameUserInfo.buddy_n_user_id_what, 0x1d);
    packet.chatLength = chat->chatLength;
    memcpy(packet.chatContent, chat->chatContent, chat->chatLength);
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::send_other_channel_chat_hyper_link(Packet_Monitor_Other_Channel_Chat_Hyper_Link *chat, CUser *user) {
    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link packet;
    packet.what_0x0a = chat->what_0x0a;
    packet.what_0x0f = stGameUserInfo.what_0x5;
    packet.what_0x13 = stGameUserInfo.charac_no;
    // 原始：server_id 与 buddy id 来自发送者 user 参数（param_2）
    packet.what_0x0e = user->stGameUserInfo.server_id;
    memcpy(packet.what_0x17, user->stGameUserInfo.buddy_n_user_id_what, 0x1d);
    packet.what_0x16f = chat->what_0x173;
    memcpy(packet.what_0x170, chat->what_0x174, chat->what_0x173);
    packet.what_0x36 = chat->what_0x3a;
    // 原始：直接成员访问（what_0x37=0x37/ what_0x3b=0x3b，GCC 4.4 把成员偏移拆成
    // add $0x30 + lea/add 余量：ORIG add $0x30; add chat; lea 0xb → edx 形态）；
    // 不能用平铺 (char*)chat + 0x30 + i*0x68 + 0xb（常量折叠为 0x3b）。
    for (int i = 0; i < chat->what_0x3a; i++) {
        memcpy(&packet.what_0x37[i * 0x68], &chat->what_0x3b[i * 0x68], 0x68);
    }
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::send_other_channel_chat_result(Packet_Monitor_Other_Channel_Chat *chat, ENUM_MONITOR_ERROR error) {
    Packet_Monitor_Other_Channel_Chat_ToUser packet;
    packet.what_0x0a = chat->what_0x0a;
    packet.sTGameUserInfo_what3_0x05 = stGameUserInfo.what_0x5;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.errorCode = error;
    // 原始：不设置 server_id（保持构造时的 0），buddy id 来自聊天包（param_1+0x1c）
    memcpy(packet.buddy_n_user_id_what, chat->buddy_n_user_id_what, 0x1d);
    packet.chatLength = chat->chatLength;
    memcpy(packet.chatContent, chat->chatContent, chat->chatLength);
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::send_other_channel_chat_result_hyper_link(Packet_Monitor_Other_Channel_Chat_Hyper_Link *chat, ENUM_MONITOR_ERROR error) {
    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link packet;
    packet.what_0x0a = chat->what_0x0a;
    packet.what_0x0f = stGameUserInfo.what_0x5;
    packet.what_0x13 = stGameUserInfo.charac_no;
    packet.what_0x35 = error;
    // 原始：buddy id 来自聊天包（param_1+0x1c）
    memcpy(packet.what_0x17, chat->buddy_n_user_id_what, 0x1d);
    packet.what_0x16f = chat->what_0x173;
    memcpy(packet.what_0x170, chat->what_0x174, chat->what_0x173);
    packet.what_0x36 = chat->what_0x3a;
    // 原始：同 send_other_channel_chat_hyper_link 的直接成员访问形态
    for (int i = 0; i < chat->what_0x3a; i++) {
        memcpy(&packet.what_0x37[i * 0x68], &chat->what_0x3b[i * 0x68], 0x68);
    }
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::update_variable_charac_info(char server_id, const char *user_id_what, char variable_what1, char variable_what2) {
    STPvPBuddyDBInfo* buddy = buddyManager.find_buddy(server_id, user_id_what);
    // 原始：NULL 提前返回（尾部 jmp; nop 形态）
    if (buddy == NULL) {
        return;
    }
    buddy->variable_what1 = variable_what1;
    buddy->variable_what2 = variable_what2;
}
