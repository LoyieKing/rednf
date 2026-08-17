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

// R10: ORIG 以该布局结构体成员直读包偏移（mov disp(%reg) 形态，非 add+mov）
#pragma pack(push, 1)
struct WebEventPacket
{
    unsigned short m_wPacketId;
    unsigned short m_wPacketSize;
    unsigned short m_wReversed1;
    unsigned int   m_dwReversed2;
    int            m_nEventCode;    // 0xa
    unsigned short m_wEventParam1;  // 0xe
    unsigned short m_wEventParam2;  // 0x10
};

// R10: TcpServer 登录/登出/心跳包：0x6(uint) 与 0xa(uchar) 以成员直读复现
// ORIG 的 mov disp(%reg) 形态
struct TcpServerPacket
{
    unsigned short m_wPacketId;
    unsigned short m_wPacketSize;
    unsigned short m_wReversed1;
    unsigned int   m_dwReversed2;   // 0x6
    unsigned char  m_byType;        // 0xa
};

// R10: Web Notice Broadcast 包：0xa(uchar)/0xb(char[0xff])/0x10a(uchar)/0x10b(char[0xff])
struct WebNoticeBroadcastPacket
{
    unsigned short m_wPacketId;
    unsigned short m_wPacketSize;
    unsigned short m_wReversed1;
    unsigned int   m_dwReversed2;
    unsigned char  m_byType;        // 0xa
    char           m_szMsg[0xff];   // 0xb
    unsigned char  m_byLen;         // 0x10a
    char           m_szMsg2[0xff];  // 0x10b
};

// R10: Prohibit User Connect 包布局（short@0xf 复现 ORIG movswl 形态）
struct ProhibitUserPacket
{
    unsigned short m_wPacketId;
    unsigned short m_wPacketSize;
    unsigned short m_wReversed1;    // 0x4
    unsigned int   m_dwReversed2;   // 0x6
    int            m_nFieldA;       // 0xa
    char           m_chFieldE;      // 0xe
    short          m_sFieldF;       // 0xf
    char           m_chField11;     // 0x11
    char           m_chField12;     // 0x12
};
#pragma pack(pop)

CApplication* CPacketTranslater::m_pclApp;

void CPacketTranslater::attach(CApplication* app)
{
    m_pclApp = app;
}

void CPacketTranslater::OnHeartBeat(PacketHeader* header)
{
    PacketHeader* hdr = header;
    if (m_pclApp != 0)
    {
        CServerHandler* handler = m_pclApp->m_serverHandler;
        if (!handler)
            return;
        unsigned char idx = ((TcpServerPacket*)hdr)->m_byType;
        try
        {
            // R10: ORIG 布局为 if(idx<=0x64){body}else{throw}（ja 跳 throw 置尾，
            // throw 临时槽在 body log 之后分配）
            if (idx <= 0x64)
            {
                handler->ResetHeartBeat(idx);
                if (!handler->IsConnectedMonitorServer(idx))
                {
                    handler->SetConnectFlag(idx, 1);
                    Packet_Monitor_Manager_Connect_OK pkt;
                    handler->SendToTcpServer(&pkt, idx);
                    printf("First Heart Beat Arrived From %d Group Monitor!\n", idx);
                    DNF_LOG_SCOPE_LINE(0x43, "./log/Monitor",
                        "First Heart Beat Arrived From %d Group Monitor!", idx);
                }
            }
            else
            {
                throw CDNFException(
                    "CPacketTranslater::OnHeartBeat() \xc3\xa4\xb3\xce \xc0\xce\xb5\xa6\xbd\xba \xbf\xc0\xb7\xf9\n");
            }
        }
        catch (CDNFException& e)
        {
            printf("CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x52, "./log/Except",
                "CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CPacketTranslater::OnHeartBeat() Exception Break");
            DNF_LOG_SCOPE_LINE(0x58, "./log/Except",
                "CPacketTranslater::OnHeartBeat() Exception Break\n");
        }
    }
}

