// ORIG 0x08301d76：按地图 AI 编排表逐条生成 APC 怪物。
// [b28d-solo-BattleMonsterDepsC2] 已修：`mob.m_instId = seq++`（ORIG 单次读 seq + lea 0x1）；
// m_level 三目改 (signed char)dim > 0 形（ORIG `test %al; setg %al; test; je`）。
// [SweepBattleMonster 2026-09-09] 已修（213 vs 212，clean 211 vs 209）：①删 `map_monster tmp`
// 直接 `Add_Mob(mob)`（省一个 0x350 临时，帧 0xa30→0x6f0，ORIG 0x6e0）；②m_field11 字节视图
// 复现 +0x11/+0x12 双 movb（WarField.h 匿名 union）；③`!m_dummyBossMap`（字段 bool 化）。
// [SweepBattleMonsterDeps 2026-09-10] 归零（211=211，ndiff 152→0，AE）：①apcIdx 声明提到
// arranges 之前 + 循环前重置（ORIG 0x08301da3/0x08301de2 双清零，4.1「先声明更深」）；
// ②begin/end 各调一次并缓存 itEnd（P13；ORIG -0x368/-0x36c 两独立槽，循环尾只 operator++/!=）；
// ③`if (!empty())` 正向包裹（P214①，ORIG `xor $1; test; je`）；④删 info 具名槽（P0，内联
// GetCurrentMapInfo()）；⑤m_level 改语句表达式 + if/else 复现 `setg` 物化（P226/P225）。
// 帧 0x6f0→0x6e0=ORIG，p167 27=27 CLEAR（仅 map_monster C1/C2、D1/D2 同址别名差）。
void CBattle_Field::add_monster_APC_AI(int& seq)
{
    if (m_pMap == NULL || m_pDungeon == NULL)
        return;
    // ORIG 0x08301da3：apcIdx(-0x18) 的初始化发射在 getAICharacters 之前（声明点在
    // arranges 之前，4.1「先声明更深」），且 mob 构造后 0x08301de2 再清零一次
    // ⇒ 原码为「函数头声明初始化 + 循环前重置」两段形。
    int apcIdx = 0;
    const std::vector<STAICharacterMapArrangeData>* arranges =
        m_pMap->getAICharacters();
    // P214①：ORIG 0x08301dc9 `call empty; xor $0x1,%eax; test %al,%al; je <函数尾>`
    // （正向包裹形）⇒ 主体包进 `if (!empty())`；早退 `if (empty()) return;` 4.4 产
    // `test; jne`（少 1 条且块布局反）。
    if (!arranges->empty())
    {
        map_monster mob;           // 默认构造一次（循环体内 memset 复位）
        apcIdx = 0;
        // ORIG 0x08301df9/0x08301e11：begin()/end() 各调一次并缓存（-0x368/-0x36c），
        // 循环尾只调 operator++ 与 operator!=（P13 缓存 end）。
        std::vector<STAICharacterMapArrangeData>::const_iterator it =
            arranges->begin();
        std::vector<STAICharacterMapArrangeData>::const_iterator itEnd =
            arranges->end();
        for (; it != itEnd; ++it)
        {
            const STAICharacterMapArrangeData& arrange = *it;
            CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(static_cast<unsigned int>(arrange.m_type));
            if (ai == NULL)
            {
                // P0：ORIG 0x08301e5a 段无 info 具名槽（call 结果直接 movzbl 0xe8(%eax)）
                LogManager::logFormat(
                    1, "battle_field.cpp", __PRETTY_FUNCTION__,
                    0x649,
                    "Unknown APC Type(%d). In Dungeon(%d), Map(%d) [Map Kind : %s]",
                    arrange.m_type, m_pDungeon->get_index(), m_pMap->get_index(),
                    GetCurrentMapInfo()->m_layeredMap ? "layeredMap" : "generalMap");
                continue;
            }

            // ORIG 在每次成功取表后 memset 整个 map_monster（0x34c 字节）复位；
            // 默认构造后的空 vector 内部为全零，memset 不改变其可析构状态。
            std::memset(&mob, 0, sizeof(map_monster));
            mob.m_mobId = apcIdx;
            mob.m_mobIndex = arrange.m_type;
            mob.m_instId = seq++;
            mob.m_roleType = static_cast<char>(arrange.m_roleType);        // arrange+0x14
            if (mob.m_roleType == 8 && !GetCurrentMapInfo()->m_dummyBossMap)
            {
                m_apcList.push_back(mob.m_instId);
            }
            // P226/P225：ORIG 0x08301f93 `test %al,%al; setg %al; test %al,%al; je`（比较
            // 结果零槽物化）⇒ 语句表达式包住条件、if/else 分臂（三目被 4.4 折成 `jle` 直跳）。
            if (({ static_cast<signed char>(
                       m_pDungeon->get_dimension_possible()) > 0; }))
                mob.m_level = static_cast<char>(m_standardDimensionLevel);
            else
                mob.m_level = static_cast<char>(ai->getLevel());
            mob.m_dropCnt = arrange.m_camp;                                // arrange+0x10 → +0x2c
            // mob+0x11(低)←posY、+0x12(高)←posX（ORIG 逐字节写）
            mob.m_field11b.m_field11b0 = static_cast<unsigned char>(arrange.m_posY);
            mob.m_field11b.m_field11b1 = static_cast<unsigned char>(arrange.m_posX);

            GetCurrentMapInfo()->Add_Mob(mob);
            ++apcIdx;
        }
    }
}