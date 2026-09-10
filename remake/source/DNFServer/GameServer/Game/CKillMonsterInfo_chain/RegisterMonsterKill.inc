bool CKillMonsterInfo::RegisterMonsterKill(
    std::map<unsigned int, unsigned char>& kills,
    unsigned int mobIdx, unsigned char diff, bool isAPC)
{
    if (diff > 4)
        return false;

    // [b28d-solo-CKillMonsterInfo_chain] ORIG 0854d24c：`movzbl isAPC; xor $1;
    // test; je <APC 路径>` ⇒ 原码极性为 `if (!isAPC)`（find_monster 直落）。
    // 两个校验结果均落具名槽（`mov %eax,-0x10/-0xc(%ebp); cmpl $0x0,槽; jne`，
    // P0 槽往返 ⇒ 原码有具名局部）；直比调用结果会多 sete 物化。
    if (!isAPC)
    {
        void* pMon = G_CDataManager()->find_monster(static_cast<int>(mobIdx));
        if (pMon == 0)
            return false;
    }
    else
    {
        CAICharacter* pAI = G_CDataManager()->m_aiCharacterList->get(mobIdx);
        if (pAI == 0)
            return false;
    }

    // [b28d-7 P167] ORIG 无具名 flag 局部：两分支各自内联
    // GetDungeonDifficultyFlag(diff)（ORIG 2 次 vs 旧版 1 次），make_pair 第二参
    // 为 uchar 右值（0x0854d2xx 符号 `make_pair<unsigned int&, unsigned char>`），
    // 且记录分支用 `(*it).second`（ORIG 无 operator->，用 operator*）。
    std::map<unsigned int, unsigned char>::iterator it = kills.find(mobIdx);
    if (it != kills.end())
    {
        // ORIG 记录分支只有 1 次 operator->（复合赋值单次取址），
        // 旧版读+写两次 it->second 多 1 个调用点。
        it->second |= (unsigned char)GetDungeonDifficultyFlag(diff);
        alter();
    }
    else
    {
        // [b28d-solo-CKillMonsterInfo_chain] ORIG 两成功路径共享尾 `mov $1,%eax`
        // （记录分支 `call alter; jmp +0x1bd`，插入成功分支 fall-through）⇒
        // 尾部单一 return true，分支内不得各自 return。
        std::pair<std::map<unsigned int, unsigned char>::iterator, bool> ins =
            kills.insert(std::make_pair(
                mobIdx,
                static_cast<unsigned char>(GetDungeonDifficultyFlag(diff))));
        if (!ins.second)
        {
            LogManager::logFormat(
                1, "KillMonsterInfo.cpp",
                "bool CKillMonsterInfo::RegisterMonsterKill(killMonsterMap&, "
                "unsigned int, unsigned char, bool)",
                199, "KillMonsterInfo::RegisterMonsterKill Fail(%u, %u)",
                mobIdx, static_cast<unsigned int>(diff));
            return false;
        }
        alter();
    }
    return true;
