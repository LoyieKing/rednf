// df_manager_r — CPacketTranslater / parse_string（ORIG DNFPacketTranslater.cpp）
#include "DNFPacketTranslater.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DNFApplication.h"
#include "DNFServerHandler.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFTcpServerInterface.h"
#include "DNFUdpHandler.h"
#include "Packet_Monitor_Manager_Connect_OK.h"
#include "Packet_Web_Notice_InGame_Advertisement.h"
#include "Packet_Web_Notice_Single.h"
#include "Packet_Web_Prohibit_User_Connect.h"
#include "ProhibitUser.h"

CApplication* CPacketTranslater::m_pclApp;

void CPacketTranslater::attach(CApplication* app)
{
    m_pclApp = app;
}

void CPacketTranslater::OnHeartBeat(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        CServerHandler* handler = m_pclApp->m_serverHandler;
        if (!handler)
            return;
        unsigned char idx = ((char*)header)[0xa];
        if (idx > 0x64)
            throw CDNFException(
                "CPacketTranslater::OnHeartBeat() \xc3\xa4\xb3\xce \xc0\xce\xb5\xa6\xbd\xba \xbf\xc0\xb7\xf9\n");
        handler->ResetHeartBeat(idx);
        if (!handler->IsConnectedMonitorServer(idx))
        {
            handler->SetConnectFlag(idx, 1);
            Packet_Monitor_Manager_Connect_OK pkt;
            handler->SendToTcpServer(&pkt, idx);
            printf("First Heart Beat Arrived From %d Group Monitor!\n", idx);
            CMyFileLog log(__FUNCTION__, 0x43);
            log("./log/Monitor", "First Heart Beat Arrived From %d Group Monitor!", idx);
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
        CMyFileLog log(__FUNCTION__, 0x52);
        log("./log/Except", "CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnHeartBeat() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x58);
        log("./log/Except", "CPacketTranslater::OnHeartBeat() Exception Break\n");
    }
}

void CPacketTranslater::OnEventStart(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnEventStart : 0 == m_pclApp");
        m_pclApp->m_serverHandler->SendAllTcpServer(header);
        CMyFileLog log(__FUNCTION__, 0x70);
        log("./log/Web",
            "CPacketTranslater::OnEventStart() eventCode(%d), eventParam1(%d), eventParam2(%d)\n",
            *(int*)((char*)header + 0xa),
            *(unsigned short*)((char*)header + 0xe),
            *(unsigned short*)((char*)header + 0x10));
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n", e.what());
        CMyFileLog log(__FUNCTION__, 0x75);
        log("./log/Except",
            "CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n",
            e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
        CMyFileLog log(__FUNCTION__, 0x7b);
        log("./log/Except",
            "CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd\n");
    }
}

void CPacketTranslater::OnEventEnd(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnEventEnd : 0 == m_pclApp");
        m_pclApp->m_serverHandler->SendAllTcpServer(header);
        CMyFileLog log(__FUNCTION__, 0x92);
        log("./log/Web", "CPacketTranslater::OnEventEnd() eventCode(%d)\n",
            *(int*)((char*)header + 0xa));
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n", e.what());
        CMyFileLog log(__FUNCTION__, 0x97);
        log("./log/Except",
            "CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n",
            e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
        CMyFileLog log(__FUNCTION__, 0x9d);
        log("./log/Except",
            "CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd\n");
    }
}

void CPacketTranslater::OnCommonPacket(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnCommonPacket : 0 == m_pclApp");
        m_pclApp->m_serverHandler->SendAllTcpServer(header);
        CMyFileLog log(__FUNCTION__, 0xb5);
        log("./log/Web", "CPacketTranslater::OnCommonPacket() packet_id(%d)\n", header->packetId);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCommonPacket() Exception Break : %s\n", e.what());
        CMyFileLog log(__FUNCTION__, 0xbb);
        log("./log/Except",
            "CPacketTranslater::OnEventEnd() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCommonPacket() Exception Break");
        CMyFileLog log(__FUNCTION__, 0xc1);
        log("./log/Except",
            "CPacketTranslater::OnEventEnd() Exception Break\n");
    }
}

void CPacketTranslater::OnInnerPacketLogin(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
        {
            CMyFileLog log(__FUNCTION__, 0x1f0);
            log("./log/Except", "CPacketTranslater::OnInnerPacketLogin : 0 == m_pclApp");
            return;
        }
        CMyFileLog log(__FUNCTION__, 0x1f6);
        log("./log/TcpServer", "CPacketTranslater::OnInnerPacketLogin (sock:%d)",
            *(int*)((char*)header + 6));
    }
    catch (CDNFException& e)
    {
        CMyFileLog log(__FUNCTION__, 0x1fa);
        log("./log/Except", "CPacketTranslater::OnInnerPacketLogin Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1ff);
        log("./log/Except", "CPacketTranslater::OnInnerPacketLogin Exception Break\n");
    }
}

