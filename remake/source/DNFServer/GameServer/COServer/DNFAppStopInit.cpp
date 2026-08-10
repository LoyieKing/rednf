// df_coserver_r — CAppStopInit（ORIG DNFAppStopInit.cpp）
#include <stdio.h>

#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFTableBase.h"

CAppStopInit::CAppStopInit()
{
}

CAppStopInit::~CAppStopInit()
{
}

void CAppStopInit::Init(CApplication* app, int argc, char** argv)
{
    puts("RECV STOP, \xb0\xfc\xb8\xae\xc0\xda\xbf\xa1 \xc0\xc7\xc7\xd8 "
         "\xb0\xad\xc1\xa6\xb7\xce \xc1\xbe\xb7\xe1 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
    app->Clear();
    if (app->Send_Term_Signal(argv[1]) != 1)
    {
        throw CDNFException("CAppStopInit::Init()\xbf\xa1 \xc0\xc7\xc7\xd8 "
                            "\xb0\xad\xc1\xa6\xb7\xce \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xc0\xbd!");
    }
    throw CDNFException("CAppStopInit::Init()\xbf\xa1 \xc0\xc7\xc7\xd8 "
                        "\xb0\xad\xc1\xa6\xb7\xce \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xc0\xbd!_1");
}
