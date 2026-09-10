void Manager::LoadRequestBuyRestrictItem(CUser* user)
{
    // DB 请求经 DB_CerashopAddRestrict.cpp 的真实成员函数投递（ORIG 0x080e1ff6）。
    DB_LoadCerashopAddRestrictType::makeRequest(user);
