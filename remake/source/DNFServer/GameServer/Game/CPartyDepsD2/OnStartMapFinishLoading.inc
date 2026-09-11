// ---------------------------------------------------------------------------
// OnStartMapFinishLoading —— ORIG T 0x085b170a
// 报告：docs/class_func_reports/CParty/OnStartMapFinishLoading.md
// 有副本且 do_after_dungeon_start 成功处理 → giveup_game 并返回 false；
// 否则回发 0x28 cmd ok 包并返回 true。
// ---------------------------------------------------------------------------
bool CParty::OnStartMapFinishLoading(CUser* user)
{
    if (bf(this).GetDungeon() != 0)                      // thunk 0x080fdcfc ×2
    {
        // ORIG 实证双取形（无 dungeon 局部缓存），user 直传实参无 spill。
        if (do_after_dungeon_start(bf(this).GetDungeon(), user) != 0)
        {   // 入场善后失败：按放弃处理并返回 0
            giveup_game(user, false, false, false);      // B2 批次实现
            return false;
        }
    }
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);         // T 0x0867bea0
    return true;
}