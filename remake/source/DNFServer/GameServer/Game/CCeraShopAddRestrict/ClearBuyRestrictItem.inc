void Manager::ClearBuyRestrictItem(CUser* user)
{
    // [b28c-4 P183] ORIG 0x080e0da4 每处从 user 重算绝对偏移（add $0x8d1e4/...），
    // 无引用缓存槽（引用形多 -0xc 槽 + 帧 0x28→0x18 差）。
    user->m_ceraUserInfo.m_accountDaily.clear();
    user->m_ceraUserInfo.m_characDaily.clear();
    user->m_ceraUserInfo.m_field30.clear();
    user->m_ceraUserInfo.m_field48.clear();
