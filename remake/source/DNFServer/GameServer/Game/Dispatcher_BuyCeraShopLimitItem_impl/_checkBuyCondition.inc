// _checkBuyCondition —— ORIG 0x0821f7cc：逐条件校验职业/等级/建号时间/账号注册时间区间，
//   任一不符返回 7，全过返回 0。
int Dispatcher_BuyCeraShopLimitItem::_checkBuyCondition(CUser* user, const CItemLimitEdition* edition) const
{
    // 职业条件（ORIG：getCondCharacJob 两次独立调用——P48 无 CSE；<0xb 走 setle 物化，
    // 内层 get_charac_job 先入 ebx 再与第二次 getCondCharacJob 比较）。
    if (edition->getCondCharacJob() < 0xb) {
        if (user->get_charac_job() != edition->getCondCharacJob())
            return 7;
    }

    // 等级区间（ORIG：begin/end 各两次调用；getter 结果入 ebx 跨另一 getter 调用存活，
    // 比较在 int 域带符号，故方向按 ORIG 汇编右操作数先入 ebx 的求值序书写）。
    if (edition->getCondCharacLevelBegin() != 0
        && edition->getCondCharacLevelBegin() > user->get_charac_level())
        return 7;

    if (edition->getCondCharacLevelEnd() != 0
        && edition->getCondCharacLevelEnd() < user->get_charac_level())
        return 7;

    // 建号时间区间。
    if (edition->getCondCharacCreateTimeBegin() != 0
        && edition->getCondCharacCreateTimeBegin() > user->getCurCharacCreateTime())
        return 7;

    if (edition->getCondCharacCreateTimeEnd() != 0
        && edition->getCondCharacCreateTimeEnd() < user->getCurCharacCreateTime())
        return 7;

    // 账号注册时间区间。
    if (edition->getCondAccCreateTimeBegin() != 0
        && edition->getCondAccCreateTimeBegin() > user->getRegTime())
        return 7;

    if (edition->getCondAccCreateTimeEnd() != 0
        && edition->getCondAccCreateTimeEnd() < user->getRegTime())
        return 7;

    return 0;
}