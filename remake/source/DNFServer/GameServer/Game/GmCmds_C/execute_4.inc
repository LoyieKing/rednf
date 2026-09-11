// ORIG execute 0x084b2152：遍历全地下城索引表，为非特殊地下城写入通关难度记录。
void DungeonDifficulty::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    // ORIG 0x084b2172/0x084b2181：m_difficulty 每次直读成员（无具名局部，P0）；
    // 原实现多一个 unsigned char 具名局部（-0x19 槽 + store/load 往返）。
    if (m_difficulty == 0 || m_difficulty > 3)
    {
        return;
    }

    // ORIG 0x084b2190 起：list 为 const 指针（begin/end 选 const 重载
    // _ZNK…hash_map…5beginEv/3endEv），且 end 只在循环外求值一次（-0x2c 槽）。
    // [round5-GmCmds_C 判词·N-P75 封顶] 全件唯一残差=ORIG 0x084b21a0/0x084b21b5 的
    // `lea -0x24/-0x2c(%ebp),%eax`（sret 槽地址）先于 0x084b21a3/0x084b21b8 的
    // `mov -0x10(%ebp),%edx`（this）：4.1 sret-first vs 4.4 this-first（实参装载调度）。
    // 三形实测均不可翻：①const 指针形（基线）逐条如上；②引用形（P256「先取整体引用
    // 再点首成员」）逐条相同；③`const CDungeonList* volatile`（P316 左先求值序）反致
    // list 槽 -0x10→-0x1c 且多 1 条 `mov %eax,%edx`（117→118 条）⇒ 回滚。
    // 剩余 2 处为纯寄存器名/次序差（N7/N-P75），语义与指令数 117=117 全等。
    const CDungeonList* list = G_CDataManager()->getDungeonIndexList();
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator it =
        list->m_dungeonMap.begin();
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator end =
        list->m_dungeonMap.end();
    // ORIG 增量段调 ppEi（后置）⇒ 源形写 it++（P28）。
    for (; it != end; it++)
    {
        CDungeon* dungeon = it->second;
        if (dungeon == 0)
        {
            continue;
        }
        // 血色地狱(1/2)、绝望之塔、锦标赛、kind==1 的地下城不写入。
        // ORIG 0x084b21f5..0x084b2244：OR 短路链物化 %al（mov$1/mov$0 双岛 +
        // test; jne 跳过 body，无具名 bool 槽）⇒ 4.4 复现形是正极性 AND 包裹
        // （P214①：负极性短路恒 je;jne 反演）。
        if (({ dungeon->get_blood_dungeon_type() == 1 ||
               dungeon->get_blood_dungeon_type() == 2 ||
               dungeon->isTowerOfDespairDungeon() ||
               dungeon->isTournamentDungeon() ||
               dungeon->getDungeonKind() == 1; }))
        {
            continue;
        }
        // ORIG 0x084b2246 起：getDungeonClearRefW 结果为 receiver 直传
        // （无具名 clear 局部）；实参 R2L（difficulty → get_index）。
        user->getDungeonClearRefW()->addClearedDungeon(
            static_cast<short>(dungeon->get_index()),
            static_cast<char>(m_difficulty));
    }
}