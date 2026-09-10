void CUser::ResetCurCharac()
{
    getPVPResultRefW()->Clear();
    getBattleRecordRefW()->clear();
    m_list8cf80.clear();
    getDungeonClearRefW()->clear();
    m_gmBossDifficulty = 0;
    m_gmBossMapIndex = 0;
    if (getCurCharacR() != 0)
    {
        ((CInventory*)getCurCharacInvenR())->GetAvatarItemMgrW()->Reset();
        ((SkillSlot*)getCurCharacSkillR())->clear_all_skills_both();
        ((CCargo*)getCurCharacCargoR())->reset();
        getCurCharacQuestR()->reset();
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->Reset();
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->SetUser(this);
        if (isDisguiseCharac() != 0)
            setDisguiseCharac(false, 0, 0);
    }
    reset_guild_temp_exp();
    reset_guild_exp();
    reset_level_before_dungeon();
    for (int i = 1; i < 5; ++i)
    {
        // ORIG（CUser::ResetCurCharac 反汇编实证）：characNo = getCurCharacNo()，
        // 然后 getRankTable(i)->unregistBestRecord(characNo)（成员签名，见 CDeathTowerRanking.h）。
        unsigned int characNo = getCurCharacNo();
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistBestRecord(characNo);
        characNo = getCurCharacNo();
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistRanking(characNo);
    }
    resetCurCharacSchoolPoint();
    if (getCurCharacR() != 0)
    {
        // b28d-GrpD P183/P329（ORIG 0x0865851c 直证）：ORIG 该段为
        //   `call G_GameWorld`（%eax 保活）→ `call get_charac_job`（%eax）→
        //   `lea 0x58(%R),%R; mov 0x4(%R,%R,4),%R; sub $0x1,%R; mov %R,0x4(%R,%R,4)`
        //   —— **无 pWorld / job / pCnt 具名槽**；我方旧形 3 个具名量（+ 指针缓存）
        //   多占 -0xc/-0x10/-0x14 并产 shl/add 链（帧 0x38 vs ORIG 0x24）。
        //   +4+(job+0x58)*4 = +0x164+job*4 = GameWorld::m_channelUserCount[job]。
        G_GameWorld()->m_channelUserCount[get_charac_job()]--;
    }
    setCurCharac(0);
    setTagCharac(0);
    m_tradeSpaceIdx = 0xffff;
    m_field8d004 = 0xffff;
    m_pvpRoomIdx = 0xffff;
    m_field8d008 = 0xffff;
    m_field8d00c = 0xffff;
    m_field8cfcc = (UserSpace::T)0;
    m_field6ef68 = 0;
    m_field6ef69 = 0xff;
    m_guildAgitDB.m_pad[2] = 0;
    m_playingDungeonIdx = 0xffffffff;
    m_postChecksumData.m_field48 = 0;
    m_postChecksumData.m_field4c = 0;
    if (m_premium.m_mailBox != 0)
        ((WongWork::CMailBox*)m_premium.m_mailBox)->Init();
    set_guildwar_point_per_pvpplay(0);
    memset(&m_guildDB, 0, 0xbd);
    memset(&m_guildAgitDB, 0, 1);
    resetGuildCreateFlag();
    resetTempGuildNameAddr();
    reset_pvp_masterid_walkingout_me();
    m_mcap.resetExposedCount();
    m_field8d1dc = 0;
    m_field8d1e0 = 0;
    m_quest.m_field761c = 0;
    m_pcRoomAuth = 0;   // CUser+0x8cef1（原 m_quest.m_field761d 别名）
    memset(&m_reward, 0, 4);
    setForceDropFlag(false);
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_field8ec29 = 0;
