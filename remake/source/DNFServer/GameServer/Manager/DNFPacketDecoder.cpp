// df_manager_r — CPacketDecoder（ORIG DNFPacketDecoder.cpp）
#include "DNFPacketDecoder.h"

#include <stdio.h>
#include <string.h>

#include "DNFApplication.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFPacketTranslater.h"
#include "Thread.h"

CPacketDecoder::CPacketDecoder()
{
    m_udpQueue = 0;
    m_udpQLock = 0;
    m_udpBLock = 0;
    void (**table)(PacketHeader*) = (void (**)(PacketHeader*))((char*)this + 0x18);
    for (int i = 0x3e8; i <= 0x27ff; i++)
        table[i] = 0;
    table[0x3ec] = CPacketTranslater::OnHeartBeat;
    table[0x44f] = CPacketTranslater::OnCommonPacket;
    table[0x450] = CPacketTranslater::OnCommonPacket;
    table[0x4c1] = CPacketTranslater::OnCommonPacket;
    table[0x4c8] = CPacketTranslater::OnWebNoticeProhibitConnectUser;
    table[0x4c9] = CPacketTranslater::OnMonitorNoticeProhibitConnectUser;
    table[0x9d3] = CPacketTranslater::OnCommonPacket;
    table[0x9df] = CPacketTranslater::OnWebNoticeBroadcast;
    table[0xa8c] = CPacketTranslater::OnCommonPacket;
    table[0xfa0] = CPacketTranslater::OnInnerPacketLogin;
    table[0xfa1] = CPacketTranslater::OnInnerPacketLogout;
    table[0x106b] = CPacketTranslater::OnTcpServerLogin;
    table[0x106c] = CPacketTranslater::OnTcpServerLogout;
    table[0x106d] = CPacketTranslater::OnTcpServerHeartbeat;
    table[0x27e2] = CPacketTranslater::OnWebNoticeInGameAD;
}

CPacketDecoder::~CPacketDecoder()
{
    m_udpQueue = 0;
    m_udpQLock = 0;
}

CPacketDecoder* CPacketDecoderInstance()
{
    static CPacketDecoder instance;
    return &instance;
}

void CPacketDecoder::Process() { UdpProcess(); TcpProcess(); }
void CPacketDecoder::SetTCPQueue(TcpRecvQueue* q) { m_tcpQueue = q; }
void CPacketDecoder::SetUdpQueue(UdpRecvQueue* q) { m_udpQueue = q; }

void CPacketDecoder::Attach(CApplication* app)
{
    if (app)
    {
        m_udpQueue = app->Get_UdpPacketParseQ();
        m_tcpQueue = app->Get_TcpNetSystem()->Get_TcpSwapQPacket()->GetParseQ();
        m_udpQLock = app->Get_UdpQLock();
        m_udpBLock = app->Get_UdpBLock();
        m_tcpRecvQLock = app->Get_TcpNetSystem()->Get_TcpRecvQLock();
        m_tcpRecvBLock = app->Get_TcpNetSystem()->Get_TcpRecvBLock();
    }
}

void CPacketDecoder::TcpProcess()
{
    if (!m_tcpQueue)
        return;
    if (!m_tcpRecvQLock)
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    while (!m_tcpQueue->empty())
    {
        CTcpRecvBuffer* buf = m_tcpQueue->front();
        m_tcpQueue->pop();
        if (!buf)
            continue;
        PacketHeader* p = (PacketHeader*)buf;
        if (m_tcpQueue->size() > 0xa)
        {
            CMyFileLog log("TcpProcess", 0xe7);
            log("./log/TcpRecv", "cnt(%)id(%d)size(%d)ip(%d)",
                (int)m_tcpQueue->size(), p->packetId, p->packetSize,
                ((char*)buf)[6]);
        }
        if (!MsgDecode(p))
        {
            {
                CGuard<CMutex> guard(m_tcpRecvBLock);
                delete buf;
            }
            printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n",
                   p, p->packetId);
            throw CDNFException("CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
        }
        {
            CGuard<CMutex> guard(m_tcpRecvBLock);
            delete buf;
        }
    }
}

void CPacketDecoder::UdpProcess()
{
    if (!m_udpQueue)
        return;
    if (!m_udpQLock)
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    while (!m_udpQueue->empty())
    {
        CUdpRecvBuffer* buf = m_udpQueue->front();
        m_udpQueue->pop();
        if (!buf)
            continue;
        PacketHeader* p = (PacketHeader*)buf;
        if (m_udpQueue->size() > 0x64)
        {
            CMyFileLog log("UdpProcess", 0x91);
            log("./log/UdpRecv", "cnt(%d)id(%d)size(%d)",
                (int)m_udpQueue->size(), p->packetId, p->packetSize);
        }
        if (!MsgDecode(p))
        {
            {
                CGuard<CMutex> guard(m_udpBLock);
                delete buf;
            }
            printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n",
                   p, p->packetId);
            throw CDNFException("CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
        }
        {
            CGuard<CMutex> guard(m_udpBLock);
            delete buf;
        }
    }
}

char CPacketDecoder::MsgDecode(PacketHeader* header)
{
    if (!header)
        return 0;
    unsigned short id = header->packetId;
    if (id > 0x27ff || id <= 0x3e7)
    {
        printf("Unknown Packet(%d)", id);
        CMyFileLog log("MsgDecode", 0x6c);
        log("./log/PacketDecode", "Unknown Packet(%d)", id);
        return 0;
    }
    void (**handler)(PacketHeader*) = (void (**)(PacketHeader*))((char*)this + 0x18 + id * 4);
    if (!*handler)
        return 0;
    (*handler)(header);
    return 1;
}
