// df_game_r Game/ CMailBoxHelper 实现（独立 TU，从 GameStubs.cpp 拆出）。
// CUser.h 声明 FreeMailBox 与 ReqDBSendNew* 方法。
#include "CUser.h"
#include "Inven_Item.h"

namespace WongWork
{

void CMailBoxHelper::FreeMailBox(CMailBox*)
{
}

void CMailBoxHelper::ReqDBSendNewAvatarMail(CUser*, int, int, int, char, char,
                                            const char*, const char*, int)
{
}

void CMailBoxHelper::ReqDBSendNewCreatureMail(CUser*, int, const Inven_Item&,
                                              const char*, int)
{
}

void CMailBoxHelper::ReqDBSendNewSystemMail(const char*, const Inven_Item&,
                                            unsigned int, unsigned int,
                                            const char*, int, unsigned int,
                                            ENUM_SERVER_GROUP, bool, bool)
{
}

int CMailBoxHelper::ReqDBSendNewSystemMultiMail(const char*, const Inven_Item*,
                                                int, int, unsigned int,
                                                const char*, int, int,
                                                ENUM_SERVER_GROUP, bool, bool)
{
    return 0;
}

} // namespace WongWork
