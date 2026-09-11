// ORIG 0x083026da：直接遍历 GetCurrentMapInfo()->m_monsterMap（+0xc）原图，
// 能力写入落在真实地图怪物上；srand/冠军分支均重新 GetCurrentMapInfo。
// [b28d-solo-BattleMonsterDepsC2] P13/P196（已修 206→209）：ORIG begin/end 各调一次并缓存
// （-0x2c/-0x30）；条件 `movzbl 0x8; cmp $0x4,%al; jg` 为**有符号**比较 ⇒
// (signed char)mm.m_roleType > 4（unsigned 版产 ja 反极性）。
// [SweepBattleMonster 2026-09-09] 已修（213=213，clean 210=210，DIFF→near）：P0 删具名
// `ActiveStaticInfo* active`（ORIG 每处 `lea 0x38(%eax)` 重算，无具名槽）⇒ 槽图整体回正。
// 残差仅 `movsbl` 落 %eax vs ORIG %edx（N7 寄存器角色）。
void CBattle_Field::set_monster_ability()
{
    if (m_pDungeon == NULL)
        return;
    MapInfo* mapInfo = GetCurrentMapInfo();
    mapInfo->m_mapBasisLevel = rand();                                    // +0x48
    srand(static_cast<unsigned int>(GetCurrentMapInfo()->m_mapBasisLevel));

    std::map<int, map_monster>::iterator it =
        GetCurrentMapInfo()->m_monsterMap.begin();
    std::map<int, map_monster>::iterator itEnd =
        GetCurrentMapInfo()->m_monsterMap.end();
    for (; it != itEnd; ++it)
    {
        map_monster& mm = it->second;
        if (static_cast<signed char>(mm.m_roleType) > 4)     // ORIG movzbl + jg
            continue;
        CMonster* monster =
            static_cast<CMonster*>(G_CDataManager()->find_monster(mm.m_mobIndex));
        if (monster == NULL)
            continue;

        // [b28d-D9] P167 差集：ORIG 0x083026da 段 getMonsterScript 调用 3 次
        // （@+0x49/@+0x5c/@+0x89，无具名 script 局部）⇒ 各使用点内联重取（P0/P183）。
        // P0：ORIG 无 active 具名槽（每处 `lea 0x38(%eax)` 重算，仅首调用存 -0x4c 实参临时）
        // ⇒ 全部使用点写 `&mm.m_active`。
        setBasicAbilityByLevel(
            static_cast<STMonsterScript*>(monster->getMonsterScript()),
                               &mm.m_active,
                               static_cast<int>(static_cast<unsigned char>(mm.m_level)),
                               m_dungeonIndex,
                               static_cast<int>(static_cast<signed char>(mm.m_roleType)),
                               false, false);
        upgradeAbilityByMonsterParameterCategory(
            &static_cast<STMonsterScript*>(monster->getMonsterScript())
                 ->m_parameterCategories,
            &mm.m_active, mm.m_roleType == 3);
        upgradeAbilityByDifficultyOfDungeon(get_dungeon_diff(), &mm.m_active,
                                            static_cast<int>(
                                                static_cast<unsigned char>(mm.m_level)),
                                            m_pDungeon->m_vec900);
        if (mm.m_roleType == 3)
        {
            upgradeAbilityByBossFeature(&mm.m_active);
        }
        else if (mm.m_roleType == 1 || mm.m_roleType == 2)
        {
            const int elem = getRandomChampionElement(
                static_cast<STMonsterScript*>(monster->getMonsterScript()),
                GetCurrentMapInfo()->m_mapBasisLevel);
            std::vector<int> features;
            makeChampionFeatureList(mm.m_mobList,
                                    static_cast<unsigned char>(mm.m_level),
                                    GetCurrentMapInfo()->m_mapBasisLevel,
                                    features, get_dungeon_diff());
            // [S6 2026-09-11] 残差 N7：ORIG 0x08302a?? 段 `movzbl 0x8(%eax),%eax;
            // movsbl %al,%edx`（%edx 装转换结果）+ 压栈前 `mov -0x1c(%ebp),%eax`
            // （实参 elem 先读）；4.4 用 %eax 装 movsbl 结果、elem 最后读（指令数
            // 全等 210=210，ndiff 16）。显式 `static_cast<int>(static_cast<signed
            // char>(...))` 旋钮实测无效（已回滚）⇒ 分配差异判 N7。
            upgradeAbilityByChampionFeature(elem,
                                            static_cast<signed char>(mm.m_roleType),
                                            &features, &mm.m_active);
        }
    }
}