// df_coserver_r — CUser1Sig/CUser2Sig（ORIG DNFUser1Sig.cpp）
#include <stdio.h>

#include "DNFUser1Sig.h"
#include "DNFApplication.h"
#include "DNFFileLog.h"

CUser1Sig::CUser1Sig() throw()
{
}

CUser1Sig::~CUser1Sig() throw()
{
}

void CUser1Sig::handle(int sig)
{
    DNF_LOG_SCOPE_LINE(0x13, "./log/Signal", "CUser1Sig::handle( int sig )");
    if (m_app != 0)
    {
        m_app->SendTestPacket_2();
    }
}

CUser2Sig::CUser2Sig() throw()
{
}

CUser2Sig::~CUser2Sig() throw()
{
}

void CUser2Sig::handle(int sig)
{
    DNF_LOG_SCOPE_LINE(0x20, "./log/Signal", "CUser2Sig::handle( int sig )");
    if (m_app != 0)
    {
        m_app->TranslateSignal();
    }
}
