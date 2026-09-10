void CBattle_Field::add_monster_event(int& seq, int x, int y)
{
    // [b28d-Lane27] ORIG 0x083023f7/0x08302408：两条独立 je 各带 nop 岛（P63）。
    if (m_pDungeon == NULL)
        return;
    if (m_pMap == NULL)
        return;
    // [b28d-Lane27] P214①：ORIG 0x08302419 `call empty; xor $0x1,%eax; test %al,%al;
    // je <exit>`（bool callee + `!empty()` 正向包裹、body 直落；原负向早退形 4.4 生
    // `test; jne exit` 且尾部多 2 条）。
    if (!m_eventMonsterStatus.empty())                                    // +0x17c
    {
        int spawned = 0;
        const int monsterCnt =
            static_cast<int>(GetCurrentMapInfo()->m_monsterMap.size());
        unsigned int instBase = 10000;
        // [b28d-solo-BattleMonsterDepsC2] P167/P13：ORIG 调 operator*（`_ZN…deEv`）并把结果存
        // 具名局部 -0x10（`(*ev)` 形），begin/end 各一次并缓存（-0x370/-0x36c + 两转换槽）。
        std::vector<stEventMonsterStatus_t>::const_iterator ev =
            m_eventMonsterStatus.begin();
        std::vector<stEventMonsterStatus_t>::const_iterator evEnd =
            m_eventMonsterStatus.end();
        for (; ev != evEnd; ++ev)
        {
            const stEventMonsterStatus_t& evs = *ev;
            // [b28d-Lane27] ORIG 0x083024e6/0x083024f5 两条独立 `jne <循环尾>`；P296
            // 嵌套正向包裹消同层多 `continue` 的 nop+jmp 中继岛（`jne` 落点=循环尾）。
            if (evs.m_pos.first == x && evs.m_pos.second == y)
            {
            if (m_pMap->m_rivalData2.empty())                             // CMap+0x9c
                return;

            for (int k = 0; k < evs.m_monsterIdx; ++k)
            {
                map_monster mob;               // 每轮默认构造（ORIG ctor 081511c8）
                mob.m_mobIndex = evs.m_eventIdx;
                mob.m_level = static_cast<char>(m_pDungeon->get_standard_level());
                mob.m_mobId = static_cast<int>(instBase);
                // mob+0x11(低)←ev.m_field14、+0x12(高)←ev.m_field10（ORIG 逐字节写）
                mob.m_field11b.m_field11b0 = static_cast<unsigned char>(evs.m_field14);
                mob.m_field11b.m_field11b1 = static_cast<unsigned char>(evs.m_field10);
                mob.m_roleType = 0;
                mob.m_field13 = 0;
                mob.m_field14 = 0;
                mob.m_dropCnt = 100;
                mob.m_flag34 = 0;
                mob.m_bloodRound = 1;                                      // +0x34a

                if (spawned < monsterCnt)
                {
                    mob.m_instId = seq++;          // ORIG 单次读 seq + lea 0x1
                    GetCurrentMapInfo()->Add_Mob(mob);
                    ++instBase;
                    if (static_cast<unsigned int>(m_pMap->m_rivalData2.size()) +
                            10000u <=
                        instBase)
                    {
                        instBase = 10000;
                    }
                }
                ++spawned;
            }
            }
        }
    }
