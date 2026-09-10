bool CBossTower::generateRewardItem()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // b28d-54 翻正：同 generateRewardExp（ORIG 直测形，无 bool 槽）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        // [b28d-solo-CBossTower] P331：ORIG 0x08144e91 `movl $0x0,-0x40` 在
        //   GetTowerScript **之前**、itemCount 占深槽 -0x40、scriptLimit -0x3c
        //   ⇒ 同尺寸标量区 4.4 按「先声明→更深槽」排布 ⇒ itemCount 声明前置。
        // 4.4 按「先声明浅」分区（P309）⇒ 槽号对齐需 scriptLimit 声明在前、
        //   itemCount 在后；而 ORIG 的预清 `movl $0,-0x40` 在 GetTowerScript 之前
        //   ⇒ itemCount 用「裸声明分离」不可行（声明序即槽序），改 scriptLimit
        //   裸声明前置、赋值后置（P198③ 两段式的镜像）。
        int scriptLimit;                  // ebp-0x3c（4.4 先声明浅）
        int itemCount = 0;   // ORIG 0x08144e91 循环头预清（P198③ 两段式）

        stDeathTower_t* towerScript = G_CDataManager()->GetTowerScript(
            static_cast<unsigned int>(m_dungeon->get_index()));
        // ORIG 0x08144e36 +0x09d..+0x0a3：`add $0xc8,%edx`（0xc8*4=0x320）后
        // `mov 0x4(%eax,%edx,4),%eax` ⇒ 绝对槽 = towerScript + 4 + 0x320 + 4*stage
        // = stDeathTower_t::m_rewardCardNum[stage]（该数组 +0x324，int 读宽一致）。
        scriptLimit = towerScript->m_rewardCardNum[m_stageCur];

        itemCount = std::min(m_dungeon->getTowerMaxClearRewardItemNum(),
                             scriptLimit);
        itemCount = std::min(10, itemCount);

        // P0/P183：stageParam 无槽往返（ORIG `mov (%eax),%eax; mov %eax,%esi`）
        // ⇒ 内联实参，勿具名 const 局部。
        // ORIG 0x08144fb8 `call; mov %al,-0x1d; movzbl; xor $1; test; jmp`
        // （Ghidra C local_1d = 返回值 bool 局部，结果未用）⇒ 接收返回值。
        // ORIG 0x08144fc9 `mov 0xae8(%eax),%ebx` 单段 ⇒ 实参走 enum 视角
        // m_difficultyE（与形参 ENUM_DUNGEON_DIFFICULTY 同型；int 视角 +
        // 显式 cast 恒产 eax→ebx 两段，探针 /tmp/ctb_probe/p4.o f1/f2）。
        bool generated = CBossPlay::generateRewardItem(
            i, itemCount,
            static_cast<unsigned int>(member->get_charac_level()),
            m_difficultyE,
            static_cast<unsigned int>(std::min(10, (int)m_stageCur)),
            m_dungeon);
        // [b28d-solo-CBossTower] P225 dead 变体：ORIG 0x08144fb8 `mov %al,-0x19;
        // movzbl -0x19,%eax; xor $1,%eax; test %al,%al; jmp`（具名 bool 槽 +
        // 取反物化后无条件跳）⇒ `while (!generated) break;` 取代 `(void)generated`。
        while (!generated)
            break;
    }
    return 1;
