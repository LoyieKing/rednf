// df_manager_r — CSystemFailSig（ORIG DNFSystemFailSig.cpp）
#include "DNFSystemFailSig.h"

#include <stdio.h>
#include <stdlib.h>

#include "DNFApplication.h"

CSystemFailSig::CSystemFailSig() throw() {}
CSystemFailSig::~CSystemFailSig() throw() {}

void CSystemFailSig::handle(int sig)
{
    puts("Recv SIGSYS signal");
    puts("Recv SIGSYS signal --> make Dump Core file.");
    if (m_app)
        m_app->App_Stop();
    dump_core_file();
    exit(-1);
}
