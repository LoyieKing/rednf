// ORIG _ZNK13user_creature12CCreatureMgr21GetOverSkillExtraGoldEi T 0x0833c6f6：
// 装备宠存活且 itemId==0xf68a 且超技能计时有效时给 *5/100，否则 0。
// [b28d-solo-CCreatureMgr AE] 既有（裸比较 + goto 钉块序）；compare_tu AE；ORIG 0x833c6f6
// [b28d-solo-CCreatureMgr N112] 同上：多 1 条 4.4 对齐 nop，compare_tu 口径已 AE。
// [2026-09-11 收口复核 · AE] check_tu_routed 口径判 **AE**（TU 表无该件）；
//   `game_func_report --no-compile` 报的 48 vs 49 = 函数尾一条 4.4 对齐 nop
//   （±0x81 处，N112 类，compare_tu 已剥离）+ .o 重定位伪差，非语义差。
int CCreatureMgr::GetOverSkillExtraGold(int gold) const
{
    // ORIG 0x833c704：四步分断——creature==0/ItemId 不符/checkTime 假三条
    // je→共享尾 mov0 岛(0x833c777)；IsDie 真值走中段 `mov0;jmp end` 物化岛
    // （技巧 40 &&-materialize 形）；0xf68a 比较为 sete;test;je（技巧 9/22，
    // bool 取反物化而非直接 jne）。
    // ORIG 0x833c704 je→尾共享 mov0(0x833c777)：goto 钉块序（技巧 23）；
    // IsDie true → 中置 `mov0;jmp end` 物化（技巧 40）；match 用 sete;test;je
    // 直判（技巧 22，`return 0` 形反产 xor+物化岛）；checkTime 正向 fall=gold。
    // [b28d-23] P0/N96 清绑：删 register asm("al")。裸比较 `f()==K` 在 4.4 即产
    // ORIG 的 `cmp $0xf68a,%eax; sete %al; test %al,%al; je`（探针 /tmp/p23_b.cpp
    // a 形，ORIG 0x833c732 段），零栈槽、无需任何绑定。
    if (m_equippedCreature == 0)
        goto RET0;
    if (m_equippedCreature->IsDieCreature())
        return 0;
    // 正向 if（技巧 19/22）：`if(!match)` 反产 sete;xor;jne 翻转形，
    // 正向落 test %al,%al; je RET0（ORIG 0x833c735）。
    if (reinterpret_cast<user_creature::CCreatureItem*>(m_equippedCreature)
            ->GetItemId() == 0xf68a)
    {
        if (m_equippedCreature->GetOverSkillTime()->checkTime())
            return gold * 5 / 100;
    }
RET0:
    return 0;
}
