// df_manager_r — CSegmentationFaultSig（ORIG DNFSegmentationFaultSig.cpp）
#include "DNFSegmentationFaultSig.h"

#include <stdio.h>

#include "DNFApplication.h"

CSegmentationFaultSig::CSegmentationFaultSig() throw() {}
CSegmentationFaultSig::~CSegmentationFaultSig() throw() {}

void CSegmentationFaultSig::handle(int sig)
{
    puts("Recv SIGSEGV signal --> make Dump Core file.");
    if (m_app)
        m_app->App_Stop();
    dump_core_file();
}
