bool CParty::_checkGoldCardSelectable(CUser* user, eClearRewardCardType_t type,
                                      int param)
{
    // ORIG 0x085b407c：guard 链 js/cmp-jle；扣款为单语句链（P52：实参
    // m_field1e4[type][param] 先物化 %ebx，跨 getCurCharacInvenW 调用存活）。
    if (user == 0)
        return false;
    if ((int)type < 0 || (int)type > 1)
        return false;
    if (param < 0 || param > 3)
        return false;
    if (m_field1e4[(int)type][param] != 0)              // +0x1e4 金卡费用表
    {
        if (const_cast<CInventory*>(
                 reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacInvenW())
// b28d-solo-CParty: AE —— P11 去 (int)(char) 双 cast + 去 (unsigned char) 索引 cast（movsbl）；158=158
                 ->use_money(m_field1e4[(int)type][param],
                             (eMoneySubReason)0x14, true))
        {
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, 0);
            return true;
        }
        return false;
    }
    return true;
