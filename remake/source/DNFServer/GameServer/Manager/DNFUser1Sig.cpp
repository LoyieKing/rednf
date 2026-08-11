// df_manager_r — CUser1Sig / CUser2Sig（ORIG DNFUser1Sig.cpp）
#include "DNFUser1Sig.h"

#include <stdio.h>

#include "DNFApplication.h"
#include "DNFFileLog.h"

CUser1Sig::CUser1Sig() throw() {}
CUser1Sig::~CUser1Sig() throw() {}

void CUser1Sig::handle(int sig)
{
    CMyFileLog log(__FUNCTION__, 0x13);
    log("./log/Signal", "CUser1Sig::handle( int sig )");
    if (m_app)
        m_app->SendTestPacket_2();
}

CUser2Sig::CUser2Sig() throw() {}
CUser2Sig::~CUser2Sig() throw() {}

void CUser2Sig::handle(int sig)
{
    CMyFileLog log(__FUNCTION__, 0x20);
    log("./log/Signal", "CUser2Sig::handle( int sig )");
    if (m_app)
        m_app->TranslateSignal();
}
