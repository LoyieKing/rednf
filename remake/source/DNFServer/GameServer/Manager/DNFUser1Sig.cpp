// df_manager_r — CUser1Sig / CUser2Sig（ORIG DNFUser1Sig.cpp）
#include "DNFUser1Sig.h"

#include <stdio.h>

#include "DNFApplication.h"
#include "DNFFileLog.h"

CUser1Sig::CUser1Sig() throw() {}
CUser1Sig::~CUser1Sig() throw() {}

void CUser1Sig::handle(int sig)
{
    CMyFileLog log("CUser1Sig", 0x13);
    log("USER1", "SIGUSR1");
    if (m_app)
        m_app->SendTestPacket_2();
}

CUser2Sig::CUser2Sig() throw() {}
CUser2Sig::~CUser2Sig() throw() {}

void CUser2Sig::handle(int sig)
{
    CMyFileLog log("CUser2Sig", 0x20);
    log("USER2", "SIGUSR2");
    if (m_app)
        m_app->TranslateSignal();
}
