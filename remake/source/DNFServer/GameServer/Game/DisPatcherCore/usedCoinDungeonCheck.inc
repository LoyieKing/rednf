int DisPatcher_UseCoin::usedCoinDungeonCheck(CUser* user, CParty* party)
{
    if (user == 0)
    {
        return -1;
    }
    if (party == 0)
    {
        return -1;
    }
    int dungeonIndex = user->getPlayingDungeonIndex();
    if (dungeonIndex == -1)
    {
        // CParty+0xb24 内嵌 CBattle_Field。ORIG 0x81ca...：`mov 0x10(%ebp);add $0xb24;
        // mov %eax,(%esp);call` 无中间指针局部 → 表达式内联。
        dungeonIndex = reinterpret_cast<CPartyBattleFieldView*>(party)
                           ->m_battleField.get_dungeon_index();
    }
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIndex);
    if (dungeon == 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__, 0x1fa9,
                              "DisPatcher_UseCoin G_CDataManager()->find_dungeon( %d) fail", 0);
        return 0x7fffffff;
    }
    if (party->checkFreeRevivalCondition(dungeon))
    {
        return 0;
    }
    // ORIG 0x81cac0f `test %al,%al; je 远端 0x11 岛`（无 xor → 非 ! 写法）。
    // 0x11 块物理位于 result 块与 ret0 尾岛之间 = gcc else 侧布局；结构：
    if (user->getCurCharacInvenR()->IsCoinUsable())
    {
        if (party->checkUnLimitUsedCoinCondition(dungeon))
        {
            return 0;
        }
        // ORIG 块物理序：[result 块][0x11 else 岛][mov $0 fall 尾] ——
        // 非零早退 + else 显式 + 函数尾 return 0 共享 fall。
        int result = party->checkLimitUsedCoinCondition(dungeon);
        if (result != 0)
        {
            return result;
        }
    }
    else
    {
        return 0x11;
    }
    return 0;
