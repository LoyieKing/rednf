#ifndef DNF_PACKET_TRANSLATER_H_
#define DNF_PACKET_TRANSLATER_H_

#include <string>
#include <vector>

#include "PacketHeader.h"

class CApplication;

int parse_string(std::vector<std::string>& v, std::string& s, char c);

// ---- CPacketTranslater（静态 handler + 全局 m_pclApp）----
class CPacketTranslater
{
public:
    static void attach(CApplication* app);
    static void OnInnerPacketLogin(PacketHeader* header);
    static void OnInnerPacketLogout(PacketHeader* header);
    static void OnEventStart(PacketHeader* header);
    static void OnEventEnd(PacketHeader* header);
    static void OnHeartBeat(PacketHeader* header);
    static void OnTcpServerLogin(PacketHeader* header);
    static void OnTcpServerLogout(PacketHeader* header);
    static void OnTcpServerHeartbeat(PacketHeader* header);
    static void OnCommonPacket(PacketHeader* header);
    static void OnWebNoticeInGameAD(PacketHeader* header);
    static void OnWebNoticeBroadcast(PacketHeader* header);
    static void OnWebNoticeProhibitConnectUser(PacketHeader* header);
    static void OnMonitorNoticeProhibitConnectUser(PacketHeader* header);
    static CApplication* m_pclApp;
};

#endif  // DNF_PACKET_TRANSLATER_H_
