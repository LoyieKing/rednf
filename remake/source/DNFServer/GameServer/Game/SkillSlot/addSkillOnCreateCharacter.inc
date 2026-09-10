void addSkillOnCreateCharacter(_Mastered_skill* skill, int job)
{
    // [b28d-solo-SkillSlot] 修法（AE）：ORIG 0x08604fe2 形 = 两条独立守卫
    // (`cmpl $0xa; jg` / `cmpl $0; js`，P63 中继 nop/jmp)、按值 vector 拷贝
    // (C1ERKS3_ + D1Ev + EH cleanup，非引用)、size()+operator[] 索引遍历
    // (P175/P157，逐字段重发 op[])、foundEmpty 声明于循环体首、内层扫描
    // `for (j = start; j < end; ++j)`（与 end 槽比较，非立即数 6）、
    // default 分支重调 s->get_group()（P48 双臂独立调用）。
    if (job > 10)
    {
        return;
    }
    if (job < 0)
    {
        return;
    }
    std::vector<std::pair<int, int> > skillList =
        *(std::vector<std::pair<int, int> >*)(
            (char*)G_CDataManager()->m_characList + job * 0x7dc + 0x240);
    int skillIdx;
    int level;
    for (int i = 0; i < (int)skillList.size(); ++i)
    {
        bool foundEmpty = false;
        skillIdx = skillList[i].first;
        level = skillList[i].second;
        CSkill* s = G_CDataManager()->find_skill(job, skillIdx);
        if (s == 0)
        {
            continue;
        }
        int start = 0;
        int end = 0;
        if (s->is_active_skill())
        {
            start = 0;
            end = 6;
            for (int j = start; j < end; ++j)
            {
                if (skill[j].m_index == 0)
                {
                    foundEmpty = true;
                    break;
                }
            }
        }
        if (!foundEmpty)
        {
            switch (s->get_group())
            {
            case 0:
                start = 6;
                end = 0x36;
                break;
            case 1:
                start = 0x36;
                end = 0x66;
                break;
            case 2:
                start = 0x66;
                end = 0x96;
                break;
            case 3:
                start = 0x96;
                end = 0xc6;
                break;
            default:
                cMyTrace(__PRETTY_FUNCTION__, 0x549, 5)(
                    "add skill no group %d", s->get_group());
                break;
            }
        }
        for (int k = start; k < end; ++k)
        {
            if (skill[k].m_index == 0)
            {
                skill[k].m_index = (unsigned char)skillIdx;
                skill[k].m_level = (char)level;
                break;
            }
        }
    }
