// ORIG 0x089193c0（0x928 字节）：标量清零 + 数组循环清零 + 容器 clear。
// 所有已解析的数组/标量均通过 EquipmentParameterInfo 具名字段访问；重复清零
// 保留 ORIG 的最终状态，字段类型与偏移以 EquipmentParameterInfo.h 为准。
// [b28d-solo-EquipmentParameterInfo] clear：✅ 离开 DIFF（identical/AE）——5 组标量 union
// 视图 + ORIG 循环结构（含 P191 双初始化共用循环变量 i）+ 补漏清
// m_allActiveStatusResistance（真语义修正）；506=506、P167 CLEAR 20/20。
void EquipmentParameterInfo::clear()
{
    m_field0 = 2;
    m_hpMax = 0;
    m_hpMaxRate = 0;
    m_mpMax = 0;
    m_mpMaxRate = 0;
    m_physicalAttack = 0;
    m_physicalDefense = 0;
    m_magicalAttack = 0;
    m_20 = 0;
    m_magicalDefense = 0;
    m_growthLevelBegin = 0;
    m_growthLevelEnd = 0;
    for (int i = 0; i <= 1; ++i)
    {
        m_linPhyAtk[i] = 0;
        m_linPhyDef[i] = 0;
        m_linMagAtk[i] = 0;
        m_linMagDef[i] = 0;
    }
    m_50 = 0;
    for (int i = 0; i <= 1; ++i)
    {
        m_phyAtt[i] = 0;
        m_phyDef[i] = 0;
        m_magAtt[i] = 0;
        m_magDef[i] = 0;
    }
    m_physicalAbsoluteDamage = 0;
    m_physicalDamageReduce = 0;
    m_physicalAbsoluteDefense = 0;
    m_magicalAbsoluteDamage = 0;
    m_magicalDamageReduce = 0;
    m_magicalAbsoluteDefense = 0;
    m_94 = 0;
    m_98 = 0;
    m_9c = 0;
    m_a0 = 0;
    m_a4 = 0;
    m_a8 = 0;
    m_ac = 0;
    // [b28d-solo-EquipmentParameterInfo] ORIG 0x89195b3：这 4 个元素走 i=0..3 循环
    // （`add $0x2c,%edx; movl $0,(%eax,%edx,4)`），非逐字段。
    // ORIG 该循环变量为函数级具名局部：声明处双初始化（0x89195a3/0x89195aa 两条
    // `movl $0,-0x28`）+ 后续 17 元素/3 float/m_levelUpSkills 三个循环**复用同一槽**
    // ⇒ 四者共用一个变量（P191 双初始化形 + P0 槽往返）。
    int i = 0;
    for (i = 0; i <= 3; ++i)
        m_elementResistance[i] = 0;
    m_allElementalResistance = 0;
    // [b28d-solo-EquipmentParameterInfo] ORIG 0x89195e8：17 元素走 i=0..16 循环
    // （`add $0x30,%edx; movl $0,0x4(%eax,%edx,4)`）。
    for (i = 0; i <= 16; ++i)
        m_statusResistance[i] = 0;
    // [b28d-solo-EquipmentParameterInfo] 真语义修正（漏清）：ORIG 0x891960b
    // `movl $0x0,0x108(%eax)` 清 [all activestatus resistance]，旧实现整段缺失。
    m_allActiveStatusResistance = 0;
    m_inventoryLimit = 0;
    m_hpRegenSpeed = 0;
    m_mpRegenSpeed = 0;
    m_mpRegenSpeedRate = 0;
    m_moveSpeed = 0;
    m_xMoveDashSpeed = 0;
    m_yMoveDashSpeed = 0;
    m_attackSpeed = 0;
    m_castSpeed = 0;
    m_hitRecovery = 0;
    m_jumpPower = 0;
    m_physicalCriticalHit = 0;
    m_magicalCriticalHit = 0;
    m_elementalProperty = 4;
    m_activeStatusType = 0x12;
    m_activeStatusValue = 0;
    m_activeStatusLevel = 0;
    m_activeStatusFlag = 0;
    m_activeStatusValue2 = 0;
    m_vec158.clear();
    m_attackedActiveStatusType = 0x12;
    m_attackedActiveStatusValue = 0;
    m_attackedActiveStatusLevel = 0;
    m_attackedActiveStatusFlag = 0;
    m_attackedActiveStatusValue2 = 0;
    m_vec17c.clear();
    m_164 = 0;
    m_vec240.clear();
    m_164 = 0;
    m_attackedActiveStatusValue = 0;
    m_attackedActiveStatusLevel = 0;
    m_attackedActiveStatusFlag = 0;
    m_attackedActiveStatusValue2 = 0;
    m_vec17c.clear();
    m_aura188.clear();
    m_vec194.clear();
    m_addAbsoluteDamageType = 4;
    m_addAbsoluteDamageValue = 0;
    m_addAbsoluteDamageExtra = 0;
    m_addAbsoluteDefenseType = 4;
    m_addAbsoluteDefenseValue = 0;
    m_addAbsoluteDefenseExtra = 0;
    m_pushAside = 0;
    m_liftUp = 0;
    m_stuck = 0;
    m_ignoreDefense = 0;
    // [b28d-solo-EquipmentParameterInfo] ORIG 0x8919887：3 个 float 走 i=0..2 循环
    // （`lea 0x8c(%edx),%ecx; mov $0,%edx; mov %edx,(%eax,%ecx,4)`）。
    for (i = 0; i <= 2; ++i)
        m_damageRate[i] = 0.0f;
    m_hpDrain = 0;
    m_mpDrain = 0;
    for (i = 0; i <= 1; ++i)
        m_levelUpSkills[i].clear();
    m_avatar264.clear();
    m_270 = -1;
    m_roomListMoveSpeedRate = 0;
    m_str278 = "";
    m_str27c = "";
    m_antiEvil = 0;
    m_rigidity = 0;
    // [b28d-solo-EquipmentParameterInfo] ORIG 0x8919973：4 个 attack 走 i=0..3 循环
    // （`add $0xa0,%edx; movl $0,0x8(%eax,%edx,4)`）。
    for (int i = 0; i <= 3; ++i)
        m_elementAttack[i] = 0;
    for (int i = 0; i <= 17; ++i)
    {
        m_activeStatusProbability[i] = 0;
        m_activeStatusLevelTable[i] = 0;
        m_activeStatusCtrl[i] = 0;
    }
    for (int i = 0; i <= 4; ++i)
        m_activeStatusControlInfo[i] = 0;
    m_separateAttack[0] = 0;
    m_separateAttack[1] = 0;
    m_separateRate[0] = 1.0f;
    m_separateRate[1] = 1.0f;
    m_398 = 0;
    m_39c = 0;
    m_414 = 0;
    // [b28d-solo-EquipmentParameterInfo] ORIG 0x8919a7c：第二次 i=0..1 循环，同时清
    // 4 个 linear 数组与 growthGrade/growthRepair（`add $0xf8,%edx` → +0x3e0、
    // `add $0x100,%edx; movl $0,0x8(...)` → +0x408）。
    for (int i = 0; i <= 1; ++i)
    {
        m_linPhyAtk[i] = 0;
        m_linPhyDef[i] = 0;
        m_linMagAtk[i] = 0;
        m_linMagDef[i] = 0;
        m_growthGrade[i] = 0;
        m_growthRepair[i] = 0;
    }
    for (int i = 0; i <= 3; ++i)
    {
        m_linEqPhyAtk[i] = 0;
        m_linEqPhyDef[i] = 0;
        m_linEqMagAtk[i] = 0;
        m_linEqMagDef[i] = 0;
        m_linSepAtk[i] = 0;
        m_linSepRate[i] = 1.0f;
    }
    m_410 = 0;
    m_skill1b8.clear();
    m_descMap1c4.clear();
    m_skill1dc.clear();
    m_skill1e8.clear();
    m_vec1f4.clear();
    m_aura200.clear();
    m_vec214.clear();
    m_decreaseCriticalDamage = 0;
    m_210 = 0;
    m_119.m_424 = 0;
    m_119.m_425 = 0;
    m_434 = 0;
    m_438 = -1;
    m_43c = -1;
    m_upgradeCostDiscount = 0;
    m_upgradeProbIncrease = 0;
    m_assaultCostDiscount = 0;
    m_allElementalAttack = 0;
    m_8c = 0;
    m_90 = 0;
    m_rebirth450.clear();
    m_vec468.clear();
    m_var474.clear();
    m_440 = 0;
}