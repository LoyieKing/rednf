bool DB_InsertCirculationStatistic::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_CIRCULATION_STATISTIC* sig =
        /*SIG_CIRCULATION_STATISTIC*/(
            stream->GetOutBuffer<SIG_CIRCULATION_STATISTIC>());
    db->set_query(
        "inSert into log_gold_stat(channel_no,occ_time ,level,dungeon_drop,result_card,sell_store,quest_reward,death_tower_reward,illusion_tower_reward,war_area_drop,member_tax,blood_dungeon_reward,blood_dungeon_lotto,power_dungeon_drop,power_dungeon_result_card,buy_store,stamina_recovery,repair_item,private_store_commission,gold_card,gold_drop,upgrade,quest_use,mail_commission,punish_user,restrict_trade,guild_level_up,guild_skill,guild_mail,item_compound,blood_dungeon_enter,buy_cerashop,war_area_enter,assault_gold,upgrade_guild_agit,upgrade_guild_cargo) values(%d,from_unixtime(%d),%d,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)",
        G_CEnvironment()->get_channel_no(), sig->m_occTime, sig->m_level,
        sig->m_dungeonDrop, sig->m_resultCard, sig->m_sellStore,
        sig->m_questReward, sig->m_deathTowerReward,
        sig->m_illusionTowerReward, sig->m_warAreaDrop, sig->m_memberTax,
        sig->m_bloodDungeonReward, sig->m_bloodDungeonLotto,
        sig->m_powerDungeonDrop, sig->m_powerDungeonResultCard,
        sig->m_buyStore, sig->m_staminaRecovery, sig->m_repairItem,
        sig->m_privateStoreCommission, sig->m_goldCard, sig->m_goldDrop,
        sig->m_upgrade, sig->m_questUse, sig->m_mailCommission,
        sig->m_punishUser, sig->m_restrictTrade, sig->m_guildLevelUp,
        sig->m_guildSkill, sig->m_guildMail, sig->m_itemCompound,
        sig->m_bloodDungeonEnter, sig->m_buyCerashop, sig->m_warAreaEnter,
        sig->m_assaultGold, sig->m_upgradeGuildAgit,
        sig->m_upgradeGuildCargo);
    if (!db->exec(true))
        return 0;
    return 1;
}