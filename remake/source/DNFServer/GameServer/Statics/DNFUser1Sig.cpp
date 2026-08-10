// df_statics_r — DNFUser1Sig（ORIG DNFUser1Sig.o 拆分）
#include <stdio.h>

#include "DNFUser1Sig.h"
#include "DNFApplication.h"
#include "DNFFileLog.h"

CUser1Sig::CUser1Sig()
{
}
CUser2Sig::CUser2Sig()
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
void CUser2Sig::handle(int sig)
{
    DNF_LOG_SCOPE_LINE(0x20, "./log/Signal", "CUser2Sig::handle( int sig )");
    if (m_app != 0)
    {
        m_app->TranslateSignal();
    }
}
CUser2Sig::~CUser2Sig()
{
}
CUser1Sig::~CUser1Sig()
{
}
