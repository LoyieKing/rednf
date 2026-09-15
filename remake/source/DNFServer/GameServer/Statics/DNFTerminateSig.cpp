// df_statics_r — DNFTerminateSig（ORIG DNFTerminateSig.o 拆分）
#include <stdio.h>
#include <iostream>

#include "DNFTerminateSig.h"
#include "DNFApplication.h"

void CTerminateSig::handle(int sig)
{
    puts("Recv SIGTERM signal");
    if (m_app != 0)
    {
        m_app->App_Stop();
    }
}
CTerminateSig::CTerminateSig() throw()
{
}
CTerminateSig::~CTerminateSig() throw()
{
}
