// [b28d-solo-CAchievement N104/P223 封顶，2026-09-09 复测] 修法同 _forceClear：
// ORIG 0x0828ce40 无 book/inven 具名槽（GetCharacExpandData/getCurCharacInvenW
// 结果寄存器直传），改匿名全表达式链后帧 0x40/槽号/push esi+ebx 全部对齐
//（P0/P278）。残差 = 尾部岛数 ORIG 5 个 `nop; jmp +0x248` + 尾裸 nop vs 4.4
// 4 个（invenItem==0 与 addInfo!=titleIdx 共落点，our 168）或 6 个（our 172，
// 见下）；本函数只此一处形态差 + call 重定位标签伪差（P264）。语义等价。
// 复测四形（fresh check_tu_routed，orig=170）：①&& 短路（当前）168/4 岛；
// ②嵌套 if 168/4 岛；③否定 goto+尾标签串 172/6 岛；④早返 172/6 岛。
// P223 负例成立：ORIG 末条件直落 epilogue（+0x248）属 4.1 布局，4.4 恒给
// 中继岛 ⇒ 无法同时给出「5 岛 + 尾裸 nop」，取指令数差最小的 && 形。
void CAchievement::_forceReset(unsigned int id)
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if ((quest == 0) || (quest->m_type != 7))
    {
        return;
    }
    int titleIdx = quest->get_title_reward();
    if (titleIdx == 0)
    {
        return;
    }
    if (isClearedAchievement(id))
    {
        std::map<unsigned int, stAchievement>::iterator it =
            m_achievements.find(id);
        if (it != m_achievements.end())
        {
            // ORIG 0x828ceff：stAchievement 临时对象 ctor 清零，get_init 覆盖 trigger 字段
            stAchievement ach;
            if (!quest->get_init_achievement_trigger(ach.m_trigger0,
                                                     ach.m_trigger1,
                                                     ach.m_trigger2))
            {
                return;
            }
            it->second.m_trigger0 = ach.m_trigger0;
            it->second.m_trigger1 = ach.m_trigger1;
            it->second.m_trigger2 = ach.m_trigger2;
        }
        alter();
    }
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)4;
    int slot = 0;
    m_user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14);
    CTitleBook::_getFitSlot(titleIdx, category, slot);
    Inven_Item* titleSlot =
        ((CTitleBook*)m_user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)14))
            ->getTitleSlot(category, slot);
    if (titleSlot == 0)
    {
        return;
    }
    // ORIG 0x828cff8：addInfo!=0 分支为 fall-through（reset），addInfo==0 分支 je 跳库存检查
    if (titleSlot->m_addInfo != 0)
    {
        titleSlot->reset();
    }
    else
    {
        Inven_Item* invenItem =
            ((CInventory*)m_user->getCurCharacInvenW())->GetInvenRef(0, 0xb);
        // ORIG 0x0828d032 `je +0x247`（invenItem==0 落**独立** nop 岛）与
        // 0x0828d043 `jne +0x248`（addInfo 不等**直落** epilogue）落点不同 ⇒
        // 两条独立守卫 + 尾标签（P133①/P223 多源 goto 尾标签）：
        // 多源同目标 goto 由 4.4 逐条发射，不 jump-thread。
        if (invenItem == 0)
            goto force_reset_end;
        if (invenItem->m_addInfo == titleIdx)
        {
            invenItem->reset();
            m_user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, 0xb);
        }
    }
force_reset_end:;
}