void SkillSlot::calcReturnSpSfpPerOneSkill(int& sp, int& sfp, int level,
                                           const _Mastered_skill* buf,
                                           int slot, const CSkill* skill,
                                           bool isSteal) const
{
    int need = 0;
    int fitness = 0;
    for (int i = level + 1; buf[slot].m_level >= i; ++i)
    {
        fitness = skill->get_second_growtype_fitness(
            (int)((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType(),
            (int)((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType());
        if (fitness == 0)
        {
            fitness = skill->get_fitness(
                (int)((CUserCharacInfo*)m_parent)
                    ->getCurCharFirstGrowType());
        }
        need = skill->get_need_sp_sfp_for_level(i - 1, i);
        if (need < 0)
        {
            break;
        }
        if (0 < fitness)
        {
            need = ((100 - fitness) * need) / 100;
        }
        if (isSteal)
        {
            // b28d-36（P0）：ORIG 无 penalty 具名槽（0x086066xx 段 ebx/eax 直传）
            need = getSkillSpendSPOnSteal(need, skill->GetStealSPPenalty());
        }
        if (need < 0)
        {
            return;
        }
        if (skill->IsSpecialSkill())
        {
            sfp = sfp + need;
        }
        else
        {
            sp = sp + need;
        }
    }
