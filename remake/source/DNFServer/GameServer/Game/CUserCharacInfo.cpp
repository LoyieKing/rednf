// ============================================================================
// df_game_r 还原 —— CUserCharacInfo 全部函数（G1-2 批次，498 符号）。
// 依据 docs/class_func_reports/CUserCharacInfo.md（98 个 T 函数）与 ORIG
// 反汇编（386 个 W 访问器）逐函数实现；identical 口径编译验证。
// ============================================================================

#include "CUserCharacInfo.h"

#include <string.h>

#include <algorithm>

#include "CUser.h"
#include "CInventory.h"
#include "LogManager.h"
#include "GameWorld.h"

// ----------------------------------------------------------------------------
// _Charac_info 本地完整布局（ORIG 尺寸 0x14d1；与 SkillSlot.h 的占位不冲突）
// ----------------------------------------------------------------------------
// STL 成员用 pack 外包装类型固定布局（pack(1) 会使首次实例化缩为 0x15）
struct CharacInfoCoolList
{
    std::vector<ContinuousItemInfo> m_list;
};
struct CharacInfoEffectList
{
    std::vector<ContinuousItemInfo> m_list;
};
struct CharacInfoBloodMap
{
    std::map<int, stBloodBestRecord> m_map;
};

// m_growType 位域（ORIG getCurCharFirst/SecondGrowType 为真实成员位域提取）
union GrowTypeBits
{
    unsigned char byte;
    struct
    {
        signed char first : 4;   // bit0..3
        signed char second : 3;  // bit4..6
        signed char third : 1;   // bit7
    };
};

#pragma pack(push, 1)
struct CharacInfoFields
{
    int m_characNo;                    // +0x00
    char m_name[0x1e];                 // +0x04..0x22
    unsigned char m_vill;              // +0x22
    unsigned char m_villPvP;           // +0x23
    char m_pad24;                      // +0x24
    unsigned char m_villPrev;          // +0x25
    unsigned char m_job;               // +0x26
    short m_level;                      // +0x27
    GrowTypeBits m_growType;           // +0x29（位域）
    unsigned char m_growTypeChanging;  // +0x2a
    int m_exp;                         // +0x2b
    int m_partyBonusExp;               // +0x2f
    unsigned short m_fatigue;          // +0x33
    unsigned short m_maxFatigue;       // +0x35
    unsigned short m_fatigueGrownUpBuff;   // +0x37
    CommonTime m_createTime;           // +0x39（0x14）
    unsigned short m_usedFatigue;      // +0x4d
    unsigned short m_fatigueBattery;   // +0x4f
    unsigned int m_tutorialFlags;      // +0x51
    char m_tutorialFlags2[0x10];       // +0x55
    CReliablePerson m_reliablePerson;  // +0x65（0x10）
    unsigned char m_stamina;           // +0x75
    unsigned char m_backupStamina;     // +0x76
    unsigned short m_premiumFatigue;   // +0x77
    unsigned short m_maxPremiumFatigue;// +0x79
    long m_lastPlayTick;               // +0x7b
    long m_lastPlayTickPowerWar;       // +0x7f
    char m_pad83[5];                   // +0x83..0x88
    char m_addInfo[0x52];              // +0x88..0xda
    char m_inven[0x892 - 0xda];        // +0xda..0x892
    char m_skill[0xdaa - 0x892];       // +0x892..0xdaa
    char m_cargo[0xdbe - 0xdaa];       // +0xdaa..0xdbe
    int m_guildkey;                    // +0xdbe
    unsigned char m_guildCreateRight;  // +0xdc2
    unsigned short m_guildCreateFlag;  // +0xdc3
    char m_guildName[0x17];            // +0xdc5
    char m_guildAddr[0xd];             // +0xddc
    int m_guildExp;                    // +0xde9
    unsigned short m_guildTempExp;      // +0xded
    int m_guildTodayExp;               // +0xdef
    unsigned char m_guildMemberGrade;  // +0xdf3
    unsigned int m_dungeonHelpAbuseRatio;  // +0xdf4
    unsigned int m_dungeonHelpAbuseExp;    // +0xdf8
    short m_levelBeforeDungeon;        // +0xdfc
    unsigned int m_helpAbuseComputedRatio; // +0xdfe
    long m_loginTick;                  // +0xe02
    int m_finishPoint;                 // +0xe06
    int m_finishPointTotal;            // +0xe0a
    float m_warAreaKill[5];            // +0xe0e
    int m_memberkey;                   // +0xe22
    int m_connectUpperMember;          // +0xe26
    int m_memberBonusExp;              // +0xe2a
    int m_fatigueBufBonusExp;          // +0xe2e
    unsigned char m_uppermemberExpLevel;  // +0xe32
    int m_memberPayTexMoney;           // +0xe33
    int m_memberPayTexFatigue;         // +0xe37
    char m_overEquipFlag;              // +0xe3b
    int m_blackCount;                  // +0xe3c
    union
    {
        unsigned char m_visibleValues;              // +0xe40（整字节）
        struct
        {
            unsigned int visible : 1;               // bit0
            unsigned int growAvatar : 1;            // bit1
            unsigned int teleport : 1;              // bit2
            unsigned int m_padBits : 5;
        } m_visibleBits;
    };
    int m_assaultPlace;                // +0xe41
    int m_assaultCount;                // +0xe45
    int m_chaosPoint;                  // +0xe49
    int m_chaosExp;                    // +0xe4d
    int m_chaosModeCount;              // +0xe51
    int m_chaosKillCount;              // +0xe55
    int m_chaosDieCount;               // +0xe59
    int m_chaosDieTime;                // +0xe5d
    int m_chaosStateTime;              // +0xe61
    unsigned char m_superState;        // +0xe65
    unsigned char m_ghost;             // +0xe66
    unsigned char m_leaveTimeout;      // +0xe67
    int m_chaosKillTime;               // +0xe68
    int m_assaultFatigueCount;         // +0xe6c
    unsigned char m_opencoin;          // +0xe70
    int m_luckPoint;                   // +0xe71
    int m_luckPointUpdateTimeCount;    // +0xe75
    int m_dungeonPlayCount;            // +0xe79
    int m_expertJobType;               // +0xe7d
    int m_expertJobExp;                // +0xe81
    expert_job::CExpertJob* m_expertJob;   // +0xe85
    online_preliminary::COnlinePreliminaryTeam* m_onlinePreliminaryTeam;  // +0xe89
    unsigned short m_powerWarHP;       // +0xe8d
    unsigned char m_joinPower;         // +0xe8f
    char m_padE90;                     // +0xe90
    long m_powerWarProcessLastUpdateTime;  // +0xe91
    long m_powerWarLastPlayTime;       // +0xe95
    char m_padE99;                     // +0xe99
    CharacInfoCoolList m_coolTimeItemList;   // +0xe9a
    CharacInfoEffectList m_effectItemList;   // +0xea6
    unsigned char m_isAffectedExpDouble;   // +0xeb2
    unsigned char m_oneDayLetheFlag;       // +0xeb3
    unsigned char m_isInitSkillFlag;       // +0xeb4
    unsigned char m_oneDayLetheFlag2ND;    // +0xeb5
    unsigned char m_isInitSkillFlag2ND;    // +0xeb6
    unsigned char m_firstTimeLogin;        // +0xeb7
    unsigned char m_saveDemensionInout;    // +0xeb8
    char m_demensionInout[6];              // +0xeb9
    unsigned char m_saveBloodInout;        // +0xebf
    char m_ultimateInout[3];               // +0xec0
    unsigned char m_bloodBestRecordUpdate; // +0xec3
    int m_bloodBestRound;                  // +0xec4
    unsigned int m_bloodBestTime;          // +0xec8
    CharacInfoBloodMap m_bloodBestMap;       // +0xecc（0x18）
    unsigned char m_isExpItemAffected;     // +0xee4
    float m_expAffectRate;                 // +0xee5
    unsigned char m_isDisguiseCharac;      // +0xee9
    unsigned char m_disguiseKind;          // +0xeea
    unsigned short m_disguiseIndex;        // +0xeeb
    char m_backupSkill[0x199];             // +0xeed
    char m_backupSkill2ND[0x199];          // +0x1086
    unsigned short m_powerWarPoint;        // +0x121f
    int m_powerWarAssaultCount;            // +0x1221
    int m_powerWarAssaultVictoryCount;     // +0x1225
    unsigned short m_straightVictories;    // +0x1229
    unsigned char m_isEndLetheAtLogin;     // +0x122b
    unsigned short m_curCharacEmotion;     // +0x122c
    long m_lastItemLimitEditionBuyTime;    // +0x122e
    unsigned short m_curConditionEventStep;    // +0x1232
    unsigned short m_curConditionEventRewardStep;  // +0x1234
    unsigned short m_properDungeonClearCount;     // +0x1236
    unsigned char m_properDungeonUser;     // +0x1238
    char m_skillTreeIndex;                 // +0x1239
    unsigned short m_curCharacEventItemCount;    // +0x123a
    unsigned short m_curCharacEventAddItemCount; // +0x123c
    char m_curExEquipSlotStat;             // +0x123e
    unsigned char m_eventCharacterGrowtype;     // +0x123f
    char m_pad1240;                    // +0x1240
    unsigned char m_memberDungeonFlag;     // +0x1241
    unsigned char m_revengeCount;          // +0x1242
    unsigned char m_revengeDungeonCount;   // +0x1243
    unsigned char m_villageAttackDBUpdate; // +0x1244
    char m_linkCharacSlotIndex;            // +0x1245
    unsigned char m_characLinkType;        // +0x1246
    int m_characLinkBonusExp;              // +0x1247
    int m_characLinkBonusGold;             // +0x124b
    char m_pad124f[5];                     // +0x124f
    char m_skillCommand[0x18];             // +0x1254
    unsigned short m_memberBonusFatigue;   // +0x126c
    unsigned char m_guildSecede;           // +0x126e
    int m_lastPlayDungeonIndex;            // +0x126f
    unsigned char m_upgradeSeparateTrue;   // +0x1273
    unsigned char m_upgradeSeparateFalse;  // +0x1274
    char m_curEquipslotFlag;               // +0x1275
    unsigned char m_curChannelEquipslotSwitch;  // +0x1276
    unsigned char m_curExpandEquipslotSwitch;   // +0x1277
    unsigned char m_bringUpRewardFlag;     // +0x1278
    unsigned char m_curUseBringUpMaterialCount; // +0x1279
    unsigned char m_curUseBringUpMaterialTotal; // +0x127a
    unsigned char m_growthPowerExpRewardRight;  // +0x127b
    int m_seriaBufBonusExp;                // +0x127c
    unsigned char m_growthPowerReward;     // +0x1280
    union
    {
        stAuraAvatarOption m_auraAvatarOption;  // +0x1281（ORIG 结构复制形态）
        int m_auraAvatarOptionArr[3];           // +0x1281（ORIG 下标形态）
    };
    int m_premiumBonusExp;                 // +0x128d
    int m_weekendBonusExp;                 // +0x1291
    char m_pad1295[8];                     // +0x1295
    WpBonusPointResult m_wpBonus;    // +0x129d（8 字节）
    ExpBonusPointResult m_expBonus;   // +0x12a5（8 字节）
    int m_chaosResponTime;                 // +0x12ad
    int m_assaultMoney;                    // +0x12b1
    unsigned short m_assaultedUserId;      // +0x12b5
    CharacterOption m_characterOption;     // +0x12b7
    unsigned char m_strikerSkillIndex;     // +0x14b7
    int m_tagCharacNo;                     // +0x14b8
    char m_pad14bc;                        // +0x14bc
    unsigned char m_blueMarbleEnterCount;  // +0x14bd
    int m_blueMarbleUserDeadCount;         // +0x14be
    int m_maxEquipLevel;                   // +0x14c2
    int m_characViewState;                 // +0x14c6
    seriaRoom_AniDeco m_seriaRoomAniDeco;  // +0x14ca
    unsigned char m_cubePremiumContract;   // +0x14ce
    unsigned char m_cubePremiumCount;      // +0x14cf
    unsigned char m_cubePremiumSaved;      // +0x14d0
};
#pragma pack(pop)
int DEFAULT_MAX_FATIGUE = 0;  // ORIG 全局（0x9370f5c），原 GameStubs.cpp 定义迁移

#define CUR ((CharacInfoFields*)m_selected)

CUserCharacInfo::~CUserCharacInfo()
{
    m_dungeonClear.~CDungeonClear();
    m_battleRecord.~map();
    m_pvpResult.~PvpResultType();
}

void CUserCharacInfo::useWinPoint(int point, eWPSubReason)
{
    enableSavePvP();
    if (m_pvpResult.m_winPoint >= point)
        m_pvpResult.m_winPoint -= point;
    else
        m_pvpResult.m_winPoint = 0;
}

void CUserCharacInfo::updateLastItemLimitEditionBuyTime(long t)
{
    if (m_selected)
    {
    CUR->m_lastItemLimitEditionBuyTime = t;
    }
}


void CUserCharacInfo::subtractCurCharacExp(int exp)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_exp -= exp;
    }
}


void CUserCharacInfo::subUltimateInoutValue(char index)
{
    CUR->m_saveBloodInout = 1;
    --CUR->m_ultimateInout[index];
}

