// nsl::TCPSendThread.cpp
#include <stdio.h>
#include <pthread.h>

#include "TCPSendThread.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "DataPool.h"
#include "TCPUser.h"
#include "Message.h"
#include "TraceLog.h"
#include "TMsgCell.h"
#include "PacketDesign.h"
#include "CommonDataPool.h"

namespace nsl {

TCPSendThread::TCPSendThread()
    : sendQueue()
{
    mQueueSize = 0;
    sequence = 0;
    pthread_mutex_init(&senderLock, NULL);
    pthread_cond_init(&isEmpty, NULL);
}

TCPSendThread::~TCPSendThread()
{
}

void TCPSendThread::PushSendMsg(Message* msg)
{
    G_TraceLog()->sysLog(8, "In PushSendUser");
    pthread_mutex_lock(&senderLock);
    TCPUser* u = msg->getUserFromMessage();
    if ((u->isAboutToDisconnect()) || (u->isDisconnected()))
    {
        u->SetSending(false);
        msg->initialize();
        SendDataPoolFree(msg, u);
        // Pass __int64 directly — ORIG loads both 32-bit halves into ebx/esi
        G_TraceLog()->sysLog(8, "SEND FREE msg=%d", Message::ident);
        pthread_cond_signal(&isEmpty);
        pthread_mutex_unlock(&senderLock);
        G_TraceLog()->sysLog(8, "66don't put this user");
    }
    else
    {
        u->IncPendingSendNum();
        u->SetSending(true);
        if (msg->GetWouldBlock())
        {
            u->PushWouldBlockMessage(msg);
        }
        else
        {
            u->PushSendMessage(msg);
        }
        sendQueue.push(u);
        mQueueSize = mQueueSize + 1;
        pthread_cond_signal(&isEmpty);
        pthread_mutex_unlock(&senderLock);
        G_TraceLog()->sysLog(8, "out PushSendUser");
    }
}

Message* TCPSendThread::PopSendMsg()
{
    G_TraceLog()->sysLog(8, "In PopSendUser this=%x", this);
    pthread_mutex_lock(&senderLock);
    while (sendQueue.size() == 0)
    {
        pthread_cond_wait(&isEmpty, &senderLock);
    }
    TCPUser* u = sendQueue.front();
    Message* msg = u->PopSendMessage();
    sendQueue.pop();
    mQueueSize = mQueueSize - 1;
    // ORIG: getUserFromMessage() result goes straight into DecPendingSendNum (no re-store to u)
    msg->getUserFromMessage()->DecPendingSendNum();
    pthread_mutex_unlock(&senderLock);
    G_TraceLog()->sysLog(8, "Out PopSendUser");
    return msg;
}

void TCPSendThread::SendDataPoolFree(Message* msg, TCPUser* pUser)
{
    pUser->SetSending(false);
    msg->initialize();
    // ORIG 二进制实测（0x81608d4，EUC-KR）："SEND 요청 끝------------- msg=%d"
    G_TraceLog()->sysLog(8,
                         "SEND \xbf\xe4\xc3\xbb \xb3\xa1------------- msg=%d",
                         Message::ident);
    pApp->super_DataPools.getCommonDataPool(msg->mOwnerWorkId)->destroySendMessage(msg);
}

void TCPSendThread::loop(void* temp)
{
    printf("Start up TCPSendThread-%p\n", temp);
    G_TraceLog()->sysLog(8, "Start up TCPSendThread");
    // ORIG DWARF 局部声明顺序（decl 221/222/225/227/233/240/242，fbreg 对应
    // ebp 槽 -0x20/-0x1c/-0x18/-0x80040/-0x14/-0x10/-0xc）：
    // msg, cell, encMsg, tmpbuffer, zipMsg, pUser, CompressLen。
    Message* msg;
    CMsgCell* cell;
    CMsgCell* encMsg;
    (void)encMsg;
    TMsgCell<524288> tmpbuffer;
    CMsgCell* zipMsg;
    (void)zipMsg;
    TCPUser* pUser;
    int CompressLen;
    (void)CompressLen;
    // ORIG：DWARF 有 DW_TAG_label RETRY_MSG（decl 251，low_pc=循环顶），
    // 回边/continue 全部 goto RETRY_MSG（避免 while+continue 产生 jmp+1;nop 汇合）。
RETRY_MSG:
    msg = PopSendMsg();
    pUser = msg->getUserFromMessage();
    if ((pUser->isAboutToDisconnect()) || (pUser->isDisconnected()))
    {
        SendDataPoolFree(msg, pUser);
        goto RETRY_MSG;
    }
    cell = msg->getCellFromMessage();
    // ORIG: GetPacket() result stays in edx; post-inc sequence
    cell->GetPacket()->sequence = sequence++;
    G_TraceLog()->sysLog(8, "SEND MSG who id=%d, add=%p", pUser->mUserId, pUser);
    // ORIG re-gets cell/packet for each log (no stored pPCK for ct/id)
    G_TraceLog()->sysLog(8, "SEND PCK ct    =%d", msg->getCellFromMessage()->GetPacket()->getCategory());
    G_TraceLog()->sysLog(8, "SEND PCK id    =%d", msg->getCellFromMessage()->GetPacket()->getPacketID());
    // ORIG：无 sLength 局部（DWARF 无此变量），GetPacket() 结果直接留在
    // ebx 跨 G_TraceLog() 传递；表达式内联进 sysLog 实参。
    G_TraceLog()->sysLog(8, "SEND PCK size  =%d", msg->getCellFromMessage()->GetPacket()->sLength);
    // ORIG: setne/test/jne — mask != 0 时直接跳到 do_free；写入路径放
    // else 块，使 SetSending 块自然落入 do_free（无 jmp+nop）。
    if (msg->getDataTypeMask(2) != 0)
    {
    }
    else
    {
        // ORIG: 日志为 "여기3"（EUC-KR \xbf\xa9\xb1\xe2 + '3'），无 size 实参；
        // 注意 \xe2 后接 '3' 会被解析为十六进制转义，必须拆串。
        G_TraceLog()->sysLog(8, "\xbf\xa9\xb1\xe2" "3");
        if (pUser->onWriteByCMsg(cell) == 0)
        {
            msg->setOffDataTypeMask(0);
            msg->setOnDataTypeMask(0);
            // ORIG: "Would block 이므로 다시 큐에 넣는다. size=%d"（EUC-KR）
            G_TraceLog()->sysLog(8, "Would block \xc0\xcc\xb9\xc7\xb7\xce \xb4\xd9\xbd\xc3 \xc5\xa5\xbf\xa1 \xb3\xd6\xb4\xc2\xb4\xd9. size=%d", cell->GetSize());
            msg->SetWouldBlock();
            PushSendMsg(msg);
            goto RETRY_MSG;
        }
        if (pUser->GetPendingSendNum() == 0)
        {
            pUser->SetSending(false);
        }
    }
    // ORIG：-0xc(%ebp) 即 CompressLen 槽（DWARF decl 242），do_free 处
    // 直接写 0，无额外 ret 局部（避免多占一个槽位）。
    CompressLen = 0;
    SendDataPoolFree(msg, pUser);
    goto RETRY_MSG;
}

} // namespace nsl
