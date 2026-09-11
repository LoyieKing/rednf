// ----------------------------------------------------------------------------
// ORIG T 0x081b44c0：user 空→0；FindIndexByCharacNo + CheckCharacIndex 收窄
// 校验；有效则 m_characs[idx].Reward(user, reward) 直传返回值（无规范化）。
// [b28d-27] 返回类型 int→bool（P9）：ORIG 尾 `call Reward; leave; ret` 直传单字节，
// 无 movzbl 规范化；CUser.h:791 已由 Main 代落（mangled 不含返回类型，ABI 零影响）。
// ----------------------------------------------------------------------------
bool UserMercenaryInfoMgr::ReturnCharac(CUser* user, unsigned int characNo,
                                        competition_reward_result& reward)
{
    if (user == 0)
        return 0;
    const int index = (int)FindIndexByCharacNo(characNo);  // b28 P211 同族
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return 0;
    return m_characs[index].Reward(user, reward);
}