void CPacketTranslater::OnEventStart(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnEventStart : 0 == m_pclApp");
        m_pclApp->m_serverHandler->SendAllTcpServer(header);
        // R10: ORIG 为临时对象调用形态，实参在 ctor 前求值入 callee-saved 寄存器
        PacketHeader* hdr = header;  // R10: ORIG local_24@-0x20，兼对齐 throw 槽位
        DNF_LOG_SCOPE_LINE(0x70, "./log/Web",
            "CPacketTranslater::OnEventStart() eventCode(%d), eventParam1(%d), eventParam2(%d)\n",
            ((WebEventPacket*)hdr)->m_nEventCode,
            ((WebEventPacket*)hdr)->m_wEventParam1,
            ((WebEventPacket*)hdr)->m_wEventParam2);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x75, "./log/Except",
            "CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n",
            e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
        DNF_LOG_SCOPE_LINE(0x7b, "./log/Except",
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
        // R10: 临时对象调用形态（ORIG 实测）
        PacketHeader* hdr = header;  // R10: ORIG local_24@-0x20，兼对齐 throw 槽位
        DNF_LOG_SCOPE_LINE(0x92, "./log/Web", "CPacketTranslater::OnEventEnd() eventCode(%d)\n",
            ((WebEventPacket*)hdr)->m_nEventCode);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x97, "./log/Except",
            "CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n",
            e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
        DNF_LOG_SCOPE_LINE(0x9d, "./log/Except",
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
        // R10: 临时对象调用形态（ORIG 实测）
        DNF_LOG_SCOPE_LINE(0xb5, "./log/Web",
            "CPacketTranslater::OnCommonPacket() packet_id(%d)\n", header->packetId);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCommonPacket() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0xbb, "./log/Except",
            "CPacketTranslater::OnEventEnd() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCommonPacket() Exception Break");
        DNF_LOG_SCOPE_LINE(0xc1, "./log/Except",
            "CPacketTranslater::OnEventEnd() Exception Break\n");
    }
}

void CPacketTranslater::OnInnerPacketLogin(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
        {
            DNF_LOG_SCOPE_LINE(0x1f0, "./log/Except",
                "CPacketTranslater::OnInnerPacketLogin : 0 == m_pclApp");
            return;
        }
        PacketHeader* hdr = header;
        DNF_LOG_SCOPE_LINE(0x1f6, "./log/TcpServer",
            "CPacketTranslater::OnInnerPacketLogin (sock:%d)", hdr->m_connNo);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1fa, "./log/Except",
            "CPacketTranslater::OnInnerPacketLogin Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1ff, "./log/Except",
            "CPacketTranslater::OnInnerPacketLogin Exception Break\n");
    }
}

void CPacketTranslater::OnInnerPacketLogout(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
        {
            DNF_LOG_SCOPE_LINE(0x20a, "./log/Except",
                "CPacketTranslater::OnInnerPacketLogout : 0 == m_pclApp");
            return;
        }
        // R10: ORIG local_1c@-0x18 + 链式调用 + 成员直读 + 临时日志形态
        PacketHeader* hdr = header;
        CTcpServer* server = m_pclApp->Get_ServerHandler()
            ->GetTcpServer(((TcpServerPacket*)hdr)->m_dwReversed2);
        if (!server)
        {
            DNF_LOG_SCOPE_LINE(0x215, "./log/TcpServer",
                "CPacketTranslater::OnInnerPacketLogout Invalid Server Instance(sock:%d)",
                ((TcpServerPacket*)hdr)->m_dwReversed2);
            return;
        }
        unsigned char idx = server->GetServerIndex();
        if (!m_pclApp->Get_ServerHandler()->DeleteTcpServer(idx))
        {
            DNF_LOG_SCOPE_LINE(0x21d, "./log/TcpServer",
                "CPacketTranslater::OnInnerPacketLogout DeleteTcpServer fail(sock:%d)",
                ((TcpServerPacket*)hdr)->m_dwReversed2);
            return;
        }
        DNF_LOG_SCOPE_LINE(0x221, "./log/TcpServer",
            "CPacketTranslater::OnInnerPacketLogout DeleteTcpServer Success(TYPE:%d, sock:%d)",
            idx, ((TcpServerPacket*)hdr)->m_dwReversed2);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x225, "./log/Except",
            "CPacketTranslater::OnInnerPacketLogout Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x22a, "./log/Except",
            "CPacketTranslater::OnInnerPacketLogout Exception Break\n");
    }
}

