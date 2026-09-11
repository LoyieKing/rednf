// ----------------------------------------------------------------------------
// ORIG T 0x08142b72 _ZN8WongWork10CBossTower17handleEPLPCommandEi：
// bool 门（party 非空且 GetEPLPState()==2 才放行）；cmd(unsigned)<=2 时成员名表
// 清零、逐有效成员记 LeaveDungeon(level 固定 0) 并 ReturnToVillage；随后
// _destroy，返回 1；门未放行返回 0。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08142b72：bool 物化块 + 帧 +0x10（P197④/N82）。
// [b28d-solo-CBossTower] P226 否定式语句表达式
//   `if (({ (m_party==0) || (GetEPLPState()!=2); })) return 0;` 一次翻正物化岛/
//   极性/尾块。memberNames 改 `{0}` 复现单段 rep stos。
// [b28f-SweepCBossTower] AE（105:105，偏移全对齐）：范围检查改连续 case
//   switch（P286/P320）复现 ORIG 0x08142bb3 `mov 0xc(%ebp),%eax; add
//   $0x0,%eax; cmp $2,%eax; ja <_destroy>` 的「形参载入 + 寄存器比较 + ja
//   落点」；残余仅 4.1 特有 int→unsigned no-op `add $0x0,%eax`（4.4 恒消除，
//   P41 反向），由 clean 口径归一。
int CBossTower::handleEPLPCommand(int cmd)
{
    // ORIG 0x08142b7c..0x08142bb3：skip = (m_party==0) || (GetEPLPState()!=2)
    // 物化岛 mov$1/jmp/mov$0 + test %al,%al;je 无槽 ⇒ P226 否定式语句表达式。
    if (({ (m_party == 0) || (m_party->GetEPLPState() != 2); }))
        return 0;

    {
        // ORIG 0x08142bb3 `mov 0xc(%ebp),%eax; add $0x0,%eax; cmp $2,%eax; ja`
        // （int→unsigned 转换的 no-op add 为 4.1 特有；4.4 消除该条，P41 反向）
        // ⇒ 用连续 case switch 复现「形参载入 + 寄存器比较 + ja 落 _destroy」
        // （P286/P320），残余仅 1 条 no-op add。
        switch (cmd)
        {
        case 0:
        case 1:
        case 2:
            {
            char memberNames[255] = {0};          // ebp-0x110（ORIG 单段 rep stos）

            const char partyTypeByte =
                static_cast<char>(m_party->m_battleField.m_bloodState);   // 低字节 movsbl
            m_party->getMemberNames(memberNames);

            for (int i = 0; i <= 3; ++i)
            {
                CUser* member = m_party->get_user(i);
                if (member == 0 || m_party->checkValidUser(i) != 1)
                    continue;
                member->m_historyLog.LeaveDungeon(
                    m_dungeon->GetDungeonName(), 0, memberNames,
                    static_cast<int>(partyTypeByte));
            }
            m_party->ReturnToVillage();
            }
            break;
        default:
            break;
        }

        _destroy();
        return 1;
    }
    return 0;
}