void Inter_CreateCharac::process_createGift(CUser* user, SIG_CREATE_CHARAC* sig)
{
    // [G2A-PC 本轮负结论 N，ndiff=47] 差异集中在 bool 物化与跨调用保活位点：
    //   · [124:129] ORIG `movzbl %al,%eax; mov %eax,<S>; cmpl $0x0,<S>; sete %al;
    //     movzbl %al,%edi`（ORIG 自身亦落槽再读）vs 本方 [121:124] `sete %al; mov %al,<S>;
    //     cmpl $0x0,<S>`；
    //   · [132:135] ORIG `mov %eax,%esi`（免槽）vs 本方 `mov %eax,<S>`（落槽）；
    //   · [144] 本方多 4 条槽往返 `movzbl <S>,%esi; movzbl <S>,%ebx; mov <S>,%ecx`。
    //   expire/hasExpire/noExpire 的具名与否两侧交错，register 化需先锁定变量集合，
    //   误改会引入新槽序平移，故留 N。
    if (user == 0)
        return;

    const int job = static_cast<int>((char)sig->m_slot);
    createChracScript::rewardInfoList gifts;
    G_CDataManager()->get_createCharacterGift(job, gifts);
    if (gifts.m_list.empty())
        return;

    const long regTime = static_cast<long>(user->getRegTime());
    const ENUM_SERVER_GROUP group =
        static_cast<ENUM_SERVER_GROUP>(user->GetServerGroupToPvP());
    const size_t rewardCount = gifts.m_list.size();
    for (size_t r = 0; r < rewardCount; ++r)
    {
        createChracScript::rewardInfo& info = gifts.m_list[r];
        // 时间窗过滤：(from<1 或 from<=regTime) && (to<1 或 regTime<to)
        if (!((info.m_startTime < 1 || info.m_startTime <= regTime) &&
              (info.m_endTime < 1 || regTime < info.m_endTime)))
            continue;
        for (std::vector<int>::iterator it = info.m_items.begin();
             it != info.m_items.end(); it++)   // P28：ORIG 增量段 ppEi ⇒ 后置++
        {
            Inven_Item item;                       // ORIG 局部 Inven_Item C1
            CItem* ci = G_CDataManager()->find_item(*it);
            if (ci == 0)
                continue;
            const int itemIdx = ci->get_index();
            ci->make_item(item);                   // vtable 槽 +0x08
            item.set_add_info(1);
            char body[256];
            strncpy(body,
                    g_scriptStringManager_.findString(
                        4, info.m_text.c_str(), reinterpret_cast<bool*>(0)),
                    0xff);
            if (itemIdx == 0)
                continue;
            const int expire = ci->getExpirationDate();
            const bool hasExpire = expire == 0;
            const bool noExpire = expire != 0;
            const unsigned int bodyLen = static_cast<unsigned int>(strlen(body));
            const char* sender = g_scriptStringManager_.findString(
                4, info.m_title.c_str(), reinterpret_cast<bool*>(0));
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                sender, item, 0u, sig->m_characNo, body,
                static_cast<int>(bodyLen),
                static_cast<unsigned int>(expire), group,
                hasExpire, noExpire);
        }
    }
