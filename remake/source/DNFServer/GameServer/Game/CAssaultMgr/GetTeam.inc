bool CAssaultPlace::GetTeam(CUser* user, ENUM_ASSAULT_TEAM& team)
{
    // b28 BL6: N96/P0 清理 register asm + P182 强撑 pad——还原自然形；
    // P210 定义前置（移于 CAssaulter::GetUser 之前）复现 ORIG 0x28 保守帧。
    // ORIG `cmp; sete; test; je` 物化岛 = 4.1.2 O0 setcc 顶产物（P197④/N93附
    // 同族），4.4 恒 `cmp; je` 直跳，残差封顶。
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].GetUser() == user) {
            team = m_assaulters[i].GetTeam();
            return true;
        }
    }
    return false;
}