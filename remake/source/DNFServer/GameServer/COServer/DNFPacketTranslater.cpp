// df_coserver_r — CPacketTranslater（ORIG DNFPacketTranslater.cpp）
#include <stdio.h>
#include <time.h>

#include "DNFApplication.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "DNFTableBase.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFGameServer.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CApplication* CPacketTranslater::m_pclApp = 0;

// ORIG 实测（0x8057fbd）：pkt 复制到局部（-0x28）后经成员位移访问
// （movzbl 0xc(%eax) / movzwl 0xa(%eax) / mov 0xe(%eax,%edx,4)）。
struct Packet_ReplyUserInfo : public PacketHeader
{
    unsigned short m_count;     // +0xa
    unsigned char m_channel;    // +0xc
    unsigned char m_group;      // +0xd
    unsigned int m_userIDs[];   // +0xe
} __attribute__((packed));

struct Packet_HeartBeat : public PacketHeader
{
    unsigned char m_channel;    // +0xa
    unsigned char m_group;      // +0xb
} __attribute__((packed));

// ORIG 实测：OnLogin 的 pkt 经局部复制后按成员位移访问
// （mov 0xa(%eax) / movzbl 0xe(%eax) / movzbl 0xf(%eax)）。
struct Packet_Login : public PacketHeader
{
    unsigned int m_userID;      // +0xa
    unsigned char m_channel;    // +0xe
    unsigned char m_group;      // +0xf
} __attribute__((packed));

void CPacketTranslater::attach(CApplication* app)
{
    m_pclApp = app;
}

void CPacketTranslater::OnLogin(PacketHeader* pkt)
{
    PacketHeader* p = pkt;
    if (m_pclApp == 0)
    {
        return;
    }
    try
    {
        CGameServer* new_gs = m_pclApp->FindGameServer(
            ((Packet_Login*)p)->m_group, ((Packet_Login*)p)->m_channel);
        register bool not_found = !new_gs;
        if (not_found)
        {
            DNF_LOG_SCOPE_LINE(0x42,"./log/Channel",
                "Not Found M_ID(%s) Group No(%d) Channel No(%d)",
                NumberToString(((Packet_Login*)p)->m_userID, 0),
                ((Packet_Login*)p)->m_group, ((Packet_Login*)p)->m_channel);
            return;
        }
        CGameServer* old_gs;
        CUserManager* um = &m_pclApp->m_userManager;
        CUser user;
        if (um->FindUser(((Packet_Login*)p)->m_userID, user))
        {
            old_gs = user.GetGameServer();
            if (old_gs != 0 && old_gs->GetGroupNo() != ((Packet_Login*)p)->m_group)
            {
                Packet_Monitor_UDP_User_Getout getout;
                getout.m_userID = ((Packet_Login*)p)->m_userID;
                old_gs->SendToGameServer((char*)&getout, 0xe);
                getout.m_userID = ((Packet_Login*)p)->m_userID;
                new_gs->SendToGameServer((char*)&getout, 0xe);
                DNF_LOG_SCOPE_LINE(0x66,"./log/User",
                    "DOUBLE : ID(%s) Already Gr(%d) Ch(%d)\tCurrent Gr(%d) Ch(%d)",
                    NumberToString(((Packet_Login*)p)->m_userID, 0),
                    old_gs->GetGroupNo() & 0xff, old_gs->GetChannelNo() & 0xff,
                    ((Packet_Login*)p)->m_group, ((Packet_Login*)p)->m_channel);
            }
        }
        else
        {
            um->CreateUser(((Packet_Login*)p)->m_userID, new_gs);
        }
    }
    catch (CDNFException& e)
    {
        register const char* s = e.what();
        DNF_LOG_SCOPE_LINE(0x75, "./log/Except", "CPacketTranslater::OnLogin() Exception Break : %s\n", s);
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x7a, "./log/Except", "CPacketTranslater::OnLogin() Exception Break\n");
    }
}

