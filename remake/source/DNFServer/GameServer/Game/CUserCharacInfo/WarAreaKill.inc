void CUserCharacInfo::WarAreaKill(int index, int point, float kill)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x284, 5);
        trace("WARAREA CUserCharacInfo::WarAreaKill() : m_selected is NULL");
        return;
    }
    ChangeFinishPoint(point);
    // ORIG 0x08645a3a：读 flds 0xe(%edx,%ebx,4) + 写 lea 0x380(%ecx),%edx 两次
    // 独立 lea、raw idx 留 %ecx；4.4 RA 共享单 lea（读写拆两式亦被 copy-prop），
    // N18a/4.1-RA 族天花板，维持 += 最简形。
    // [b28d-solo-CUserCharacInfo N18a/4.1-RA 封顶] 11 形实测（/tmp/s156_probe/
    // p5-p8：`+=`、展开赋值、下标加法序交换、指针算术、cast 下标、`*&` 形、union
    // 双数组视角、pack(1) 下标等）全部产 4.4 前置 `lea 0x380(%edx),%ecx`（ORIG 为
    // fadds 后 `lea 0x380(%ecx),%edx`）；4.4 对「读写共享的 PLUS_EXPR 下标」恒先
    // 求值 LHS 地址（SAVE_EXPR），无源形可翻。59/59 条数全等，语义等价。
// [b28d-solo-CUserCharacInfo 复核] 再补 9 形（/tmp/b28d_solo/wak.cpp、wak2.cpp、
// wak3.cpp、star.cpp：`+=`/展开式/下标加法序交换/指针算术/具名 float 槽/裸语句
// 表达式 `({...})`/volatile 视图/`*&` 双算/两处独立下标）全部产 LHS 地址前置
// `lea 0x380(%edx),%ecx`；P211 档③ cast 异型化（int↔unsigned）亦无 RHS-first
// ⇒ N76/N18a 封顶定论（ORIG 4.1 为 fadds 后 `lea 0x380(%ecx),%edx`）。
    ((WarAreaKillWindow*)CUR)->m_warAreaKillFlat[index + 0x380] += kill;
    ((CUser*)this)->m_historyLog.FPAdd(
        point, CUR->m_finishPoint, CUR->m_finishPointTotal,
        index == 4 ? (eFPAddReason)1 : eFPAddReason_0);
