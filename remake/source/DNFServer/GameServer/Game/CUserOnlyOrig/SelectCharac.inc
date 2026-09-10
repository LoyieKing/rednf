int CUser::SelectCharac(int idx, int tagIdx)
{
    if (getCurCharacR() != 0) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x11fb, 5);
        trace("CUser::SelectCharac m_selected is not null");
        return 0;
    }
    // [L6 判据⑫/P0] ORIG 0x0864cd92 段为 `mov $0x0/%eax; test %al,%al; je`（值在 %al，
    // 无栈槽）vs 我方 `mov %al,-0x3f(%ebp); cmpb $0x0,-0x3f(%ebp)`（多槽物化）⇒
    // 原码无具名 bool 局部（idxValid/tagValid 各占一槽 -0x3f/-0x43）⇒ 条件内联（P0）。
    if ((idx >= 0) && (static_cast<int>(m_characList.size()) > idx)) {
        setCurCharac(&m_characList[idx]);
        if ((tagIdx >= 0) && (static_cast<int>(m_characList.size()) > tagIdx)) {
            setTagCharac(&m_characList[tagIdx]);
            const CInventory* tagInven =
                reinterpret_cast<const CInventory*>(getTagCharacInvenR());
            const_cast<CInventory*>(tagInven)->set_parent(this);
            const SkillSlot* tagSkill =
                reinterpret_cast<const SkillSlot*>(getTagCharacSkillR());
            const_cast<SkillSlot*>(tagSkill)->set_parent(this);
        }
        if (getCurCharacR() == 0) {
            cMyTrace trace2(__PRETTY_FUNCTION__, 0x120c, 5);
            trace2("CUser::SelectCharac m_selected is null at %d", idx);
            return 0;
        }
        if (getCurCharacR()->m_stamina < 100) {
            // [Lane6 2026-09-10 真语义 bug 修复] ORIG 0x0864cf45..0x0864d02c 逐条：
            // `limits[0]=600(0x258)`@-0x68 → curSec@-0x28 → lastTick@-0x24 → difftime→double@-0x20
            // → `limits[1]=(int)elapsed`@-0x6c → `int zero=0`@-0x44 →
            // `limits[1]=max(zero,min(limits[0],limits[1]))`（min 结果经 eax 直入 max 第 2 参、
            // 终值存回 -0x6c）→ svals[0]@-0x40 = stamina + (limits[1]*100)/600、svals[1]=100@-0x3c
            // → setCurCharacStamina(min(svals[1],svals[0]))。
            // 旧译文：读**未初始化** limits[1]、漏 `limits[0]=600`、并把结果写回 limits[0]
            // ⇒ 疲劳恢复量计算错误（真 bug）；且多出 minv/limits 缓存槽（cache-slot 扫描命中）。
            int limits[2];
            limits[0] = 600;
            unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
            long lastTick = getCurCharacLastPlayTick();
            double elapsed = difftime(static_cast<time_t>(curSec),
                                      static_cast<time_t>(lastTick));
            limits[1] = static_cast<int>(elapsed);
            int zero = 0;
            limits[1] = std::max(zero, std::min(limits[0], limits[1]));
            int svals[2];
            svals[0] = static_cast<int>(getCurCharacR()->m_stamina)
                       + (limits[1] * 100) / 600;
            svals[1] = 100;
            setCurCharacStamina(
                static_cast<unsigned char>(std::min(svals[1], svals[0])));
        }
        char growType = static_cast<char>(getCurCharacGrowType());
        int level = get_charac_level();
        int job = get_charac_job();
        const char* name = getCurCharacName();
        HistoryLog::WriteSelectCh(*reinterpret_cast<FILE* const*>(&m_field796f8),
                                  name, job, level, static_cast<int>(growType));
        const CInventory* curInven = getCurCharacInvenR();
        const_cast<CInventory*>(curInven)->set_parent(this);
        const SkillSlot* curSkill =
            reinterpret_cast<const SkillSlot*>(getCurCharacSkillR());
        const_cast<SkillSlot*>(curSkill)->set_parent(this);
        UserQuest* quest = getCurCharacQuestR();
        quest->set_parent(this);   // 声明已由 Main 代落 CUser.h（ORIG W _ZN10UserQuest10set_parentEP5CUser）
        const CCargo* curCargo = getCurCharacCargoR();
        const_cast<CCargo*>(curCargo)->set_parent(this);
        job = get_charac_job();
        G_GameWorld()->m_channelUserCount[job] += 1;
        return 1;
    }
    cMyTrace trace3(__PRETTY_FUNCTION__, 0x1246, 5);
    trace3("CUser::SelectCharac :charac_index : %d", idx);
    return 0;