void CPacketTranslater::OnTcpServerLogin(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        // R10: ORIG local_18@-0x14 + 链式调用 + 成员直读 + 临时日志形态
        PacketHeader* hdr = header;
        if (m_pclApp->Get_ServerHandler()
                ->GetTcpServer(((TcpServerPacket*)hdr)->m_byType))
        {
            DNF_LOG_SCOPE_LINE(0x239, "./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogin Duplicate Server Instance(TYPE:%d, sock:%d)",
                ((TcpServerPacket*)hdr)->m_byType,
                ((TcpServerPacket*)hdr)->m_dwReversed2);
            return;
        }
        int created = m_pclApp->Get_ServerHandler()->CreateTcpServer(
            ((TcpServerPacket*)hdr)->m_byType,
            ((TcpServerPacket*)hdr)->m_dwReversed2);
        if (created == 0)
        {
            DNF_LOG_SCOPE_LINE(0x242, "./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogin CreateTcpServer fail(TYPE:%d, sock:%d)\n",
                ((TcpServerPacket*)hdr)->m_byType,
                ((TcpServerPacket*)hdr)->m_dwReversed2);
            return;
        }
        printf("CPacketTranslater::OnTcpServerLogin(TYPE:%d, sock:%d)\n",
            ((TcpServerPacket*)hdr)->m_byType,
            ((TcpServerPacket*)hdr)->m_dwReversed2);
        DNF_LOG_SCOPE_LINE(0x250, "./log/TcpServer",
            "CPacketTranslater::OnTcpServerLogin(TYPE:%d, sock:%d)",
            ((TcpServerPacket*)hdr)->m_byType,
            ((TcpServerPacket*)hdr)->m_dwReversed2);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x254, "./log/Except",
            "CPacketTranslater::OnTcpServerLogin Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x259, "./log/Except",
            "CPacketTranslater::OnTcpServerLogin Exception Break\n");
    }
}

void CPacketTranslater::OnTcpServerLogout(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        // R10: ORIG local_14@-0x10 + 链式调用 + 成员直读 + 临时日志形态
        PacketHeader* hdr = header;
        if (!m_pclApp->Get_ServerHandler()
                ->GetTcpServer(((TcpServerPacket*)hdr)->m_byType))
        {
            DNF_LOG_SCOPE_LINE(0x269, "./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogout Invalid Server Instance(TYPE:%d, sock:%d)",
                ((TcpServerPacket*)hdr)->m_byType,
                ((TcpServerPacket*)hdr)->m_dwReversed2);
            return;
        }
        if (!m_pclApp->Get_ServerHandler()
                ->DeleteTcpServer(((TcpServerPacket*)hdr)->m_byType))
        {
            DNF_LOG_SCOPE_LINE(0x26f, "./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogout DeleteTcpServer fail(TYPE:%d, sock:%d)",
                ((TcpServerPacket*)hdr)->m_byType,
                ((TcpServerPacket*)hdr)->m_dwReversed2);
            return;
        }
        printf("CPacketTranslater::OnTcpServerLogout(TYPE:%d, sock:%d)",
            ((TcpServerPacket*)hdr)->m_byType,
            ((TcpServerPacket*)hdr)->m_dwReversed2);
        DNF_LOG_SCOPE_LINE(0x273, "./log/TcpServer",
            "CPacketTranslater::OnTcpServerLogout(TYPE:%d, sock:%d)",
            ((TcpServerPacket*)hdr)->m_byType,
            ((TcpServerPacket*)hdr)->m_dwReversed2);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x277, "./log/Except",
            "CPacketTranslater::OnTcpServerLogout Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x27c, "./log/Except",
            "CPacketTranslater::OnTcpServerLogout Exception Break\n");
    }
}

