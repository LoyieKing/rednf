// ============================================================================
// _MakeDropItems —— ORIG T 0x0830adf6（0xa51 字节）。
// 生成器驱动掉落：填 WongWork::stGenerateRefData_t（两遍 blob 写入，中间穿插
// 条件生成调用），经 CItemGeneratorMgr 各 eGenerateType 槽生成 Inven_Item 集，
// 再逐项转 map_item（分配 dropIdx/createTick、蓝矿金币缩放、幸运归属）推入
// itemList 与 MapInfo::Add_Item。
// ref blob 槽位按 ORIG 写入宽度逐条对齐（+0x34..+0x42 为调用方上下文区）。
// ============================================================================
void CBattle_Field::_MakeDropItems(std::list<map_item>& itemList,
                                   MapInfo& mapInfo, const map_monster& mob,
                                   unsigned short uid, int dieReason, int a,
                                   int b, bool hellPartyKilled,
                                   std::vector<bool>& flags, float rate,
                                   int premiumRate)
{
    const int curSec = GlobalData::s_systemTime_.getCurSec();       // -0x40
    const char roleCvt = convertAPCRoleType2MonsterRoleType(
        static_cast<char>(mob.m_roleType));                          // -0x39

    WongWork::stGenerateRefData_t ref;                               // -0xc0
    ref.m_dropRate = b;                                              // +0x00

    // ---- 第一遍 blob（ORIG +0x067..+0x114） ----
    // [b28d-20] P183 地址重物化：ORIG 每处均为 `mov %al,-0x??(%ebp)` 单条 ref
    // 成员直写；refBlob/mobBlob 指针缓存形每处多 mov+lea/add 两条。
    ref.m_pad37 = static_cast<char>(get_dungeon_diff());             // +0x37
    // [b28d-20] ORIG `test %al,%al; setg %al; test %al,%al; je`：无 dimPossible
    // 具名槽（P0 表达式直用）；具名 char 局部会多一槽并改分支形。
    if (static_cast<signed char>(m_pDungeon->get_dimension_possible()) > 0)
        ref.m_rateSlot = static_cast<char>(m_standardDimensionLevel);
    else
        ref.m_rateSlot = static_cast<char>(m_pDungeon->get_standard_level());
    ref.m_field35 = roleCvt;                                         // +0x35
    ref.m_field34 = static_cast<char>(mob.m_level);                  // +0x34 ← mob.level
    ref.m_field38 = static_cast<unsigned char>(dieReason);           // +0x38
    ref.m_field39 = mob.m_field11b.m_field11b1;                      // +0x39 ← mob.field11 hi
    ref.m_field3c = a;                                               // +0x3c dword
    // [b28d-20] ORIG 0830aee0 `movb $0x0,-0x80(%ebp)` 为 1 字节写（P10 宽度判据）。
    ref.m_field40_byte = 0;                                          // +0x40 byte
    ref.m_field41 = static_cast<unsigned char>(getItemDropDungeonType()); // +0x41
    ref.m_forceTag = flags;                                          // +0x08 vector<bool> op=
    ref.m_field1c = rate;                                            // +0x1c float

    // ---- 幸运点收集 + 蓝矿 buff 死存储（ORIG +0x136..+0x268） ----
    // [b28d-20] P183：ORIG 每次 `lea -0x78(%ebp),%eax; add $0xc,%eax` 重算
    // m_luckPoint 地址，无 resultLuck 指针缓存槽。
    int blueBuff1 = 0;                                               // -0x38
    int blueBuff2 = 0;                                               // -0x34
    // [b28d-20] ORIG 发射序 blueBuff1/2 清零 → result C1（P212 声明序=发射序）。
    WongWork::stGenerateResult_t result;                             // -0x78
    if (m_party != 0)
    {
        m_party->GetLuckPoint(&result.m_luckPoint, true);
        if (m_party->IsSinglePlay() && m_party->getManager() != 0)
        {
            if (m_party->getManager()->getMoveSpace() == 1)
            {
                BlueMarble* marble = static_cast<BlueMarble*>(
                    m_party->getManager()->getBlueMarble());
                if (marble != reinterpret_cast<BlueMarble*>(0))
                {
                    blueBuff1 = marble->getBuff(
                        m_party->getManager(),
                        static_cast<BlueMarbleBuffType::T>(1));
                    blueBuff2 = marble->getBuff(
                        m_party->getManager(),
                        static_cast<BlueMarbleBuffType::T>(2));
                    // [b28d-20] ORIG -0x9b/-0x98/-0x94/-0x90 落在 ref 对象内部
                    // （ref=-0xc0，sizeof 0x48）⇒ 不是独立局部，而是 ref 字段写入：
                    // +0x25=1 / +0x28=get_charac_job() / +0x2c=get_charac_level() /
                    // +0x30=blueBuff2（原 (void) 丢弃 = 真语义缺口）。
                    // ORIG 逐处重调 getManager（P183 禁缓存 ⇒ 原样内联重取）。
                    ref.m_field25 = 1;
                    // [SweepMonsterDrop 2026-09-09] m_field28 int→ENUM_CHARACTERJOB
                    // （P254）：get_charac_job() 返回 int ⇒ 必须显式 cast；探针实测
                    // cast 形仍为单条直存（与 ORIG 0x0830b03d `mov %eax,-0x98` 同形）。
                    ref.m_field28 = static_cast<ENUM_CHARACTERJOB>(
                        m_party->getManager()->get_charac_job());
                    ref.m_field2c = m_party->getManager()->get_charac_level();
                    ref.m_field30 = blueBuff2;
                }
            }
        }
    }
    setPremiumDropRate(ref, premiumRate);

    // ---- 维度/绝望塔标志（ORIG +0x28d..+0x2cb） ----
    // [b28d-20] ORIG 写 -0x7c/-0x7d 落在 ref 对象内部 ⇒ 是 ref 字段
    // （m_field44_byte=ultFlag、m_field43=dimFlag），非独立局部。
    // 成员直读单条 movzbl（P235：取址视图多一条 add）；比较按 signed char（jle/je）。
    // ORIG 物化形：ultFlag 为 if/else 直赋（movb $1/$0）；dimFlag 为三目经 eax。
    if (static_cast<signed char>(m_pDungeon->m_bloodDungeonType) > 0)
        ref.m_field44_byte = 1;
    else
        ref.m_field44_byte = 0;
    // [N5A] m_field43 为 bool（消费者形 `movzbl;xor $1;test;je` 双处实证；h_probe H2 对
    // uchar 目标反产 `test;jne`）。ORIG 的 store 形（0x0830b0b5..0xc1）只能由「向**字节
    // 左值**写 int 三目」产生（4.1.2/4.4.7 对 bool 目标恒 setne：e_probe E1..E5 /
    // x_probe X1..X7 实测）⇒ 经 CItemGenerator.h 的 union 字节视角写（u_probe.cpp 实证）。
    ref.m_field43Byte = static_cast<signed char>(m_pDungeon->m_89e) != 0 ? 1 : 0;
    // [b28d-20] ORIG 无 notBoss 局部槽（使用点内联 `movzbl 0x349; xor $1`）
    // ⇒ P0 判读：使用点直写 !mob.m_isBoss（bool 字段裸测形）。

    // ---- 条件生成 1：普通图随机生成（ORIG +0x2e4..+0x32d，gen0.random） ----
    if (!ref.m_field43 && !ref.m_field44_byte && !mob.m_isBoss)
    {
        G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_0)
            ->generateRandomItem(ref, result);
    }

    // ---- 第二遍 blob（ORIG +0x32f..+0x425；覆盖第一遍大部分槽位） ----
    if (static_cast<signed char>(m_pDungeon->get_dimension_possible()) > 0)
        ref.m_field35 = static_cast<char>(m_standardDimensionLevel);
    else
        ref.m_field35 = static_cast<char>(m_pDungeon->get_standard_level());
    ref.m_rateSlot = static_cast<char>(get_dungeon_diff());          // +0x36
    ref.m_rankBonusIndex = mob.m_mobIndex;                           // +0x38 dword
    ref.m_field3c_byte = static_cast<unsigned char>(mob.m_roleType); // +0x3c ← roleType
    ref.m_field34 = static_cast<char>(mob.m_level);                  // +0x34 ← level
    ref.m_field3d = mob.m_field11b.m_field11b0;                      // +0x3d ← field11 lo
    ref.m_field3e_byte = static_cast<unsigned char>(dieReason);      // +0x3e
    ref.m_field3f =
        static_cast<unsigned char>(m_party->getMemberMaxLevel());    // +0x3f
    // [b28d-20] ORIG 0x0830b1c7 `movzbl -0x120(%ebp)` 读的是 hellPartyKilled 槽
    // （序言 0x28(%ebp)→-0x120），不是 uid（-0x11c）⇒ 原 uid 赋值是错参数来源。
    ref.m_field40_byte = static_cast<unsigned char>(hellPartyKilled); // +0x40 byte
    // [b28d-20] ORIG 0x0830b1d4 `movzwl; test %ax,%ax; setne %al` ⇒ 直接赋 bool 表达式。
    ref.m_field41 = static_cast<unsigned char>(
        mob.m_hellPartyGroupIdx != 0);                               // +0x41
    ref.m_field42_byte = static_cast<unsigned char>(mob.m_hellPartyScriptIdx); // +0x42
    // [b28d-20] ORIG -0x7b = ref+0x45（m_field45）⇒ bloodFlag 是 ref 字段写入。
    ref.m_field45 = mob.m_bloodRoundFlag != 0 ? 1 : 0;               // +0x45
    // [b28d-20] ORIG `mov %al,-0x9c(%ebp)` = ref+0x24（m_field24）⇒ 返回值写入
    // ref 字段（原 (void) 丢弃 = 真语义缺口）。
    ref.m_field24 = static_cast<char>(m_party->GetPartyMemberForceDropFlag());

    // ---- 条件生成 2：普通图指定生成（ORIG +0x42b..+0x47c，gen0.specific） --
    // [b28d-20] ORIG 0x0830b232 `movzbl -0x7f(%ebp)` = ref+0x41（m_field41，
    // hellPartyGroupIdx!=0），非 m_field45（后者全函数写后未读 = 死存储）。
    if (!mob.m_isBoss || ref.m_field41)
    {
        G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_0)
            ->generateSpecificItem(ref, result);
    }

    // ---- 无条件：getDropItems 槽写入 + gen4.specific（ORIG +0x47e..+0x4cd）-
    ref.m_field34p = m_pDungeon->getDropItems();                     // +0x34 指针
    G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_4)
        ->generateSpecificItem(ref, result);

    ref.m_field35 = static_cast<char>(mob.m_level);                  // +0x35 ← level

    // ---- 条件生成 3：难度档随机（ORIG +0x4dc..+0x5c2，gen3.random） -------
    if (!ref.m_field43 && !ref.m_field44_byte && !mob.m_isBoss)
    {
        if (dieReason >= 1 && dieReason <= 4)
        {
            // dm+0x524 = m_serverParameter.m_weekendBouns4b4.m_dropRoutineByParty
            // （+0x68+0x4b4+8；union 数组视角 ⇒ 单条 `movzbl 0x524(%eax,%edx,1)`，
            // 取址形会多 add，P235/P311）
            // [b28d-20] ORIG movzbl+movsbl 后存 **int** 槽（-0x28）⇒ 局部为 int。
            const int cnt = static_cast<signed char>(
                G_CDataManager()->m_serverParameter.m_weekendBouns4b4
                    .m_dropRoutineByParty[dieReason - 1]);
            for (int i = 0; i < cnt; ++i)
            {
                G_CDataManager()->getItemGenerator()
                    ->getGenerator(
                        WongWork::CItemGeneratorMgr::eGenerateType_3)
                    ->generateRandomItem(ref, result);
            }
        }
        else
        {
            G_CDataManager()->getItemGenerator()->getGenerator(
                    WongWork::CItemGeneratorMgr::eGenerateType_3)
                ->generateRandomItem(ref, result);
        }
    }

    // ---- 条件生成 4/5：PC 房事件加成（ORIG +0x5c4..+0x71f，gen5/gen6） -----
    if (!ref.m_field43 && !ref.m_field44_byte)
    {
        if (m_party != 0
            && GlobalData::s_event_manager->GetRepeatEvent(0x16)->IsEventing(0)
            && m_party->hasPCRoomPlayer() && !mob.m_isBoss)
        {
            G_CDataManager()->getItemGenerator()->getGenerator(
                    WongWork::CItemGeneratorMgr::eGenerateType_5)
                ->generateRandomItem(ref, result);
        }
        if (m_party != 0
            && GlobalData::s_event_manager->GetRepeatEvent(0x30)->IsEventing(0)
            && m_party->hasPCRoomPlayer() && !mob.m_isBoss)
        {
            G_CDataManager()->getItemGenerator()->getGenerator(
                    WongWork::CItemGeneratorMgr::eGenerateType_6)
                ->generateRandomItem(ref, result);
        }
    }

    // ---- 条件生成 6：PC 房玩家追加（ORIG +0x726..+0x76f，gen7.random） -----
    // [b28d-20] ORIG 0x0830b51c 直接 `mov (%eax),%eax; mov %eax,(%esp); call
    // hasPCRoomPlayer`，无 m_party 判空（P140：ORIG 无防御时不补）。
    if (m_party->hasPCRoomPlayer())
    {
        G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_7)
            ->generateRandomItem(ref, result);
    }

    // ---- 结果落地（ORIG +0x771..+0x9ee） ----------------------------------
    int seq = 0;                                                     // -0x30
    // [b28d-20] P13/P283：ORIG 循环前 begin 一次、end 一次（各自独立槽），cond 用
    // 两缓存迭代器比较；把 end() 写进 for 条件会每轮重调 end。
    std::vector<Inven_Item>::iterator gi = result.m_dropItems.begin();
    std::vector<Inven_Item>::iterator giEnd = result.m_dropItems.end();
    for (; gi != giEnd; ++gi)
    {
        map_item mi;                                                 // -0x114
        mi.m_count = 1;
        mi.m_itemIndex = mapInfo.m_dropItemCnt++;  // N5A: ORIG 复用 eax（无 reload）
        mi.m_dropIndex = uid;
        mi.m_createTick = curSec;
        // P83/N84：ORIG 为 Inven_Item(0x3d) 结构整体拷贝（15×mov dword+1×movb，
        // 非 rep movsl）；memcpy 在 4.4 恒退化为 call。
        mi.m_item = *gi;

        // 蓝矿金币缩放：单人且 manager 在移动空间且 itemIdx==0 时按脚本系数
        // ROUND((buff*f2+f1)*gold) 缩放 item.m_addInfo2(+0x07)。
        if (m_party != 0)
        {
            if (m_party->IsSinglePlay() && m_party->getManager() != 0)
            {
                if (m_party->getManager()->getMoveSpace() == 1)
                {
                    if (mi.m_item.m_addInfo == 0)
                    {
                        const float f1 = reinterpret_cast<BfkcBlueMarbleScriptView*>(
                            G_CDataManager()->getBlueMarbleScript())->m_f1;
                        const float f2 = reinterpret_cast<BfkcBlueMarbleScriptView*>(
                            G_CDataManager()->getBlueMarbleScript())->m_f2;
                        // [b28d-20] ORIG 直接改写 blueBuff1 槽（无独立 buff 局部），且公式为
                        // 纯 (int) 截断（fistpl+RC=11，无 fadds 0.5）⇒ 原 BFKC_ROUND 的
                        // +0.5f 多出 flds 0.5/faddp 且语义不同（四舍五入 vs 截断）=真 bug。
                        // P243：ORIG 首条 fildl 即 addInfo2（目标在左）⇒ 目标写在乘法左侧。
                        if (blueBuff1 < 1)
                            blueBuff1 = 1;
                        mi.m_item.m_addInfo2 = static_cast<int>(
                            static_cast<float>(mi.m_item.m_addInfo2)
                            * (blueBuff1 * f2 + f1));
                    }
                }
            }
        }

        mi.m_ownerId = result.m_luckPoint.GetUserGainItem(seq);
        itemList.push_back(mi);
        mapInfo.Add_Item(mi);
        seq += 1;
    }
}