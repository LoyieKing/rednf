// df_manager_r — CAppStopInit（ORIG DNFAppStopInit.cpp）
#include "DNFAppStopInit.h"

#include <stdio.h>

#include "DNFApplication.h"
#include "DNFTableBase.h"

CAppStopInit::CAppStopInit() {}
CAppStopInit::~CAppStopInit() {}

void CAppStopInit::Init(CApplication* app, int argc, char** argv)
{
    puts("RECV STOP, Manager had stoped this program.");
    app->Clear();
    if (app->Send_Term_Signal(std::string(argv[1])))
        throw CDNFException("By CAppStopInit::Init(), this app had stoped!");
    throw CDNFException("By CAppStopInit::Init(), this app had stoped!_1");
}