void CUserCharacInfo::subDemensionInoutValue(char index)
{
    if (index > 5 || index < 0)
        return;
    CUR->m_saveDemensionInout = 1;
    --CUR->m_demensionInout[index];
}

void CUserCharacInfo::set_uppermember_exp_level(unsigned char level)
{
    if (m_selected)
    {
        if (level != 0 && CUR->m_uppermemberExpLevel > level)
        {
            LogManager::logFormat(1, "user.cpp",
                                  "void CUserCharacInfo::set_uppermember_exp_level(unsigned char)",
                                  0x2d2, "set member exp level error , name(%s) , level(%d), new level(%d)",
                                  (char*)CUR + 4, (unsigned int)CUR->m_uppermemberExpLevel,
                                  (unsigned int)level);
        }
        else
        {
            CUR->m_uppermemberExpLevel = level;
        }
    }
}


void CUserCharacInfo::set_member_pay_tex_money_to_upper(int money)
{
    if (m_selected)
    {
    CUR->m_memberPayTexMoney = money;
    }
}


void CUserCharacInfo::set_member_pay_tex_fatigue_to_upper(int fatigue)
{
    if (m_selected)
    {
    CUR->m_memberPayTexFatigue = fatigue;
    }
}


void CUserCharacInfo::set_level_before_dungeon(short level)
{
    if (m_selected)
    {
    CUR->m_levelBeforeDungeon = level;
    }
}


void CUserCharacInfo::set_guild_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildExp = exp;
}


void CUserCharacInfo::set_guild_create_right(unsigned char right)
{
    if (m_selected)
    {
    enableSaveCharacEtc();
    CUR->m_guildCreateRight = right;
    }
}


void CUserCharacInfo::set_growth_power_reward(int reward)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_growthPowerReward = (unsigned char)reward;
    }
}


void CUserCharacInfo::set_growth_power_exp_reward_right(bool flag)
{
    if (m_selected)
    {
    CUR->m_growthPowerExpRewardRight = flag;
    }
}


void CUserCharacInfo::set_connect_upper_member(unsigned int member)
{
    if (m_selected)
    {
    CUR->m_connectUpperMember = member;
    }
}


void CUserCharacInfo::set_charac_visible_values(char values)
{
    if (m_selected)
    {
    CUR->m_visibleValues = (unsigned char)values;
    }
}


void CUserCharacInfo::set_charac_visible_teleport_(bool flag)
{
    if (m_selected)
    {
        CUR->m_visibleBits.teleport = flag;
    }
}


void CUserCharacInfo::set_charac_visible_grow_avatar(bool flag)
{
    if (m_selected)
    {
        CUR->m_visibleBits.growAvatar = flag;
    }
}


void CUserCharacInfo::set_charac_seria_buf_bonus_exp(int exp)
{
    if (m_selected)
    {
    CUR->m_seriaBufBonusExp = exp;
    }
}


void CUserCharacInfo::set_charac_party_bonus_exp(int exp)
{
    if (m_selected)
    {
    CUR->m_partyBonusExp = exp;
    }
}


void CUserCharacInfo::set_charac_memberkey(unsigned int key)
{
    if (m_selected)
    {
    CUR->m_memberkey = key;
    }
}


void CUserCharacInfo::set_charac_member_bonus_exp(int exp)
{
    if (m_selected)
    {
    CUR->m_memberBonusExp = exp;
    }
}


void CUserCharacInfo::set_charac_guildkey(unsigned int key)
{
    if (m_selected)
    {
    CUR->m_guildkey = key;
    }
}


void CUserCharacInfo::set_charac_fatigue_buf_bonus_exp(int exp)
{
    if (m_selected)
    {
    CUR->m_fatigueBufBonusExp = exp;
    }
}


void CUserCharacInfo::set_charac_createtime(long t)
{
    ((CommonTime*)((char*)m_selected + 0x39))->SetCreateTime(t);
}

void CUserCharacInfo::set_aura_avatar_option_value(int index, int value)
{
    if (m_selected && index >= 0 && index <= 2)
        CUR->m_auraAvatarOptionArr[index] = value;
}


void CUserCharacInfo::set_aura_avatar_option(stAuraAvatarOption& option)
{
    if (m_selected)
        CUR->m_auraAvatarOption = option;
}



void CUserCharacInfo::setWeekendBonusFatigue(unsigned short value)
{
    if (m_selected)
    {
    CUR->m_maxFatigue = (unsigned short)(DEFAULT_MAX_FATIGUE + value);
    }
}


void CUserCharacInfo::setWeekendBonusExp(unsigned int exp, int percent)
{
    if (m_selected)
    {
        CUR->m_weekendBonusExp = (int)(long long)((double)exp * (percent / 100.0f));
    }
}


void CUserCharacInfo::setUserEventCharacterFlag(unsigned char flag) { m_userEventCharacterFlag = flag; }

void CUserCharacInfo::setUpgradeSeparateTrueFlag(char flag)
{
    if (m_selected)
    {
    CUR->m_upgradeSeparateTrue = (unsigned char)flag;
    }
}


void CUserCharacInfo::setUpgradeSeparateFalseFlag(char flag)
{
    if (m_selected)
    {
    CUR->m_upgradeSeparateFalse = (unsigned char)flag;
    }
}


void CUserCharacInfo::setUltimateInoutValue(char index, char value)
{
    CUR->m_saveBloodInout = 1;
    CUR->m_ultimateInout[index] = (unsigned char)value;
}

void CUserCharacInfo::setTagCharac(_Charac_info* p) { m_tag = p; }

void CUserCharacInfo::setStraightVictories(unsigned short count)
{
    if (m_selected)
    {
    CUR->m_straightVictories = count;
    }
}


void CUserCharacInfo::setSeriaRoomAniDecoInfo(seriaRoom_AniDeco info)
{
    if (m_selected)
        CUR->m_seriaRoomAniDeco = info;
}



void CUserCharacInfo::setSaveCharacOption(bool flag)
{
    m_saveCharacOption = flag;
}

int CUserCharacInfo::setPremiumBonusExp(unsigned int exp, int percent)
{
    if (m_selected)
    {
        CUR->m_premiumBonusExp = (int)(long long)((double)exp * (percent / 100.0f));
        return CUR->m_premiumBonusExp;
    }
    return 0;
}


void CUserCharacInfo::setPowerWarHP(unsigned short hp)
{
    if (m_selected)
    {
    CUR->m_powerWarHP = hp;
    }
}


void CUserCharacInfo::setMemberDungeonFlag(bool flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_memberDungeonFlag = flag;
    }
}


void CUserCharacInfo::setLinkCharacSlotIndex(char index)
{
    if (m_selected)
    {
    CUR->m_linkCharacSlotIndex = index;
    }
}


void CUserCharacInfo::setGuildMemberGrade(unsigned char grade)
{
    if (m_selected)
    {
    CUR->m_guildMemberGrade = grade;
    }
}


void CUserCharacInfo::setGuildCreateFlag(unsigned short flag)
{
    if (m_selected)
    {
    CUR->m_guildCreateFlag = (unsigned short)(CUR->m_guildCreateFlag | flag);
    }
}


void CUserCharacInfo::setDisguiseCharac(bool flag, unsigned char kind, unsigned short index)
{
    if (m_selected)
    {
    CUR->m_isDisguiseCharac = flag;
    setDisguiseKind(kind);
    setDisguiseIndex(index);
    }
}


void CUserCharacInfo::setDisguiseKind(unsigned char kind)
{
    if (m_selected)
    {
    CUR->m_disguiseKind = kind;
    }
}


void CUserCharacInfo::setDisguiseIndex(unsigned short index)
{
    if (m_selected)
    {
    CUR->m_disguiseIndex = index;
    }
}


void CUserCharacInfo::setDemensionInoutValue(char index, char value)
{
    if (index > 5 || index < 0)
        return;
    CUR->m_saveDemensionInout = 1;
    CUR->m_demensionInout[index] = (unsigned char)value;
}

void CUserCharacInfo::setCurrCharacJoinPower(char power)
{
    enableSaveCharacStat();
    if (m_selected)
        CUR->m_joinPower = (unsigned char)power;
}




void CUserCharacInfo::setCurrCharacBlackCount(unsigned int count)
{
    if (m_selected)
    {
    CUR->m_blackCount = count;
    }
}


void CUserCharacInfo::setCurUseBringUpMaterialTotal(unsigned char total)
{
    if (m_selected)
    {
    CUR->m_curUseBringUpMaterialTotal = total;
    }
}


void CUserCharacInfo::setCurEquipslotFlag(char flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_curEquipslotFlag = flag;
    }
}


void CUserCharacInfo::setCurCharacVill(char vill)
{
    if (!m_selected)
        return;
    CUR->m_villPrev = CUR->m_vill;
    if (G_GameWorld()->IsPVPChannel())
    {
        CUR->m_villPvP = 2;
    }
    else
    {
        enableSaveCharacStat();
        CUR->m_vill = (unsigned char)vill;
    }
}



bool CUserCharacInfo::setCurCharacTutorialFlag(unsigned int flag)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        if (flag < 0x20)
        {
            if ((CUR->m_tutorialFlags & (1u << (unsigned char)flag)) != 0)
                return false;
            CUR->m_tutorialFlags |= 1u << flag;
            return true;
        }
        if (flag < 0x80)
        {
            int index = (int)(flag - 0x20);
            int byteIndex = (int)(((unsigned int)(index >> 0x1f) >> 0x1d) + index) >> 3;
            if (byteIndex >= 0 && byteIndex < 0x10)
            {
                unsigned char bit = (unsigned char)(flag % 8);
                if ((((signed char)CUR->m_tutorialFlags2[byteIndex]) >> bit & 1) != 0)
                    return false;
                CUR->m_tutorialFlags2[byteIndex] =
                    (unsigned char)((1 << (unsigned char)bit) | CUR->m_tutorialFlags2[byteIndex]);
                return true;
            }
            return false;
        }
    }
    return false;
}

void CUserCharacInfo::setCurCharacTradeGoldDaily(unsigned int gold)
{
    enableSaveCharacStat();
    CUR->m_reliablePerson.SetTradeGoldDaily(gold);
}

void CUserCharacInfo::setCurCharacTagCharacNo(unsigned int no)
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    CUR->m_tagCharacNo = no;
    }
}


void CUserCharacInfo::setCurCharacStrikerSkillIndex(unsigned char index)
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    CUR->m_strikerSkillIndex = index;
    }
}


void CUserCharacInfo::setCurCharacStamina(unsigned char value)
{
    enableSaveCharacStat();
    CUR->m_stamina = value;
}



void CUserCharacInfo::setCurCharacPremiumFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_premiumFatigue = value;
    }
}


void CUserCharacInfo::setCurCharacMaxPremiumFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacEtc();
    CUR->m_maxPremiumFatigue = value;
    }
}


void CUserCharacInfo::setCurCharacLevel(int level)
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    CUR->m_level = (unsigned short)level;
    }
}


void CUserCharacInfo::setCurCharacLastPlayTickPowerWar(long tick)
{
    enableSaveCharacStat();
    CUR->m_lastPlayTickPowerWar = tick;
}



void CUserCharacInfo::setCurCharacLastPlayTick(long tick)
{
    enableSaveCharacStat();
    CUR->m_lastPlayTick = tick;
}



void CUserCharacInfo::setCurCharacLastPlayDungeonIndex(unsigned int index)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_lastPlayDungeonIndex = index;
    }
}


void CUserCharacInfo::setCurCharacHelpAbuseComputedRatio(unsigned int ratio)
{
    if (m_selected)
    {
    CUR->m_helpAbuseComputedRatio = ratio;
    }
}


void CUserCharacInfo::setCurCharacGuildSecede(char flag)
{
    CUR->m_guildSecede = (unsigned char)flag;
}

void CUserCharacInfo::setCurCharacFatigueGrownUpBuff(unsigned short value)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        CUR->m_fatigueGrownUpBuff = value;
    }
}




void CUserCharacInfo::setCurCharacFatigue(int fatigue)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_fatigue = (unsigned short)fatigue;
    }
}


void CUserCharacInfo::setCurCharacExp(int exp)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_exp = exp;
    }
}


void CUserCharacInfo::setCurCharacEventCharacterGrowtype(char growtype)
{
    enableSaveCharacEtc();
    CUR->m_eventCharacterGrowtype = (unsigned char)growtype;
}



void CUserCharacInfo::setCurCharacBackupStamina(unsigned char value)
{
    if (m_selected)
    {
    CUR->m_backupStamina = value;
    }
}


void CUserCharacInfo::setCurCharac(_Charac_info* p)
{
    enableSaveCharacInfo();
    enableSaveCharacStat();
    enableSaveCharacEtc();
    m_selected = p;
}

void CUserCharacInfo::setCurCharGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    enableSaveCharacInfo();
    CUR->m_growType.byte = (unsigned char)(
        (CUR->m_growType.byte & 0xf0) | ((char)(first << 4) >> 4 & 0xf));
    CUR->m_growType.byte = (unsigned char)(
        (CUR->m_growType.byte & 0x8f) | (((char)(second << 5) >> 5 & 7) << 4));
}



