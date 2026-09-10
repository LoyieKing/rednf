bool CActionPointManager::GetTodayRewardAndSendMail(
    CUser& user, _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward)
{
    const std::vector<APSystem::_TodayRewardItem>& todays =
        G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)->m_todayRewards;

    std::size_t count = todays.size();
    const APSystem::_TodayRewardItem* entry = 0;
    // [b28d-solo-APSystemOnlyOrig] P212：for-init 的 i 提至函数头（全头部声明），
    // 4.4 才按「先声明深」分配（-0x14，ORIG 形）；块内声明会占 -0xc。
    unsigned int i;
    for (i = 0; i < count; ++i)
    {
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08122168..0x081221be：三子句
        // `&&` 短路（jne/jb/ja 同落 mov$0 岛 + test;je，P214① 正极性形）；
        // 拆成 if/continue 会产 setne 物化 + 比较序反转（每轮 +3 条）。
        if (reward.m_itemIdx == todays[i].m_itemIdx &&
            !(m_sig.m_apSum < todays[i].m_minAp) &&
            !(m_sig.m_apSum > todays[i].m_maxAp))
        {
            if (m_sig.m_isRewardItem[reward.m_itemIdx] == 1)  // this+0x11+idx
            {
                continue;
            }
            m_sig.m_isRewardItem[reward.m_itemIdx] = 1;
            entry = &todays[i];
            break;
        }
    }
    // [round5-C05 2026-09-10] 尾 fail 块嵌套形（本波配方①）：ORIG 0x0812220b/0x08122242
    // 两处 `cmpl $0x0,…; je 0812237a <+0x276>`（同一尾部 `mov $0,%eax` 块）⇒ 双层嵌套
    // + 尾部单一 `return false;`；平铺 return 恒被 4.4 折成 `jne <cont>; mov $0; jmp`。
    if (entry != 0)
    {
        m_flag00 = 1;   // LAB 入口 *this = 1（find_item 之前）

        CItem* itemDef = G_CDataManager()->find_item(entry->m_rewardItemIdx);
        Inven_Item item;
        if (itemDef != 0)
        {
            itemDef->make_item(item);
            // ORIG：count ushort 先零扩展，再完整写入 packed Inven_Item +0x07。
            item.m_addInfo2 = static_cast<int>(entry->m_count);
            item.m_addInfo = entry->m_rewardItemIdx;

            const char* title =
                g_scriptStringManager_.findString(4, "chn_game_server_msg_82", 0);
            char titleBuf[30];
            strncpy(titleBuf, title, 0x1d);
            const char* msg =
                g_scriptStringManager_.findString(4, "chn_game_server_msg_83", 0);
            char msgBuf[256];
            strncpy(msgBuf, msg, 0xff);

            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item, 0, user.get_charac_no(-1), msgBuf, strlen(msgBuf),
                8, user.GetServerGroup(), false, false);
            return true;
        }
    }
    return false;
