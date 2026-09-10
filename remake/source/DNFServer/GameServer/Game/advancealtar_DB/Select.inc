bool DB_Load::Select(_CharacAdvanceAltarDbData& data)
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    mysql->set_query(
                     "select ridable_id, ticket_free, ticket_cera, \t\t\t\t  star_game, star_cera, "
                     "star_usable, \t\t\t\t  survival_best, star_reset_count, is_unlock_stage_effect, "
                     "\t\t\t\t  stage_list, slot_list, buy_item_list, reward_list \t\t\t\t  from "
                     "charac_advance_altar\t\t\t\t\t  where charac_no = %u",
                     data.m_characNo);
    if (!mysql->exec(true))
        return false;

    const int nRows = mysql->get_n_rows();
    for (int row = 0; row < nRows && row < 1; ++row)
    {
        if (!mysql->fetch())
            return false;

        // ORIG 行基址 local_14 = &data + row*0x518 + 4 → m_db 起
        //（行距 = sizeof(_AdvanceAltarDbData) = 0x518）。
        // [b28d-solo-advancealtar_DB] ORIG 0x0813decc `imul $0x518,%eax,%eax;
        //   add 0xc(%ebp),%eax; add $0x4,%eax`（add 链）；char* 加法形产 lea 链。
        //   指针算术 `&m_db[0] + row` 试形。
        _AdvanceAltarDbData* dbRow = &data.m_db[0] + row;
        // [b28d-solo-advancealtar_DB] P89/P0：ORIG 无 view 槽（行头字段以
        //   dbRow+disp 直取，_RidableBase 与 _RidableDbHead 逐字节同布局）⇒ 直接
        //   用 m_ridable 具名成员，删 m_head() 调用与引用槽。
        int col = 0;

        int ridableId = 0;
        if (!mysql->get_int(col++, ridableId))                       // [0x0811692c]
            return false;
        if (!isValidRidableId(static_cast<RidableId::T>(ridableId)))
        {
            LogManager::logFormat(1, "localglobal/global_AdvanceAltar_Dispatcher.cpp",
                                  __PRETTY_FUNCTION__,
                                  0xb1, "invalid RidableId=%d", ridableId);
            return false;
        }
        dbRow->m_ridable.m_ridableId = static_cast<RidableId::T>(ridableId);

        if (!mysql->get_short(col++, dbRow->m_ridable.m_ticket))      // [0x0814201c get_short(int,int&)]
            return false;
        if (!mysql->get_short(col++, dbRow->m_ridable.m_freeTicket))
            return false;
        if (!mysql->get_int(col++, dbRow->m_ridable.m_curStar))       // [0x0811692c]
            return false;
        if (!mysql->get_int(col++, dbRow->m_ridable.m_starPoolBase))
            return false;
        if (!mysql->get_int(col++, dbRow->m_ridable.m_starPoint))
            return false;
        if (!mysql->get_short(col++, dbRow->m_ridable.m_maxClearRound))  // [0x080fddec get_short(int,short&)]
            return false;
        if (!mysql->get_short(col++, dbRow->m_ridable.m_resetCount))
            return false;
        short unlockFlag = 0;
        if (!mysql->get_short(col++, unlockFlag))                     // [0x080fddec get_short(int,short&)]
            return false;
        // ORIG：`movzwl` 零扩展 unlockFlag → `test %ax,%ax; jle`（有符号16位 ≤0→0，
        // 即 0x8000..0xFFFF 负值判 0，1..0x7fff 判 1）→ 两臂各自
        // `mov -0x10(%ebp),%eax; movb $0x1/$0x0,0x20(%eax)`。三目形会产
        // `mov $1,%eax; jmp; mov $0,%eax; mov %al,...` 物化岛（P47/P214①）⇒ if/else 直存。
        if (unlockFlag > 0)
            dbRow->m_ridable.m_field20 = 1;
        else
            dbRow->m_ridable.m_field20 = 0;

        std::memset(g_StrStageBuffer, 0, kMaxStrBuffer);
        std::memset(g_StrSlotBuffer, 0, kMaxStrBuffer);
        std::memset(g_StrBuyItemBuffer, 0, kMaxStrBuffer);
        std::memset(g_StrRewardBuffer, 0, kMaxStrBuffer);

        if (!convertZipToString(mysql, col++, g_ZipStageBuffer, kMaxZipBuffer, g_StrStageBuffer, kMaxStrBuffer))
            return false;
        if (!convertZipToString(mysql, col++, g_ZipSlotBuffer, kMaxZipBuffer, g_StrSlotBuffer, kMaxStrBuffer))
            return false;
        if (!convertZipToString(mysql, col++, g_ZipBuyItemBuffer, kMaxZipBuffer, g_StrBuyItemBuffer, kMaxStrBuffer))
            return false;
        if (!convertZipToString(mysql, col++, g_ZipRewardBuffer, kMaxZipBuffer, g_StrRewardBuffer, kMaxStrBuffer))
            return false;

        // [b28d-solo-advancealtar_DB] P183：ORIG 每个调用点重发
        //   `mov row; imul $0x518; add &data; add $0x25` 全链（基址为 &data 非 dbRow）
        //   ⇒ 参数写 data.m_db[row].m_* 而非 dbRow->m_*。
        if (!parseStage(g_StrStageBuffer, data.m_db[row].m_stages, kMaxStageCount))
            return false;
        if (!parseSlot(g_StrSlotBuffer, data.m_db[row].m_slots, kMaxSlotCount))
            return false;
        if (!parseBuyItem(g_StrBuyItemBuffer, data.m_db[row].m_buyItems, kMaxBuyItemCount))
            return false;
        if (!parseReward(g_StrRewardBuffer, data.m_db[row].m_rewards, kMaxRewardCount))
            return false;
    }
    return true;
