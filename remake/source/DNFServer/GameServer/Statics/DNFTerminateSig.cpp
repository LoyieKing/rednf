// df_statics_r — DNFTerminateSig（ORIG DNFTerminateSig.o 拆分）
#include <stdio.h>

#include "DNFTerminateSig.h"
#include "DNFApplication.h"

CTerminateSig::CTerminateSig()
{
}
void CTerminateSig::handle(int sig)
{
    puts("Recv SIGTERM signal");
    if (m_app != 0)
    {
        m_app->App_Stop();
    }
}
CTerminateSig::~CTerminateSig()
{
}
