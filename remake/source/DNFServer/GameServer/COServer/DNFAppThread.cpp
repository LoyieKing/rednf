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
        if (m_queue != 0 && m_lock != 0 && m_app != 0)
        {
            m_app->Get_UserManager();
            DNFFLib::Sleep_Ext(5, 0);
            puts("AppThread Thread Start!");
            m_running = 1;
            while (m_running != 0)
            {
                CFrameCountHandler* f = m_frame.GetFrameCountInfo();
                if (f->m_state != 0 && 1 < (unsigned char)f->m_state && f->m_state == 3)
                {
                    m_frame.SaveProcess(m_saveInterval);
                }
                CPacketDecoderInstance()->Process((std::queue<CUdpRecvBuffer*>*)m_queue,
                                                   (CMutex*)m_lock);
                DNFFLib::Sleep_Ext(0, 1);
            }
            return;
        }
        throw CDNFException("AppThread is Not Ready!\n");
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