void CUserCharacInfo::setCurCharChangingGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    CUR->m_growTypeChanging = (unsigned char)(
        (CUR->m_growTypeChanging & 0xf0) | ((char)(first << 4) >> 4 & 0xf));
    CUR->m_growTypeChanging = (unsigned char)(
        (CUR->m_growTypeChanging & 0x8f) | (((char)(second << 5) >> 5 & 7) << 4));
}



void CUserCharacInfo::setCurChannelEquipslotSwitch(char flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_curChannelEquipslotSwitch = (unsigned char)flag;
    }
}


bool CUserCharacInfo::setConctractOfCubePremiumInfo(bool active, unsigned char count)
{
    if (!m_selected)
        return false;
    CUR->m_cubePremiumSaved = 1;
    CUR->m_cubePremiumContract = active;
    CUR->m_cubePremiumCount = count;
    return true;
}

void CUserCharacInfo::setCharacViewState(int state)
{
    if (m_selected)
    {
    CUR->m_characViewState = state;
    }
}


void CUserCharacInfo::setCharacOption(CharacterOption& option)
{
    if (m_selected)
        CUR->m_characterOption = option;
}




void CUserCharacInfo::setCharacLinkBonusGold(unsigned int gold)
{
    if (m_selected)
    {
    CUR->m_characLinkBonusGold = gold;
    }
}


void CUserCharacInfo::setCharacLinkBonusExp(unsigned int exp)
{
    if (m_selected)
    {
    CUR->m_characLinkBonusExp = exp;
    }
}


void CUserCharacInfo::setBringUpRewardFlag(bool flag)
{
    if (m_selected)
    {
    CUR->m_bringUpRewardFlag = flag;
    }
}


void CUserCharacInfo::setBlueMarbleEnterCount(unsigned char count)
{
    if (m_selected)
    {
    CUR->m_blueMarbleEnterCount = count;
    }
}


void CUserCharacInfo::setBloodBestRecord(unsigned int time, int round, int record, bool flag)
{
    if (m_selected)
    {
    if (round > 0xf)
        round = 0xf;
    std::map<int, stBloodBestRecord>::iterator it =
        CUR->m_bloodBestMap.m_map.find(record);
    if (it == CUR->m_bloodBestMap.m_map.end())
    {
        if (!flag)
            setBloodBestRecordUpdate(true);
        int local[2];
        local[0] = round;
        local[1] = (int)time;
        CUR->m_bloodBestMap.m_map.insert(
            std::make_pair<int&, stBloodBestRecord&>(
                local[0], *(stBloodBestRecord*)&record));
        CUR->m_bloodBestRound = round;
        CUR->m_bloodBestTime = time;
    }
    else
    {
        CUR->m_bloodBestRound = it->second.m_round;
        CUR->m_bloodBestTime = it->second.m_time;
        if (CUR->m_bloodBestRound <= round &&
            (CUR->m_bloodBestTime != time || CUR->m_bloodBestRound != round))
        {
            if (CUR->m_bloodBestRound == round)
            {
                setBloodBestRecordUpdate(true);
                CUR->m_bloodBestTime = std::min(CUR->m_bloodBestTime, time);
            }
            else
            {
                setBloodBestRecordUpdate(true);
                CUR->m_bloodBestTime = time;
                CUR->m_bloodBestRound = round;
            }
            it->second.m_round = CUR->m_bloodBestRound;
            it->second.m_time = CUR->m_bloodBestTime;
        }
    }
    }
}


void CUserCharacInfo::setBloodBestRecordUpdate(bool flag)
{
    if (m_selected)
    {
    CUR->m_bloodBestRecordUpdate = flag;
    }
}


void CUserCharacInfo::reset_level_before_dungeon()
{
    if (m_selected)
    {
    CUR->m_levelBeforeDungeon = 0;
    }
}


void CUserCharacInfo::reset_is_end_lethe_at_login()
{
    if (m_selected)
    {
    CUR->m_isEndLetheAtLogin = 0;
    }
}


void CUserCharacInfo::reset_guild_today_exp()
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildTodayExp = 0;
}


void CUserCharacInfo::reset_guild_temp_exp()
{
    if (m_selected)
    {
    CUR->m_guildTempExp = 0;
    }
}


void CUserCharacInfo::reset_guild_exp()
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildExp = 0;
}


void CUserCharacInfo::resetWeekendBonusExp()
{
    if (m_selected)
    {
    CUR->m_weekendBonusExp = 0;
    }
}


void CUserCharacInfo::resetTempGuildNameAddr()
{
    if (m_selected)
    {
    memset((char*)CUR + 0xdc5, 0, 0x17);
    memset((char*)CUR + 0xddc, 0, 0xd);
    }
}


void CUserCharacInfo::resetStraightVictories()
{
    if (m_selected)
    {
    CUR->m_straightVictories = 0;
    }
}


void CUserCharacInfo::resetPremiumBonusExp()
{
    if (m_selected)
    {
    CUR->m_premiumBonusExp = 0;
    }
}


void CUserCharacInfo::resetGuildCreateFlag()
{
    if (m_selected)
    {
    CUR->m_guildCreateFlag = 0;
    }
}


void CUserCharacInfo::resetCurUseBringUpMaterialCount()
{
    if (m_selected)
    {
    CUR->m_curUseBringUpMaterialCount = 0;
    }
}


void CUserCharacInfo::resetCurCharacUsedFatigue()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_usedFatigue = 0;
    }
}


void CUserCharacInfo::resetCurCharacSchoolPoint()
{
    if (m_selected)
    {
    CUR->m_reliablePerson.m_schoolPoint = 0;
    }
}


void CUserCharacInfo::resetCurCharacOverEquipFlag()
{
    if (!m_selected)
        return;
    CUR->m_overEquipFlag = 0;
}



void CUserCharacInfo::resetCurCharacMemberBonusFatigue()
{
    enableSaveCharacStat();
    CUR->m_memberBonusFatigue = 0;
}




void CUserCharacInfo::resetCharacTutorialFlag()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_tutorialFlags = 0;
    memset(CUR->m_tutorialFlags2, 0, 0x10);
    }
}


void CUserCharacInfo::resetCharacFatigueGrownUpBuff()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_fatigueGrownUpBuff = 0;
    }
}


void CUserCharacInfo::resetCharGuildInfo()
{
    if (m_selected)
    {
    CUR->m_guildkey = 0;
    CUR->m_guildTempExp = 0;
    CUR->m_guildExp = 0;
    CUR->m_guildMemberGrade = 0;
    CUR->m_guildCreateRight = 0;
    CUR->m_guildCreateFlag = 0;
    CUR->m_guildTodayExp = 0;
    }
}


void CUserCharacInfo::resetBlueMarbleUserDeadCount()
{
    if (m_selected)
    {
    CUR->m_blueMarbleUserDeadCount = 0;
    }
}


void CUserCharacInfo::resetBlueMarbleEnterCount()
{
    if (m_selected)
    {
    CUR->m_blueMarbleEnterCount = 0;
    }
}


unsigned char CUserCharacInfo::needCurCharacCheckOverEquip()
{
    if (!m_selected)
        return 0;
    return (unsigned char)CUR->m_overEquipFlag;
}


unsigned char CUserCharacInfo::is_growth_power_exp_reward_right() const
{
    return m_selected ? CUR->m_growthPowerExpRewardRight : 0;
}

bool CUserCharacInfo::is_connect_upper_member()
{
    if (m_selected && CUR->m_connectUpperMember != 0)
        return true;
    return false;
}




bool CUserCharacInfo::isSetGuildCreateFlag(unsigned short flag)
{
    if (m_selected)
    {
        if ((CUR->m_guildCreateFlag & flag) == flag)
            return true;
        return false;
    }
    return false;
}



unsigned char CUserCharacInfo::isSavedPremiumCubeInfo() const
{
    if (!m_selected)
        return 0;
    return CUR->m_cubePremiumSaved;
}


char CUserCharacInfo::isSaveSkill() { return m_saveSkill; }

char CUserCharacInfo::isSaveQuest() { return m_saveQuest; }

char CUserCharacInfo::isSavePvP() { return m_savePvP; }

char CUserCharacInfo::isSaveInven() { return m_saveInven; }

char CUserCharacInfo::isSaveDungeonClear() { return m_saveDungeonClear; }

char CUserCharacInfo::isSaveCharacOption() { return m_saveCharacOption; }

char CUserCharacInfo::isSaveCharacInfo() { return m_saveCharacInfo; }

char CUserCharacInfo::isSaveCharacEtc() { return m_saveCharacEtc; }

char CUserCharacInfo::isSaveCargo() { return m_saveCargo; }

char CUserCharacInfo::isSaveBattleRecord() { return m_saveBattleRecord; }

unsigned char CUserCharacInfo::isJoinPowerWar() const
{
    return m_selected ? CUR->m_joinPower : 0;
}

unsigned short CUserCharacInfo::getPowerWarHP()
{
    return m_selected ? CUR->m_powerWarHP : 0;
}

bool CUserCharacInfo::isExistItem(int itemIdx)
{
    if (!m_selected)
        return false;
    if (((const CCargo*)((const char*)m_selected + 0xdaa))->check_item_exist(itemIdx) != -1)
        return true;
    if (((const CInventory*)((const char*)m_selected + 0xda))->check_item_exist(itemIdx) != -1)
        return true;
    return false;
}



unsigned char CUserCharacInfo::isDisguiseCharac()
{
    return m_selected ? CUR->m_isDisguiseCharac : 0;
}


unsigned char CUserCharacInfo::isBloodBestRecordUpdate()
{
    return CUR->m_bloodBestRecordUpdate;
}

bool CUserCharacInfo::initContractOfCubePremiumInfo(bool active, unsigned char count)
{
    if (!m_selected)
        return false;
    CUR->m_cubePremiumSaved = 0;
    CUR->m_cubePremiumContract = active;
    CUR->m_cubePremiumCount = count;
    return true;
}

void CUserCharacInfo::increaseBlueMarbleUserDeadCount()
{
    if (m_selected)
    {
    ++CUR->m_blueMarbleUserDeadCount;
    }
}


void CUserCharacInfo::increaseBlueMarbleEnterCount()
{
    if (m_selected)
    {
    ++CUR->m_blueMarbleEnterCount;
    }
}


void CUserCharacInfo::incStraightVictories()
{
    if (m_selected)
    {
    ++CUR->m_straightVictories;
    }
}


void CUserCharacInfo::incCurUseBringUpMaterialCount()
{
    if (m_selected)
    {
    ++CUR->m_curUseBringUpMaterialCount;
    ++CUR->m_curUseBringUpMaterialTotal;
    }
}


void CUserCharacInfo::incCurCharacUsedFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_usedFatigue = (unsigned short)(CUR->m_usedFatigue + value);
    }
}


void CUserCharacInfo::incCurCharacMemberBonusFatigue(short value)
{
    enableSaveCharacStat();
    CUR->m_memberBonusFatigue =
        (unsigned short)(CUR->m_memberBonusFatigue + value);
}



void CUserCharacInfo::incCurCharacLevel()
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    ++CUR->m_level;
    }
}


unsigned char CUserCharacInfo::get_uppermember_exp_level()
{
    return m_selected ? CUR->m_uppermemberExpLevel : 0;
}

int CUserCharacInfo::get_pvp_result_oldexp_point() const { return m_pvpResult.m_oldExpPoint; }

int CUserCharacInfo::get_pvp_result_exp_point() const { return m_pvpResult.m_expPoint; }

int CUserCharacInfo::get_pvp_grade() const { return m_pvpResult.m_pvpGrade; }

int CUserCharacInfo::get_member_pay_tex_money_to_upper() const
{
    return m_selected ? CUR->m_memberPayTexMoney : 0;
}

int CUserCharacInfo::get_member_pay_tex_fatigue_to_upper() const
{
    return m_selected ? CUR->m_memberPayTexFatigue : 0;
}

int CUserCharacInfo::get_guild_today_exp()
{
    return m_selected ? CUR->m_guildTodayExp : 0;
}

int CUserCharacInfo::get_guild_temp_exp()
{
    return m_selected ? CUR->m_guildTempExp : 0;
}

int CUserCharacInfo::get_guild_exp()
{
    return m_selected ? CUR->m_guildExp : 0;
}

unsigned char CUserCharacInfo::get_guild_create_right()
{
    return m_selected ? CUR->m_guildCreateRight : 0;
}

unsigned char CUserCharacInfo::get_growth_power_reward() const
{
    return m_selected ? CUR->m_growthPowerReward : 0;
}

int CUserCharacInfo::get_connect_upper_member()
{
    return m_selected ? CUR->m_connectUpperMember : 0;
}

unsigned char CUserCharacInfo::get_charac_visible_values()
{
    return m_selected ? CUR->m_visibleValues : 0;
}

int CUserCharacInfo::get_charac_visible_grow_avatar()
{
    if (m_selected && CUR->m_visibleBits.growAvatar)
        return 1;
    return 0;
}


int CUserCharacInfo::get_charac_seria_buf_bonus_exp() const
{
    if (m_selected)
    {
        int value = CUR->m_seriaBufBonusExp;
        CUR->m_seriaBufBonusExp = 0;
        return value;
    }
    return 0;
}


