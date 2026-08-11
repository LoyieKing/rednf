// df_coserver_r — CPacketDecoder/CInnerMsgHandler（ORIG DNFPacketDecoder.cpp）
#include <stdio.h>
#include <time.h>

#include "DNFApplication.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTranslater.h"
#include "DNFTableBase.h"
#include "DNFUdpRecvBuffer.h"
#include "DNFFileLog.h"

CInnerMsgHandler::CInnerMsgHandler() throw()
{
}

CInnerMsgHandler::~CInnerMsgHandler() throw()
{
}

CPacketDecoder* CPacketDecoderInstance()
{
    static CPacketDecoder instance;
    return &instance;
}

CPacketDecoder::CPacketDecoder()
{
    m_poolLock = 0;
    for (int i = 1000; i < 0x27fd; i++)
    {
        m_handlers[i] = 0;
    }
    m_handlers[0xbb8] = &CPacketTranslater::OnLogin;
    m_handlers[0xbb9] = &CPacketTranslater::OnLogout;
    m_handlers[0x3f9] = &CPacketTranslater::OnReplyUserInfo;
    m_handlers[0xbba] = &CPacketTranslater::OnHeartBeat;
}

CPacketDecoder::~CPacketDecoder()
{
}

void CPacketDecoder::Attach(CApplication* app)
{
    if (app != 0)
    {
        m_poolLock = app->Get_BLock();
    }
}

int CPacketDecoder::MsgDecode(PacketHeader* pkt)
{
    if (pkt == 0)
    {
        return 0;
    }
    if (*(unsigned short*)pkt < 0x27fd && 999 < *(unsigned short*)pkt)
    {
        if (m_handlers[*(unsigned short*)pkt] == 0)
        {
            DNF_LOG_SCOPE_LINE(0x44, "./log/Decoder",
                "CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.\n",
                *(unsigned short*)pkt);
            return 0;
        }
        m_handlers[*(unsigned short*)pkt](pkt);
        // ORIG 实测（0x8056e3e）：间接调用后有一条对齐 nop（else 块跳转目标
        // 落在偶数地址）；编译器布局差异无法用源码表达式复现，显式补 nop。
        __asm__ __volatile__("nop");
        return 1;
    }
    printf("Game Message with identifier %d has arrived.\n", *(unsigned short*)pkt);
    DNF_LOG_SCOPE_LINE(0x5a,"./log/Decoder",
        "CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.\n",
        *(unsigned short*)pkt);
    return 0;
}

void CPacketDecoder::Process(std::queue<CUdpRecvBuffer*>* q, CMutex* lock)
{
    if (q != 0 && lock != 0)
    {
        PacketHeader* pkt = 0;
        {
            CGuard<CMutex> g(lock);
            if (!q->empty())
            {
                pkt = (PacketHeader*)q->front();
                q->pop();
            }
        }
        if (pkt != 0)
        {
            if (MsgDecode(pkt) != 1)
            {
                {
                    CGuard<CMutex> g((CMutex*)m_poolLock);
                    CUdpRecvBuffer::operator delete(pkt);
                }
                throw CDNFException(
                    "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
            }
            {
                CGuard<CMutex> g((CMutex*)m_poolLock);
                CUdpRecvBuffer::operator delete(pkt);
            }
        }
        return;
    }
    throw CDNFException("CPacketDecoder is Not Ready!\n");
}
