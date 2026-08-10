//
// 原始：packet_proc 包处理器 + g_packet_table + g_user_manager 位于 PacketProc.cpp TU
//（_GLOBAL__I_g_user_manager 桩，初始化 ios_base::Init + CUserManager）。
//
#pragma once

class ISessionManager;
class CNetworkSession;
struct PacketHeader;

typedef int (*DispatcherFunction)(ISessionManager* sessionManager, CNetworkSession* networkSession, PacketHeader* packetHeader);

struct packet_table_item {
    int packet_id;
    DispatcherFunction packet_handler;
};

class CUserManager;
extern CUserManager g_user_manager;

extern packet_table_item* get_packet_table();
extern int get_packet_table_size();