int CUserCharacInfo::get_charac_party_bonus_exp() const
{
    return m_selected ? CUR->m_partyBonusExp : 0;
}

int CUserCharacInfo::get_charac_memberkey()
{
    return m_selected ? CUR->m_memberkey : 0;
}

int CUserCharacInfo::get_charac_member_bonus_exp() const
{
    return m_selected ? CUR->m_memberBonusExp : 0;
}

int CUserCharacInfo::get_charac_level_before_dungeon() const
{
    if (m_selected)
    {
        if (CUR->m_levelBeforeDungeon != 0)
            return CUR->m_levelBeforeDungeon;
        return CUR->m_level;
    }
    return 0;
}


int CUserCharacInfo::get_charac_job() const
{
    return m_selected ? (signed char)CUR->m_job : 0;
}

int CUserCharacInfo::get_charac_guildkey()
{
    return m_selected ? CUR->m_guildkey : 0;
}

int CUserCharacInfo::get_charac_fatigue_buf_bonus_exp() const
{
    return m_selected ? CUR->m_fatigueBufBonusExp : 0;
}

int CUserCharacInfo::get_charac_exp() const
{
    return m_selected ? CUR->m_exp : 0;
}

int CUserCharacInfo::get_charac_10_level_section() const
{
    if (m_selected)
    {
        if (CUR->m_level % 10 == 0)
            return CUR->m_level;
        return CUR->m_level - (CUR->m_level % 10) + 10;
    }
    return 10;
}

int CUserCharacInfo::getWeekendBonusExp()
{
    return m_selected ? CUR->m_weekendBonusExp : 0;
}

unsigned char CUserCharacInfo::getUserEventCharacterFlag() { return m_userEventCharacterFlag; }

char CUserCharacInfo::getUpgradeSeparateTrueFlag()
{
    return m_selected ? (char)CUR->m_upgradeSeparateTrue : 0;
}

char CUserCharacInfo::getUpgradeSeparateFlaseFlag()
{
    return m_selected ? (char)CUR->m_upgradeSeparateFalse : 0;
}

unsigned char CUserCharacInfo::getUltimateInoutValue(char index)
{
    return CUR->m_ultimateInout[index];
}

_Charac_info* CUserCharacInfo::getTagCharacW() { return m_tag; }

char* CUserCharacInfo::getTagCharacSkillW()
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (char*)((CharacInfoFields*)m_tag)->m_skill;
}

const char* CUserCharacInfo::getTagCharacSkillR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (const char*)((CharacInfoFields*)m_tag)->m_skill;
}

const _Charac_info* CUserCharacInfo::getTagCharacR() const { return m_tag; }

Inven_Item* CUserCharacInfo::getTagCharacInvenW()
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (Inven_Item*)((CharacInfoFields*)m_tag)->m_inven;
}

const Inven_Item* CUserCharacInfo::getTagCharacInvenR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (const Inven_Item*)((CharacInfoFields*)m_tag)->m_inven;
}

CCargo* CUserCharacInfo::getTagCharacCargoW()
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (CCargo*)((CharacInfoFields*)m_tag)->m_cargo;
}

const CCargo* CUserCharacInfo::getTagCharacCargoR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (const CCargo*)((CharacInfoFields*)m_tag)->m_cargo;
}

const void* CUserCharacInfo::getTagCharacAddInfoR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (const void*)((CharacInfoFields*)m_tag)->m_addInfo;
}

unsigned short CUserCharacInfo::getStraightVictories() const
{
    return m_selected ? CUR->m_straightVictories : 0;
}

const char* CUserCharacInfo::getSkillCommand() const
{
    return m_selected ? (const char*)CUR->m_skillCommand : 0;
}

seriaRoom_AniDeco CUserCharacInfo::getSeriaRoomAniDecoInfo()
{
    if (m_selected)
        return CUR->m_seriaRoomAniDeco;
    seriaRoom_AniDeco result;
    result.m_field2 = -1;
    return result;
}

void CUserCharacInfo::getSeparatedTextUnits(std::vector<std::string>& out,
                                            const char* text, const char* sep,
                                            int maxUnits)
{
    char buffer[0x100];
    out.clear();
    memset(buffer, 0, sizeof(buffer));
    int offset = 6;
    while (true)
    {
        bool stop = maxUnits != 0 && (int)out.size() >= maxUnits - 1;
        if (stop)
            break;
        const char* found = strstr(text + offset, sep);
        if (found == 0)
            break;
        int length = (int)(found - (text + offset));
        if (length != 0)
        {
            strncpy(buffer, text + offset, length);
            buffer[length] = 0;
            out.push_back(std::string(buffer, std::allocator<char>()));
        }
        offset += (int)strlen(sep) + length;
    }
    if (text[offset] != 0)
        out.push_back(std::string(text + offset, std::allocator<char>()));
}

int CUserCharacInfo::getRequestRenameState() const
{
    return m_selected ? CUR->m_characViewState : 0;
}

int CUserCharacInfo::getPremiumBonusExp() const
{
    return m_selected ? CUR->m_premiumBonusExp : 0;
}

PvpResultType* CUserCharacInfo::getPVPResultW()
{
    enableSavePvP();
    return &m_pvpResult;
}

PvpResultType* CUserCharacInfo::getPVPResultRefW()
{
    enableSavePvP();
    return &m_pvpResult;
}
const PvpResultType* CUserCharacInfo::getPVPResultRefR() const
{
    return &m_pvpResult;
}

PvpResultType* CUserCharacInfo::getPVPResultR() const
{
    return const_cast<PvpResultType*>(&m_pvpResult);
}

unsigned char CUserCharacInfo::getMemberDungeonFlag() const
{
    return m_selected ? (int)(unsigned char)CUR->m_memberDungeonFlag : 1;
}


char CUserCharacInfo::getLinkCharacSlotIndex()
{
    return m_selected ? (int)(unsigned char)CUR->m_linkCharacSlotIndex : -1;
}


long CUserCharacInfo::getLastItemLimitEditionBuyTime() const
{
    return m_selected ? CUR->m_lastItemLimitEditionBuyTime : 0;
}

unsigned char CUserCharacInfo::getGuildMemberGrade()
{
    return m_selected ? CUR->m_guildMemberGrade : 0;
}

WongWork::CDungeonClear* CUserCharacInfo::getDungeonClearRefW()
{
    enableSaveDungeonClear();
    return &m_dungeonClear;
}

const WongWork::CDungeonClear* CUserCharacInfo::getDungeonClearRefR() const
{
    return &m_dungeonClear;
}
unsigned char CUserCharacInfo::getDisguiseKind()
{
    return m_selected ? CUR->m_disguiseKind : 0;
}

unsigned short CUserCharacInfo::getDisguiseIndex()
{
    return m_selected ? CUR->m_disguiseIndex : 0;
}

unsigned char CUserCharacInfo::getDemensionInoutValue(char index)
{
    if (index > 5 || index < 0)
        return 0;
    return CUR->m_demensionInout[index];
}

int CUserCharacInfo::getCurrCharacBlackCount() const
{
    return m_selected ? CUR->m_blackCount : 0;
}

unsigned char CUserCharacInfo::getCurUseBringUpMaterialTotal() const
{
    return m_selected ? CUR->m_curUseBringUpMaterialTotal : 0;
}

unsigned char CUserCharacInfo::getCurUseBringUpMaterialCount() const
{
    return m_selected ? CUR->m_curUseBringUpMaterialCount : 0;
}

unsigned char CUserCharacInfo::getCurExpandEquipslotSwitch()
{
    return m_selected ? (int)(unsigned char)CUR->m_curExpandEquipslotSwitch : -1;
}


unsigned char CUserCharacInfo::getCurEquipslotFlag()
{
    return m_selected ? (int)(unsigned char)CUR->m_curEquipslotFlag : -1;
}


_Charac_info* CUserCharacInfo::getCurCharacW() { return m_selected; }

unsigned char CUserCharacInfo::getCurCharacVillPrev() const
{
    return m_selected ? CUR->m_villPrev : 0;
}

unsigned char CUserCharacInfo::getCurCharacVill4Save() const
{
    return CUR->m_vill;
}

int CUserCharacInfo::getCurCharacTutorialFlag()
{
    return m_selected ? CUR->m_tutorialFlags : 0;
}

unsigned int CUserCharacInfo::getCurCharacTradeGoldTotalBillion()
{
    return m_selected ? CUR->m_reliablePerson.GetTradeGoldTotalBillion() : 0;
}

unsigned int CUserCharacInfo::getCurCharacTradeGoldTotal()
{
    return m_selected ? CUR->m_reliablePerson.GetTradeGoldTotal() : 0;
}

unsigned int CUserCharacInfo::getCurCharacTradeGoldDaily()
{
    return m_selected ? CUR->m_reliablePerson.GetTradeGoldDaily() : 0;
}

int CUserCharacInfo::getCurCharacTagCharacNo() const
{
    return m_selected ? CUR->m_tagCharacNo : 0;
}

unsigned char CUserCharacInfo::getCurCharacStrikerSkillIndex() const
{
    return m_selected ? CUR->m_strikerSkillIndex : 0;
}

unsigned char CUserCharacInfo::getCurCharacStamina() const
{
    return CUR->m_stamina;
}

char* CUserCharacInfo::getCurCharacSkillW()
{
    enableSaveSkill();
    return (char*)CUR->m_skill;
}

const char* CUserCharacInfo::getCurCharacSkillR() const
{
    return (const char*)CUR->m_skill;
}

int CUserCharacInfo::getCurCharacSchoolPoint()
{
    return m_selected ? CUR->m_reliablePerson.m_schoolPoint : 0;
}

const _Charac_info* CUserCharacInfo::getCurCharacR() const { return m_selected; }

unsigned short CUserCharacInfo::getCurCharacPremiumFatigue() const
{
    return m_selected ? CUR->m_premiumFatigue : 0;
}

unsigned int CUserCharacInfo::getCurCharacPayCoin() const
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->GetPayCoin();
    return 0;
}



int CUserCharacInfo::getCurCharacNo() const
{
    return m_selected ? CUR->m_characNo : 0;
}

int CUserCharacInfo::getCurCharacMoney() const
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->get_money();
    return 0;
}



unsigned short CUserCharacInfo::getCurCharacMemberBonusFatigue()
{
    return CUR->m_memberBonusFatigue;
}

unsigned short CUserCharacInfo::getCurCharacMaxPremiumFatigue() const
{
    return m_selected ? CUR->m_maxPremiumFatigue : 0;
}

unsigned short CUserCharacInfo::getCurCharacMaxFatigue() const
{
    return m_selected ? CUR->m_maxFatigue : 0;
}

long CUserCharacInfo::getCurCharacLastPlayTickPowerWar() const
{
    return CUR->m_lastPlayTickPowerWar;
}

long CUserCharacInfo::getCurCharacLastPlayTick() const
{
    return CUR->m_lastPlayTick;
}

int CUserCharacInfo::getCurCharacLastPlayDungeonIndex()
{
    return m_selected ? CUR->m_lastPlayDungeonIndex : 0;
}

CInventory* CUserCharacInfo::getCurCharacInvenW()
{
    enableSaveInven();
    return (CInventory*)CUR->m_inven;
}

Inven_Item* CUserCharacInfo::getCurCharacInvenRefW()
{
    enableSaveInven();
    return (Inven_Item*)CUR->m_inven;
}

const Inven_Item* CUserCharacInfo::getCurCharacInvenRefR() const
{
    return (const Inven_Item*)CUR->m_inven;
}

const CInventory* CUserCharacInfo::getCurCharacInvenR() const
{
    return (const CInventory*)CUR->m_inven;
}

unsigned int CUserCharacInfo::getCurCharacHelpAbuseComputedRatio() const
{
    return m_selected ? CUR->m_helpAbuseComputedRatio : 0;
}

unsigned char CUserCharacInfo::getCurCharacGuildSecede()
{
    return m_selected ? CUR->m_guildSecede : 0;
}

unsigned char CUserCharacInfo::getCurCharacGrowType() const
{
    return m_selected ? CUR->m_growType.byte : 0;
}

unsigned short CUserCharacInfo::getCurCharacFatigueGrownUpBuff() const
{
    return m_selected ? CUR->m_fatigueGrownUpBuff : 0;
}

unsigned short CUserCharacInfo::getCurCharacFatigue() const
{
    return m_selected ? CUR->m_fatigue : 0;
}

int CUserCharacInfo::getCurCharacExp() const
{
    return m_selected ? CUR->m_exp : 0;
}

unsigned int CUserCharacInfo::getCurCharacEventCoin() const
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->GetEventCoin();
    return 0;
}



unsigned char CUserCharacInfo::getCurCharacEventCharacterGrowtype()
{
    return m_selected ? CUR->m_eventCharacterGrowtype : 0;
}

int CUserCharacInfo::getCurCharacEscaladeTutorialFlag(unsigned int flag)
{
    if (!m_selected)
        return 0;
    if (flag < 0x20)
    {
        if ((CUR->m_tutorialFlags & (1u << (unsigned char)flag)) != 0)
            return 1;
        return 0;
    }
    if (flag < 0x80)
    {
        int index = (int)(flag - 0x20);
        int byteIndex = (int)(((unsigned int)(index >> 0x1f) >> 0x1d) + index) >> 3;
        if (byteIndex >= 0 && byteIndex < 0x10)
        {
            return (int)((signed char)CUR->m_tutorialFlags2[byteIndex]) >>
                       (flag % 8 & 0x1f) & 1;
        }
        return 0;
    }
    return 0;
}

