// df_manager_r — CTerminateSig（ORIG DNFTerminateSig.cpp）
#include "DNFTerminateSig.h"

#include <stdio.h>

#include "DNFApplication.h"

CTerminateSig::CTerminateSig() throw() {}
CTerminateSig::~CTerminateSig() throw() {}

void CTerminateSig::handle(int sig)
{
    puts("CTerminateSig");
    if (m_app)
        m_app->App_Stop();
}
