bool CSkillChanger::_fnEnumQuestClear(unsigned int questIdx, char* args)
{
    // b28d-L3 复测（P0/P183）：ORIG 0x0860a6da 为**形参自增指针**形 —— 每个槽
    //   `mov 0xc(%ebp),%eax; lea 0x4(%eax),%edx; mov %edx,0xc(%ebp); mov (%eax),%eax;
    //    mov %eax,-0x30…(%ebp)`（6 连，偏移步进 4）⇒ 直接对 args 做 `+= 4` 步进，
    //   不设 `slots` 中间层（旧形多一具名槽 -0x34 且读形为 `mov -0x34(%ebp),%eax;
    //   add $0x4,%eax`）。
    int* spA = *(int**)args;
    args += 4;
    int* spB = *(int**)args;
    args += 4;
    int* sfpA = *(int**)args;
    args += 4;
    int* sfpB = *(int**)args;
    args += 4;
    CSkillChanger* self = *reinterpret_cast<CSkillChanger**>(args);
    args += 4;
    CUser* user = *reinterpret_cast<CUser**>(args);

    Quest const* quest = G_CDataManager()->find_quest(static_cast<int>(questIdx));
    if (quest == 0)
        return true;
    if (quest->m_fieldB8 != 0 && quest->m_fieldB8 != 0x19)
        return true;

    // ORIG 0x0860a781：`movl $0x0,-0x14(%ebp)` 预清后再 call（P198① 两句式）。
    std::vector<RewardItemInfo>* rewards = 0;
    rewards = user->getUserQuestRewardItem(quest, user);   // ORIG _ZN5CUser22…PKS_
    for (std::vector<RewardItemInfo>::const_iterator it = rewards->begin();
         it != rewards->end(); ++it)
    {
        const int itemIdx = it->m_itemIdx;   // +0x00
        const int count = it->m_count;       // +0x04
        self->_addSPRelatedItem(spA, spB, itemIdx, count);
        self->_addSFPRelatedItem(sfpA, sfpB, itemIdx, count);
    }
    return true;
