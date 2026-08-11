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
    // ORIG：if (!Send_Term_Signal(...)) throw "stoped!"；成功走 _1。
    // 此前缺 !，成功/失败消息与 ORIG 相反（二进制实测 0x81580d0 / 0x8158100）。
    if (!app->Send_Term_Signal(std::string(argv[1])))
        throw CDNFException("By CAppStopInit::Init(), this app had stoped!");
    throw CDNFException("By CAppStopInit::Init(), this app had stoped!_1");
}
