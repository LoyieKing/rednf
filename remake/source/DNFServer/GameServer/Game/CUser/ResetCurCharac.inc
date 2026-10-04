// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865851c | _ZN5CUser14ResetCurCharacEv
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
        // [L9-GrpL 2026-09-11 P0/P183 去具名局部] ORIG 0x0865851c 该循环段（报告反汇编
        //   0x086585xx）为 `call getCurCharacNo; mov %eax,%ebx`（**无 characNo 槽**）→
        //   `mov -0xc(%ebp),%eax`（循环变量 i 读自 -0xc）→ `mov %ebx,0x4(%esp)`；
        //   我方具名 `unsigned int characNo` 多占 -0xc 槽、把循环变量 i 顶到 -0x10，
        //   且每处多 store+load 一对（`mov %eax,-0xc(%ebp)` / `mov -0xc(%ebp),%edx`）；
        //   段末 `addl $0x1,-0xc` vs 我方 -0x10 亦由此。⇒ 内联实参（ORIG 无该局部）。
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistBestRecord(getCurCharacNo());
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistRanking(getCurCharacNo());
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
    m_partyIndex = 0xffff;
    m_tradeSpaceIdx = 0xffff;
    m_pvpRoomIdx = 0xffff;
    m_warRoomIndex = 0xffff;
    m_blueMarbleIndex = 0xffff;
    m_moveSpace = (UserSpace::T)0;
    m_gmUserFlag = 0;   // ORIG 0x08658793 movb $0x0,0x711d8（+0x711d8 直证）
    m_forceItemDrop = 0;
    m_field6ef69 = 0xff;
    m_guildAgitDB.m_pad[2] = 0;
    m_playingDungeonIdx = 0xffffffff;
    m_progLogout = 0;   // ORIG 0x086587c8 movb $0x0,0x711e4（+0x711e4 直证）
    m_postChecksumData.m_field4c = 0;
    m_postChecksumData.m_field48 = 0;
    if (m_premium.m_mailBox != 0)
        ((WongWork::CMailBox*)m_premium.m_mailBox)->Init();
    set_guildwar_point_per_pvpplay(0);
    memset(&m_guildDB, 0, 0xbd);
    memset(&m_guildAgitDB, 0, 1);
    resetGuildCreateFlag();
    resetTempGuildNameAddr();
    reset_pvp_masterid_walkingout_me();
    m_mcap.resetExposedCount();
    m_revivalTime = 0;
    m_useAPCPotionTime = 0;
    m_quest.m_villageAttackedReward = 0;
    m_pcRoomAuth = 0;   // CUser+0x8cef1（原 m_quest.m_field761d 别名）
    memset(&m_reward, 0, 4);
    setForceDropFlag(false);
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_comboSkillSet = 0;
}