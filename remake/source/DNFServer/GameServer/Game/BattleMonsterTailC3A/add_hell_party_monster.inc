void CBattle_Field::add_hell_party_monster(map_monster& mob, int& uid, int& seq,
                                           std::map<unsigned short, STHellPartyGroupOrder>* groups)
{
    // ORIG 入口 4 条具名局部初始化（0x083071c5..0x083071da：-0x24/-0x20/-0x1c/-0x18
    // = 0/0/1/0）。-0x24 为命中率累加器（成功插入后与 -0x20 成对清零，跨外层迭代
    // 保留）；-0x20/-0x1c 在 ORIG 全函数内只写不读（P171 死写入照抄，标 [推断]）；
    // -0x18 是 totalRate 的 int 槽（先置 0、循环内赋 u16 零扩展值）。
    int accumulatedRate = 0;
    int accumulatedTotalRate = 0;   // [推断] ORIG -0x20（仅入口/成功路径清零）
    int rateRollState = 1;          // [推断] ORIG -0x1c（仅入口置 1）
    int totalRate = 0;              // ORIG -0x18（int；非 unsigned short）

    GetCurrentMapInfo()->m_hellPartyGroup.clear();   // MapInfo+0x8c

    for (std::map<unsigned short, STHellPartyGroupOrder>::iterator oit = groups->begin();
         oit != groups->end(); ++oit)
    {
        std::list<STHellPartyGroupRate> rates(oit->second.m_rates);

        // ORIG 0x08307239 先求 rit=rates.begin()（sret -0x3ac），随后才取权重/掷点。
        std::list<STHellPartyGroupRate>::iterator rit = rates.begin();

        // 按当前地狱派对难度取该组总权重（u16 零扩展）后掷点。ORIG +0x0a3 段：
        // `movzwl 0x4(%eax,%edx,2)`（pair.second 的 +0x04 = m_rateByDifficulty[diff]）
        // → `movzwl %ax,%eax; mov %eax,-0x18`（int 槽，两次零扩展为 4.1 固有形）。
        totalRate = oit->second.m_rateByDifficulty[m_hellPartyDifficulty];
        const int threshold = get_rand_int(totalRate);                  // -0x14
        while (rit != rates.end())
        {
            if (rit->m_difficulty != m_hellPartyDifficulty)
            {
                rit++;                                   // 难度不匹配：仅推进
                continue;
            }

            accumulatedRate += rit->m_rate;
            if (accumulatedRate < threshold)
            {
                rit++;
                continue;
            }

            // find_hellparty_group 返回 stHellPartyMonsterGroupList 视图：
            // int m_monsterIdx[16]@+0x00（>0 结尾）、unsigned char m_rate[16]@+0x40。
            int i = 0;                               // ORIG +0x164：i=0 在 find 之前发射
            stHellPartyMonsterGroupList* group =
                reinterpret_cast<stHellPartyMonsterGroupList*>(
                    G_CDataManager()->find_hellparty_group(rit->m_groupIdx));
            if (group == NULL)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    "void CBattle_Field::add_hell_party_monster(map_monster&, int&, int&, "
                    "MAP_HELL_PARTY_GROUP_ORDER*)",
                    0x1076,
                    "HELLPARTY_LOG : HELLPARTY GROUP INDEX (%d) , MAP INDEX (%d) : "
                    "ADD HELL PARTY GROUP ERROR!!\n",
                    rit->m_groupIdx, GetCurrentMapInfo()->m_mapIndex);
                break;                                   // 清理副本 → 推进外层迭代
            }

            for (; group->m_monsterIdx[i] > 0; ++i)
            {
                // ORIG +0x1f0 形：seq 值读入 %eax → 写 mob → lea 0x1 → 写 seq（P231
                // 裸 register 提示免槽保活，非 asm 绑定，P0 合规）。
                register int curMobId = seq;
                mob.m_mobId = curMobId;                  // +0x00 ← seq
                seq = curMobId + 1;
                mob.m_hellPartyGroupIdx = oit->first;    // +0x334 ← 组序表键（隐式 u16→s16）
                mob.m_hellPartyGroupOrder =
                    static_cast<short>(rit->m_groupIdx); // +0x336 ← 组索引
                mob.m_level = static_cast<char>(
                    m_pDungeon->get_standard_level());   // +0x10 byte ← 副本标准等级
                mob.m_mobIndex = group->m_monsterIdx[i]; // +0x0c
                // ORIG +0x26c `test; jne <movb $5 块>`（0 块直落）⇒ 反极性写法。
                if (group->m_rate[i] == 0)
                    mob.m_roleType = 0;                  // +0x08 byte
                else
                    mob.m_roleType = 5;

                register int curInstId = uid;            // ORIG +0x280 同形
                mob.m_instId = curInstId;                // +0x04 ← uid
                uid = curInstId + 1;
                mob.m_dropCnt = 100;                     // +0x2c ← 100（ORIG movl $0x64）
                mob.m_field13 = 1;                       // +0x13 byte
                mob.m_field14 = 0;                       // +0x14 byte

                // +0x11/+0x12 为逐字节写：脚本 tag 字节与常量 1。
                mob.m_field11b.m_field11b0 =
                    static_cast<stHellParty_t*>(
                        G_CDataManager()->get_hellparty_script_values())
                        ->m_field6c[m_hellPartyDifficulty];   // +0x6c[diff]
                mob.m_field11b.m_field11b1 = 1;

                mob.m_hellPartyScriptIdx = m_hellPartyDifficulty;     // +0x338 byte

                // ORIG 08151aea 只发一次拷贝构造（C2ERKS_），实参直传临时 ⇒ 匿名
                // 临时形（P0/P278），勿用具名 copy（会多一次按值传参拷贝 + 析构）。
                GetCurrentMapInfo()->Add_Mob(map_monster(mob));
            }

            GetCurrentMapInfo()->m_hellPartyGroup.insert(   // MapInfo+0x8c
                std::make_pair(rit->m_groupIdx, i - 1));

            accumulatedRate = 0;                         // 仅成功路径清零（-0x24/-0x20 成对）
            accumulatedTotalRate = 0;
            break;
        }
        // rates 副本析构（RAII）→ 推进外层迭代（ORIG 尾部 ++oit）。
    }