unsigned char* CUserCharacInfo::getCurCharacEscaladeTutorialFlag()
{
    return m_selected ? (unsigned char*)CUR->m_tutorialFlags2 : 0;
}

long CUserCharacInfo::getCurCharacCreateTime() const
{
    return CUR->m_createTime.operator long();
}

unsigned int CUserCharacInfo::getCurCharacCoin() const
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->GetCoin();
    return 0;
}



CCargo* CUserCharacInfo::getCurCharacCargoW()
{
    enableSaveCargo();
    return (CCargo*)CUR->m_cargo;
}

const CCargo* CUserCharacInfo::getCurCharacCargoR() const
{
    return (const CCargo*)CUR->m_cargo;
}

void* CUserCharacInfo::getCurCharacAddInfoW()
{
    enableSaveCharacInfo();
    return (void*)CUR->m_addInfo;
}

void* CUserCharacInfo::getCurCharacAddInfoRefW()
{
    enableSaveCharacInfo();
    return (void*)CUR->m_addInfo;
}

const void* CUserCharacInfo::getCurCharacAddInfoRefR() const
{
    return (const void*)CUR->m_addInfo;
}

const void* CUserCharacInfo::getCurCharacAddInfoR() const
{
    return (const void*)CUR->m_addInfo;
}

char CUserCharacInfo::getCurCharSecondGrowType() const
{
    return m_selected ? (char)CUR->m_growType.second : 0;
}

char CUserCharacInfo::getCurCharFirstGrowType() const
{
    return m_selected ? (char)CUR->m_growType.first : 0;
}

unsigned char CUserCharacInfo::getCurChannelEquipslotSwitch()
{
    return m_selected ? (int)(unsigned char)CUR->m_curChannelEquipslotSwitch : -1;
}


char* CUserCharacInfo::getCreateGuildName()
{
    return m_selected ? (char*)CUR + 0xdc5 : 0;
}

char* CUserCharacInfo::getCreateGuildAddr()
{
    return m_selected ? (char*)CUR + 0xddc : 0;
}

bool CUserCharacInfo::getContractOfCubePremiumInfo(unsigned char& active, unsigned char& count)
{
    if (!m_selected)
        return false;
    *(&active) = CUR->m_cubePremiumContract != 0 ? 1 : 0;
    *(&count) = CUR->m_cubePremiumCount;
    return true;
}


char CUserCharacInfo::getCharacLinkType()
{
    return m_selected ? (char)CUR->m_characLinkType : 0;
}

int CUserCharacInfo::getCharacLinkBonusGold()
{
    return m_selected ? CUR->m_characLinkBonusGold : 0;
}

int CUserCharacInfo::getCharacLinkBonusExp()
{
    return m_selected ? CUR->m_characLinkBonusExp : 0;
}

unsigned char CUserCharacInfo::getBringUpRewardFlag() const
{
    return m_selected ? CUR->m_bringUpRewardFlag : 0;
}

int CUserCharacInfo::getBlueMarbleUserDeadCount() const
{
    return m_selected ? CUR->m_blueMarbleUserDeadCount : 0;
}

unsigned char CUserCharacInfo::getBlueMarbleEnterCount() const
{
    return m_selected ? CUR->m_blueMarbleEnterCount : 0;
}

unsigned int CUserCharacInfo::getBloodBestTimeRecord()
{
    return CUR->m_bloodBestTime;
}

int CUserCharacInfo::getBloodBestRoundRecord()
{
    return CUR->m_bloodBestRound;
}

std::map<int, GameResultType>* CUserCharacInfo::getBattleRecordRefW()
{
    enableSaveBattleRecord();
    return &m_battleRecord;
}

const std::map<int, GameResultType>* CUserCharacInfo::getBattleRecordRefR() const
{
    return &m_battleRecord;
}

void CUserCharacInfo::enableSaveSkill() { m_saveSkill = 1; }

void CUserCharacInfo::enableSaveQuest() { m_saveQuest = 1; }

void CUserCharacInfo::enableSavePvP() { m_savePvP = 1; }

void CUserCharacInfo::enableSaveInven() { m_saveInven = 1; }

void CUserCharacInfo::enableSaveDungeonClear() { m_saveDungeonClear = 1; }

void CUserCharacInfo::enableSaveCharacEtc() { m_saveCharacEtc = 1; }

void CUserCharacInfo::enableSaveCargo() { m_saveCargo = 1; }

void CUserCharacInfo::enableSaveBattleRecord() { m_saveBattleRecord = 1; }

void CUserCharacInfo::disableSavedPremiumCubeInfo()
{
    if (!m_selected)
        return;
    CUR->m_cubePremiumSaved = 0;
}



void CUserCharacInfo::disableSaveSkill() { m_saveSkill = 0; }

void CUserCharacInfo::disableSaveQuest() { m_saveQuest = 0; }

void CUserCharacInfo::disableSavePvP() { m_savePvP = 0; }

void CUserCharacInfo::disableSaveInven() { m_saveInven = 0; }

void CUserCharacInfo::disableSaveDungeonClear() { m_saveDungeonClear = 0; }

void CUserCharacInfo::disableSaveCharacStat() { m_saveCharacStat = 0; }

void CUserCharacInfo::disableSaveCharacManageInfo() { m_saveCharacManageInfo = 0; }

void CUserCharacInfo::disableSaveCharacInfo() { m_saveCharacInfo = 0; }

void CUserCharacInfo::disableSaveCharacEtc() { m_saveCharacEtc = 0; }

void CUserCharacInfo::disableSaveCargo() { m_saveCargo = 0; }

void CUserCharacInfo::disableSaveBattleRecord() { m_saveBattleRecord = 0; }

void CUserCharacInfo::decCurCharacLevel()
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    --CUR->m_level;
    }
}


void CUserCharacInfo::decCurCharacFatigueGrownUpBuff(unsigned int value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    if ((unsigned int)CUR->m_fatigueGrownUpBuff > value)
        CUR->m_fatigueGrownUpBuff = (unsigned short)(CUR->m_fatigueGrownUpBuff - value);
    else
        CUR->m_fatigueGrownUpBuff = 0;
    }
}


void CUserCharacInfo::checkBonusPoint()
{
    if (!m_selected)
        return;
    resetBonusPoint();
    Inven_Item* invenItem = 0;
    for (int slot = 10; slot < 0x16; ++slot)
    {
        invenItem = ((CInventory*)(Inven_Item*)getCurCharacInvenR())->GetInvenRef(0, slot);
        if (invenItem == 0 || invenItem->m_addInfo == 0)
            continue;
        CItem* found = G_CDataManager()->find_item(invenItem->m_addInfo);
        if (found == 0)
            continue;
        if (*(int*)((char*)((CEquipItem*)found)->getWpBonusPoint() + 0) == 0)
        {
            CUR->m_wpBonus.m_base += *(int*)((char*)((CEquipItem*)found)->getWpBonusPoint() + 4);
        }
        else if (*(int*)((char*)((CEquipItem*)found)->getWpBonusPoint() + 0) == 1)
        {
            CUR->m_wpBonus.m_add += *(int*)((char*)((CEquipItem*)found)->getWpBonusPoint() + 4);
        }
        if (*(int*)((char*)((CEquipItem*)found)->getExpBonusPoint() + 0) == 0)
        {
            CUR->m_expBonus.m_base += *(int*)((char*)((CEquipItem*)found)->getExpBonusPoint() + 4);
        }
        else if (*(int*)((char*)((CEquipItem*)found)->getExpBonusPoint() + 0) == 1)
        {
            CUR->m_expBonus.m_add += *(int*)((char*)((CEquipItem*)found)->getExpBonusPoint() + 4);
        }
    }
}



void CUserCharacInfo::resetBonusPoint()
{
    if (m_selected)
    {
    memset((char*)m_selected + 0x129d, 0, 8);
    memset((char*)m_selected + 0x12a5, 0, 8);
    }
}


void CUserCharacInfo::getWpBonusPoint(WpBonusPointResult& out)
{
    if (m_selected)
        out = CUR->m_wpBonus;
}




void CUserCharacInfo::getExpBonusPoint(ExpBonusPointResult& out)
{
    if (m_selected)
        out = CUR->m_expBonus;
}




int CUserCharacInfo::calc_disp_help_abuse_ratio(int param)
{
    int levelUpExp = get_level_up_exp(get_charac_level());
    float helpRate =
        (float)((double)getCurCharacDungeonHelpAbuseExp() / (double)levelUpExp);
    int ratio = (int)((float)getCurCharacDungeonHelpAbuseRatio() + 1000000.0f * helpRate);
    int result = 0;
    if (get_charac_level() + param > 0)
        result = ratio / (get_charac_level() + param);
    return std::min(1000000, result);
}

int CUserCharacInfo::get_level_up_exp(int level)
{
    return G_CDataManager()->get_level_exp(level + 1) - G_CDataManager()->get_level_exp(level);
}

unsigned int CUserCharacInfo::getCurCharacDungeonHelpAbuseRatio() const
{
    return m_selected ? CUR->m_dungeonHelpAbuseRatio : 0;
}

unsigned int CUserCharacInfo::getCurCharacDungeonHelpAbuseExp() const
{
    return m_selected ? CUR->m_dungeonHelpAbuseExp : 0;
}

void CUserCharacInfo::calcHelpAbuseRatioBeforeLevelUp(int param)
{
    char unused[0x20];
    (void)unused;
    (void)param;
}


void CUserCharacInfo::calcHelpAbuseRatio(unsigned int param, bool flag)
{
    char unused[0x24];
    unused[0] = (char)flag;
    (void)param;
}




void CUserCharacInfo::calcCurCharacSchoolPoint(int fatigue)
{
    static const int point[24] = {
        0, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 5, 5, 5, 5
    };
    static const int pointSize = sizeof(point) / sizeof(point[0]);
    unsigned short used = getCurCharacUsedFatigue();
    if (fatigue / 10 == used / 10)
        return;
    unsigned int index = used / 10;
    if (index > 0x17)
        index = 0x17;
    CUR->m_reliablePerson.m_schoolPoint += point[index];
}


unsigned short CUserCharacInfo::getCurCharacUsedFatigue()
{
    return m_selected ? CUR->m_usedFatigue : 0;
}

void CUserCharacInfo::add_member_pay_tex_money_to_upper(int money)
{
    if (m_selected)
    {
    CUR->m_memberPayTexMoney += money;
    }
}


void CUserCharacInfo::add_member_pay_tex_fatigue_to_upper(int fatigue)
{
    if (m_selected)
    {
    CUR->m_memberPayTexFatigue += fatigue;
    }
}


void CUserCharacInfo::add_guild_today_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildTodayExp = CUR->m_guildTodayExp + exp;
}


void CUserCharacInfo::add_guild_temp_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildTempExp = (unsigned short)(CUR->m_guildTempExp + (short)exp);
}


void CUserCharacInfo::add_guild_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildExp = CUR->m_guildExp + exp;
}


void CUserCharacInfo::addCurcharacStamina(unsigned char value)
{
    enableSaveCharacStat();
    CUR->m_stamina = (unsigned char)(CUR->m_stamina + value);
}



void CUserCharacInfo::addCurCharacExp(int exp)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_exp += exp;
    }
}


void CUserCharacInfo::WarAreaKill(int index, int point, float kill)
{
    if (!m_selected)
    {
        cMyTrace trace("void CUserCharacInfo::WarAreaKill(int, int, float)", 0x284, 5);
        trace("WARAREA CUserCharacInfo::WarAreaKill() : m_selected is NULL");
        return;
    }
    ChangeFinishPoint(point);
    CUR->m_warAreaKill[index] += kill;
    ((CUser*)this)->m_historyLog.FPAdd(
        point, CUR->m_finishPoint, CUR->m_finishPointTotal,
        index == 4 ? (eFPAddReason)1 : eFPAddReason_0);
}

void CUserCharacInfo::VillageAttack_NotDBUpdate()
{
    if (m_selected)
    {
    CUR->m_villageAttackDBUpdate = 0;
    }
}


void CUserCharacInfo::VillageAttack_DBUpdate()
{
    if (m_selected)
    {
    CUR->m_villageAttackDBUpdate = 1;
    }
}


int CUserCharacInfo::UseFinishPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace("unsigned int CUserCharacInfo::UseFinishPoint(int)", 0x220, 5);
        trace("WARAREA CUserCharacInfo::UseFinishPoint() : m_selected is NULL");
        return 0;
    }
    ChangeFinishPoint(-point);
    ((CUser*)this)->m_historyLog.FPSub(
        point, CUR->m_finishPoint, CUR->m_finishPointTotal, eFPSubReason_0);
    return CUR->m_finishPoint;
}

void CUserCharacInfo::SubCurCharacChaosPoint(int point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosPoint -= point;
    if (CUR->m_chaosPoint < 0)
        CUR->m_chaosPoint = 0;
    }
}