void CPacketTranslater::OnLogout(PacketHeader* pkt)
{
    CUser user;
    PacketHeader* p = pkt;
    if (m_pclApp != 0)
    {
        try
        {
            CUserManager* um = &m_pclApp->m_userManager;
            if (!um->FindUser(((Packet_DoubleCheck_UDP_Logout*)p)->m_userID, user)) { }
            else
            {
                if (user.GetGameServer() != 0)
                {
                    if (!um->DeleteUser((Packet_DoubleCheck_UDP_Logout*)p))
                    {
                        DNF_LOG_SCOPE_LINE(0x99,"./log/User",
                            "[NO USER_DeleteUser_False] Disconnected User DB ID(%s), Group(%d), Channel(%d)",
                            NumberToString(((Packet_DoubleCheck_UDP_Logout*)p)->m_userID, 0),
                            ((Packet_DoubleCheck_UDP_Logout*)p)->m_group,
                            ((Packet_DoubleCheck_UDP_Logout*)p)->m_channel);
                    }
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x9d,"./log/User",
                        "[NO USER_GameServer_Diff] Disconnected User DB ID(%s), Group(%d), Channel(%d), GameServer(%d)",
                        NumberToString(((Packet_DoubleCheck_UDP_Logout*)p)->m_userID, 0),
                        ((Packet_DoubleCheck_UDP_Logout*)p)->m_group,
                        ((Packet_DoubleCheck_UDP_Logout*)p)->m_channel,
                        user.GetGameServer());
                }
            }
        }
        catch (CDNFException& e)
        {
            printf("CPacketTranslater::OnLogout() Exception Break : %s\n", e.what());
            register const char* s = e.what();
            DNF_LOG_SCOPE_LINE(0xa8, "./log/Except", "CPacketTranslater::OnLogout() Exception Break : %s\n", s);
        }
        catch (...)
        {
            puts("CPacketTranslater::OnLogout() Exception Break");
            DNF_LOG_SCOPE_LINE(0xae, "./log/Except", "CPacketTranslater::OnLogout() Exception Break\n");
        }
    }
}

void CPacketTranslater::OnHeartBeat(PacketHeader* pkt)
{
    CGameServer* gs;
    Packet_HeartBeat* p = (Packet_HeartBeat*)pkt;
    CServerHandler* handler;
    unsigned char channel;
    unsigned char group;
    if (m_pclApp == 0)
    {
        return;
    }
    handler = m_pclApp->m_serverHandler;
    if (handler == 0)
    {
        return;
    }
    try
    {
        channel = p->m_channel;
        group = p->m_group;
        if (!(100 < group || channel == 0 || 0xbe < channel))
        {
            handler->ResetHeartBeat(group, channel);
            if (!handler->IsConnectedGameServer(group, channel))
            {
                handler->SetConnectFlag(group, channel, true);
                gs = m_pclApp->FindGameServer(group, channel);
                if (gs != 0)
                {
                    Packet_CutOff_UDP_Call_UserInfo pkt2;
                    gs->SendToGameServer((char*)&pkt2, pkt2.packetSize);
                }
            }
            printf("Server(%dGroup:%dChennel) Heart Beat Arrived.\n", group, channel);
        }
        else
        {
            throw CDNFException("CPacketTranslater::OnHeartBeat() Channel Index Error\n");
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
        register const char* s = e.what();
        DNF_LOG_SCOPE_LINE(0xea,"./log/Except", "CPacketTranslater::OnHeartBeat() Exception Break : %s\n",
            s);
    }
    catch (...)
    {
        puts("CPacketTranslater::OnHeartBeat() Exception Break");
        DNF_LOG_SCOPE_LINE(0xf0, "./log/Except", "CPacketTranslater::OnHeartBeat() Exception Break\n");
    }
}

void CPacketTranslater::OnReplyUserInfo(PacketHeader* pkt)
{
    CGameServer* gs;
    Packet_ReplyUserInfo* p = (Packet_ReplyUserInfo*)pkt;
    register int channel = p->m_channel;
    register int group = p->m_group;
    DNF_LOG_SCOPE_LINE(0x102, "./log/Reboot", "[GAME SERVER] Group(%d) Channel(%d)", group, channel);
    try
    {
        if (m_pclApp != 0)
        {
            gs = m_pclApp->FindGameServer(p->m_group, p->m_channel);
            if (gs == 0)
            {
                throw CDNFException("CPacketTranslater::OnReplyUserInfo : pclGameServer == 0");
            }
            else
            {
                CUserManager* um = &m_pclApp->m_userManager;
                CUser user;
                for (int i = 0; i < p->m_count; i++)
                {
                    if (!um->FindUser(p->m_userIDs[i], user))
                    {
                        um->CreateUser(p->m_userIDs[i], gs);
                    }
                }
            }
        }
        else
        {
            throw CDNFException("CPacketTranslater::OnReplyUserInfo : m_pclApp == 0");
        }
    }
    catch (CDNFException& e)
    {
        register const char* s = e.what();
        DNF_LOG_SCOPE_LINE(0x11b,"./log/Except", "CPacketTranslater::OnReplyUserInfo() Exception Break : %s\n",
            s);
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x120, "./log/Except", "CPacketTranslater::OnReplyUserInfo() Exception Break\n");
    }
}
