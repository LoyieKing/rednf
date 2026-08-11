// df_coserver_r — CAppThread（ORIG DNFAppThread.cpp）
#include <stdio.h>
#include <queue>
#include <string.h>
#include <sys/times.h>

#include "DNFAppThread.h"
#include "DNFApplication.h"
#include "DNFPacketDecoder.h"
#include "DNFTableBase.h"
#include "DNFUdpRecvBuffer.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CAppThread::CAppThread()
    : m_app(0)
{
    m_queue = 0;
    m_lock = 0;
    m_reserved = 0;
}

CAppThread::~CAppThread()
{
    m_queue = 0;
    m_lock = 0;
    m_reserved = 0;
    m_app = 0;
}

void CAppThread::dispatch(void* param)
{
    try
    {
        m_frame.InitFrameCountInfo(m_app, 0x22, 1000);
        // ORIG 实测：OR 条件 + 内联 throw（test/je ×2 + jne 到就绪块）形态；
        // AND 条件 + 尾部 throw 会生成 je ×3 前跳（仅差块布局）。
        if (m_queue == 0 || m_lock == 0 || m_app == 0)
        {
            throw CDNFException("AppThread is Not Ready!\n");
        }
        // ORIG 实测：Get_UserManager 返回值存入死局部（-0x24(%ebp)）——保留以对齐帧布局
        CUserManager* um = m_app->Get_UserManager();
        (void)um;
        DNFFLib::Sleep_Ext(5, 0);
        puts("AppThread Thread Start!");
        m_running = 1;
        while (m_running)
        {
            CFrameCountHandler* f = m_frame.GetFrameCountInfo();
            if (f->m_state != 0 && 1 < (unsigned char)f->m_state && f->m_state == 3)
            {
                m_frame.SaveProcess(m_saveInterval);
            }
            CPacketDecoderInstance()->Process(m_queue, m_lock);
            DNFFLib::Sleep_Ext(0, 1);
        }
        return;
    }
    catch (CDNFException& e)
    {
        printf("CApplication::Process() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x63, "./log/process", "CAppThread::dispatch() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CApplication::Process() Exception Break");
        DNF_LOG_SCOPE_LINE(0x68, "./log/process", "CAppThread::dispatch() Exception Break\n");
    }
}

void CAppThread::attach(CApplication* app, int idx)
{
    if (app != 0)
    {
        m_app = app;
        m_queue = app->Get_QPacket() + idx;
        m_lock = app->Get_QLock() + idx;
        m_saveInterval = idx;
    }
}