void CUserCharacInfo::SetVisible()
{
    CUR->m_visibleBits.visible = 1;
}

void CUserCharacInfo::SetSaveDemensionInoutValue(bool flag)
{
    CUR->m_saveDemensionInout = flag;
}




void CUserCharacInfo::SetSaveBloodInoutValue(bool flag)
{
    CUR->m_saveBloodInout = flag;
}




void CUserCharacInfo::SetReliablePerson(bool flag)
{
    m_reliablePerson = flag;
}

void CUserCharacInfo::SetProperDungeonUser()
{
    if (m_selected)
    {
    CUR->m_properDungeonUser = 1;
    }
}


void CUserCharacInfo::SetPowerWarProcessInfoLastUpdateTime(long t)
{
    if (m_selected)
    {
    CUR->m_powerWarProcessLastUpdateTime = t;
    }
}


void CUserCharacInfo::SetPowerWarLastPlayTime(long t)
{
    if (m_selected)
    {
    CUR->m_powerWarLastPlayTime = t;
    }
}


void CUserCharacInfo::SetOverTradeGoldCount(unsigned int count) { m_overTradeGoldCount = count; }

void CUserCharacInfo::SetOpencoin(char value)
{
    if (m_selected)
    {
        if ((char)CUR->m_opencoin != value)
        {
            enableSaveCharacStat();
            CUR->m_opencoin = (unsigned char)value;
        }
    }
}




void CUserCharacInfo::SetOneDayLetheFlag2ND(unsigned char flag)
{
    if (m_selected)
    {
    CUR->m_oneDayLetheFlag2ND = flag;
    }
}


void CUserCharacInfo::SetOneDayLetheFlag(unsigned char flag)
{
    if (m_selected)
    {
    CUR->m_oneDayLetheFlag = flag;
    }
}


void CUserCharacInfo::SetLoginTick(long tick)
{
    CUR->m_loginTick = tick;
}

void CUserCharacInfo::SetIsEndLetheAtLogin()
{
    if (m_selected)
    {
    CUR->m_isEndLetheAtLogin = 1;
    }
}


void CUserCharacInfo::SetInvisible()
{
    CUR->m_visibleBits.visible = 0;
}

void CUserCharacInfo::SetFirstTimeLogin(bool flag)
{
    CUR->m_firstTimeLogin = flag;
}

void CUserCharacInfo::SetCurRevengeDungeonCount(unsigned char count)
{
    if (m_selected)
    {
    CUR->m_revengeDungeonCount = count;
    }
}


void CUserCharacInfo::SetCurExpEquipSlotStat(char stat)
{
    if (m_selected)
    {
    CUR->m_curExEquipSlotStat = stat;
    }
}


void CUserCharacInfo::SetCurCharacSuperState(bool flag)
{
    if (m_selected)
    {
    CUR->m_superState = flag;
    }
}


void CUserCharacInfo::SetCurCharacSkillTreeIndex(char index)
{
    if (m_selected)
    {
    enableSaveCharacInfo();
    CUR->m_skillTreeIndex = index;
    }
}


void CUserCharacInfo::SetCurCharacRevengeData(unsigned char count, unsigned char ticket)
{
    if (m_selected)
    {
    CUR->m_revengeCount = count;
    CUR->m_revengeDungeonCount = ticket;
    }
}


void CUserCharacInfo::SetCurCharacOption(CharacterOption& option)
{
    if (m_selected)
        CUR->m_characterOption = option;
}




void CUserCharacInfo::SetCurCharacOnlinePreliminaryTeam(
    online_preliminary::COnlinePreliminaryTeam* team)
{
    if (m_selected)
    {
    CUR->m_onlinePreliminaryTeam = team;
    }
}


void CUserCharacInfo::SetCurCharacMaxEquipLevel(int level)
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    CUR->m_maxEquipLevel = level;
    }
}


void CUserCharacInfo::enableSaveCharacManageInfo() { m_saveCharacManageInfo = 1; }

void CUserCharacInfo::SetCurCharacLuckPointUpdateTimeCount(int count)
{
    if (m_selected)
    {
    CUR->m_luckPointUpdateTimeCount = count;
    }
}


void CUserCharacInfo::SetCurCharacLeaveTimeout(bool flag)
{
    if (m_selected)
    {
    CUR->m_leaveTimeout = flag;
    }
}


void CUserCharacInfo::SetCurCharacGhost(bool flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_ghost = flag;
    }
}


void CUserCharacInfo::SetCurCharacExpertJobType(int type)
{
    if (m_selected)
    {
    enableSaveCharacInfo();
    CUR->m_expertJobType = type;
    }
}


void CUserCharacInfo::enableSaveCharacInfo() { m_saveCharacInfo = 1; }

void CUserCharacInfo::SetCurCharacExpertJobExp(int exp)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_expertJobExp = exp;
    }
}


void CUserCharacInfo::SetCurCharacExpertJob(expert_job::CExpertJob* job)
{
    if (m_selected)
    {
    CUR->m_expertJob = job;
    }
}


void CUserCharacInfo::SetCurCharacEmotion(short emotion)
{
    enableSaveCharacStat();
    CUR->m_curCharacEmotion = (unsigned short)emotion;
}

void CUserCharacInfo::SetCurCharacChaosPoint(int point)
{
    if (m_selected)
    {
    CUR->m_chaosPoint = point;
    }
}


void CUserCharacInfo::SetCurCharacChaosKillTime(int t)
{
    if (m_selected)
    {
    CUR->m_chaosKillTime = t;
    }
}


void CUserCharacInfo::SetCurCharacAssaultedUserId(unsigned short userId)
{
    if (m_selected)
    {
    CUR->m_assaultedUserId = userId;
    }
}


void CUserCharacInfo::SetCurCharacAssaultMoney(int money)
{
    if (m_selected)
    {
    CUR->m_assaultMoney = money;
    }
}


void CUserCharacInfo::SetCurCharacAssaultFatigueCount(int count)
{
    if (m_selected)
    {
    CUR->m_assaultFatigueCount = count;
    }
}


void CUserCharacInfo::SetCharacStartPlayTick(long tick) { m_characStartPlayTick = tick; }

void CUserCharacInfo::SetAssaultPlace(int place)
{
    if (m_selected)
    {
    CUR->m_assaultPlace = place;
    }
}


void CUserCharacInfo::ResetRevengeDungeonClear()
{
    if (m_selected)
    {
    CUR->m_revengeDungeonCount = 0;
    CUR->m_revengeCount = 0;
    }
}


void CUserCharacInfo::ResetPowerWarAssaultRecord()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_powerWarAssaultCount = 0;
    CUR->m_powerWarAssaultVictoryCount = 0;
    }
}


void CUserCharacInfo::ResetCurCharacEventItemCount()
{
    if (m_selected)
    {
    CUR->m_curCharacEventItemCount = 0;
    }
}


void CUserCharacInfo::ResetCurCharacEventAddItemCount()
{
    if (m_selected)
    {
    CUR->m_curCharacEventAddItemCount = 0;
    }
}


void CUserCharacInfo::ResetCurCharacDungeonPlayCount()
{
    enableSaveCharacStat();
    SetCurCharacDungeonPlayCount(0);
}

void CUserCharacInfo::LiveCurCharac()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_ghost = 0;
    CUR->m_chaosDieTime = 0;
    CUR->m_chaosStateTime = 0;
    if (!GlobalData::s_power_manager->IsPowerWarEventOn() ||
        getCurCharacVill() != 7)
    {
        CUR->m_stamina = CUR->m_backupStamina;
    }
    }
}


unsigned char CUserCharacInfo::IsVillageAttackDBUpdate() const
{
    return m_selected ? CUR->m_villageAttackDBUpdate : 0;
}

unsigned char CUserCharacInfo::IsSaveDemensionInoutValue()
{
    return CUR->m_saveDemensionInout;
}

char CUserCharacInfo::IsSaveCharacStat() { return m_saveCharacStat; }

char CUserCharacInfo::IsSaveCharacManageInfo() { return m_saveCharacManageInfo; }

unsigned char CUserCharacInfo::IsSaveBloodInoutValue()
{
    return CUR->m_saveBloodInout;
}

unsigned char CUserCharacInfo::IsProperDungeonUser()
{
    return m_selected ? CUR->m_properDungeonUser : 0;
}

unsigned char CUserCharacInfo::IsFirstTimeLogin()
{
    return CUR->m_firstTimeLogin;
}

unsigned char CUserCharacInfo::IsExpItemAffected()
{
    return m_selected ? CUR->m_isExpItemAffected : 0;
}


unsigned char CUserCharacInfo::IsEndLetheAtLogin()
{
    return m_selected ? CUR->m_isEndLetheAtLogin : 0;
}


int CUserCharacInfo::IsCurCharacVisible()
{
    if (m_selected && CUR->m_visibleBits.visible)
        return 1;
    return 0;
}


unsigned char CUserCharacInfo::IsCurCharacLeaveTimeout()
{
    return m_selected ? CUR->m_leaveTimeout : 0;
}

bool CUserCharacInfo::IsBusyDisjointStore()
{
    if (GetCurCharacExpertJob() != 0)
    {
        if (GetCurCharacExpertJob()->GetType() == 3)
            return true;
    }
    return false;
}


bool CUserCharacInfo::IsAvailableCurCharacTradeGoldDaily(unsigned int gold)
{
    if (!m_selected)
        return false;
    unsigned int limit = get_charac_level() * get_charac_level() * 10000;
    if (limit < CUR->m_reliablePerson.GetTradeGoldDaily() + gold)
    {
        cMyTrace trace(
            "bool CUserCharacInfo::IsAvailableCurCharacTradeGoldDaily(unsigned int)",
            0x3f1, 0);
        trace("TRADE_RESTIRCTION charac_name(%s), charac_level(%d), limit_gold(%u), "
              "add_gold(%u), use_gold(%u)",
              getCurCharacName(), get_charac_level(), limit, gold,
              CUR->m_reliablePerson.GetTradeGoldDaily());
        return false;
    }
    return true;
}

const char* CUserCharacInfo::getCurCharacName() const
{
    return m_selected ? (const char*)((char*)m_selected + 4) : "";
}


void CUserCharacInfo::InitFinishPoint()
{
    if (m_selected)
    {
    CUR->m_finishPoint = 0;
    CUR->m_finishPointTotal = 0;
    memset((char*)CUR + 0xe0e, 0, 0x14);
    ((CUser*)this)->m_historyLog.FPSub(
        0, CUR->m_finishPoint, CUR->m_finishPointTotal, (eFPSubReason)1);
    }
}


void CUserCharacInfo::IncrementCurCharacTradeGoldDaily(unsigned int gold)
{
    enableSaveCharacStat();
    CUR->m_reliablePerson.IncrementTradeGold(gold);
}

void CUserCharacInfo::IncreasePowerWarPoint(short point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_powerWarPoint = (unsigned short)(CUR->m_powerWarPoint + point);
    if ((short)CUR->m_powerWarPoint > 9999)
        CUR->m_powerWarPoint = 9999;
    }
}


void CUserCharacInfo::IncProperDungeonClearCount()
{
    if (m_selected)
    {
    ++CUR->m_properDungeonClearCount;
    }
}


void CUserCharacInfo::IncPowerWarAssaultVictoryCount()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    ++CUR->m_powerWarAssaultVictoryCount;
    }
}


void CUserCharacInfo::IncPowerWarAssaultCount()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    ++CUR->m_powerWarAssaultCount;
    }
}


void CUserCharacInfo::IncFatigueBatteryCharging(short value)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        ServerParameterScript* serverParam = (ServerParameterScript*)((char*)G_CDataManager() + 0x68);
        unsigned short old = CUR->m_fatigueBattery;
        CUR->m_fatigueBattery = (unsigned short)(CUR->m_fatigueBattery + value);
        if ((short)CUR->m_fatigueBattery < (short)old ||
            (int)(short)CUR->m_fatigueBattery > *(unsigned short*)((char*)serverParam + 0x5c0))
        {
            CUR->m_fatigueBattery = *(unsigned short*)((char*)serverParam + 0x5c0);
        }
    }
}




void CUserCharacInfo::IncCurConditionEventStep()
{
    if (m_selected)
    {
        ++CUR->m_curConditionEventStep;
        if ((short)CUR->m_curConditionEventStep > 0xff)
            CUR->m_curConditionEventStep = 0xff;
    }
}




void CUserCharacInfo::IncCurConditionEventRewardStep()
{
    if (m_selected)
    {
        ++CUR->m_curConditionEventRewardStep;
        if ((short)CUR->m_curConditionEventRewardStep > 0xff)
            CUR->m_curConditionEventRewardStep = 0xff;
    }
}




void CUserCharacInfo::IncCurCharacLuckPointUpdateTimeCount()
{
    if (m_selected)
        CUR->m_luckPointUpdateTimeCount += 1;
}




void CUserCharacInfo::IncCurCharacLuckPoint(int point)
{
    enableSaveCharacStat();
    if (GetCurCharacLuckPoint() == 100000)
        return;
    SetCurCharacLuckPoint(GetCurCharacLuckPoint() + point);
}