void CPacketTranslater::OnTcpServerHeartbeat(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        // R10: ORIG local_18@-0x14 + 链式调用 + 成员直读 + 临时日志形态
        PacketHeader* hdr = header;
        CTcpServer* server = m_pclApp->Get_ServerHandler()
            ->GetTcpServer(((TcpServerPacket*)hdr)->m_byType);
        if (!server)
        {
            DNF_LOG_SCOPE_LINE(0x28d, "./log/TcpServer",
                "CPacketTranslater::OnTcpServerHeartbeat Invalid Server Instance(TYPE:%d, sock:%d)",
                ((TcpServerPacket*)hdr)->m_byType,
                ((TcpServerPacket*)hdr)->m_dwReversed2);
            return;
        }
        server->NotifyHeartbeat();
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x299, "./log/Except",
            "CPacketTranslater::OnTcpServerHeartbeat Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x29e, "./log/Except",
            "CPacketTranslater::OnTcpServerHeartbeat Exception Break\n");
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
        // R10: 临时对象调用形态（ORIG 实测）
        DNF_LOG_SCOPE_LINE(0x2ae, "./log/Web",
            "OnWebNoticeInGameAD() packet_id(%d)\n", header->packetId);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x2b2, "./log/Except",
            "CPacketTranslater::OnWebNoticeInGameAD Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x2b7, "./log/Except",
            "CPacketTranslater::OnWebNoticeInGameAD Exception Break\n");
    }
}

