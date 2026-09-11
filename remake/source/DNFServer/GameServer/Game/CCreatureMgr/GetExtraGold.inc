// ORIG _ZNK13user_creature12CCreatureMgr12GetExtraGoldEi T 0x0833c31a：
// 无装备宠/已死亡 → 0；按宠物种族 id 给额外金币比例（*5 或 *3 再 /100）。
// 两个独立 if（非 ||）：ORIG 字节序两条独立 0 返回路径（je +0xfc / jmp +0x101）。
// [b28d-solo-CCreatureMgr AE] 既有（P0 具名 int 槽 + goto 块序）；compare_tu AE；ORIG 0x833c31a
// [b28d-solo-CCreatureMgr N112] game_func_report 口径多 1 条 4.4 对齐 nop（jmp 后
// mov$0 岛前）；compare_tu 已 AE（-falign-jumps 不可控，N112 剥离口径）。
// [2026-09-11 收口复核 · AE] check_tu_routed 口径（nop padding 剥离）判 **AE**
//   （TU 表无该件）；`game_func_report --no-compile` 报的 91 vs 92 是 .o 级重定位
//   伪差 + 一条对齐 nop（N124/N112），非语义差，勿据其改码。
int CCreatureMgr::GetExtraGold(int gold) const
{
    if (m_equippedCreature == 0)
        goto RET0;                          // ORIG je→+0xfc（default 岛）
    if (m_equippedCreature->IsDieCreature())
        return 0;
    // [b28d-23 翻正] ORIG 每个返回块为 `mov %eax,-N(%ebp); mov -N(%ebp),%eax;
    // jmp end`（具名 int 局部槽往返，P0）且 4 块各占独立槽
    // -0x18/-0x14/-0x10/-0xc（N82：4.1 不复用槽）。跳表实测（ORIG .rodata
    // 0x8c2d9f4，43 项）：块序 = {0x16,0x17,0x18}*5 → {0x2a,0x3f}*3 →
    // {0x40}*5 → {0x36,0x37,0x38}*3，故按块序书写 case 并各给具名局部。
    switch (m_equippedCreature->GetCreatureId())
    {
        case 0x16:
        case 0x17:
        case 0x18:
        {
            int extra = gold * 5 / 100;
            return extra;
        }
        case 0x2a:
        case 0x3f:
        {
            int extra = gold * 3 / 100;
            return extra;
        }
        case 0x40:
        {
            int extra = gold * 5 / 100;
            return extra;
        }
        case 0x36:
        case 0x37:
        case 0x38:
        {
            int extra = gold * 3 / 100;
            return extra;
        }
        default:
            goto RET0;
    }
RET0:
    return 0;
}
