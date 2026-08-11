// df_manager_r — CFrameCountHandler（ORIG DNFTickHandler.cpp）
#include "DNFTickHandler.h"

#include <stdio.h>
#include <string.h>
#include <sys/times.h>

#include "DNFTableBase.h"
#include "DNFFileLog.h"

CFrameCountHandler::CFrameCountHandler()
{
    m_field28 = 0;
    m_app = 0;
}

void CFrameCountHandler::SaveProcess()
{
    m_field28++;
    if (m_field28 != 0)
    {
        CMyFileLog log(__FUNCTION__, 0xa8);
        log("./log/frame", "FPS(%02d) / DFC(%02d)\n", m_field18, m_field4);
        m_field28 = 0;
    }
}

void CFrameCountHandler::SaveProcess(int n)
{
    m_field28++;
    if (m_field28 != 0)
    {
        CMyFileLog log(__FUNCTION__, 0xb8);
        log("./log/frame", "Thread(%2d) / FPS(%02d) / DFC(%02d)", n, m_field18, m_field4);
        m_field28 = 0;
    }
}

void CFrameCountHandler::InitFrameCountInfo(CApplication* app, unsigned int a, unsigned short b)
{
    if (!a)
        throw CDNFException("CFrameCountHandler::InitFrameCountInfo() Exception Break!");
    m_app = app;
    memset(this, 0, 0x28);
    m_field4 = a;
    m_field8 = 100 / a;
}

void* CFrameCountHandler::GetFrameCountInfo()
{
    struct tms tms;
    // ORIG：入口零初始化 4 个局部（-0x2c diff / -0x28 diff2 / -0x24 pad / -0x1c pad），
    // frame（-0x20）不初始化；0x63 检查前重算 m_field10 - m_fieldC。
    unsigned int diff = 0;
    unsigned int diff2 = 0;
    unsigned int pad1 = 0;
    unsigned int frame;
    unsigned int pad2 = 0;
    m_field24 = 0;
    if (!m_field0[0])
    {
        m_field0[0] = 1;
        m_field14 = 0;
        m_fieldC = times(&tms);
        if (m_fieldC == (unsigned int)-1)
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        return this;
    }
    m_field10 = times(&tms);
    if (m_field10 == -1)
        throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
    if (m_fieldC > (unsigned int)m_field10)
        m_fieldC = m_field10;
    diff = m_field10 - m_fieldC;
    frame = diff / m_field8;
    if (m_field14 < frame)
    {
        m_field14++;
        m_field24 = 1;
    }
    diff2 = m_field10 - m_fieldC;
    if (diff2 > 0x63)
    {
        m_field18 = m_field14;
        m_field24 = 2;
        m_field14 = 0;
        m_fieldC = m_field10 - diff + 0x64;
        m_field20 = 0;
        m_field25++;
        if (m_field25 > 0x3b)
        {
            m_field24 = 3;
            m_field25 = 0;
            m_field26++;
            if (m_field26 > 0x3b)
            {
                m_field24 = 4;
                m_field26 = 0;
            }
        }
    }
    return this;
}
