// ---------------------------------------------------------------------------
// CUser::SelectCharac(int idx, int tagIdx) T 0x0864cd92 sz0x3cd
// 报告：docs/class_func_reports/CUser/SelectCharac.md
// 选择主角色/tag 角色：设 curCharac/set_parent 链、stamina 离线恢复
//（elapsed=min(600,difftime); min(stamina+elapsed*100/600,100)）、
// HistoryLog::WriteSelectCh、GameWorld::m_channelUserCount[job]++。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864cd92：p167=CLEAR，gap=16；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864cd92：N82 封顶——帧/槽差 + x87（p167 CLEAR，gap 16）。
// [K7 2026-09-11 §6.5 PRETTY 串盲区] ORIG .rodata 实串 = `bool CUser::SelectCharac(int, int)`
// ⇒ 返回类型 bool（同 CUser.h:3047，K4CUser 同刻落头）。
bool CUser::SelectCharac(int idx, int tagIdx)
{
    if (getCurCharacR() != 0) {
        // [FL5 2026-09-10 负结论·已回滚] 全表达式临时链（P37/P227）在本件 ndiff 278→278 中性
        // （ours 280 不变）⇒ 不达标，维持两语句具名形；勿再试。
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
            // [SubC a-1 2026-09-11] ORIG 0x0864cea0-0x0864cec9 无指针中转槽（直用 %eax）；
            // 我方两个具名 const 指针各占 -0x20/-0x1c（帧 sub $0xb4 vs ORIG $0x9c = 6 槽 0x18）。
            const_cast<CInventory*>(
                reinterpret_cast<const CInventory*>(getTagCharacInvenR()))->set_parent(this);
            const_cast<SkillSlot*>(
                reinterpret_cast<const SkillSlot*>(getTagCharacSkillR()))->set_parent(this);
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
            // [SubC a-3 2026-09-11] ORIG 0x0864cf9d/0x0864cfa4/0x0864d006/0x0864d00d 四条取址
            // 皆单条 lea；我方 `limits[2]/svals[2]` 的 `[1]` 元素被 4.4 拆成 `lea base; add $4`。
            int limitMax = 600;
            unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
            long lastTick = getCurCharacLastPlayTick();
            double elapsed = difftime(static_cast<time_t>(curSec),
                                      static_cast<time_t>(lastTick));
            int elapsedSec = static_cast<int>(elapsed);
            int zero = 0;
            elapsedSec = std::max(zero, std::min(limitMax, elapsedSec));
            int staminaNew = static_cast<int>(getCurCharacR()->m_stamina)
                             + (elapsedSec * 100) / 600;
            int staminaMax = 100;
            setCurCharacStamina(static_cast<unsigned char>(
                std::min(staminaMax, staminaNew)));
        }
        char growType = static_cast<char>(getCurCharacGrowType());
        int level = get_charac_level();
        int job = get_charac_job();
        const char* name = getCurCharacName();
        // [SubC a-4 2026-09-11] ORIG 0x0864d05f `mov 0x8(%ebp),%edx; mov 0x796f8(%edx),%edx`
        // = 直读字段；`*reinterpret_cast<FILE* const*>(&m_field796f8)` 产 `add $0x796f8,%eax` 间接形。
        HistoryLog::WriteSelectCh(reinterpret_cast<FILE*>(m_field796f8),
                                  name, job, level, static_cast<int>(growType));
        // [SubC a-1 2026-09-11] ORIG 0x0864d080-0x0864d0e3 同样无中转槽（4×2 条）。
        const_cast<CInventory*>(getCurCharacInvenR())->set_parent(this);
        const_cast<SkillSlot*>(
            reinterpret_cast<const SkillSlot*>(getCurCharacSkillR()))->set_parent(this);
        getCurCharacQuestR()->set_parent(this);   // 声明已由 Main 代落 CUser.h（ORIG W _ZN10UserQuest10set_parentEP5CUser）
        const_cast<CCargo*>(getCurCharacCargoR())->set_parent(this);
        // [SubC a-2 2026-09-11] ORIG 0x0864d0e8-0x0864d107：`call get_charac_job` 结果留 %eax
        // 直用（无二次落槽 + 两次回读）；旧 `job = get_charac_job(); ...[job]` 多 store+load。
        // 复核账（SubC 01:2x 用 01:21:35 产物）：本项**净消 1 条**（GW 段我方 11 vs ORIG 10，
        // 多出 `mov %eax,%ecx`——4.4 需保活 job 原值，ORIG 就地复用 `%eax`）；
        // a-1(12)+a-2(1)+a-3(2)+a-4(1)=16 ⇒ 281→265 精确吻合。
        // 残差 3 条（封顶）：HistoryLog 段我方 28 vs ORIG 23（+5，4.1 用 %edi/%esi/%ebx 跨调用
        // 保活）+ GW 段 +1 − 序言缺 push %edi/%esi（−2）；帧差 0x8 属 4.4 槽排布/outgoing。
        // [新负结论·勿试] 把 HistoryLog 的 4 个具名参数内联成实参表达式会**方向反转**：
        // 4.4 改发「每参数 call; mov %eax,0xN(%esp)」共 20 条 ⇒ 我方 257 < ORIG 262（gap 转正反劣）。
        G_GameWorld()->m_channelUserCount[get_charac_job()] += 1;
        return 1;
    }
    cMyTrace trace3(__PRETTY_FUNCTION__, 0x1246, 5);
    trace3("CUser::SelectCharac :charac_index : %d", idx);
    return 0;
}