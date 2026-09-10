void Inter_RequestCharacSkillInfo::_PutSkillInfoToCharac(
    char slotFlag, const _Mastered_skill* skill, SkillSlot* skillSlot)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 31/31：size 具名局部改 **unsigned int**
    // （P211 档③/P274 同异型旋钮）⇒ 4.4 实参装载序翻为 dest-first 且成员偏移取址
    // 恢复单条 `lea 0x46(%eax),%edx`（原 `mov;add` 两段 + size-first 序为 int 型固有）。
    // 探针 /tmp/b28d_ile/probe2.cpp g1(SS*,const MS*,unsigned) 逐条同构。
    unsigned int size = 0x198;
    if (slotFlag == -1 || slotFlag == 0)
    {
        memcpy(skillSlot->m_skillSlot, skill, size);
    }
    else if (slotFlag == 1)
    {
        memcpy(skillSlot->m_skillSlot2, skill, size);
    }
