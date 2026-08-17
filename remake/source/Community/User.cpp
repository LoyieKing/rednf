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
                                                         stGameUserInfo.m_name,
                                                         stGameUserInfo.m_level,
                                                         stGameUserInfo.m_growType)) {
            flag = true;
            // 来自反编译/DWARF 的推断：应调用 user->update_variable_charac_info，在buddy用户的buddy列表中更新我的信息
            user->update_variable_charac_info(stGameUserInfo.server_id,
                                        stGameUserInfo.m_name,
                                        stGameUserInfo.m_level,
                                        stGameUserInfo.m_growType);
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
    if (buddy->m_level == variable_what1 && buddy->m_growType == variable_what2) {
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
    packet.m_level = stGameUserInfo.m_level;
    packet.m_growType = stGameUserInfo.m_growType;
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
    output.m_level = stGameUserInfo.m_level;
    output.m_job = stGameUserInfo.m_job;
    output.m_growType = stGameUserInfo.m_growType;
    output.m_sex = stGameUserInfo.m_sex;
    memcpy(output.m_name, stGameUserInfo.m_name, 0x1d);
}
void CUser::notice_add_buddy_fail(char reason_what1, unsigned char error_code) {
    Packet_Notice_Add_PvP_Buddy_Result packet;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.m_uid = stGameUserInfo.m_uid;
    packet.m_resultType = reason_what1;
    packet.m_errorCode = error_code;
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::notice_add_buddy_success(char reason_what, CUser *user) {
    Packet_Notice_Add_PvP_Buddy_Result packet;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.m_uid = stGameUserInfo.m_uid;
    // 来自反编译/DWARF 的推断：server_id和m_name来自参数user，不是来自this
    // 原始顺序：m_resultType 在 m_errorCode 之前赋值
    packet.m_resultType = reason_what;
    packet.m_errorCode = 0;
    packet.server_id = user->stGameUserInfo.server_id;
    memcpy(packet.m_name, user->stGameUserInfo.m_name, 0x1d);
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::notice_login_logout(CUser::eLoginout loginout) {
    Packet_Notice_PvP_Buddy_In_Out packet;
    packet.m_loginout = loginout;
    packet.channel_no = stGameUserInfo.channel_no;
    packet.server_id = stGameUserInfo.server_id;
    memcpy(packet.m_name, stGameUserInfo.m_name, 0x1d);
    for (std::vector<STPvPBuddyDBInfo>::iterator _it_buddie = buddyManager.buddies.begin(); _it_buddie != buddyManager.buddies.end(); ++_it_buddie) {
        CUser* user = g_user_manager.find_user((*_it_buddie).server_id, (*_it_buddie).m_name);
        if (user != NULL) {
            packet.m_characNo = user->stGameUserInfo.charac_no;
            packet.m_uid = user->stGameUserInfo.m_uid;
            user->networkSession->Send((char *)&packet, packet.packetSize);
        }
    }
}
void CUser::notice_remove_buddy_fail(unsigned char error_code) {
    Packet_Response_Remove_PvP_Buddy packet;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.m_uid = stGameUserInfo.m_uid;
    packet.m_errorCode = error_code;
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::notice_remove_buddy_success(char server_id, const char *user_id_what) {
    Packet_Response_Remove_PvP_Buddy packet;
    packet.charac_no = stGameUserInfo.charac_no;
    packet.m_uid = stGameUserInfo.m_uid;
    packet.m_errorCode = 0;
    packet.server_id = server_id;
    memcpy(packet.m_name, user_id_what, 0x1d);
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::req_add_buddy(CUser *buddy) {
    Packet_Notice_Request_Add_PvP_Buddy packet;
    // 来自反编译/DWARF 的推断：packet内charac_no和what_0x5来自buddy，发送目标是buddy的网络会话
    packet.charac_no = buddy->stGameUserInfo.charac_no;
    packet.m_uid = buddy->stGameUserInfo.m_uid;
    packet.server_id = stGameUserInfo.server_id;
    memcpy(packet.m_name, stGameUserInfo.m_name, 0x1d);
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
            user->get_buddy_manager()->del_buddy(stGameUserInfo.server_id, stGameUserInfo.m_name);
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
    packet.m_uid = stGameUserInfo.m_uid;
    packet.buddyCount = buddyManager.get_size();
    // 原始：i = 0 初始化在 begin() 调用之前（ORIG movl $0x0,-0x14 先于 begin call）；
    // iBuddy 为 copy-init（iterator iBuddy = begin()），避免默认构造调用。
    int i = 0;
    std::vector<STPvPBuddyDBInfo>::iterator iBuddy = buddyManager.buddies.begin();
    while (iBuddy != buddyManager.buddies.end()) {
        if (i > 31) {
            break;
        }
        CUser* user = g_user_manager.find_user((*iBuddy).server_id, (*iBuddy).m_name);
        // 原始：user != NULL 分支内联在前（je 跳 NULL 分支），极性以 ORIG 反汇编为准
        if (user != NULL) {
            packet.buddies[i].isOnline = true;
            packet.buddies[i].channel_no = user->stGameUserInfo.channel_no;
        } else {
            packet.buddies[i].isOnline = false;
            packet.buddies[i].channel_no = -1;
        }
        // 原始：Ghidra 还原出 local_10 = &local_43 的指针形态（-0xc 槽），
        // 字段访问经指针重载（mov -0xc,%eax; movzbl (%eax)），非直接栈槽偏移。
        // 43 字节拷贝以临时对象形式出现（-0x3f 槽，紧邻迭代器槽之后，
        // 在 ORIG 布局中为立即分配的临时体而非延迟的命名局部）。
        STPvPBuddyDBInfo *buddyPtr = &(STPvPBuddyDBInfo(*iBuddy));
        packet.buddies[i].server_id = buddyPtr->server_id;
        packet.buddies[i].charac_no = buddyPtr->charac_no;
        packet.buddies[i].m_level = buddyPtr->m_level;
        packet.buddies[i].m_job = buddyPtr->m_job;
        packet.buddies[i].m_growType = buddyPtr->m_growType;
        packet.buddies[i].m_sex = buddyPtr->m_sex;
        memcpy(packet.buddies[i].m_name, buddyPtr->m_name, 0x1d);
        i++;
        ++iBuddy;
    }
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::send_other_channel_chat(Packet_Monitor_Other_Channel_Chat *chat, CUser *user) {
    Packet_Monitor_Other_Channel_Chat_ToUser packet;
    packet.m_senderCharId = chat->m_chatType;
    packet.m_idByChannel = stGameUserInfo.m_uid;
    packet.m_uniqCharNo = stGameUserInfo.charac_no;
    // 原始：server_id 与 buddy id 来自发送者 user 参数（param_2），而非 this
    packet.m_serverId = user->stGameUserInfo.server_id;
    memcpy(packet.m_name, user->stGameUserInfo.m_name, 0x1d);
    packet.m_msgLen = chat->m_msgLen;
    memcpy(packet.m_msg, chat->m_msg, chat->m_msgLen);
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::send_other_channel_chat_hyper_link(Packet_Monitor_Other_Channel_Chat_Hyper_Link *chat, CUser *user) {
    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link packet;
    packet.m_senderCharId = chat->m_chatType;
    packet.m_idByChannel = stGameUserInfo.m_uid;
    packet.m_uniqCharNo = stGameUserInfo.charac_no;
    // 原始：server_id 与 buddy id 来自发送者 user 参数（param_2）
    packet.m_serverId = user->stGameUserInfo.server_id;
    memcpy(packet.m_name, user->stGameUserInfo.m_name, 0x1d);
    packet.m_msgLen = chat->m_msgLen;
    memcpy(packet.m_msg, chat->m_msg, chat->m_msgLen);
    packet.m_itemCount = chat->m_itemCount;
    // 原始：what_0x37/what_0x3b 为 2D 数组 char[3][0x68]（0x138 == 3×0x68，
    // 头文件 2026-08-11 按 ORIG 反汇编还原），索引形态 &arr[i] 使 GCC 4.4 产生
    // ORIG 的 add $0x30 + add $0x7/0xb 分步地址计算（平铺 (char*)chat+0x30+i*0x68+0xb
    // 会常量折叠为 0x3b，助记符不一致）。
    for (int i = 0; i < chat->m_itemCount; i++) {
        memcpy(&packet.m_items[i], &chat->m_items[i], 0x68);
    }
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::send_other_channel_chat_result(Packet_Monitor_Other_Channel_Chat *chat, ENUM_MONITOR_ERROR error) {
    Packet_Monitor_Other_Channel_Chat_ToUser packet;
    packet.m_senderCharId = chat->m_chatType;
    packet.m_idByChannel = stGameUserInfo.m_uid;
    packet.m_uniqCharNo = stGameUserInfo.charac_no;
    packet.m_type = error;
    // 原始：不设置 server_id（保持构造时的 0），buddy id 来自聊天包（param_1+0x1c）
    memcpy(packet.m_name, chat->buddy_n_user_id_what, 0x1d);
    packet.m_msgLen = chat->m_msgLen;
    memcpy(packet.m_msg, chat->m_msg, chat->m_msgLen);
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::send_other_channel_chat_result_hyper_link(Packet_Monitor_Other_Channel_Chat_Hyper_Link *chat, ENUM_MONITOR_ERROR error) {
    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link packet;
    packet.m_senderCharId = chat->m_chatType;
    packet.m_idByChannel = stGameUserInfo.m_uid;
    packet.m_uniqCharNo = stGameUserInfo.charac_no;
    packet.m_type = error;
    // 原始：buddy id 来自聊天包（param_1+0x1c）
    memcpy(packet.m_name, chat->buddy_n_user_id_what, 0x1d);
    packet.m_msgLen = chat->m_msgLen;
    memcpy(packet.m_msg, chat->m_msg, chat->m_msgLen);
    packet.m_itemCount = chat->m_itemCount;
    // 原始：同 send_other_channel_chat_hyper_link 的 2D 数组索引形态
    for (int i = 0; i < chat->m_itemCount; i++) {
        memcpy(&packet.m_items[i], &chat->m_items[i], 0x68);
    }
    networkSession->Send((char *)&packet, packet.packetSize);
}
void CUser::update_variable_charac_info(char server_id, const char *user_id_what, char variable_what1, char variable_what2) {
    STPvPBuddyDBInfo* buddy = buddyManager.find_buddy(server_id, user_id_what);
    // 原始：NULL 提前返回（尾部 jmp; nop 形态）
    if (buddy == NULL) {
        return;
    }
    buddy->m_level = variable_what1;
    buddy->m_growType = variable_what2;
}
