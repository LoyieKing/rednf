// df_statics_r — DNFSegmentationFaultSig（ORIG DNFSegmentationFaultSig.o 拆分）
#include <stdio.h>

#include "DNFSegmentationFaultSig.h"
#include "DNFApplication.h"

void CSegmentationFaultSig::handle(int sig)
{
    puts("Recv SIGSEGV signal --> make Dump Core file.");
    if (m_app != 0)
    {
        m_app->App_Stop();
    }
    dump_core_file();
}
CSegmentationFaultSig::~CSegmentationFaultSig() throw()
{
}
CSegmentationFaultSig::CSegmentationFaultSig() throw()
{
}
