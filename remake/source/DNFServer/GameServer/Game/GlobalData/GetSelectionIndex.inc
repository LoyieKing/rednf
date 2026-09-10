bool GetSelectionIndex(CUser& user, int& outIndex, int type)
{
    const _Charac_info* cur = user.getCurCharacR();
    if (cur == 0)
    {
        return false;
    }

    // 角色数据 +0x27 short 选择索引；isAffectedPremium(0x16) 时 +5；
    // type==2 用未加成原值（ORIG 重读）。+0x27 = _Charac_info::m_level（short，
    // ORIG 0827121e movzwl 0x27(%eax); cwtl，CUser.h 字段化已落，CUserOnlyOrig 确认）。
    int selIdx = (int)cur->m_level;
    if (user.isAffectedPremium((ENUM_PREMIUM_TYPE)0x16))
    {
        selIdx += 5;
    }
    if (type == 2)
    {
        selIdx = (int)cur->m_level;
    }

    // ORIG：std::max<int>(0, selIdx)（模板调用，非 ?: cmov），再统一经
    // GetRewardSection(idx, type) 三分支分派（ORIG 不在此内联 type 分支）。
    // [b24ab] ORIG 将 max 结果解引用直接内联为 GetRewardSection 首参（无 chosen
    // 中间槽，P134/P197③形）
    EventRewardSection* section = GetRewardSection(std::max(0, selIdx), type);
    if (section == 0)
    {
        return false;
    }
    outIndex = (int)section->m_field0;                 // 段首字节
    return true;
