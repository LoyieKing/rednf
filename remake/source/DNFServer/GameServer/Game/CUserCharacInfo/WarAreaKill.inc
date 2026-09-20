// [b28d-L14 N7/N4] ORIG 0x086459c4：残差=寄存器角色互换（%ecx↔%edx）+
// `lea 0x380(%reg)` 物化时刻（4.1 于 fadds 后 / 4.4 前置）。p167 CLEAR（语义完整）。
// [S1Charac 2026-09-11 复核] 59/59、p167 CLEAR。真残差 2 处仅寄存器角色：
// ORIG 0x08645a28 `mov 0xc(%ebp),%ecx` / 0x08645a3a `flds 0xe(%edx,%ebx,4)` /
// 0x08645a41 `lea 0x380(%ecx),%edx` / `fstps 0xe(%eax,%edx,4)`；我方同段为
// `mov 0xc(%ebp),%edx` / `lea 0x380(%edx),%ecx` / `fstps 0xe(%eax,%ecx,4)`
// ⇒ 4.1/4.4 RA 命名差（前面两轮 20 形已证无源形可翻）。维持 N82/N18a。
// [W15ANearHunt1 2026-09-11 收尾复核] routed 复编（07:15）本 TU 非 identical 仅 4 件，
//   已逐件指令级复核：本件 59/60 ndiff2（RA 角色互换，见上）；calc_disp_help_abuse_ratio
//   68/68 ndiff8（残差 = (double)uint 匿名 8B 临时与 cw/除数槽相对 ebp 整体下移 8B +
//   帧 0x68↔0x58，N95/N82附）；GetContinuousItemInfo 100/104 ndiff10（4.4 把
//   index+0x460 CSE 至 %esi 跨 call 保活并多发 push %esi，且 disp 0x10 折成
//   `lea 0x2(%esi),%edx`，N82附/N4）；其余 1 件为 static_init 口径差（勿碰）。
//   ⇒ 「disp/scale≥2 折叠」与「槽按首次使用序先浅后深」均为 4.4 定则，无源码旋钮；
//   四数 251/391/1/3/38 与复核前逐项相同（本轮零代码改动）。
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
// [P8 2026-09-11 第二遍回扫] 新钥匙复核：prett 符号 size=0x33 与 ORIG 相等、
//   串 CLEAR、p167 CLEAR；n1_v=DIFF 59/60 的唯一残差仍是 4.1/4.4 RA 命名
//   （%ecx/%edx 互换 + lea 位置）⇒ 维持封顶，未改动。
    // [PoolA 2026-09-11 试形判词] 常量在左（`0x380 + index`）实测**无效**：仍
    // 59/60 ndiff2（lea 提前、%ecx↔%edx 互换，逐值不变），已回滚 ⇒ 维持 N。
    m_selected->m_warAreaKill[index] += kill;
    ((CUser*)this)->m_historyLog.FPAdd(
        point, m_selected->m_fieldE06, m_selected->m_fieldE0A,
        index == 4 ? (eFPAddReason)1 : eFPAddReason_0);
}