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

void CPacketTranslater::attach(CApplication* app)
{
    m_pclApp = app;
}

void CPacketTranslater::OnLogin(PacketHeader* pkt)
{
    if (m_pclApp == 0)
    {
        return;
    }
    try
    {
        unsigned char group = *(unsigned char*)((char*)pkt + 0xf);
        unsigned char channel = *(unsigned char*)((char*)pkt + 0xe);
        unsigned int user_id = *(unsigned int*)((char*)pkt + 0xa);
        CGameServer* new_gs = m_pclApp->FindGameServer(group, channel);
        if (new_gs != 0)
        {
            CUserManager* um = &m_pclApp->m_userManager;
            CUser user;
            if (!um->FindUser(user_id, user))
            {
                um->CreateUser(user_id, new_gs);
            }
            else
            {
                CGameServer* old_gs = user.GetGameServer();
                if (old_gs != 0 && old_gs->GetGroupNo() != group)
                {
                    Packet_Monitor_UDP_User_Getout getout;
                    getout.m_userID = user_id;
                    old_gs->SendToGameServer((char*)&getout, 0xe);
                    new_gs->SendToGameServer((char*)&getout, 0xe);
                    register const char* s = NumberToString(user_id, 0);
                    register int g = old_gs->GetGroupNo() & 0xff;
                    register int ch = old_gs->GetChannelNo() & 0xff;
                    DNF_LOG_SCOPE_LINE(0x66,"./log/User",
                        "DOUBLE : ID(%s) Already Gr(%d) Ch(%d)\tCurrent Gr(%d) Ch(%d)",
                        s, g, ch, group, channel);
                }
            }
            return;
        }
        register const char* s = NumberToString(user_id, 0);
        DNF_LOG_SCOPE_LINE(0x42,"./log/Channel", "Not Found M_ID(%s) Group No(%d) Channel No(%d)",
            s, group, channel);
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
    if (m_pclApp != 0)
    {
        try
        {
            CUserManager* um = &m_pclApp->m_userManager;
            unsigned int user_id = *(unsigned int*)((char*)pkt + 0xa);
            if (um->FindUser(user_id, user))
            {
                if (user.GetGameServer() == 0)
                {
                    register const char* s = NumberToString(user_id, 0);
                    DNF_LOG_SCOPE_LINE(0x9d,"./log/User",
                        "[NO USER_GameServer_Diff] Disconnected User DB ID(%s), Group(%d), Channel(%d), GameServer(%d)",
                        s, *(unsigned char*)((char*)pkt + 0xf),
                        *(unsigned char*)((char*)pkt + 0xe), user.GetGameServer());
                }
                else if (!um->DeleteUser((Packet_DoubleCheck_UDP_Logout*)pkt))
                {
                    register const char* s = NumberToString(user_id, 0);
                    DNF_LOG_SCOPE_LINE(0x99,"./log/User",
                        "[NO USER_DeleteUser_False] Disconnected User DB ID(%s), Group(%d), Channel(%d)",
                        s, *(unsigned char*)((char*)pkt + 0xf),
                        *(unsigned char*)((char*)pkt + 0xe));
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
    if (m_pclApp != 0 && m_pclApp->m_serverHandler != 0)
    {
        try
        {
            unsigned char channel = *(unsigned char*)((char*)pkt + 0xa);
            unsigned char group = *(unsigned char*)((char*)pkt + 0xb);
            if (100 < group || channel == 0 || 0xbe < channel)
            {
                throw CDNFException("CPacketTranslater::OnHeartBeat() Channel Index Error\n");
            }
            m_pclApp->m_serverHandler->ResetHeartBeat(group, channel);
            if (!m_pclApp->m_serverHandler->IsConnectedGameServer(group, channel))
            {
                m_pclApp->m_serverHandler->SetConnectFlag(group, channel, true);
                CGameServer* gs = m_pclApp->FindGameServer(group, channel);
                if (gs != 0)
                {
                    Packet_CutOff_UDP_Call_UserInfo pkt2;
                    gs->SendToGameServer((char*)&pkt2, *(unsigned short*)((char*)&pkt2 + 2));
                }
            }
            printf("Server(%dGroup:%dChennel) Heart Beat Arrived.\n", group, channel);
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
}

void CPacketTranslater::OnReplyUserInfo(PacketHeader* pkt)
{
    unsigned char channel = *(unsigned char*)((char*)pkt + 0xc);
    unsigned char group = *(unsigned char*)((char*)pkt + 0xd);
    DNF_LOG_SCOPE_LINE(0x102, "./log/Reboot", "[GAME SERVER] Group(%d) Channel(%d)", group, channel);
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnReplyUserInfo : m_pclApp == 0");
        }
        CGameServer* gs = m_pclApp->FindGameServer(group, channel);
        if (gs == 0)
        {
            throw CDNFException("CPacketTranslater::OnReplyUserInfo : pclGameServer == 0");
        }
        CUserManager* um = &m_pclApp->m_userManager;
        CUser user;
        unsigned short count = *(unsigned short*)((char*)pkt + 0xa);
        for (int i = 0; i < count; i++)
        {
            unsigned int user_id = *(unsigned int*)((char*)pkt + 0xe + i * 4);
            if (!um->FindUser(user_id, user))
            {
                um->CreateUser(user_id, gs);
            }
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
