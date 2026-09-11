// ----------------------------------------------------------------------------
// ORIG T 0x08144e36 _ZN8WongWork10CBossTower18generateRewardItemEv：
// 逐有效成员：层数奖励上限 = min(10, min(GetTowerScript 槽值,
// getTowerMaxClearRewardItemNum()))，其中 GetTowerScript 返回 it->second+4
// （&stDeathTower_t::m_rateA[0]），ORIG 读 ret+4+(stage+200)*4（stage>=1 落在
// m_cntB 区）；层数参数 = min(10, m_stageCur)；直调基类
// CBossPlay::generateRewardItem(i, itemCount, level, difficulty, 层数参数,
// m_dungeon)（ORIG call 08149914 非虚）。恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08144e36：实参/局部装载序（P23/P75）+ bool 死物化（P197④）；
//   invalid 局部已去、调用序列全等。
// [b28d-solo-CBossTower] generated bool 局部 + P225 dead 变体
//   `while (!generated) break;`（Main 代落 callee bool 后复现
//   `mov %al,-0x19; movzbl; xor $1; test; jmp`）+ 实参走 m_difficultyE（union
//   双视角，单段 `mov 0xae8(%eax),%ebx`）+ stageParam 内联。
// [b28f-SweepCBossTower] AE（116:116）：旧残余「itemCount 预清位置 vs 槽号
//   互换」用「scriptLimit **裸声明前置** + 赋值后置」解开——4.4 按声明序
//   「先声明浅」（P309）⇒ scriptLimit=-0x3c（浅）/itemCount=-0x40（深）与 ORIG
//   全等，且 itemCount 的 `movl $0,-0x40` 落回 GetTowerScript 之前
//   （P198③ 两段式镜像；旧「声明前置」形把两槽号互换，实测更劣）。
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
}