void CPacketTranslater::OnInnerPacketLogout(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
        {
            CMyFileLog log(__FUNCTION__, 0x20a);
            log("./log/Except", "CPacketTranslater::OnInnerPacketLogout : 0 == m_pclApp");
            return;
        }
        int port = *(int*)((char*)header + 6);
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        CTcpServer* server = handler->GetTcpServer((unsigned int)port);
        if (!server)
        {
            CMyFileLog log(__FUNCTION__, 0x215);
            log("./log/TcpServer", "CPacketTranslater::OnInnerPacketLogout Invalid Server Instance(sock:%d)",
                port);
            return;
        }
        unsigned char idx = server->GetServerIndex();
        handler = m_pclApp->Get_ServerHandler();
        if (!handler->DeleteTcpServer(idx))
        {
            CMyFileLog log(__FUNCTION__, 0x21d);
            log("./log/TcpServer", "CPacketTranslater::OnInnerPacketLogout DeleteTcpServer fail(sock:%d)",
                port);
            return;
        }
        CMyFileLog log(__FUNCTION__, 0x221);
        log("./log/TcpServer",
            "CPacketTranslater::OnInnerPacketLogout DeleteTcpServer Success(TYPE:%d, sock:%d)",
            idx, port);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log(__FUNCTION__, 0x225);
        log("./log/Except", "CPacketTranslater::OnInnerPacketLogout Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x22a);
        log("./log/Except", "CPacketTranslater::OnInnerPacketLogout Exception Break\n");
    }
}

void CPacketTranslater::OnTcpServerLogin(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        int port = *(int*)((char*)header + 6);
        unsigned char idx = ((char*)header)[0xa];
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        if (handler->GetTcpServer(idx))
        {
            CMyFileLog log(__FUNCTION__, 0x239);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogin Duplicate Server Instance(TYPE:%d, sock:%d)",
                idx, port);
            return;
        }
        handler = m_pclApp->Get_ServerHandler();
        if (!handler->CreateTcpServer(idx, port))
        {
            CMyFileLog log(__FUNCTION__, 0x242);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogin CreateTcpServer fail(TYPE:%d, sock:%d)\n",
                idx, port);
            return;
        }
        printf("CPacketTranslater::OnTcpServerLogin(TYPE:%d, sock:%d)\n", idx, port);
        CMyFileLog log(__FUNCTION__, 0x250);
        log("./log/TcpServer", "CPacketTranslater::OnTcpServerLogin(TYPE:%d, sock:%d)\n", idx, port);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log(__FUNCTION__, 0x254);
        log("./log/Except", "CPacketTranslater::OnTcpServerLogin Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x259);
        log("./log/Except", "CPacketTranslater::OnTcpServerLogin Exception Break\n");
    }
}

void CPacketTranslater::OnTcpServerLogout(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        unsigned char idx = ((char*)header)[0xa];
        int port = *(int*)((char*)header + 6);
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        if (!handler->GetTcpServer(idx))
        {
            CMyFileLog log(__FUNCTION__, 0x269);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogout Invalid Server Instance(TYPE:%d, sock:%d)",
                idx, port);
            return;
        }
        if (!handler->DeleteTcpServer(idx))
        {
            CMyFileLog log(__FUNCTION__, 0x26f);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogout DeleteTcpServer fail(TYPE:%d, sock:%d)",
                idx, port);
            return;
        }
        printf("CPacketTranslater::OnTcpServerLogout(TYPE:%d, sock:%d)", idx, port);
        CMyFileLog log(__FUNCTION__, 0x273);
        log("./log/TcpServer", "CPacketTranslater::OnTcpServerLogout(TYPE:%d, sock:%d)", idx, port);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log(__FUNCTION__, 0x277);
        log("./log/Except", "CPacketTranslater::OnTcpServerLogout Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x27c);
        log("./log/Except", "CPacketTranslater::OnTcpServerLogout Exception Break\n");
    }
}

void CPacketTranslater::OnTcpServerHeartbeat(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        unsigned char idx = ((char*)header)[0xa];
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        CTcpServer* server = handler->GetTcpServer(idx);
        if (!server)
        {
            CMyFileLog log(__FUNCTION__, 0x28d);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerHeartbeat Invalid Server Instance(TYPE:%d, sock:%d)",
                idx, *(int*)((char*)header + 6));
            return;
        }
        server->NotifyHeartbeat();
    }
    catch (CDNFException& e)
    {
        CMyFileLog log(__FUNCTION__, 0x299);
        log("./log/Except", "CPacketTranslater::OnTcpServerHeartbeat Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x29e);
        log("./log/Except", "CPacketTranslater::OnTcpServerHeartbeat Exception Break\n");
    }
}

