bool Inter_LoadRewardMedal::SendMail(CUser& user, unsigned int medalCount)
{
    const std::vector<_MedalRewardItem>& medals =
        G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)->m_medalRewards;

    std::size_t count = medals.size();
    const _MedalRewardItem* hit = 0;
    // [b28d-solo-APSystemOnlyOrig] P212：同 GetTodayRewardAndSendMail ——
    // i 提至函数头，4.4 按「先声明深」落在 -0x14（ORIG 形）。
    unsigned int i;
    for (i = 0; i < count; ++i)
    {
        if (!(medalCount < medals[i].m_field00) && !(medals[i].m_field02 < medalCount))
        {
            hit = &medals[i];
            break;
        }
        if (!(medalCount < medals[i].m_field00) && medals[i].m_field00 == 100)
        {
            hit = &medals[i];
            break;
        }
    }
    // [round5-C05 2026-09-10] 尾 fail 块嵌套形（本波配方①）：ORIG 0x08124fc3/0x08124ff4
    // 两处 `cmpl $0x0,…; je <函数尾部独立 mov $0,%eax 块>`；平铺 return 恒被 4.4 折成
    // `jne <cont>; mov $0; jmp`（每处 +2）。两个检查共享同一尾块 ⇒ 双层嵌套 + 尾部
    // 单一 `return false;`。（探针 /tmp/tail_probe.cpp fA/fB。）
    if (hit != 0)
    {
        CItem* itemDef = G_CDataManager()->find_item(hit->m_field04);
        Inven_Item item;
        if (itemDef != 0)
        {
            itemDef->make_item(item);
            // ORIG：count ushort 先零扩展，再完整写入 packed Inven_Item +0x07。
            item.m_addInfo2 = static_cast<int>(hit->m_field08);
            item.m_addInfo = hit->m_field04;

            const char* title =
                g_scriptStringManager_.findString(4, "chn_game_server_msg_82", 0);
            char titleBuf[30];
            strncpy(titleBuf, title, 0x1d);
            const char* msg =
                g_scriptStringManager_.findString(4, "chn_game_server_msg_83", 0);
            char msgBuf[256];
            strncpy(msgBuf, msg, 0xff);

            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item, 0, user.get_charac_no(-1), msgBuf, strlen(msgBuf), 8,
                user.GetServerGroup(), false, false);
            return true;
        }
    }
    return false;
