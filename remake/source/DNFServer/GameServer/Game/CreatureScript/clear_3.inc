void AttackInfo::clear()
{
    m_damage = 0.0f;
    m_absoluteDamage = 0;
    m_damageBonus = 0;
    m_attackType = (AttackInfo::ENUM_ATTACKTYPE)0;
    m_weaponDamageApply = 0;
    m_humanActiveStatusRate = 1.0f;
    m_damageReaction = 1;
    m_pushAside = 0;
    m_liftUp = 0;
    m_criticalHit = 0.0f;
    m_stuck = 0.0f;
    m_stuckProbIncrease = 0;
    m_knuckBack = 0;
    m_knuckBackAngle = 0;
    m_weakness = 0.0f;
    m_weakness2 = 0.0f;
    m_bounce = 0;
    m_ignoreDefense = 0;
    m_hitWav = "";
    m_attackDirection = 1;
    m_hitInfo = 5;
    m_blood = 0;
    m_field5c = -1;
    m_field60 = -1.0f;
    m_isUseWeaponHitDelayRate = 0;
    m_hitDirection = 0;
    m_ignoreSuperArmor = 0;
    m_ignoreWeight = 0;
    m_hpDrain = 0;
    m_mpDrain = 0;
    m_field80 = 0.0f;    // ORIG 0x88904e3 mov $0x0,%edx;mov %edx,0x80（float 0 中转）
    m_field84 = 100.0f;  // ORIG 0x88904f2 mov $0x42c80000,%edx;mov %edx,0x84（float 100）
    m_isHitNumberIncreasing = 1;
    m_forceHitStunTime = 0;
    m_moduleTag = 3;
    m_ignoreDiehard = 0;
    m_hitCheckByScreenPos = 0;
    // ORIG 循环1（0x98+i*4 = (i==2)：m_attackFlags[0..2]）
    for (int i = 0; i < 3; ++i)
        m_attackFlags[i] = (i == 2);
    m_elements.clear();
    // ORIG 循环2：type/value/int/int2/int3 + vector 清空
    for (int i = 0; i < 3; ++i)
    {
        m_activeStatusType[i] = 0x12;
        m_activeStatusValue[i] = 0.0f;
        m_activeStatusInt[i] = 0;
        m_activeStatusInt2[i] = 0;
        m_activeStatusInt3[i] = 0;
        m_activeStatusVectors[i].clear();
    }
    // ORIG 循环3：damageRate = 100.0f
    for (int i = 0; i < 3; ++i)
        m_damageRate[i] = 100.0f;
    m_maxTargetHitNumber = -1;
    m_catchObjects.clear();
