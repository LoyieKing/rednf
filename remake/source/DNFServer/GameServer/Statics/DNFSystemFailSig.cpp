// df_statics_r — DNFSystemFailSig（ORIG DNFSystemFailSig.o 拆分）
#include <stdio.h>
#include <stdlib.h>

#include "DNFSystemFailSig.h"
#include "DNFApplication.h"

CSystemFailSig::CSystemFailSig() throw()
{
}
void CSystemFailSig::handle(int sig)
{
    puts("Recv SIGSYS signal");
    puts("Recv SIGSYS signal --> make Dump Core file.");
    if (m_app != 0)
    {
        m_app->App_Stop();
    }
    dump_core_file();
    exit(-1);
}
CSystemFailSig::~CSystemFailSig() throw()
{
}
