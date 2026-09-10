int CInventory::gain_money(int amount, eMoneyAddReason reason, bool bLog, int param)
{
    // b28d-solo-CInventory：207/207，帧 0x6c/0x5c + bool spill/trace/money 三槽位差 ⇒ N82附（聚合对象优先占低槽）封顶。
    // b28d-37：ORIG 0x84ff29c 槽序 money(-0x40) < trace(-0x38) < limit(-0x28)
    // < oldMoney(-0x24) < gained(-0x20) < i(-0x1c)。块内 `int money` 被 4.4
    // 分配到浅槽(-0x1c)且整帧 +0x10；按 P212 提到函数头并置于槽深序首位复现。
    // [N82附 封顶] 实测 money 仍落 -0x2c（4.4 聚合对象优先占低槽 + 标量分区
    // 排布，ORIG 4.1 按生命周期穿插），limit/oldMoney/gained/i 已与 ORIG 逐槽
    // 相同；帧 0x6c vs 0x5c 与 bool 截断槽 -0x4c vs -0x3c 为分区后果 ⇒ NEAR 封顶。
    // [b28d-PoolMisc2 复验] 再补「money[2] 数组化推深槽」形：4.4 反把 trace 对象
    // 挤到 -0x40、money[0] 读回多一条 mov ⇒ ndiff **16→24** 劣化，已回滚标量形。
    // [判据⑩ 实测回退] 「带初值者深」（limit 提到 money 之前）在本件**无效**
    // （-0x3c 仍 2/2、-0x28 反由 4→2）⇒ 维持原声明序，槽差记 N82附/N126 族。
    // [b28d-round5 复攻] 删 level/accName 两个具名局部（ORIG 0x084ff2bb..0x084ff2ee 直证
    // 无槽：`call get_acc_name; mov %eax,%esi` / `call get_charac_level; mov %eax,%ebx`
    // 跨 G_CDataManager 保活后直接作 GetMoneyLimitPerLevel 实参）⇒ 该段现已与 ORIG
    // 逐条一致（含 push ebx/esi 与调用序）。残余 = 整帧 0x6c vs 0x5c（我方 money/bool
    // spill 槽分区与 ORIG 不同：-0x4c vs -0x3c），N82附 族封顶。
    int money;
    int limit = 0x7fffffff;
    int oldMoney;
    unsigned int gained;
    unsigned int i;

    if (m_pParent != 0)
    {
        // [b28d-round5 复攻，ORIG 0x084ff2bb..0x084ff2ee 直证] ORIG 无 level/accName
        // 具名槽：`call get_acc_name; mov %eax,%esi` / `call get_charac_level;
        // mov %eax,%ebx`（callee-saved 跨 G_CDataManager 保活）+ `call G_CDataManager`
        // 三段后直接 GetMoneyLimitPerLevel ⇐ 与 use_money 同形（P0：无槽 ⇒ 内联实参）。
        limit = G_CDataManager()->GetMoneyLimitPerLevel(
            ((CUserCharacInfo*)m_pParent)->get_charac_level(), m_pParent->get_acc_name());
    }
    oldMoney = m_money;
    if (limit - amount < m_money)
    {
        m_money = limit;
        money = m_money;
        register int level2 = ((CUserCharacInfo*)m_pParent)->get_charac_level();
        register int characNo = m_pParent->get_charac_no(-1);
        register char* numStr = NumberToString(m_pParent->get_acc_id(), 0);
        // [b28d-PoolMisc2 复验零效] 本件把 cMyTrace 改 P227 全表达式临时链（use_money
        // 靠它 61→24）⇒ ndiff **16→16 零效**：本件三个保活值已是 `register`（level2/
        // characNo/numStr），ctor call 位置本就在其后，无块可翻；结构残差 struct=0，
        // 纯槽号。已回滚具名形。
        cMyTrace local_3c(__PRETTY_FUNCTION__, 0xef1, 4);
        local_3c("over money limit:m_id(%s),charac_no(%d),level(%d) gain_money inven(%d)+add(%d)",
                 numStr, characNo, level2, money, amount);
    }
    else
    {
        m_money += amount;
    }
    if (m_pParent != 0 && isGainedGoldFromDungeonReason(reason))
    {
        gained = m_pParent->getDungeonGainedGold()->incGainedGold(amount);
        for (i = 0; i < gained; i++)
        {
            m_pParent->getHackAnalyzer()->addServerHackCnt(
                m_pParent, (WongWork::ENUM_HACK_TYPE)0x2bf, 1u, 0u, 0u);
        }
    }
    if (bLog && m_pParent != 0)
    {
        m_pParent->m_historyLog.MoneyAdd(m_money, amount, reason);
    }
    if (m_pParent != 0)
    {
        if (reason == 4 || reason == 0x1a)
        {
            write_log_gain_money(reason, m_money - oldMoney - param, m_pParent);
        }
        else
        {
            write_log_gain_money(reason, m_money - oldMoney, m_pParent);
        }
        if (m_money - oldMoney > 0)
        {
            m_pParent->GetGoldControl()->AddGold(m_money - oldMoney, reason);
        }
    }
    return m_money - oldMoney;
