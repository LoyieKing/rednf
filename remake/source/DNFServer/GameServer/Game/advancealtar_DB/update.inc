bool DB_Update::update(const _CharacAdvanceAltarDbData& data)
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    bool result = true;
    // [b28d-solo-advancealtar_DB] P283：ORIG 0x0813f25a `movl $0,-0x20; jmp <cond>`
    //   = for 底部条件布局（row<=0 继续）；旧 `for(;;){if(row>0)return result;}` 使
    //   4.4 条件置顶（多 cmpl/jle + jmp 岛）。
    for (int row = 0; row <= 0; ++row)
    {
        // ORIG 基址 local_20 = &data + 4（= m_db）；各列以 m_db 内具名成员寻址
        //（update.md 0x0813ec5e：`movzbl 0x517` 取 m_update、`mov 0x4/0x8/0xc/0x10/0x14`
        //  取 m_head 前 5 列、`movswl 0x18`、`movzwl 0x1a`、`movzbl 0x20`、
        //  数组指针 `add $0x21/0x1b1/0x229/0x481`）。
        // [b28d-solo-advancealtar_DB] ORIG 0x0813f25f `imul $0x518,%eax,%eax` ⇒ 行下标
        //   是变量 row（旧 m_db[0] 常量下标被折成单条 add，且丢失 imul）。
        const _AdvanceAltarDbData& db = data.m_db[row];
        // [b28d-solo-advancealtar_DB] P89/P0：m_head() 在 -O0 是真实 call 且引用占槽；
        //   ORIG 行头字段以 db+disp 直取（无 view 槽）⇒ 直接用 m_ridable 成员。
        if (db.m_update == 1 && isValidRidableId(db.m_ridable.m_ridableId))
        {
            std::string stageStr;
            std::string slotStr;
            std::string buyItemStr;
            std::string rewardStr;
            // [b28d-solo-advancealtar_DB] P183：ORIG 各调用点重发 `add $0x21/0x1b1/
            //   0x229/0x481` 全链（无 stages/slots/... 具名指针槽）。
            makeStageString(stageStr, db.m_stages, 0x32);
            makeSlotString(slotStr, db.m_slots, 0xc);
            makeBuyItemString(buyItemStr, db.m_buyItems, 0x32);
            makeRewardString(rewardStr, db.m_rewards, 0x1e);

            unsigned int zipStageLen = 0, zipSlotLen = 0, zipBuyItemLen = 0, zipRewardLen = 0;
            std::memset(g_ZipStageBuffer, 0, 0x800);
            std::memset(g_ZipSlotBuffer, 0, 0x800);
            std::memset(g_ZipBuyItemBuffer, 0, 0x800);
            std::memset(g_ZipRewardBuffer, 0, 0x800);

            // [b28d-solo-advancealtar_DB] P167：ORIG 调 `_ZNKSs5c_strEv`（我方 data()
            //   是 EXTRA 发明调用点）⇒ 4 处 data() 全改 c_str()。
            if (!convertStringToZip(g_ZipStageBuffer, 0x800, stageStr.c_str(),
                                    static_cast<unsigned>(stageStr.size()), zipStageLen))
                return false;
            if (!convertStringToZip(g_ZipSlotBuffer, 0x800, slotStr.c_str(),
                                    static_cast<unsigned>(slotStr.size()), zipSlotLen))
                return false;
            if (!convertStringToZip(g_ZipBuyItemBuffer, 0x800, buyItemStr.c_str(),
                                    static_cast<unsigned>(buyItemStr.size()), zipBuyItemLen))
                return false;
            if (!convertStringToZip(g_ZipRewardBuffer, 0x800, rewardStr.c_str(),
                                    static_cast<unsigned>(rewardStr.size()), zipRewardLen))
                return false;

            char* qStage = mysql->blob_to_str(0, g_ZipStageBuffer, static_cast<int>(zipStageLen));
            char* qSlot = mysql->blob_to_str(1, g_ZipSlotBuffer, static_cast<int>(zipSlotLen));
            char* qBuyItem = mysql->blob_to_str(2, g_ZipBuyItemBuffer, static_cast<int>(zipBuyItemLen));
            char* qReward = mysql->blob_to_str(3, g_ZipRewardBuffer, static_cast<int>(zipRewardLen));
            mysql->set_query(
                             "update charac_advance_altar \t\t\t\t  set ticket_free = %d, ticket_cera = %d, "
                             "\t\t\t\t  star_game = %d, star_cera = %d, star_usable = %d, \t\t\t\t  "
                             "survival_best = %d, star_reset_count = %d, is_unlock_stage_effect = %d, "
                             "\t\t\t\t  stage_list = '%s', \t\t\t\t  slot_list = '%s', \t\t\t\t  "
                             "buy_item_list = '%s', \t\t\t\t  reward_list = '%s' \t\t\t\t  where "
                             "charac_no = %u and ridable_id = %d",
                             db.m_ridable.m_ticket, db.m_ridable.m_freeTicket, db.m_ridable.m_curStar,
                             db.m_ridable.m_starPoolBase,
                             db.m_ridable.m_starPoint, static_cast<int>(db.m_ridable.m_maxClearRound),
                             static_cast<int>(static_cast<unsigned short>(db.m_ridable.m_resetCount)),
                             static_cast<int>(db.m_ridable.m_field20),
                             qStage, qSlot, qBuyItem, qReward,
                             data.m_characNo, (int)db.m_ridable.m_ridableId);
            result = mysql->exec(true);
            if (!(result && mysql->getAffectedRowCount() != 0))
            {
                qStage = mysql->blob_to_str(0, g_ZipStageBuffer, static_cast<int>(zipStageLen));
                qSlot = mysql->blob_to_str(1, g_ZipSlotBuffer, static_cast<int>(zipSlotLen));
                qBuyItem = mysql->blob_to_str(2, g_ZipBuyItemBuffer, static_cast<int>(zipBuyItemLen));
                qReward = mysql->blob_to_str(3, g_ZipRewardBuffer, static_cast<int>(zipRewardLen));
                mysql->set_query(
                                 "insert into charac_advance_altar \t\t\t\t\t\t  (charac_no, ridable_id, "
                                 "\t\t\t\t\t\t  ticket_free, ticket_cera, \t\t\t\t\t\t  star_game, "
                                 "star_cera, star_usable, \t\t\t\t\t\t  survival_best, star_reset_count, "
                                 "is_unlock_stage_effect, \t\t\t\t\t\t  stage_list, \t\t\t\t\t\t  "
                                 "slot_list, \t\t\t\t\t\t  buy_item_list, \t\t\t\t\t\t  reward_list) "
                                 "\t\t\t\t\t\t  values \t\t\t\t\t\t  (%u, %d, \t\t\t\t\t\t  %d, %d, "
                                 "\t\t\t\t\t\t  %d, %d, %d,\t\t\t\t\t\t  %d, %d, %d, \t\t\t\t\t\t  '%s', "
                                 "\t\t\t\t\t\t  '%s', \t\t\t\t\t\t  '%s', \t\t\t\t\t\t  '%s')",
                                 data.m_characNo, (int)db.m_ridable.m_ridableId,
                                 db.m_ridable.m_ticket, db.m_ridable.m_freeTicket,
                                 db.m_ridable.m_curStar, db.m_ridable.m_starPoolBase,
                                 db.m_ridable.m_starPoint,
                                 static_cast<int>(db.m_ridable.m_maxClearRound),
                                 static_cast<int>(static_cast<unsigned short>(db.m_ridable.m_resetCount)),
                                 static_cast<int>(db.m_ridable.m_field20),
                                 qStage, qSlot, qBuyItem, qReward);
                result = mysql->exec(true);
            }
            result = true;   // ORIG movl $1（处理完成位）
        }
    }
    return result;
