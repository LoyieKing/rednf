#ifndef DNF_PACKET_TRANSLATER_H_
#define DNF_PACKET_TRANSLATER_H_

#include "PacketHeader.h"

class CApplication;

// ---- CPacketTranslater：静态 m_pclApp ----
class CPacketTranslater
{
public:
    static void attach(CApplication* app);
    static void OnLogin(PacketHeader* pkt);
    static void OnLogout(PacketHeader* pkt);
    static void OnHeartBeat(PacketHeader* pkt);
    static void OnReplyUserInfo(PacketHeader* pkt);
    static CApplication* m_pclApp;
};

#endif // DNF_PACKET_TRANSLATER_H_
