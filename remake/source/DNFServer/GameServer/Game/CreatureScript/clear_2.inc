// ORIG 0x833dfac：复位全部标量 + clear 各容器。
void CreatureScript::clear()
{
    m_evolutionMultyId.clear();
    m_id = 0;
    m_name = "";
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_layer = 1;
    m_field18 = (int)0xffffffce;
    m_field1c = 0;
    m_field20 = 0;
    m_directionCorrect = 1;
    // [b28d-solo] P197③+P191：ORIG 0x0833dfac 在 m_directionCorrect 之后有**两条**
    // 连续 `movl $0,-0xc` = `int i = 0;`（声明+初始化，发射点即声明点）与
    // `for (i = 0; ...)` 各一条；两个循环共用同一槽 -0xc。
    int i = 0;
    // ORIG 0x833dfac：循环 i=0..1 清 m_moveSpeed[i] 与 m_moveSpeed[i+2]
    // （覆盖 +0x28..+0x34），再单独清 +0x38（moveSpeed[4]）。
    for (i = 0; i <= 1; ++i)
    {
        m_moveSpeedLo[i] = 0;
        m_moveSpeedHi[i] = 0;
    }
    m_moveSpeedHi[2] = 0;
    m_gravity = 1500;
    m_startLevel = 1;
    m_permissionLevel = 1;
    m_maxLevel = 1;
    m_parentMaxLevel = 1;
    // ORIG 0x833dfac：循环 i=0..2 清 m_artifactSlotRed/Blue/Green（+0x50..+0x58）。
    for (i = 0; i <= 2; ++i)
        m_artifactSlots[i] = 0;
    m_learnOverSkillLevel = 1;
    m_skillMP = 0;
    m_overSkillMP = 0;
    m_skillType = 0;
    m_overSkillType = 0;
    m_basicMotion = "";
    m_walkMotion = "";
    m_runMotion = "";
    // ORIG clear 不复位 m_flyMotion（+0x7c）
    m_skillMotion = "";
    m_overSkillMotion = "";
    m_responseMotion = "";
    m_specialMotion = "";
    m_basicOverEffect = "";
    m_walkOverEffect = "";
    m_runOverEffect = "";
    // ORIG clear 不复位 m_defaultOverEffect（+0xb4）
    m_skillOverEffect = "";
    m_overSkillOverEffect = "";
    m_responseOverEffect = "";
    m_specialOverEffect = "";
    m_basicUnderEffect = "";
    m_walkUnderEffect = "";
    m_runUnderEffect = "";
    // ORIG clear 不复位 m_defaultUnderEffect（+0xd4）
    m_skillUnderEffect = "";
    m_overSkillUnderEffect = "";
    m_responseUnderEffect = "";
    m_specialUnderEffect = "";
    // [b28d-solo] ORIG 0x833e2f6..0x833e37f：getExp 组（+0x80/0x84/0x88）与
    // equip 组（+0x9c/0xa0/0xa4）在 effects 组之后才写（源码序=发射序）。
    m_getExpMotion = "";
    m_getExpOverMotion = "";
    m_getExpUnderMotion = "";
    m_equipMotion = "";
    m_equipOverMotion = "";
    m_equipUnderMotion = "";
    m_additionalAnimations.clear();
    m_defaultParticle = "";
    m_responseSoundTag = "";
    m_wingSoundTag = "";
    m_walkSoundTag = "";
    m_runSoundTag = "";
    m_revisionX = 0;
    m_revisionY = 0;
    m_etcMotion.clear();
    m_attackInfo.clear();
    m_skillString.clear();
    m_skillLevelValues.clear();
    m_overSkillLevelValues.clear();
    m_piercing.clear();
    m_intData.clear();
    m_stringData.clear();
    m_directory = "";
    m_evolutionQuestId = 0;
    m_evolutionCreatureId = 0;
    m_evolutionLevel = 0;
    // [b28d-solo] ORIG 0x0833e4e5..0x833e591 尾段顺序：randomMotion→标量→
    // itemEvolCreatureIds→skillInfo/overSkillInfo→**usingRandomSkill**→skillList/
    // overSkillList→**selectiveGrow**→eventEvlouteCreature。
    m_randomMotion.clear();
    m_defaultRandomMotionRate = 0;
    m_increaseRandomMotionRate = 0;
    m_itemEvolution = 0;
    m_itemEvolCreatureIds.clear();
    m_skillInfo.clear();
    m_overSkillInfo.clear();
    m_usingRandomSkill = 0;
    m_skillList.clear();
    m_overSkillList.clear();
    m_selectiveGrow.clear();
    m_eventEvlouteCreature.clear();
}