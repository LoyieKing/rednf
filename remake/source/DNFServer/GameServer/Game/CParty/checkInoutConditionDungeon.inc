// CParty::checkInoutConditionDungeon @ 0x85ac59c —— 进入副本前的进出条件判定，
// 返回 RetryFailReason 值（10 无副本 / 10 禁止进入 / 1 疲劳 / 2 材料 / 4 维度 /
// 7 其它 / 8 封印门 / 9 可进入）。
// 依赖若干 CParty / CWorldMap / CWorldMapList 方法（ORIG 符号已声明，定义桩由
// 并行批次提供）：CheckEnterDimensionDungeon / check_dungeon_start /
// CheckEnterVillageAttackRevenge / CheckDestroyConditionSealDoor /
// CheckMemberFatigue / CWorldMapList::find_world_map /
// CWorldMap::IsInHellDungeon / CWorldMap::hasDeathTower。
// [SweepCParty] 补真缺口：循环内 get_user 三次独立调用（P0/P183，旧码 u 缓存 ⇒ MISS 2）
// + param[4] 改 memset（ORIG 两次 call memset）。p167 28=28 CLEAR（余 C1/C2 别名）。
int CParty::checkInoutConditionDungeon()
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS CParty::get_user + memset（2 项）。@0x85ac59c
    if (m_battleField.m_pDungeon == 0)
        return 10;
    if (GetEPLPState() <= (char)1)
        return 10;
    for (int i = 0; i < 4; ++i)
    {
        // P0/P183：ORIG 0x085ac59c 段三次独立 get_user（条件 1 次 + 疲劳比较 2 次），
        // 无 u 缓存（p167 曾 MISS 2）。
        if (get_user(i) == 0 ||
            (char)m_battleField.m_pDungeon->get_blood_dungeon_type() > 0)
            continue;
        if (get_user(i)->getCurCharacTotalFatigue() ==
            get_user(i)->getCurCharacTotalMaxFatigue())
            return 1;
    }
    unsigned char flag = 0;
    Inven_Item items[4];
    int param[4];
    // ORIG 0x085ac59c：两次 call memset（0xf4 / 0x10）；`={0}` 会被 4.4 内联成 movl。
    memset(items, 0, 0xf4);
    memset(param, 0, 0x10);
    // [A2 2026-09-11 P59 + P248 档①] ORIG 0x085ac711 `call; xor $0x1,%eax; test %al,%al;
    // je <body>`，fail 块（mov $2,%ebx; jmp）直落其前；callee 已声明 bool
    // （CParty.h:478）⇒ 取反物化 xor + 前置早退。旧 `== 1` 形产 `test %al,%al; je` 且
    // fail 块落函数尾（实测缺 1 条 xor、与 ORIG 块序相反）。
    if (!CheckEnterAncientDungeon(m_battleField.m_pDungeon, flag, items, param))
    {
        return 2;
    }
    {
        // [2026-09-10 试形 P261/P279] ORIG 无右值槽、以 `cmp $0xab,%eax` 链直比 call 结果
        // （0x085ac59c 段两处）⇒ 原码为 `switch(call())` 而非 if 链（if 链恒带具名槽）。
        switch (CheckEnterDimensionDungeon(m_battleField.m_pDungeon))
        {
        case 0xab:
        case 0xad:
            return 4;
        }
        // [G7-sub] ORIG 0x085ac76a..0x085ac781：`call; cmp $0xa,%eax; je L7;
        // cmp $0xa,%eax; jl next; sub $0xf3,%eax; cmp $4,%eax; ja next; L7:` ——
        // 结果**全程无槽**（eax 直用，P0 ⇒ 原码无具名 int）且为「先等值再范围剥离」
        // 决策树 ⇒ 原码是 switch（P163/P46/P251；case 10 + 连续范围 0xf3..0xf7）。
        // 旧码 `int r = …; if (!(r != 10 && …))` 多 1 store + 2 load（-0x30 槽）。
        switch (check_dungeon_start(m_battleField.m_pDungeon))
        {
        case 10:
        case 0xf3:
        case 0xf4:
        case 0xf5:
        case 0xf6:
        case 0xf7:
            return 7;
        }
        if (CheckEnterVillageAttackRevenge(m_battleField.m_pDungeon) != 0)
            return 7;
        CWorldMap* worldMap = 0;
        int worldIdx = G_GameWorld()->GetWorldMapIndex(m_manager);
        if (worldIdx > 0)
        {
            worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
        }
        // [G7-sub] ORIG 0x085ac7fc..0x085ac836：条件逐项 `je <false 岛>` + 共享
        // `mov $1,%eax; jmp / mov $0,%eax` 物化 + `test %al,%al; je <body>`，
        // 且**全函数无 hellDun 槽**（原码无具名 bool）⇒ P316/P226 语句表达式
        // 零槽短路物化。旧码 `bool hellDun=false; if(…) hellDun=true; if(hellDun)`
        // 多 2 条 movb 槽写 + 1 条 cmpb（我方 -0x1e）。
        // 旧码的 `bfView` 引用具名多 1 store +1 load/访问（ORIG 0x085ac81a 单
        // `mov 0x8(%ebp),%eax; mov 0xcd4(%eax),%eax`）⇒ 直接成员访问（P183）。
        if (({ worldMap != 0 &&
                worldMap->IsInHellDungeon() != 0 &&
                m_battleField.IsEnableHellDungeon() != 0 &&
                m_battleField.m_hellPartyMode == 1; }))
        {
            std::vector<int> vec;
            // [G7-sub] ORIG 0x085ac86b..0x085ac88b：`call; xor $1,%eax; test %al,%al;
            // je <flag=1 块>` —— 返回值**无槽**；flag=0/`return 8` 直落在前 ⇒ 原码是
            // 「直调 + 提前 return」形（P0 无具名 + P168a 取反极性）。旧码 `int sealR`
            // 多 1 store + 2 load（我方 -0x1c）。
            if (CheckDestroyConditionSealDoor(m_battleField.m_pDungeon, vec,
                                              worldMap->m_field34) != 1)
            {
                m_sealDoorFlag = 0;
                return 8;
            }
            m_sealDoorFlag = 1;
        }
        // [G7-sub] ORIG 0x085ac8cc..0x085ac92d：逐项 `xor $1,%eax; test %al,%al;
        // je <false 岛>`（P168a 取反族）+ `mov $1/$0,%eax` 物化，**无 noFat 槽**
        // ⇒ 原码是 `!(A||B||C||D)` 直用，非具名 bool。旧码 `bool noFat` 多 2 条
        // movb 槽写 + 1 条 cmpb（我方 -0x1d）。
        // [负结论·2026-09-11 本轮实测] 全函数唯一残余差 = 本条件第 4 项
        // `m_advanceAltarSurvivalFlag`：ORIG 0x085ac9xx `movzbl 0x800(%eax),%eax;
        // xor $1,%eax; test %al,%al; je <false 岛>` vs 我方 `movzbl …; test %al,%al;
        // jne <false 岛>`（差 1 条，269/268）。试形：De Morgan 全展开
        // （`A!=1 && !(B&&C) && !D`）实测 codegen **零变化**（objdump 逐条核
        // 28a9..28b2 仍 test/jne）⇒ 4.4 对 unsigned char 直读恒不产 xor，判 N。
        // 已回滚该试形（源码复原为 `!(A||B||C||D)`）。
        if (!(m_battleField.m_bloodState == 1 ||
              (worldMap != 0 && worldMap->hasDeathTower()) ||
              m_battleField.m_pDungeon->isNoFatigueDungeon() ||
              m_battleField.m_pDungeon->m_advanceAltarSurvivalFlag))
        {
            // [G7-sub] ORIG 0x085ac931/0x085ac93c：`call CheckMemberFatigue;
            // mov %eax,-0x1c(%ebp); cmpl $0,-0x1c(%ebp); jle` —— 结果**落槽**后比较
            // ⇒ 原码有具名 int 局部（P0 有槽往返即有局部）。旧码直测无槽。
            int memberFatigue = CheckMemberFatigue();
            if (memberFatigue > 0)
                return 1;
        }
        return 9;
    }
// ---------------------------------------------------------------------------
// [G7 判词·口径伪影，勿据此回滚] 本件 codegen 收敛（286→267 条，ORIG 269）使本段长度
// 0x3fe→0x3bd（奇偶翻转），导致紧随其后的自由函数 `_Z33getHellPartyEntranceItemNeedCounti`
// 结束地址变奇 ⇒ 4.4 在其 `ret` 后补 1 条函数间对齐 nop（我方 size 21 vs ORIG 20；
// .o 实证 `2920 ret` + `2921 nop`，函数体前 20 条与 ORIG 逐条相同）。
// 该 nop 属 guide §6.1「对齐填充」豁免口径，无寄存器/内存效果 ⇒ 账本上表现为
// identical→ae 的**桶内移动**（合计 473 不变、diff 33 不变），**不是** identical 下降；
// Main 2026-09-11 裁决：保留本件改善，不得为恢复该计数回滚本文件。
}