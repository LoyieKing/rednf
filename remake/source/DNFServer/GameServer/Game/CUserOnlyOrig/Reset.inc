// ---------------------------------------------------------------------------
// _Charac_info::Reset() T 0x086822b8 sz0x7d9
// 报告：docs/class_func_reports/_Charac_info/Reset.md（## 汇编+## 反编译 C 全文）。
// 全字段清零 + CommonTime::clear + _Additioal_info::clear + CInventory::reset +
// SkillSlot::clear_all_skills_both + CCargo::reset + 两 map clear +
// tagCharacVisibleFlag::reset + stAuraAvatarOption::reset。
// ---------------------------------------------------------------------------
void _Charac_info::Reset()
{
    // 2026-09-04 §4.3 转换：删除 char* p 基址视图（ORIG 0x086822b8 全程
    // mov 0x8(%ebp),%eax; movx $imm,off(%eax) 直读 this，无局部 spill）。
    // 具名字段按 CUser.h 布局；pad 区写保留「布局未建模」裸视图（待 Main 代落）。
    m_characNo = 0;                                   // +0x00 movl
    memset(m_name, 0, 0x1e);                          // +0x04..0x21
    m_vill = 0;                                    // +0x22 movb
    m_villFixed = 0;                                    // +0x23 movb
    m_job = 0;                                    // +0x26 movb
    m_level = 0;                                      // +0x27 movw
    m_growthType.m_growthType = 0;                       // +0x29 movb（具名 STGrowthType）
    m_pad2a.m_growthType = 0;                         // +0x2a movb（具名 STGrowthType）
    m_exp = 0;                                    // +0x2b movl
    m_partyBonusExp = 0;             // +0x2f movl（pad 未建模）
    m_fatigue = 0;                                    // +0x33 movw
    m_maxFatigue = 0;                                    // +0x35 movw
    m_fatigueGrownUpBuff = 0;                                    // +0x37 movw
    reinterpret_cast<CommonTime*>(m_delReqTime)->clear();   // +0x39 call
    m_premiumFatigue = 0;                                    // +0x77 movw
    m_maxPremiumFatigue = 0;                                    // +0x79 movw
    m_lastPlayTick = 0;                                    // +0x7b movl
    m_lastPlayTickPowerWar = 0;                                    // +0x7f movl
    m_additioalInfo.clear();                                  // +0x88 call（具名子对象；判据②消视图 cast）
    m_curCharacInven.reset();
    m_skillSlot.clear_all_skills_both();
    m_cargo.reset();
    m_guildId = 0;                                    // +0xdbe movl
    m_guildCreateRight = 0;                                   // +0xdc2 movb
    m_loginTick = 0;  // +0xe02 movl（pad 未建模）
    m_resetDwordDf4 = 0;                              // +0xdf4 movl
    m_resetDwordDf8 = 0;                              // +0xdf8 movl
    m_helpAbuseComputedRatio = 0;  // +0xdfe movl（pad）
    m_finishPoint = 0;  // +0xe06 movl（pad）
    m_finishPointTotal = 0;  // +0xe0a movl（pad）
    memset(m_warAreaKill, 0, sizeof(m_warAreaKill));      // +0xe0e memset(0x14)（pad）
    m_memberkey = 0;                                   // +0xe22 movl
    m_connectUpperMember = 0;  // +0xe26 movl（pad）
    m_memberBonusExp = 0;  // +0xe2a movl（pad）
    m_memberPayTexMoneyToUpper = 0;  // +0xe33 movl（pad）
    m_memberPayTexFatigueToUpper = 0;  // +0xe37 movl（pad）
    m_fatigueBufBonusExp = 0;  // +0xe2e movl（pad）
    m_uppermemberExpLevel = 0; // movb（pad）（Reset.md 证据）
    m_needCheckOverEquipItem = 0;                     // +0xe3b movb
    m_pad83 = 0;                                   // +0x83 movb
    m_field84 = 0;           // +0x84 movl（pad83 后继，未建模）
    m_blackCount = 0;            // +0xe3c movl（pad）
    m_guildExp = 0;  // +0xde9 movl（pad）
    m_guildTempExp = 0; // +0xded movw（pad）
    m_guildTodayExp = 0;  // +0xdef movl（pad）
    m_guildMemberGrade = 0; // movb（pad）（Reset.md 证据）
    m_levelBeforeDungeon = 0;          // +0xdfc movw（pad）
    m_assaultPlace = 0;  // +0xe41 movl（pad）
    m_chaosPoint = 0;                                   // +0xe49 movl
    m_chaosExp = 0;                                   // +0xe4d movl
    m_assaultCount = 0;                                   // +0xe45 movl
    m_chaosModeCount = 0;                                   // +0xe51 movl
    m_chaosKillCount = 0;                                   // +0xe55 movl
    m_chaosDieCount = 0;                                   // +0xe59 movl
    m_chaosStateTime = 0;            // +0xe61 movl（pad）
    memset(&m_powerWarData, 0, 0xc);  // +0xe8d memset(0xc)（具名 STPowerWarCharacData）
    m_powerWarData.m_data3 = 1;      // +0xe90 movb $1（具名；ctor 亦置 1，Reset 复查同形）
    m_villSpecial = 0;                                    // +0x24 movb
    m_superState = 0; // movb（pad）（Reset.md 证据）
    m_chaosDieTime = 0;                                   // +0xe5d movl
    m_ghost = 0; // movb（pad）（Reset.md 证据）
    m_leaveTimeout = 0; // movb（pad）（Reset.md 证据）
    m_chaosKillTime = 0;                                   // +0xe68 movl
    m_assaultFatigueCount = 0;            // +0xe6c movl（pad）
    m_luckPoint = 0;                                   // +0xe71 movl
    m_luckPointUpdateTimeCount = 0;            // +0xe75 movl（pad）
    m_dungeonPlayCount = 0;                                   // +0xe79 movl
    m_expertJobType = 0;                                   // +0xe7d movl
    m_expertJobExp = 0;                                   // +0xe81 movl
    m_expertJob = 0;            // +0xe85 movl（pad）
    m_onlinePreliminaryTeam = 0;  // +0xe89 movl（pad）
    m_fieldE99 = 0; // movb（pad）（Reset.md 证据）
    m_schoolPoint = 0;  // +0x71 movl（CReliablePerson 16B 视图内）
    m_isAffectedExpDouble = 0; // movb（pad）（Reset.md 证据）
    m_isInitSkillFlag = 0; // movb（pad）（Reset.md 证据）
    m_saveDemensionInoutValue = 0; // movb（pad）（Reset.md 证据）
    m_firstTimeLogin = 0; // movb（pad）（Reset.md 证据）
    memset(m_demensionInout, 0, 6);  // +0xeb9 memset(6)
    m_saveBloodInoutValue = 0; // movb（pad）（Reset.md 证据）
    memset(m_ultimateInout, 0, 3);   // +0xec0 memset(3)
    m_bloodBestRecordUpdate = 0; // movb（pad）（Reset.md 证据）
    m_bloodBestRoundRecord = -1;   // +0xec4 movl $-1（pad）
    m_bloodBestTimeRecord = 400000000;  // +0xec8 movl（pad）
    m_bloodBestRecords.clear();                       // +0xecc map::clear（内联）
    m_expItemAffected = 0;                                  // +0xee4 movb（pad）
    // ORIG 0x086822b8+0x443 `mov $0x0,%edx; mov %edx,0xee5(%eax)`：float 成员 0.0f
    // 物化两段形（probe p14.w1；Main 已落 CUser.h:637 int→float，同宽）。
    m_expAffectRate = 0.0f;     // +0xee5（Reset.md 证据）
    m_fatigueBattery = 0;                                    // +0x4f movw
    m_disguiseCharac = 0; // movb（pad）（Reset.md 证据）
    m_disguiseKind = 0; // movb（pad）（Reset.md 证据）
    m_disguiseIndex = 0;  // +0xeeb movw（pad）
    memset(&m_premiumLethe, 0, 0x199);   // +0xeed memset(0x199)（具名 SIG_LOAD_PREMIUM_LETHE）
    memset(m_tradeRestrictData, 0, 0x10);             // +0x55 memset(0x10)
    m_powerWarPoint = 0;                                  // +0x121f movw
    m_powerWarAssaultCount = 0;                                  // +0x1221 movl
    m_powerWarAssaultVictoryCount = 0;                                  // +0x1225 movl
    m_straightVictories = 0;         // +0x1229 movw（pad）
    m_endLetheAtLogin = 0;                                 // +0x122b movb（pad）
    m_emotion = 0;     // +0x122c movw（pad）
    m_lastItemLimitEditionBuyTime = 0;       // +0x122e movl（pad）
    m_conditionEventStep = 0;     // +0x1232 movw（pad）
    m_conditionEventRewardStep = 0;   // +0x1234 movw（pad）
    m_properDungeonClearCount = 0;   // +0x1236 movw（pad）
    m_properDungeonUser = 0;                               // +0x1238 movb（pad）
    m_skillTreeIndex = 0xff;                               // +0x1239 movb $0xff
    m_eventItemCount = 0;         // +0x123a movw（pad）
    m_eventAddItemCount = 0;     // +0x123c movw（pad）
    m_exEquipSlotStat = 0;                                  // +0x123e movb
    m_memberDungeonFlag = 1;                                  // +0x1241 movb $1
    m_villageAttackCount = 0;                                 // +0x1242 movb（pad）
    m_revengeDungeonCount = 0;                                 // +0x1243 movb（pad）
    m_villageAttackDBUpdate = 0;                                 // +0x1244 movb（pad）
    m_linkCharacSlotIndex = 0xff;                              // +0x1245 movb $0xff（pad）
    m_characLinkType = 0;                             // +0x1246 movb
    m_characLinkBonusExp = 0;           // +0x1247 movl（pad）
    m_characLinkBonusGold = 0;       // +0x124b movl（pad）
    m_linkBonusSetting = 0;                           // +0x124f movb
    m_field1250 = 0;           // +0x1250 movl（pad）
    m_skillCommandsMap.clear();                       // +0x1254 map::clear（内联）
    m_memberBonusFatigue = 0;                                  // +0x126c movw
    m_guildSecede = 0;                                  // +0x126e movb
    m_lastPlayDungeonIndex = 0;                                  // +0x126f movl
    m_upgradeSeparateTrueFlag = 0;                                 // +0x1273 movb（pad）
    m_upgradeSeparateFalseFlag = 0;                                 // +0x1274 movb（pad）
    m_equipslotFlag = 0;                                  // +0x1275 movb
    m_channelEquipslotSwitch = 0;                                  // +0x1276 movb
    m_expandEquipslotSwitch = 0;                                  // +0x1277 movb
    m_bringUpRewardFlag = 0;                                  // +0x1278 movb
    m_pad1279 = 0;                                    // +0x1279 movb
    m_useBringUpMaterialTotal = 0;                                  // +0x127a movb
    m_tagVisibleFlags.reset();
    m_growthPowerExpRewardRight = 0;                                 // +0x127b movb（pad）
    m_seriaBufBonusExp = 0;       // +0x127c movl（pad）
    m_growthPowerReward = 0;                                  // +0x1280 movb
    m_auraAvatarOption.reset();
    m_weekendBonusExp = 0;  // +0x1291 movl（pad）
    m_premiumBonusExp = 0;  // +0x128d movl（pad）
    m_field1295 = 0;  // +0x1295 movl（pad）
    m_field1299 = 0;  // +0x1299 movl（pad）
    memset(&m_wpBonusPointResult, 0, 8);    // +0x129d memset(8)（具名）
    memset(&m_expBonusPointResult, 0, 8);   // +0x12a5 memset(8)（具名）
    m_chaosResponTime = 0;                                  // +0x12ad movl
    m_assaultMoney = 0;           // +0x12b1 movl（pad）
    m_assaultedUserId = 0;     // +0x12b5 movw（pad）
    memset(m_characterOption.m_pad, 0, 0x200);     // +0x12b7 memset(0x200)（具名 CharacterOption）
    m_strikerSkillIndex = 0;                                  // +0x14b7 movb
    m_tagCharacNo = 0;                                  // +0x14b8 movl
    m_tagCharacInfoSent = 0;                                 // +0x14bc movb（pad）
    m_blueMarbleEnterCount = 0;                                 // +0x14bd movb（pad）
    m_blueMarbleUserDeadCount = 0;       // +0x14be movl（pad）
    m_maxEquipLevel = 0;       // +0x14c2 movl（pad）
    m_viewState = 0;                                  // +0x14c6 movl
    m_aniDeco.m_field0 = 0;  // +0x14ca movw（具名 seriaRoom_AniDeco）
    m_aniDeco.m_field2 = 0;  // +0x14cc movw（具名）
    m_cubePremiumActive = 0;                                 // +0x14ce movb（pad）
    m_cubePremiumCount = 0;                                 // +0x14cf movb（pad）
    m_savedPremiumCubeInfo = 0;                                 // +0x14d0 movb（pad）
}