void CPacketTranslater::OnWebNoticeInGameAD(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        Packet_Web_Notice_InGame_Advertisement pkt;
        m_pclApp->m_serverHandler->SendAllTcpServer(&pkt);
        CMyFileLog log(__FUNCTION__, 0x2ae);
        log("./log/Web", "OnWebNoticeInGameAD() packet_id(%d)\n", header->packetId);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log(__FUNCTION__, 0x2b2);
        log("./log/Except", "CPacketTranslater::OnWebNoticeInGameAD Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x2b7);
        log("./log/Except", "CPacketTranslater::OnWebNoticeInGameAD Exception Break\n");
    }
}

void CPacketTranslater::OnWebNoticeBroadcast(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnWebNoticeBroadcast : 0 == m_pclApp");
        int len = ((char*)header)[0x10a];
        CMyFileLog log(__FUNCTION__, 0x1b2);
        log("./log/test", "%d, %s, %d, %s\n",
            ((char*)header)[0xa], (char*)header + 0xb, len, (char*)header + 0x10b);
        if (m_pclApp && m_pclApp->m_serverHandler)
        {
            std::vector<std::string> parts;
            std::string s((char*)header + 0xb, len);
            parse_string(parts, s, ',');
            for (std::vector<std::string>::iterator it = parts.begin();
                 it != parts.end(); ++it)
            {
                std::string tok = *it;
                int ch = atoi(tok.c_str());
                if (ch <= 0x64)
                {
                    Packet_Web_Notice_Single pkt;
                    ((char*)&pkt)[0xa] = len;
                    memset((char*)&pkt + 0xb, 0, 0xff);
                    strncpy((char*)&pkt + 0xb, (char*)header + 0x10b, len);
                    m_pclApp->m_serverHandler->SendToTcpServer((char*)&pkt, 0x10a, ch);
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log(__FUNCTION__, 0x1e0);
        log("./log/Except", "CPacketTranslater::OnWebNoticeBroadcast() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1e5);
        log("./log/Except", "CPacketTranslater::OnWebNoticeBroadcast() Exception Break\n");
    }
}

void CPacketTranslater::OnWebNoticeProhibitConnectUser(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(
                "CPacketTranslater::OnWebNoticeProhibitConnectUser : 0 == m_pclApp");
        Packet_Web_Prohibit_User_Connect pkt;
        memcpy(&pkt, header, 0x12);
        *(unsigned short*)((char*)&pkt + 2) = 0x13;
        int m_id = pkt.m_fieldA;
        int ip = *(int*)((char*)&pkt + 6);
        int port = *(unsigned short*)((char*)&pkt + 4);
        char flag = ((char*)&pkt)[0xe];
        short time = *(short*)((char*)&pkt + 0xf);
        {
            CMyFileLog log(__FUNCTION__, 0xdb);
            log("./log/ProhibitUser",
                "CPacketTranslater::OnWebNoticeProhibitConnectUser m_id : %d, flag( %d ), time( %d ), ip( %d ), port( %d )\n",
                m_id, flag, time, ip, port);
        }
        CUserManager* um = &m_pclApp->m_userManager;
        if (flag && um->DeleteProhibitUser(m_id))
        {
            CMyFileLog log(__FUNCTION__, 0xe6);
            log("./log/Web",
                "CPacketTranslater::OnWebNoticeProhibitConnectUser Delete Err  m_id : %d, flag( %d ), time( %d ), ip( %d ), port( %d )\n",
                m_id, flag, time, ip, port);
            m_pclApp->m_serverHandler->SendAllTcpServer((PacketHeader*)&pkt);
            return;
        }
        CDNFProhibitUser* pu = um->FindProhibitUser(m_id);
        if (!pu)
        {
            pu = new CDNFProhibitUser;
            pu->SetMonitorWaitTime(m_id, 2);
            pu->SetIpPort(ip, port);
            um->InsertProhibitUser(m_id, pu);
            m_pclApp->m_serverHandler->SendAllTcpServer((PacketHeader*)&pkt);
            return;
        }
        ((char*)header)[0x11] = 2;
        {
            CMyFileLog log(__FUNCTION__, 0x113);
            log("./log/ProhibitUser",
                "CPacketTranslater::OnWebNoticeProhibitConnectUser SendToClient, m_id : %d, ip( %d ), port( %d ), m_bIsConnect(%d), m_bProhibitConnect(%d)\n",
                m_id, ip, port, ((char*)header)[0x11], ((char*)header)[0xe]);
        }
        if (!((CUdpHandler*)m_pclApp->Get_UdpHandler())
                 ->SendToClient((char*)header, 0x13, port, 0, ip))
            throw CDNFException(strerror(errno));
    }
    catch (CDNFException& e)
    {
        CMyFileLog log(__FUNCTION__, 0x11b);
        log("./log/Except",
            "CPacketTranslater::OnWebNoticeProhibitConnectUser() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x120);
        log("./log/Except",
            "CPacketTranslater::OnWebNoticeProhibitConnectUser() Exception Break\n");
    }
}

void CPacketTranslater::OnMonitorNoticeProhibitConnectUser(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(
                "CPacketTranslater::OnMonitorNoticeProhibitConnectUser : 0 == m_pclApp");
        CUserManager* um = &m_pclApp->m_userManager;
        int key = *(int*)((char*)header + 0xa);
        char flag = ((char*)header)[0xe];
        short time = *(short*)((char*)header + 0xf);
        CDNFProhibitUser* pu = um->FindProhibitUser(key);
        if (!pu)
        {
            CMyFileLog log(__FUNCTION__, 0x138);
            log("./log/ProhibitUser",
                "CPacketTranslater::OnMonitorNoticeProhibitConnectUser Time Out, m_id : %d, flag( %d ), time( %d )\n",
                key, flag, time);
            return;
        }
        unsigned int uip;
        unsigned short uport;
        pu->GetIpPort(uip, uport);
        if (uip == 0 && uport == 0)
            return;
        if (flag == 2)
        {
            *(unsigned short*)((char*)header) = 0x4c8;
            ((char*)header)[0x11] = 2;
            *(unsigned short*)((char*)header + 2) = 0x12;
            CMyFileLog log(__FUNCTION__, 0x14a);
            log("./log/ProhibitUser",
                "CPacketTranslater::OnMonitorNoticeProhibitConnectUser SendToClient, m_id : %d, ip( %d ), port( %d ), m_bIsConnect(%d), m_bProhibitConnect(%d)\n",
                key, uip, uport, ((char*)header)[0x11], ((char*)header)[0xe]);
            if (!((CUdpHandler*)m_pclApp->Get_UdpHandler())
                     ->SendToClient((char*)header, 0x12, uport, 0, uip))
                throw CDNFException(strerror(errno));
            return;
        }
        pu->IncreMonitorRetPacket();
        pu->SetProhibitUserInfo(((char*)header)[0x11]);
        CMyFileLog log(__FUNCTION__, 0x157);
        log("./log/ProhibitUser",
            "CPacketTranslater::OnMonitorNoticeProhibitConnectUser Check IP Port, m_id : %d, server group(%d), cnt(%d), m_bIsConnect(%d)",
            key, ((char*)header)[0x12], pu->GetMonitorRetPacketCnt(), ((char*)header)[0x11]);
        if (pu->GetMonitorRetPacketCnt() >= m_pclApp->m_serverHandler->GetAlivedMonitorServer())
        {
            Packet_Web_Prohibit_User_Connect pkt;
            ((char*)&pkt)[0xe] = 0x7f;
            *(unsigned short*)((char*)&pkt + 0xf) = 0;
            ((char*)&pkt)[0x11] = pu->GetConnectFlag();
            CMyFileLog log2("OnMonitorNoticeProhibitConnectUser", 0x165);
            log2("./log/ProhibitUser",
                "CPacketTranslater::OnMonitorNoticeProhibitConnectUser SendToClient, m_id : %d, ip( %d ), port( %d ), m_bIsConnect(%d)\n",
                key, uip, uport, pu->GetConnectFlag());
            if (!((CUdpHandler*)m_pclApp->Get_UdpHandler())
                     ->SendToClient((char*)&pkt, 0x12, uport, 0, uip))
                throw CDNFException(strerror(errno));
        }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log(__FUNCTION__, 0x180);
        log("./log/Except",
            "CPacketTranslater::OnMonitorNoticeProhibitConnectUser Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x185);
        log("./log/Except",
            "CPacketTranslater::OnMonitorNoticeProhibitConnectUser Exception Break\n");
    }
}

int parse_string(std::vector<std::string>& v, std::string& s, char c)
{
    int len = s.size();
    int pos = 0;
    std::string tok;
    while (pos < len)
    {
        int found = s.find(c, pos);
        if (found == (int)std::string::npos)
        {
            tok = s.substr(pos, len - pos);
            if (!tok.empty())
                v.push_back(tok);
            break;
        }
        tok = s.substr(pos, found - pos);
        if (!tok.empty())
            v.push_back(tok);
        pos = found + 1;
    }
    return 1;
}

// 强制发出原版存在的 libstdc++ 分配器弱符号
template class std::allocator<std::string>;
