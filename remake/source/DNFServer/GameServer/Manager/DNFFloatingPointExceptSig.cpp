// df_manager_r — CFloatingPointExceptSig（ORIG DNFFloatingPointExceptSig.cpp）
#include "DNFFloatingPointExceptSig.h"

#include <stdio.h>

#include "DNFApplication.h"

CFloatingPointExceptSig::CFloatingPointExceptSig() throw() {}
CFloatingPointExceptSig::~CFloatingPointExceptSig() throw() {}

void CFloatingPointExceptSig::handle(int sig)
{
    puts("Recv SIGFPE signal --> make Dump Core file.");
    if (m_app)
        m_app->App_Stop();
    dump_core_file();
}
