bool CEventCharacterHandler::_MasterNewSkill(CUser* user, int skillIdx, int count)
{
    // [b28d-solo-CEventCharacterHandler] ORIG 0x0848e7d6 用 `movzbl 0x1(%eax)`
    // （disp 折叠）。旧判词「源形不可择」作废：buf 声明为数组指针
    // `unsigned char (*)[2]` 后 4.4 复现 `mov slotNo,%eax; add %eax,%eax;
    // add buf,%eax; movzbl 1(%eax)`（/tmp/s107/m3.cpp k1/k2 探针实证）。
    // ORIG 0x0848e7d6：skill 两段式（入口 movl $0,-0x18 预清）；job 内联
    // rvalue 经 %ebx 跨 G_CDataManager call（P197②）；getCurCharacSkillR
    // 两独立重调（+0x5a/+0x8e，P48）。
    const CSkill* skill = 0;
    skill = (const CSkill*)G_CDataManager()->find_skill(user->get_charac_job(),
                                                        skillIdx);
    if (skill == 0)
    {
        return false;
    }

    int group = skill->get_group();
    int slotNo = ((SkillSlot*)user->getCurCharacSkillR())->get_skillslot_no(
        skillIdx, group, ENUM_SKILL_TREE_KIND_0, true);
    unsigned char (*buf)[2] = (unsigned char (*)[2])
        ((SkillSlot*)user->getCurCharacSkillR())->get_skillslot_buf(
            ENUM_SKILL_TREE_KIND_0);
    if (buf == 0)
    {
        return false;
    }

    stBuySkillInfo info;
    info.m_field0 = 0;
    info.m_skillIdx = (char)skillIdx;
    // ORIG +0xc5：`add -0xc(buf),%eax; movzbl 0x1(%eax)`——(buf+2*slotNo) 先行、
    // +1 留 disp（数组指针视图触发 4.4 disp 折叠）。
    info.m_count = (char)((char)count - (char)buf[slotNo][1]);
    if ((signed char)info.m_count <= 0)
    {
        return false;
    }

    user->master_new_skill(info, true);
    return true;
}