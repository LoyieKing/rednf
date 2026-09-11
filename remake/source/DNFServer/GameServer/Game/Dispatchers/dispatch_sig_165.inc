// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08222270 orig/ours=74/83 ndiff-blocks=9
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN36Dispatcher_BloodRoundUiPrepareFinish12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN36Dispatcher_Blood
int Dispatcher_BloodRoundUiPrepareFinish::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8222270：战斗态+血轮地下城且非（bloodType<1 或物品等级==1）→ 血轮生成计时器
    if (user == 0)
    {
        return LineFunc(0xdbbf,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xdbc0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() != 5)
    {
        return LineFunc(0xdbc2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    // [L7DispG4 2026-09-11 · P59/P222 极性包裹] ORIG 0x0822232e/0x08222340 两条守卫均为
    //   `je <0x0822239f 单尾出口>`（cmp/je 跳**共享尾 return 0**，非就地 `mov $0,%eax`）
    //   ⇒ 源码形 = `if (party) { if (pDungeon) { … } }` + 尾单 `return 0`。
    // [L7DispG4 2026-09-11 · 语义轴 + P331 槽律] ORIG 0x08222334 直证：
    //   `mov -0x10(%ebp),%eax; mov 0xcac(%eax),%eax; mov %eax,-0xc(%ebp)`
    //   + `cmpl $0x0,-0xc(%ebp); je <return 0>` ⇒ **pDungeon 是具名指针槽 -0xc**，
    //   且 ORIG 的字段读走槽回读（`mov -0xc(%ebp),%eax; movzbl 0x89f(%eax)`）。
    if (party != 0)
    {
    CDungeon* pDungeon = party->m_battleField.m_pDungeon;
    if (pDungeon != 0)
    {
    // [L7DispG4 2026-09-11 · 语义轴 + 类型轴] ORIG 0x08222346/0x08222352：
    //   `movzbl 0x89f(%eax),%eax; test %al,%al; jle <skip>` ⇒ **有符号**比较
    //   `(signed char)type > 0`（我方原 unsigned `< 1` 产 `je`，且把 mi==0 归入
    //   timer 支 —— 与 ORIG 相反）。ORIG 0x08222364/0x0822236d：GetMapInfo 结果
    //   **直用 %eax**（无 mi 槽、**无 mi 判空**，`movzbl 0x1(%eax),%eax; xor $1;
    //   test %al,%al; je <skip>`）⇒ 删发明防御、去具名 mi。
    //   ORIG 0x0822236f..0x0822237d：`mov $1,%eax / mov $0,%eax / test %al,%al;
    //   je <return 0>` = 4.1 对 `&&` 的 bool 物化岛 ⇒ 条件写成 `A && B`。
    if ((signed char)pDungeon->m_bloodDungeonType > 0 &&
        !reinterpret_cast<CBattle_Field*>(&party->m_battleField)->GetMapInfo()->m_visited)
    {
        reinterpret_cast<CBattle_Field*>(&party->m_battleField)
            ->BloodSpawnTimerManager(false, false);
    }
    }
    }
    return 0;
}