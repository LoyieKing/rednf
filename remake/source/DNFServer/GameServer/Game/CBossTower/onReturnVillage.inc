// ----------------------------------------------------------------------------
// ORIG T 0x08144fca _ZN8WongWork10CBossTower15onReturnVillageEv：
// party 存在且 GetEPLPState()==2 时：成员名表 0xff 清零、逐有效成员记
// LeaveDungeon(dungeon, m_difficulty, names, partyType 低字节)，ReturnToVillage
// 后 _destroy。恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08144fca：尾岛/极性 + 大数组清零槽基差（N78/N82）。
// [b28d-solo-CBossTower] m_difficulty 两段已由 union 双视角 + m_difficultyE
//   翻正（`mov 0xae8(%eax),%eax; mov %eax,%ebx`）。
// [b28f-SweepCBossTower N 复核] ORIG 0x08144fde 为无条件 `jmp +0x150`
//   （字节 e9 37 01 00 00，objdump 实证；Ghidra C 亦只给 `return 1`），且
//   全二进制对 `test %eax,%eax` 紧跟 `jmp` 的形仅 5 处（onReturnVillage /
//   CCreatureItemConverter::SetEggItem / CTradeSpace::_IsTradable /
//   CParty::check_allmember_loading / ExportMapScript）⇒ 系 4.1 前端残留
//   死块（同 CTradeSpace 0x08529e54 `and $1;test;jmp` 判例），body 运行期
//   不可达；4.1.2 探针（/tmp/retprobe{,2}.cpp，空 then/嵌套 if/if-else/goto
//   五形）与 4.4 均产不出该形 ⇒ N 封顶。重建保留真实业务语义（body 可达），
//   残余 = 我们多 `je <第二条件>` + 两条 receiver 重载（ORIG 复用 eax=m_party）
//   vs ORIG 单条 `jmp`（104:103）。
// [L5Free49to60 2026-09-11 复核] fresh 103 vs 104 不变；bcmp 唯一差异 = ORIG 头部
//   无条件 `jmp`（死块前端残留）vs 我方 `je <第二条件>; mov 0x8(%ebp),%eax` 两条
//   ⇒ 维持 N（不为凑形态引入伪死块）。
int CBossTower::onReturnVillage()
{
    if (m_party != 0 && m_party->GetEPLPState() == 2)
    {
        char memberNames[255] = {0};           // ebp-0x120（ORIG 0x08144fd9 单段
                                               // rep stos 0x3f dwords）

        const char partyTypeByte =
            static_cast<char>(m_party->m_battleField.m_bloodState);   // 低字节 movsbl
        m_party->getMemberNames(memberNames);

        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = m_party->get_user(i);
            // ORIG 0x08145094..0x081450a0：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
            if (member == 0 || m_party->checkValidUser(i) != 1)
                continue;
            member->m_historyLog.LeaveDungeon(m_dungeon->GetDungeonName(),
                                              m_difficultyE, memberNames,
                                              static_cast<int>(partyTypeByte));
        }
        m_party->ReturnToVillage();
        _destroy();
    }
    return 1;
}