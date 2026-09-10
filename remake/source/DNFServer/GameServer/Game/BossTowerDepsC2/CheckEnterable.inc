int TowerOfDespairMgr::CheckEnterable(CUser* user, const CDungeon* dungeon)
{
    if (user == 0)
    {
        return 0x15;
    }

    if (user->get_charac_level() < static_cast<int>(m_minLevel))
    {
        return 0xe;
    }

    TOD_UserState* state = reinterpret_cast<TOD_UserState*>(
        user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
    if (state->getEnterCount() > 0)
    {
        if (!user->isGMUser())
        {
            // ORIG：两值跨后续调用保活于 callee-saved（%esi/%ebx，无栈槽）⇒
            // 按实参右→左求值内联书写（P0/P52），勿落具名局部（旧形两槽
            // ⇒ 帧 0x60→0x78 且缺 push %esi/%ebx）。
            if (!CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                                        state->getLastClearTime(),
                                        GlobalData::s_systemTime_.getCurSec()))
            {
                return 0xf5;
            }
        }

        // ORIG 无 inven 具名槽（两次独立 `getCurCharacInvenR()` 内联调用，
        // P0/P48）⇒ 内联成员链。ORIG 也无 Inven_Item 具名对象（直接对
        // GetInvenSlot 的匿名 sret 结果读 +0x7 = m_addInfo2）。
        int slot = user->getCurCharacInvenR()->check_item_exist(
            dungeon->m_entryRequiredItem.m_itemIdx);
        if (slot == -1)
        {
            return 0xf5;
        }

        // ORIG：cmp %eax,%edx 后 **jge 直跳**（无 setl 物化）⇒ 用 goto 形
        //（早退 return 会被 4.4 物化成 setl;test;je）。ORIG 直接
        // `mov -0x4a(%ebp),%edx`（sret+0x7 内联读）⇒ 字段直访问，getter 会多发 call。
        if (user->getCurCharacInvenR()->GetInvenSlot(1, slot).m_addInfo2 >=
            dungeon->m_entryRequiredItem.m_itemCount)
            goto L_slot_ok;
        return 0x11;
    L_slot_ok:;   // [Main 中断恢复] 标签后必须有语句（原为 `L_slot_ok:` 紧跟 `}` ⇒ 语法错）
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return 0x15;
    }
    if (party->get_member_count() > 1)
    {
        return 0xab;
    }
    return 0;
