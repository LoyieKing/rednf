const char* CAssaulter::GetCharacName() const    // ORIG T 0x82e663c
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（清单 NEAR 1 → AE；路由口径复验）
    // b28d-L3: IDENTICAL_AE — b28d-L3：路由后 AE（ORIG 0x82e663c
    // ORIG 0x82e663c：je→尾岛 `mov $""`，call 分支 fall-through 在前（技巧23 双 goto 钉块序）
    if (m_user == 0)
        goto EMPTY;
    return m_user->getCurCharacName();
EMPTY:
    return "";
