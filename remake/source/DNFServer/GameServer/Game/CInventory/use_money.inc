bool CInventory::use_money(int amount, eMoneySubReason reason, bool bLog)
{
    // b28d-solo-CInventory：163/166，N82 槽号错位封顶（P0 具名局部已还原）。
    // ORIG 0x084ff54c 槽序 -0x40(具名 money 缓存) / -0x3c(bool spill) / -0x30(trace)
    // / -0x20(oldMoney) / -0x1c(limit)；4.4 分配为 -0xc/-0x10 族 ⇒ 全帧槽号差
    // [b28d-PoolMisc2 修成 ndiff 61→8（行口径）/ struct 15→0，件 DIFF→NEAR]
    // 两处源形：①P227 cMyTrace 改全表达式临时（ORIG trace ctor 在三值保活拷贝之后）；
    // ②P212 `int overMoney;` 提到**函数头第一个声明**（4.4 同 size 组按声明序由深到浅）。
    // 结构残差归零后余 8 行全为槽号 ⇒ structdiff 硬判 N82 封顶。
    // （N82：4.1 不复用槽/聚合优先低槽）。具名局部 overMoney 已按 P0 还原（store+load
    // 两处），语义闭合；残余仅槽号与 cMyTrace 变参装载序，判 N82 封顶。
    // [b28d-PoolMisc2 P212 试验] ORIG overMoney 落**最深**槽 -0x40（在 bool spill
    // -0x3c 之下）⇒ 按 4.4「先声明=更深」把声明提到函数头，块内仅赋值。
    // [b28d-PoolMisc2 修成] 声明必须放**函数头第一位**（4.4 同 size 组按声明序由
    // 深到浅）⇒ 单此一改 ndiff 61→24→**8**。撑 overMoney[4]（16B）再抢深度实测反劣
    // （帧 0x5c→0x6c、多 `mov -0x40; mov -0x50` 拷贝，ndiff 8→14）⇒ 回滚标量形。
    // [K8CInventory 2026-09-11 · 试形回退] ORIG overMoney 落 -0x40（紧邻 bool 溢出槽
    // -0x3c 之下，直证 0x084ff5f4 `mov %eax,-0x40(%ebp)`），曾疑其属最深块内标量组
    // ⇒ 实测把 `int overMoney` 移入 `if (limit < m_money)` 块内（块内声明）**劣化**：
    // 槽差从 4 条（-0x40↔-0x24、-0x30↔-0x34）涨到 12 条（(4,)×10 + (-36,)×2），
    // 4.4 把块内标量放到浅区而非 ORIG 的深区 ⇒ 维持函数头首位声明（P212 已落地形）。
    int overMoney;
    int oldMoney;
    int limit;

    if (m_money < amount || amount < 0)
    {
        return 0;
    }
    oldMoney = m_money;
    m_money -= amount;
    limit = 0x7fffffff;
    if (m_pParent != 0)
    {
        limit = G_CDataManager()->GetMoneyLimitPerLevel(
            ((CUserCharacInfo*)m_pParent)->get_charac_level(), m_pParent->get_acc_name());
    }
    if (limit < m_money)
    {
        m_money = limit;
        // b28d-solo-CInventory [P0]：ORIG `mov 0x4(%eax),%eax; mov %eax,-0x40(%ebp)` 与
        // 日志处 `mov -0x40(%ebp),%eax` 两次槽往返 ⇒ 原码有具名局部缓存（非成员直读）。
        // [b28d-PoolMisc2 P227 试验] ORIG 0x084ff54c：`mov %eax,-0x40(%ebp)` 之后是
        // 三条 callee-saved 保活拷贝（`mov %eax,%edi/%esi/%ebx` = NumberToString /
        // get_charac_no / get_charac_level 返回值），**cMyTrace ctor call 在其后**
        // ⇒ 原码 cMyTrace 是全表达式临时（P227），非具名局部（具名形 4.4 把 ctor
        // 提前到 vararg 求值之前，产 trace 块错位 6 行）。
        overMoney = m_money;
        cMyTrace(__PRETTY_FUNCTION__, 0xf30, 4)(
            "over money limit:m_id(%s),charac_no(%d),level(%d) use_money inven(%d)-sub(%d)",
            NumberToString(m_pParent->get_acc_id(), 0), m_pParent->get_charac_no(-1),
            ((CUserCharacInfo*)m_pParent)->get_charac_level(), overMoney, amount);
    }
    if (m_money < 0)
    {
        m_money = 0;
    }
    if (m_pParent != 0)
    {
        write_log_use_money(reason, oldMoney - m_money, m_pParent);
        if (oldMoney - m_money > 0)
        {
            m_pParent->GetGoldControl()->SubGold(oldMoney - m_money, reason);
        }
    }
    if (bLog && m_pParent != 0 && amount > 0)
    {
        m_pParent->m_historyLog.MoneySub(m_money, amount, reason);
    }
    return 1;
}