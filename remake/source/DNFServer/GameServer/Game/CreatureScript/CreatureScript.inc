// ORIG 0x833e5ac：构造全部字符串/向量/映射成员（自动成员构造）后调 clear()。
CreatureScript::CreatureScript()
    : m_name(), m_basicMotion(), m_walkMotion(), m_runMotion(), m_flyMotion(),
      m_getExpMotion(), m_getExpOverMotion(), m_getExpUnderMotion(),
      m_skillMotion(), m_overSkillMotion(), m_responseMotion(), m_specialMotion(),
      m_equipMotion(), m_equipOverMotion(), m_equipUnderMotion(),
      m_basicOverEffect(), m_walkOverEffect(), m_runOverEffect(),
      m_defaultOverEffect(),
      m_skillOverEffect(), m_overSkillOverEffect(), m_responseOverEffect(),
      m_specialOverEffect(), m_basicUnderEffect(), m_walkUnderEffect(),
      m_runUnderEffect(), m_defaultUnderEffect(), m_skillUnderEffect(),
      m_overSkillUnderEffect(), m_responseUnderEffect(), m_specialUnderEffect(),
      m_additionalAnimations(), m_defaultParticle(), m_responseSoundTag(),
      m_wingSoundTag(), m_walkSoundTag(), m_runSoundTag(),
      m_etcMotion(), m_attackInfo(), m_skillString(), m_skillLevelValues(),
      m_overSkillLevelValues(), m_piercing(), m_intData(), m_stringData(),
      m_directory(), m_evolutionMultyId(), m_selectiveGrow(), m_randomMotion(),
      m_itemEvolCreatureIds(), m_skillInfo(), m_overSkillInfo(), m_skillList(),
      m_overSkillList(), m_eventEvlouteCreature()
{
    // ORIG 0x833e5ac：仅构造类类型成员（标量未初始化，由 clear() 复位）。
    clear();
}