void CPacketTranslater::OnWebNoticeBroadcast(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnWebNoticeBroadcast : 0 == m_pclApp");
        // R10: ORIG local_2c@-0x28 + 临时日志形态（第 4 实参先求值入栈槽）
        PacketHeader* hdr = header;
        DNF_LOG_SCOPE_LINE(0x1b2, "./log/test", "%d, %s, %d, %s\n",
            ((WebNoticeBroadcastPacket*)hdr)->m_byType,
            (char*)hdr + 0xb,
            ((WebNoticeBroadcastPacket*)hdr)->m_byLen,
            (char*)hdr + 0x10b);
        // R10: ORIG local_28@-0x24（m_serverHandler 一次装载）
        if (m_pclApp)
        {
            CServerHandler* handler = m_pclApp->m_serverHandler;
            if (!handler)
                return;
            {
                // R10: ORIG string 构造形态：s(ptr) 无长度、tok 默认构造 + operator=
                std::vector<std::string> parts;
                std::string s((char*)hdr + 0xb);
                std::string tok;
                parse_string(parts, s, ',');
                for (std::vector<std::string>::iterator it = parts.begin(),
                         end = parts.end(); it != end; ++it)
                {
                    tok = *it;
                    unsigned char ch = (unsigned char)atoi(tok.c_str());
                    if (ch <= 0x64)
                    {
                        Packet_Web_Notice_Single pkt;
                        // R10: 成员访问复现 ORIG 的 mov %al,-0x168(%ebp) 直写
                        pkt.m_len = ((WebNoticeBroadcastPacket*)hdr)->m_byLen;
                        memset(pkt.m_text, 0, 0xff);
                        strncpy(pkt.m_text, (char*)hdr + 0x10b,
                            ((WebNoticeBroadcastPacket*)hdr)->m_byLen);
                        handler->SendToTcpServer((char*)&pkt, 0x10a, ch);
                    }
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1e0, "./log/Except",
            "CPacketTranslater::OnWebNoticeBroadcast() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1e5, "./log/Except",
            "CPacketTranslater::OnWebNoticeBroadcast() Exception Break\n");
    }
}

void CPacketTranslater::OnWebNoticeProhibitConnectUser(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(
                "CPacketTranslater::OnWebNoticeProhibitConnectUser : 0 == m_pclApp");
        // R10: ORIG local_2c@-0x28
        PacketHeader* hdr = header;
        Packet_Web_Prohibit_User_Connect pkt;
        memcpy(&pkt, hdr, 0x12);
        pkt.packetSize = 0x13;
        // R10: 临时日志形态（前两个实参溢出栈槽 -0x90/-0x8c）
        DNF_LOG_SCOPE_LINE(0xdb, "./log/ProhibitUser",
            "CPacketTranslater::OnWebNoticeProhibitConnectUser m_id : %d, flag( %d ), time( %d ), ip( %d ), port( %d )\n",
            ((ProhibitUserPacket*)hdr)->m_nFieldA,
            ((ProhibitUserPacket*)hdr)->m_chFieldE,
            ((ProhibitUserPacket*)hdr)->m_sFieldF,
            ((ProhibitUserPacket*)hdr)->m_dwReversed2,
            ((ProhibitUserPacket*)hdr)->m_wReversed1);
        CUserManager* um = &m_pclApp->m_userManager;
        // R10: ORIG je 目标为 Find 路径 → if(flag!=0){Delete}else{Find}
        if (((ProhibitUserPacket*)hdr)->m_chFieldE != 0)
        {
            if (um->DeleteProhibitUser(((ProhibitUserPacket*)hdr)->m_nFieldA) == 0)
            {
                DNF_LOG_SCOPE_LINE(0xe6, "./log/Web",
                    "CPacketTranslater::OnWebNoticeProhibitConnectUser Delete Err  m_id : %d, flag( %d ), time( %d ), ip( %d ), port( %d )\n",
                    ((ProhibitUserPacket*)hdr)->m_nFieldA,
                    ((ProhibitUserPacket*)hdr)->m_chFieldE,
                    ((ProhibitUserPacket*)hdr)->m_sFieldF,
                    ((ProhibitUserPacket*)hdr)->m_dwReversed2,
                    ((ProhibitUserPacket*)hdr)->m_wReversed1);
            }
            m_pclApp->m_serverHandler->SendAllTcpServer((PacketHeader*)&pkt);
        }
        else
        {
            CDNFProhibitUser* pu;
            if ((pu = um->FindProhibitUser(((ProhibitUserPacket*)hdr)->m_nFieldA)) == 0)
            {
                pu = new CDNFProhibitUser;
                pu->SetMonitorWaitTime(((ProhibitUserPacket*)hdr)->m_nFieldA, 2);
                pu->SetIpPort(((ProhibitUserPacket*)hdr)->m_dwReversed2,
                    ((ProhibitUserPacket*)hdr)->m_wReversed1);
                um->InsertProhibitUser(((ProhibitUserPacket*)hdr)->m_nFieldA, pu);
                m_pclApp->m_serverHandler->SendAllTcpServer((PacketHeader*)&pkt);
                return;
            }
            ((ProhibitUserPacket*)hdr)->m_chField11 = 2;
            DNF_LOG_SCOPE_LINE(0x113, "./log/ProhibitUser",
                "CPacketTranslater::OnWebNoticeProhibitConnectUser SendToClient, m_id : %d, ip( %d ), port( %d ), m_bIsConnect(%d), m_bProhibitConnect(%d)\n",
                ((ProhibitUserPacket*)hdr)->m_nFieldA,
                ((ProhibitUserPacket*)hdr)->m_dwReversed2,
                ((ProhibitUserPacket*)hdr)->m_wReversed1,
                ((ProhibitUserPacket*)hdr)->m_chField11,
                ((ProhibitUserPacket*)hdr)->m_chFieldE);
            if (!((CUdpHandler*)m_pclApp->Get_UdpHandler())
                    ->SendToClient((char*)hdr, 0x13,
                        ((ProhibitUserPacket*)hdr)->m_wReversed1, 0,
                        ((ProhibitUserPacket*)hdr)->m_dwReversed2))
                throw CDNFException(strerror(errno));
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x11b, "./log/Except",
            "CPacketTranslater::OnWebNoticeProhibitConnectUser() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x120, "./log/Except",
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
        // R10: ORIG local_2c@-0x28 / local_28@-0x24；key/flag/time 直接内联读取
        PacketHeader* hdr = header;
        CUserManager* um = &m_pclApp->m_userManager;
        CDNFProhibitUser* pu;
        if ((pu = um->FindProhibitUser(((ProhibitUserPacket*)hdr)->m_nFieldA)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x138, "./log/ProhibitUser",
                "CPacketTranslater::OnMonitorNoticeProhibitConnectUser Time Out, m_id : %d, flag( %d ), time( %d )\n",
                ((ProhibitUserPacket*)hdr)->m_nFieldA,
                ((ProhibitUserPacket*)hdr)->m_chFieldE,
                ((ProhibitUserPacket*)hdr)->m_sFieldF);
            return;
        }
        unsigned int uip;
        unsigned short uport;
        pu->GetIpPort(uip, uport);
        if (uip == 0 && uport == 0)
            return;
        if (((ProhibitUserPacket*)hdr)->m_chFieldE == 2)
        {
            ((ProhibitUserPacket*)hdr)->m_wPacketId = 0x4c8;
            ((ProhibitUserPacket*)hdr)->m_chField11 = 2;
            ((ProhibitUserPacket*)hdr)->m_wPacketSize = 0x12;
            DNF_LOG_SCOPE_LINE(0x14a, "./log/ProhibitUser",
                "CPacketTranslater::OnMonitorNoticeProhibitConnectUser SendToClient, m_id : %d, ip( %d ), port( %d ), m_bIsConnect(%d), m_bProhibitConnect(%d)\n",
                ((ProhibitUserPacket*)hdr)->m_nFieldA,
                uip,
                uport,
                ((ProhibitUserPacket*)hdr)->m_chField11,
                ((ProhibitUserPacket*)hdr)->m_chFieldE);
            if (!((CUdpHandler*)m_pclApp->Get_UdpHandler())
                     ->SendToClient((char*)hdr, 0x12, uport, 0, uip))
                throw CDNFException(strerror(errno));
            return;
        }
        pu->IncreMonitorRetPacket();
        pu->SetProhibitUserInfo(((ProhibitUserPacket*)hdr)->m_chField11);
        DNF_LOG_SCOPE_LINE(0x157, "./log/ProhibitUser",
            "CPacketTranslater::OnMonitorNoticeProhibitConnectUser Check IP Port, m_id : %d, server group(%d), cnt(%d), m_bIsConnect(%d)",
            ((ProhibitUserPacket*)hdr)->m_nFieldA,
            (unsigned char)((ProhibitUserPacket*)hdr)->m_chField12,
            (char)pu->GetMonitorRetPacketCnt(),
            ((ProhibitUserPacket*)hdr)->m_chField11);
        if ((char)pu->GetMonitorRetPacketCnt() >= m_pclApp->m_serverHandler->GetAlivedMonitorServer())
        {
            Packet_Web_Prohibit_User_Connect pkt;
            pkt.m_bIsConnect = pu->GetConnectFlag();
            pkt.m_time = 0;
            pkt.m_flag = 0x7f;
            DNF_LOG_SCOPE_LINE(0x165, "./log/ProhibitUser",
                "CPacketTranslater::OnMonitorNoticeProhibitConnectUser SendToClient, m_id : %d, ip( %d ), port( %d ), m_bIsConnect(%d)\n",
                ((ProhibitUserPacket*)hdr)->m_nFieldA,
                uip,
                uport,
                pkt.m_bIsConnect);
            pkt.m_id = ((ProhibitUserPacket*)hdr)->m_nFieldA;
            pkt.packetSize = 0x12;
            if (!((CUdpHandler*)m_pclApp->Get_UdpHandler())
                     ->SendToClient((char*)&pkt, 0x12, uport, 0, uip))
                throw CDNFException(strerror(errno));
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x180, "./log/Except",
            "CPacketTranslater::OnMonitorNoticeProhibitConnectUser Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x185, "./log/Except",
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
