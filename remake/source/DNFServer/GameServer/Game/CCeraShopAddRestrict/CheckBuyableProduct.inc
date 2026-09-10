int Manager::CheckBuyableProduct(CUser* user, unsigned int ipgNo,
                                 unsigned int itemIdx, int type)
{
    UserInfo& ui = user->m_ceraUserInfo;
    if (type == 0) {
        ui.m_field30.clear();
        ui.m_field48.clear();
    }
    InfoDaily* info = m_loader.GetRestrictInfo(ipgNo);
    if (info == 0)
        return 1;
    if (info->m_end < (int)time(0))
        return 0;
    // [b28c-4 P46/P208] ORIG 0x080e0bee：m_type 单次装载 + `test;je / cmp $1;je /
    // jmp 零块` 决策链，body 全部后置 ⇒ switch 形（if 链被 4.4 就地展开成反向
    // `jne skip` 且二次装载 m_type）。
    switch (info->m_type) {
    case 0:
        return FindUserInfoDetail(ui.m_field30, ui.m_accountDaily,
                                  ipgNo, info, itemIdx);
    case 1:
        return FindUserInfoDetail(ui.m_field48, ui.m_characDaily,
                                  ipgNo, info, itemIdx);
    default:
        return 0;
    }