void CUserCharacInfo::IncCurCharacEventAddItemCount(unsigned short count)
{
    if (m_selected)
    {
    CUR->m_curCharacEventAddItemCount =
        (unsigned short)(CUR->m_curCharacEventAddItemCount + count);
    }
}


void CUserCharacInfo::IncCurCharacDungeonPlayCount()
{
    enableSaveCharacStat();
    SetCurCharacDungeonPlayCount(GetCurCharacDungeonPlayCount() + 1);
}

void CUserCharacInfo::SetCurCharacDungeonPlayCount(int count)
{
    if (m_selected)
    {
    CUR->m_dungeonPlayCount = count;
    }
}


void CUserCharacInfo::IncCurCharacChaosModeCount()
{
    enableSaveCharacStat();
    SetCurCharacChaosModeCount(GetCurCharacChaosModeCount() + 1);
}

void CUserCharacInfo::SetCurCharacChaosModeCount(int count)
{
    if (m_selected)
    {
    CUR->m_chaosModeCount = count;
    }
}


void CUserCharacInfo::IncCurCharacChaosKillCount()
{
    enableSaveCharacStat();
    SetCurCharacChaosKillCount(GetCurCharacChaosKillCount() + 1);
}

void CUserCharacInfo::SetCurCharacChaosKillCount(int count)
{
    if (m_selected)
    {
    CUR->m_chaosKillCount = count;
    }
}


void CUserCharacInfo::IncCurCharacChaosDieCount()
{
    enableSaveCharacStat();
    SetCurCharacChaosDieCount(GetCurCharacChaosDieCount() + 1);
}

void CUserCharacInfo::SetCurCharacChaosDieCount(int count)
{
    if (m_selected)
    {
    CUR->m_chaosDieCount = count;
    }
}


void CUserCharacInfo::IncCurCharacAssaultFatigueCount()
{
    if (m_selected)
    {
    ++CUR->m_assaultFatigueCount;
    }
}


void CUserCharacInfo::IncCurCharacAssaultCount()
{
    enableSaveCharacStat();
    SetAssaultCount(GetAssaultCount() + 1);
}

void CUserCharacInfo::SetAssaultCount(int count)
{
    if (m_selected)
    {
    CUR->m_assaultCount = count;
    }
}


void CUserCharacInfo::GmSetCurCharacChaosPoint(int point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosPoint = point;
    if (CUR->m_chaosPoint < 0)
        CUR->m_chaosPoint = 0;
    if (CUR->m_chaosPoint > 100)
        CUR->m_chaosPoint = 100;
    }
}


int CUserCharacInfo::GetWinPoint() { return m_pvpResult.m_winPoint; }

float CUserCharacInfo::GetWarAreaKillCountTotal()
{
    if (!m_selected)
    {
        cMyTrace trace("float CUserCharacInfo::GetWarAreaKillCountTotal()", 0x22f, 5);
        trace("WARAREA CUserCharacInfo::GetWarAreaKillCountTotal() : m_selected is NULL");
        return 0.0f;
    }
    float total = 0.0f;
    for (int i = 0; i < 4; ++i)
        total += CUR->m_warAreaKill[i];
    return total;
}


float CUserCharacInfo::GetWarAreaKillCount(int index)
{
    if (!m_selected)
    {
        cMyTrace trace("float CUserCharacInfo::GetWarAreaKillCount(int)", 0x260, 5);
        trace("WARAREA CUserCharacInfo::GetWarAreaKillCount() : m_selected is NULL");
        return 0.0f;
    }
    return CUR->m_warAreaKill[index];
}

int CUserCharacInfo::GetUserStateByAssault()
{
    if (m_selected != 0)
    {
        if (CUR->m_superState != 0)
            return 6;
        if (CUR->m_ghost != 0)
            return 4;
        if (CUR->m_assaultPlace != 0)
            return 5;
    }
    return 0;
}


unsigned short CUserCharacInfo::GetUserPowerWarPoint()
{
    return m_selected ? CUR->m_powerWarPoint : 0;
}

char CUserCharacInfo::GetReliablePerson() { return m_reliablePerson; }

unsigned short CUserCharacInfo::GetProperDungeonClearCount() const
{
    return m_selected ? CUR->m_properDungeonClearCount : 0;
}

long CUserCharacInfo::GetPowerWarProcessInfoLastUpdateTime()
{
    return m_selected ? CUR->m_powerWarProcessLastUpdateTime : 0;
}

long CUserCharacInfo::GetPowerWarLastPlayTime()
{
    return m_selected ? CUR->m_powerWarLastPlayTime : 0;
}

unsigned short CUserCharacInfo::GetPowerWarHP()
{
    return m_selected ? CUR->m_powerWarHP : 0;
}

void CUserCharacInfo::SetIsInitSkillFlag(bool flag)
{
    if (m_selected)
    {
        CUR->m_isInitSkillFlag = flag ? 1 : 0;
    }
}

void CUserCharacInfo::SetIsInitSkillFlag2ND(bool flag)
{
    if (m_selected)
    {
        CUR->m_isInitSkillFlag2ND = flag ? 1 : 0;
    }
}

void CUserCharacInfo::IncVillageAttackCount()
{
    if (m_selected)
    {
        ++CUR->m_revengeCount;
        CUR->m_villageAttackDBUpdate = 1;
    }
}

int CUserCharacInfo::GetPowerWarAssaultVictoryCount()
{
    return m_selected ? CUR->m_powerWarAssaultVictoryCount : 0;
}

int CUserCharacInfo::GetPowerWarAssaultCount()
{
    return m_selected ? CUR->m_powerWarAssaultCount : 0;
}

int CUserCharacInfo::GetOverTradeGoldCount() { return m_overTradeGoldCount; }

unsigned char CUserCharacInfo::GetOpencoin()
{
    return CUR->m_opencoin;
}

unsigned char CUserCharacInfo::GetOneDayLetheFlag2ND() const
{
    return m_selected ? CUR->m_oneDayLetheFlag2ND : 0;
}

unsigned char CUserCharacInfo::GetOneDayLetheFlag() const
{
    return m_selected ? CUR->m_oneDayLetheFlag : 0;
}

long CUserCharacInfo::GetLoginTick()
{
    if (!m_selected)
        return 0;
    return CUR->m_loginTick;
}


int CUserCharacInfo::GetFinishPointTotal()
{
    if (!m_selected)
    {
        cMyTrace trace("int CUserCharacInfo::GetFinishPointTotal()", 0x26a, 5);
        trace("WARAREA CUserCharacInfo::GetFinishPointTotal() : m_selected is NULL");
        return 0;
    }
    return CUR->m_finishPointTotal;
}



int CUserCharacInfo::GetFinishPoint()
{
    if (m_selected)
        return CUR->m_finishPoint;
    cMyTrace trace("int CUserCharacInfo::GetFinishPoint()", 0x244, 5);
    trace("WARAREA CUserCharacInfo::GetFinishPoint() : m_selected is NULL");
    return 0;
}


unsigned short CUserCharacInfo::GetFatigueBatteryCharging()
{
    return m_selected ? CUR->m_fatigueBattery : 0;
}

unsigned short CUserCharacInfo::GetFatigue() const
{
    return m_selected ? CUR->m_fatigue : 0;
}

float CUserCharacInfo::GetExpAffectRate()
{
    return m_selected ? CUR->m_expAffectRate : 0.0f;
}


unsigned char CUserCharacInfo::GetCurVillageAttackCount() const
{
    return m_selected ? CUR->m_revengeCount : 0;
}

unsigned char CUserCharacInfo::GetCurRevengeDungeonCount() const
{
    return m_selected ? CUR->m_revengeDungeonCount : 0;
}

unsigned char CUserCharacInfo::GetCurExEquipSlotStat()
{
    return m_selected ? (unsigned char)CUR->m_curExEquipSlotStat : 0;
}

unsigned short CUserCharacInfo::GetCurConditionEventStep() const
{
    return m_selected ? CUR->m_curConditionEventStep : 0;
}

unsigned short CUserCharacInfo::GetCurConditionEventRewardStep() const
{
    return m_selected ? CUR->m_curConditionEventRewardStep : 0;
}

unsigned char CUserCharacInfo::GetCurCharacSuperState()
{
    return m_selected ? CUR->m_superState : 0;
}

int CUserCharacInfo::GetCurCharacSkillTreeIndex() const
{
    return m_selected ? (signed char)CUR->m_skillTreeIndex : 0;
}

const char* CUserCharacInfo::GetCurCharacOnlinePreliminaryTeamName() const
{
    if (m_selected)
    {
        if (CUR->m_onlinePreliminaryTeam != 0)
            return CUR->m_onlinePreliminaryTeam->GetTeamName();
    }
    return "";
}



int CUserCharacInfo::GetCurCharacOnlinePreliminaryTeamId() const
{
    if (m_selected)
    {
        if (CUR->m_onlinePreliminaryTeam != 0)
            return CUR->m_onlinePreliminaryTeam->GetTeamId();
    }
    return 0;
}



online_preliminary::COnlinePreliminaryTeam*
CUserCharacInfo::GetCurCharacOnlinePreliminaryTeam()
{
    return m_selected ? CUR->m_onlinePreliminaryTeam : 0;
}


int CUserCharacInfo::GetCurCharacMaxEquipLevel() const
{
    return m_selected ? CUR->m_maxEquipLevel : 0;
}

int CUserCharacInfo::GetCurCharacLuckPointUpdateTimeCount()
{
    return m_selected ? CUR->m_luckPointUpdateTimeCount : 0;
}

int CUserCharacInfo::GetCurCharacExpertJobType() const
{
    return m_selected ? CUR->m_expertJobType : 0;
}

int CUserCharacInfo::GetCurCharacExpertJobExp() const
{
    return m_selected ? CUR->m_expertJobExp : 0;
}

expert_job::CExpertJob* CUserCharacInfo::GetCurCharacExpertJob() const
{
    return m_selected ? CUR->m_expertJob : 0;
}

unsigned short CUserCharacInfo::GetCurCharacEventItemCount()
{
    return m_selected ? CUR->m_curCharacEventItemCount : 0;
}

unsigned short CUserCharacInfo::GetCurCharacEventAddItemCount()
{
    return m_selected ? CUR->m_curCharacEventAddItemCount : 0;
}

unsigned short CUserCharacInfo::GetCurCharacEmotion()
{
    return m_selected ? CUR->m_curCharacEmotion : 0;
}

int CUserCharacInfo::GetCurCharacDungeonPlayCount() const
{
    return m_selected ? CUR->m_dungeonPlayCount : 0;
}

int CUserCharacInfo::GetCurCharacChaosStateTime()
{
    return m_selected ? CUR->m_chaosStateTime : 0;
}

int CUserCharacInfo::GetCurCharacChaosResponTime()
{
    return m_selected ? CUR->m_chaosResponTime : 0;
}

int CUserCharacInfo::GetCurCharacChaosModeCount()
{
    return m_selected ? CUR->m_chaosModeCount : 0;
}

int CUserCharacInfo::GetCurCharacChaosKillTime()
{
    return m_selected ? CUR->m_chaosKillTime : 0;
}

int CUserCharacInfo::GetCurCharacChaosKillCount()
{
    return m_selected ? CUR->m_chaosKillCount : 0;
}

int CUserCharacInfo::GetCurCharacChaosExp()
{
    return m_selected ? CUR->m_chaosExp : 0;
}

int CUserCharacInfo::GetCurCharacChaosDieTime()
{
    return m_selected ? CUR->m_chaosDieTime : 0;
}

int CUserCharacInfo::GetCurCharacChaosDieCount()
{
    return m_selected ? CUR->m_chaosDieCount : 0;
}

const char* CUserCharacInfo::GetCurCharacBackupSkill2ND() const
{
    return m_selected ? (const char*)CUR + 0x1086 : 0;
}

const char* CUserCharacInfo::GetCurCharacBackupSkill() const
{
    return m_selected ? (const char*)CUR + 0xeed : 0;
}

unsigned short CUserCharacInfo::GetCurCharacAssaultedUserId()
{
    return m_selected ? CUR->m_assaultedUserId : 0;
}

int CUserCharacInfo::GetCurCharacAssaultMoney()
{
    return m_selected ? CUR->m_assaultMoney : 0;
}

int CUserCharacInfo::GetCurCharacAssaultFatigueCount()
{
    return m_selected ? CUR->m_assaultFatigueCount : 0;
}

void CUserCharacInfo::GetContinuousItemInfo(SIG_LOAD_ETC* p) const
{
    *(int*)((char*)p + 0x2308) = *(int*)((char*)p + 4);
    *(int*)((char*)p + 0x24a0) = *(int*)((char*)p + 4);
    const std::vector<ContinuousItemInfo>* cool = GetCurCharacCoolTimeItemListR();
    *(unsigned char*)((char*)p + 0x230c) = (unsigned char)cool->size();
    int index = 0;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        *(int*)((char*)p + 0x10 + (index + 0x460) * 8) = (*cool)[i].m_itemIdx;
        *(int*)((char*)p + 0x14 + (index + 0x460) * 8) = (*cool)[i].m_expireTime;
        ++index;
    }
    const std::vector<ContinuousItemInfo>* effect = GetCurCharacEffectItemListR();
    *(unsigned char*)((char*)p + 0x24a4) = (unsigned char)effect->size();
    index = 0;
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        *(int*)((char*)p + 8 + (index + 0x494) * 8) = (*effect)[i].m_itemIdx;
        *(int*)((char*)p + 0xc + (index + 0x494) * 8) = (*effect)[i].m_expireTime;
        ++index;
    }
}

