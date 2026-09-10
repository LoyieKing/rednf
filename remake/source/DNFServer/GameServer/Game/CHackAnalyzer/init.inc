void Secu_AccountHacking::init(CUser* user)
{
    // ORIG T 0x08278a98（init.md）：+0x10 存 user 后 resetInfo。
    m_userP = user;
    resetInfo();