const std::vector<ContinuousItemInfo>* CUserCharacInfo::GetCurCharacEffectItemListR() const
{
    return (const std::vector<ContinuousItemInfo>*)((const char*)m_selected + 0xea6);
}

const std::vector<ContinuousItemInfo>* CUserCharacInfo::GetCurCharacCoolTimeItemListR() const
{
    return (const std::vector<ContinuousItemInfo>*)((const char*)m_selected + 0xe9a);
}

int CUserCharacInfo::GetCharacPlayTick()
{
    return GlobalData::s_systemTime_.getCurSec() - GetCharacStartPlayTick();
}

long CUserCharacInfo::GetCharacStartPlayTick() { return m_characStartPlayTick; }

int CUserCharacInfo::GetAssaultPlace()
{
    return m_selected ? CUR->m_assaultPlace : 0;
}

int CUserCharacInfo::GetAssaultCount()
{
    return m_selected ? CUR->m_assaultCount : 0;
}

void CUserCharacInfo::GainCurCharacChaosExp(int exp)
{
    if (!m_selected || CUR->m_chaosPoint <= 0)
        return;
    CUR->m_chaosExp += exp;
    int percent = G_CDataManager()->get_level_exp_percent(get_charac_level(), 1);
    if (CUR->m_chaosExp > percent && percent > 0)
    {
        enableSaveCharacStat();
        CUR->m_chaosPoint -= CUR->m_chaosExp / percent;
        CUR->m_chaosExp %= percent;
        if (CUR->m_chaosPoint < 0)
        {
            CUR->m_chaosPoint = 0;
            CUR->m_chaosExp = 0;
        }
    }
}

bool CUserCharacInfo::FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot,
                                   CUser* user) const
{
    if (user != 0 && m_selected != 0)
    {
        if (((const CCargo*)((const char*)m_selected + 0xdaa))->FindItemLock(b, space, slot))
            return true;
        if (((const CInventory*)((const char*)m_selected + 0xda))->FindItemLock(b, space, slot))
            return true;
        CExpandEquipslot* expand = user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        if (expand->FindItemLock(b, space, slot))
            return true;
        if (((CTitleBook*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14))
                ->findItemLock(b, space, slot))
            return true;
    }
    return false;
}

bool CUserCharacInfo::EnablePeriodExpItem()
{
    std::vector<ContinuousItemInfo>* v =
        (std::vector<ContinuousItemInfo>*)((char*)m_selected + 0xea6);
    std::vector<ContinuousItemInfo>::iterator it = v->begin();
    unsigned int now = GlobalData::s_systemTime_.getCurSec();
    for (; it != v->end();)
    {
        CItem* item = G_CDataManager()->find_item(it->m_itemIdx);
        if (item == 0)
            return 0;
        if (item->is_stackable())
        {
            CStackableItem* stack = (CStackableItem*)item;
            if (stack->IsExpAffect())
            {
                unsigned int expire = it->m_expireTime;
                if (now < expire)
                    return 1;
                it = v->erase(it);
                SetExpAffectItemEffect(false, 0.0f);
                return 0;
            }
        }
        ++it;
    }
    return 0;
}



void CUserCharacInfo::SetExpAffectItemEffect(bool flag, float rate)
{
    if (m_selected)
    {
    CUR->m_isExpItemAffected = flag;
    CUR->m_expAffectRate = rate;
    }
}


bool CUserCharacInfo::EnablePeriodExpDouble()
{
    std::vector<ContinuousItemInfo>* v =
        (std::vector<ContinuousItemInfo>*)((char*)m_selected + 0xea6);
    std::vector<ContinuousItemInfo>::iterator it = v->begin();
    unsigned int now = GlobalData::s_systemTime_.getCurSec();
    for (; it != v->end();)
    {
        if (it->m_itemIdx == 0x1cd1)
        {
            unsigned int expire = it->m_expireTime;
            if (now < expire)
                return 1;
            it = v->erase(it);
            SetIsAffectedExpDouble(false);
            break;
        }
        it++;
    }
    return 0;
}



void CUserCharacInfo::SetIsAffectedExpDouble(bool flag)
{
    CUR->m_isAffectedExpDouble = flag;
}




void CUserCharacInfo::DieCurCharac(int responTime, CUser* user)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_ghost = 1;
    CUR->m_chaosStateTime = GlobalData::s_systemTime_.getCurSec() - CUR->m_chaosDieTime;
    CUR->m_chaosDieTime = GlobalData::s_systemTime_.getCurSec() + responTime;
    int ghostTime = 0;
    bool powerWar = GlobalData::s_power_manager->IsPowerWarEventOn() ||
                    getCurCharacVill() == 7;
    if (powerWar)
    {
        ghostTime = GlobalData::s_power_manager->GetPowerWarGhostTime(
            user, GlobalData::s_power_manager->GetPowerWarResponPenalty(user));
    }
    else
    {
        CUserCharacInfo* other = (CUserCharacInfo*)user;
        ghostTime = ((ServerParameterScript*)((char*)G_CDataManager() + 0x68))
                        ->getAssaultGhostTime(other->get_charac_level(),
                                              other->GetCurCharacChaosPoint());
    }
    if (IsCurCharacGhost() && ghostTime != 0)
    {
        int now = GlobalData::s_systemTime_.getCurSec();
        SetCurCharacChaosDieTime(now);
        SetCurCharacChaosResponTime(now + ghostTime);
    }
    }
}


int CUserCharacInfo::get_charac_level() const
{
    return m_selected ? CUR->m_level : 0;
}

unsigned char CUserCharacInfo::getCurCharacVill() const
{
    if (!m_selected)
        return 0;
    if (G_GameWorld()->IsPvPVillageMapChannel())
        return 10;
    if (G_GameWorld()->IsPVPChannel())
        return CUR->m_villPvP;
    return CUR->m_vill;
}


void CUserCharacInfo::SetCurCharacChaosResponTime(int t)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosResponTime = t;
    }
}


void CUserCharacInfo::SetCurCharacChaosDieTime(int t)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_chaosDieTime = t;
    }
}


unsigned char CUserCharacInfo::IsCurCharacGhost()
{
    return m_selected ? CUR->m_ghost : 0;
}

int CUserCharacInfo::GetCurCharacChaosPoint()
{
    return m_selected ? CUR->m_chaosPoint : 0;
}

void CUserCharacInfo::DecreaseWarPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace("void CUserCharacInfo::DecreaseWarPoint(int)", 0x297, 5);
        trace("WARAREA CUserCharacInfo::DecreaseWarPoint() : m_selected is NULL");
        return;
    }
    if (point < 1)
    {
        CUR->m_finishPointTotal = 0;
    }
    else
    {
        CUR->m_finishPointTotal -= point;
        if (CUR->m_finishPointTotal < 0)
            CUR->m_finishPointTotal = 0;
    }
    ((CUser*)this)->m_historyLog.FPSub(
        point, CUR->m_finishPoint, CUR->m_finishPointTotal, (eFPSubReason)3);
}

void CUserCharacInfo::DecreaseRevengeDungeonTicket()
{
    if (m_selected)
    {
    --CUR->m_revengeDungeonCount;
    }
}


void CUserCharacInfo::DecreasePowerWarPoint(short point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_powerWarPoint = (unsigned short)(CUR->m_powerWarPoint - point);
    if ((short)CUR->m_powerWarPoint < 0)
        CUR->m_powerWarPoint = 0;
    }
}


void CUserCharacInfo::DecFatigueBatteryCharging(short value)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        unsigned short old = CUR->m_fatigueBattery;
        CUR->m_fatigueBattery = (unsigned short)(CUR->m_fatigueBattery - value);
        if ((short)CUR->m_fatigueBattery < 0 || (short)CUR->m_fatigueBattery > (short)old)
            CUR->m_fatigueBattery = 0;
    }
}




void CUserCharacInfo::DecCurCharacLuckPoint(int percent)
{
    enableSaveCharacStat();
    SetCurCharacLuckPoint(GetCurCharacLuckPoint() - GetTenThousandPercentage(GetCurCharacLuckPoint(), percent));
}

void CUserCharacInfo::SetCurCharacLuckPoint(int point)
{
    if (m_selected)
    {
    if (point - CUR->m_luckPoint == 100000)
    {
        CUR->m_luckPoint = 100000;
    }
    else
    {
        CUR->m_luckPoint = point;
        if (CUR->m_luckPoint < 0)
            CUR->m_luckPoint = 0;
        else if (CUR->m_luckPoint > 99999)
            CUR->m_luckPoint = 99999;
    }
    }
}


int CUserCharacInfo::GetCurCharacLuckPoint()
{
    return m_selected ? CUR->m_luckPoint : 0;
}

void CUserCharacInfo::ClearUselessEffectItem(unsigned int expireTime)
{
    std::vector<ContinuousItemInfo>* v =
        (std::vector<ContinuousItemInfo>*)((char*)m_selected + 0xea6);
    for (std::vector<ContinuousItemInfo>::iterator it = v->begin(); it != v->end();)
    {
        if (it->m_expireTime < expireTime)
            it = v->erase(it);
        else
            it++;
    }
}

std::vector<ContinuousItemInfo>* CUserCharacInfo::GetCurCharacEffectItemList()
{
    return (std::vector<ContinuousItemInfo>*)((char*)m_selected + 0xea6);
}

void CUserCharacInfo::ClearUselessCooltimeItem(unsigned int expireTime)
{
    std::vector<ContinuousItemInfo>* v =
        (std::vector<ContinuousItemInfo>*)((char*)m_selected + 0xe9a);
    for (std::vector<ContinuousItemInfo>::iterator it = v->begin(); it != v->end();)
    {
        if (it->m_expireTime < expireTime)
            it = v->erase(it);
        else
            it++;
    }
}

std::vector<ContinuousItemInfo>* CUserCharacInfo::GetCurCharacCoolTimeItemList()
{
    return (std::vector<ContinuousItemInfo>*)((char*)m_selected + 0xe9a);
}

void CUserCharacInfo::ClearProperDungeonUser()
{
    if (m_selected)
    {
    CUR->m_properDungeonUser = 0;
    }
}


void CUserCharacInfo::ClearProperDungeonClearCount()
{
    SetProperDungeonClearCount(0);
}

void CUserCharacInfo::SetProperDungeonClearCount(short count)
{
    if (m_selected)
    {
    CUR->m_properDungeonClearCount = (unsigned short)count;
    }
}


void CUserCharacInfo::ClearCurConditionEventStep()
{
    SetCurConditionEventStep(0);
}

void CUserCharacInfo::SetCurConditionEventStep(short step)
{
    if (m_selected)
    {
    CUR->m_curConditionEventStep = (unsigned short)step;
    }
}


void CUserCharacInfo::ClearCurConditionEventRewardStep()
{
    SetCurConditionEventRewardStep(0);
}

void CUserCharacInfo::SetCurConditionEventRewardStep(short step)
{
    if (m_selected)
    {
    CUR->m_curConditionEventRewardStep = (unsigned short)step;
    }
}


void CUserCharacInfo::ChangeFinishPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace("void CUserCharacInfo::ChangeFinishPoint(int)", 0x24d, 5);
        trace("WARAREA CUserCharacInfo::ChangeFinishPoint() : m_selected is NULL");
        return;
    }
    if (point > 0)
        CUR->m_finishPointTotal += point;
    CUR->m_finishPoint += point;
    if (CUR->m_finishPoint < 0)
        CUR->m_finishPoint = 0;
}

CUserCharacInfo::CUserCharacInfo()
    : m_pvpResult(), m_battleRecord(), m_dungeonClear()
{
    m_selected = 0;
    m_tag = 0;
    resetSaveFlags();
}

void CUserCharacInfo::resetSaveFlags()
{
    m_saveCharacInfo = 0;
    m_saveCharacStat = 0;
    m_saveCharacEtc = 0;
    m_saveCharacManageInfo = 0;
    m_saveInven = 0;
    m_saveSkill = 0;
    m_savePvP = 0;
    m_saveQuest = 0;
    m_field08 = 0;
    m_saveDungeonClear = 0;
    m_saveBattleRecord = 0;
    m_saveCargo = 0;
    m_field0c = 0;
    m_saveCharacOption = 0;
}

void CUserCharacInfo::AddCurCharacChaosPoint(int point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosPoint += point;
    }
}


void CUserCharacInfo::enableSaveCharacStat() { m_saveCharacStat = 1; }

// ============================================================================
// GetTenThousandPercentage（ORIG 0x8550fc9 W）
// 万分比计算：value * percent / 10000（编译器以 0x68db8bad 魔数除法实现，
// 反汇编确认 = imul 后魔数除法，语义即 value*percent/10000）。
// 声明见 CUserCharacInfo.h（SetCurCharacLuckPoint 使用）。
// ============================================================================
int GetTenThousandPercentage(int value, int percent)
{
    return value * percent / 10000;
}
