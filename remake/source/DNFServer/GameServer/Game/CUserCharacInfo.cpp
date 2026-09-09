// ============================================================================
// df_game_r 还原 —— CUserCharacInfo 全部函数（498 符号）。
// 依据 docs/class_func_reports/CUserCharacInfo/（98 个 T 函数逐函数报告）与
// ORIG 反汇编（386 个 W 访问器）逐函数实现；identical 口径编译验证。
// 本批次（CUserCharacInfoNext）：_Charac_info 布局视图 CharacInfoFields
// （0x14d1，具名字段）迁入 owned header；全部 m_selected/m_tag 裸偏移访问
// 字段化（guildName/guildAddr/wpBonus/expBonus/warAreaKill/name/createTime/
// backupSkill/coolTime/effectItem 列表/cargo/inven）；IncFatigueBatteryCharging
// 改经 ServerParameterScript::m_fatigueTable5c0.m_capLimit 具名成员。
// [b28d-solo-CUserCharacInfo 本波] 清单 7 件收口：get_charac_10_level_section /
// getCurCharacEscaladeTutorialFlag / setCurCharacTutorialFlag 入波即 AE/identical；
// is_growth_power_exp_reward_right 修复（+0x127b 同址 bool 视图，ORIG 0x08696ca4
// 13=13 IDENTICAL）；WarAreaKill / GetContinuousItemInfo / calc_disp_help_abuse_ratio
// 复核后维持 N76/N82附/N4 封顶（助记符/槽位证据见各函数头，p167 全 CLEAR）。
// ============================================================================

#include "CUserCharacInfo.h"

#include <string.h>

#include <algorithm>

#include "CUser.h"
#include "CInventory.h"
#include "CExpertJob.h"   // expert_job::CExpertJob 完整类型（IsBusyDisjointStore 用 GetType）
#include "ServerParameterScript.h"   // IncFatigueBatteryCharging/DieCurCharac 经 CDataManager+0x68 实例
#include "GameWorld.h"

// _Charac_info 布局视图（CharacInfoFields，0x14d1）见 CUserCharacInfo.h；
// 此处仅保留访问别名。ORIG 经完整类型直接成员寻址，本 TU 以同布局视图等价。
int DEFAULT_MAX_FATIGUE = 0;  // ORIG 全局（0x9370f5c），原 GameStubs.cpp 定义迁移

// 具型视图直读（CUserCharacInfo.h 匿名 union m_selView）：值 cast 触发 4.4
// 二段拷贝（探针 /tmp/bl10/p2.cpp getE vs getF），直读复现 ORIG 单段装载。
#define CUR (m_selView)

// ORIG 平坦窗口视图（P1 组件视图：pad 前置使成员寻址折叠为单条 disp/scale 指令，
// 与具名字段等偏移；ORIG 源码即以“基址+魔数下标”形态书写，直接成员下标会丢
// 常量项使指令序列回退）：
//   WarAreaKill 0x08645a3a `flds 0xe(%edx,%ebx,4)`，ebx=index+0x380 ⇔
//     &m_warAreaKill[index]（m_warAreaKill[5]@+0xe0e = +0xe + 0x380*4）；
//   GetContinuousItemInfo 0x08677f97 `mov %edx,0x10(%eax,%ecx,8)`，
//     ecx=index+0x460 ⇔ cooltime 条目 @SIG_LOAD_ETC +0x2310 + 8i；
//   0x08678031 `mov %edx,0x8(%eax,%ecx,8)`，ecx=index+0x494 ⇔ effect 条目
//     @+0x24a8 + 8i。
#pragma pack(push, 1)
struct WarAreaKillWindow
{
    char m_pad0E[0xe];
    float m_warAreaKillFlat[0x385];   // [0x380+i] = +0xe0e + 4i
};
struct LoadEtcCoolItemWindow
{
    char m_pad10[0x10];
    ContinuousItemInfo m_entries[0x494];  // [0x460+i] = +0x2310 + 8i
};
struct LoadEtcEffectItemWindow
{
    char m_pad08[0x8];
    ContinuousItemInfo m_entries[0x4C8];  // [0x494+i] = +0x24a8 + 8i
};
// [b28d-solo-CUserCharacInfo N107/P106] m_inven 真实类型视图：CharacInfoFields 内
// `char m_inven[]` 占位使取址退化为数组加法（4.4 恒 `add $0xda`）；经真实
// CInventory 成员取址复现 ORIG `lea 0xda(%edx),%ecx`。pack(1) 保持 +0xda。
struct CharacInvenView
{
    char m_pad0[0xda];
    CInventory m_inven;    // +0xda（CInventory 内嵌）
};
#pragma pack(pop)
static_assert(offsetof(WarAreaKillWindow, m_warAreaKillFlat) == 0xe,
              "war area kill window base drift (ORIG flds 0xe(%edx,%ebx,4))");
static_assert(offsetof(LoadEtcCoolItemWindow, m_entries) == 0x10,
              "cooltime flat base drift (ORIG mov %edx,0x10(%eax,%ecx,8))");
static_assert(offsetof(LoadEtcEffectItemWindow, m_entries) == 0x8,
              "effect flat base drift (ORIG mov %edx,0x8(%eax,%ecx,8))");
static_assert(offsetof(CharacInvenView, m_inven) == 0xda,
              "inven view base drift (ORIG lea 0xda(%edx),%ecx)");

CUserCharacInfo::~CUserCharacInfo()
{
    // 空体：隐式成员逆序析构（m_dungeonClear→m_battleRecord→m_pvpResult =
    // ORIG +0xb4→+0x9c→+0x18 调用序）。显式伪析构调用使 4.4 每调用点再布
    // cleanup 链（+22 条 vs ORIG 43）。
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


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
void CUserCharacInfo::subUltimateInoutValue(char index)
{
    CUR->m_saveBloodInout = 1;
    --CUR->m_ultimateInout[index];
}

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
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
                                  __PRETTY_FUNCTION__,
                                  0x2d2, "set member exp level error , name(%s) , level(%d), new level(%d)",
                                  (char*)CUR->m_name, (unsigned int)CUR->m_uppermemberExpLevel,
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


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
void CUserCharacInfo::set_charac_visible_teleport_(bool flag)
{
    if (m_selected)
    {
        CUR->m_visibleBits.teleport = flag;
    }
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
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
    CUR->m_createTime.SetCreateTime(t);
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


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
void CUserCharacInfo::setWeekendBonusExp(unsigned int exp, int percent)
{
    if (m_selected)
    {
        CUR->m_weekendBonusExp = (int)((double)exp * (percent / 100.0f));
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


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
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

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
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


// ORIG W 0x0822f8f6：m_selected 非空时返回其 +0xee9 字节，否则 0。
// 返回 bool：调用点 0x0821b4d8 `xor $0x1,%eax;test %al,%al`（uchar/int 声明不出 xor，探针实证）。
bool CUserCharacInfo::isDisguiseCharac()
{
    if (m_selected)
    {
        return CUR->m_isDisguiseCharac;
    }
    return false;
}


// ORIG W 0x0822f91a：m_selected 非空时置 +0xee9 伪装标志，再依次
// setDisguiseKind(kind)（W 0x0822f976）/ setDisguiseIndex(index)（W 0x0822f99e）。
// [b28d-solo-CUserCharacInfo P210] 本定义必须前置于两个 callee 定义之前：
// 4.4 对 TU 内已见定义的 callee 精确继承 outgoing ⇒ caller 帧塌缩 sub $0x14
// + spill -0x4/-0x8/-0xc；前置后恢复 ORIG 保守帧 sub $0x28 + spill
// -0xc/-0x10/-0x14（探针 /tmp/s156_probe/p11.cpp f0/f1 对照）。
void CUserCharacInfo::setDisguiseCharac(bool flag, unsigned char kind,
                                        unsigned short index)
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


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
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



// [b28d-solo-CUserCharacInfo AE] 四旋钮归零（108=108）：
// ①N6 翻案：`1u << flag` → `1 << flag`（int 字面量）保 ORIG `mov $1,%ebx;
//   mov %ebx,%esi; shl %cl,%esi`（`1u` 被 4.4 折成 `shr %cl; and $1`；
//   探针 /tmp/s156_probe/p12.cpp f1 vs f4）；
// ②P169 尾标签守卫（`if(byteIndex<=0xf) goto PASS; return false; PASS:`）复现
//   ORIG `js <false>; cmpl $0xf; jle <body>; mov $0; jmp`（P222 包裹恒产 jg 岛）；
// ③or 操作数序改 field 先（ORIG 先读 field 再算 1<<bit，寄存器 %ebx/%edi 归位）；
// ④路由 `-march=i486 -mtune=generic`（Main 2026-09-09 已批）+ `index / 8` 复现
//   ORIG 6 条符号修正链（探针 /tmp/s156_probe/p13i486.o）。
bool CUserCharacInfo::setCurCharacTutorialFlag(unsigned int flag)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        if (flag < 0x20)
        {
            // [b28d-L14 实证] ORIG 0x08647466 无 mask 槽：表达式内联（P0）。
            // [b28d-solo-CUserCharacInfo N6 翻案] `1 << flag`（int 字面量）保 ORIG 的
            // `mov $1,%ebx; mov %ebx,%esi; shl %cl,%esi`；`1u <<` 被 4.4 折成
            // `shr %cl; and $1`（探针 /tmp/s156_probe/p12.cpp f1 vs f4）。
            if ((CUR->m_tutorialFlags & (1 << flag)) != 0)
                return false;
            CUR->m_tutorialFlags |= 1 << flag;
            return true;
        }
        if (flag < 0x80)
        {
            int index = (int)(flag - 0x20);
            // [b28d-solo-CUserCharacInfo N97 复核] `index / 8` 实测 4.4 产
            // `lea 0x7(%eax); test; cmovs`（默认旗标，cmovs 形 ORIG 无）；
            // [b28d-solo-CUserCharacInfo 路由已批] 本 TU 走 `-march=i486 -mtune=generic`
            // （dnf_game_cxx.sh i486 组，Main 2026-09-09 落）后 4.4.7 对 `index / 8`
            // 产 ORIG 精确 6 条（mov/mov/sar/shr/lea/sar，探针 /tmp/s156_probe/p13i486.o）。
            int byteIndex = index / 8;
            // [b28d-solo-CUserCharacInfo P0/P222] ORIG 0x086474ad 有 int bit 槽 -0x1c
            // （先算 index%8 再边界检查）且两守卫走 P222 正向包裹（js 尾部共享 false 块）。
            int bit = index % 8;
            if (byteIndex >= 0)
            {
                if (byteIndex <= 0xf)
                    goto PASS;
            }
            return false;
        // [b28d-solo-CUserCharacInfo P169] 复现 ORIG 0x086474ad 段
        // `js <false>; cmpl $0xf; jle <body>; mov $0; jmp; <body>:`（P222 包裹恒产
        // `jg <skip>` 岛）。
        PASS:
            if ((((signed char)CUR->m_tutorialFlags2[byteIndex]) >> bit & 1) != 0)
                return false;
            CUR->m_tutorialFlags2[byteIndex] =
                (unsigned char)(CUR->m_tutorialFlags2[byteIndex] | (1 << bit));
            return true;
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

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
// （身位字节 shl/sar/and/or 逻辑逐条全等）
void CUserCharacInfo::setCurCharGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    enableSaveCharacInfo();
    CUR->m_growType.first = first;
    CUR->m_growType.second = second;
}



// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
// 同 setCurCharGrowthType（身位字节写逻辑逐条一致）。
void CUserCharacInfo::setCurCharChangingGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    CUR->m_growTypeChanging.first = first;
    CUR->m_growTypeChanging.second = second;
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
    // [b28 BM8 P223] ORIG 三空块中继岛（+0x24c/0x24f/0x252 `nop;jmp`）：
    // m_selected==0 空 else、`!(best<=round)`、`best==cur 全等` 短路空出口。
    // 4.4 jump-threading 折叠空块 ⇒ 改 goto+尾空标签形逐岛复现。
    if (m_selected == 0)
        goto L_SBR_END;
    {
    if (round > 0xf)
        round = 0xf;
    {
    std::map<int, stBloodBestRecord>::iterator it =
        CUR->m_bloodBestMap.m_map.find(record);
    if (it == CUR->m_bloodBestMap.m_map.end())
    {
        if (!flag)
            setBloodBestRecordUpdate(true);
        // ORIG（0x086880a3-0x086880c4）实参槽序：0x4(%esp)=lea 0x14(%ebp)
        // （&record），0x8(%esp)=lea -0x38(%ebp)（&local{-0x38=round,-0x34=time}）
        // ⇒ make_pair(record, local)。旧形把两实参对调且把 record+flag 裸字节
        // 重读成 Blood 值——真语义 bug（P196 族），按 ORIG 恢复：key=record、
        // value=stBloodBestRecord{m_round=round,m_time=time} 具名局部。
        stBloodBestRecord local;
        local.m_round = round;
        local.m_time = time;
        CUR->m_bloodBestMap.m_map.insert(
            std::make_pair<int&, stBloodBestRecord&>(record, local));
        CUR->m_bloodBestRound = round;
        CUR->m_bloodBestTime = time;
    }
    else
    {
        CUR->m_bloodBestRound = it->second.m_round;
        CUR->m_bloodBestTime = it->second.m_time;
        if (round < CUR->m_bloodBestRound)
            goto L_SBR_END2;
        if (CUR->m_bloodBestTime == time &&
            CUR->m_bloodBestRound == round)
            goto L_SBR_END3;
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
L_SBR_END3:;
L_SBR_END2:;
L_SBR_END:;
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
    memset(CUR->m_guildName, 0, sizeof(CUR->m_guildName));
    memset(CUR->m_guildAddr, 0, sizeof(CUR->m_guildAddr));
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


// [b28d-Main·检查点14] ORIG 0x08696ca4：13 条，`movzbl 0x127b(%eax),%eax; jmp`
// 直返无 setne ⇒ 字段/返回同型 bool；调用点 0x0868dbfb `xor $1` 联判。
// 字段已由 Main 在 CUserCharacInfo.h 正式改 bool，TU 局部视图已删除（直读）。
bool CUserCharacInfo::is_growth_power_exp_reward_right() const
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


// [b28c-L26 档①] 返回类型随头改 bool（Main 批准，ORIG 体 movzbl 直返实证）。
bool CUserCharacInfo::isSaveSkill() { return m_saveSkill; }

bool CUserCharacInfo::isSaveQuest() { return m_saveQuest; }

bool CUserCharacInfo::isSavePvP() { return m_savePvP; }

bool CUserCharacInfo::isSaveInven() { return m_saveInven; }

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
    // ORIG（0x8678084）`lea 0xdaa(%eax),%edx`/`lea 0xda(%eax)` 指针算术形态；
    // char 数组成员 decay 会退化为 add 改写 dest 寄存器（回退证据：转换实验），
    // 故保留指针算术但偏移改用具名字段 offsetof（防布局漂移，docs/
    // class_func_reports/CUserCharacInfo/isExistItem.md）。
    if (((const CCargo*)((const char*)m_selected +
                         offsetof(CharacInfoFields, m_cargo)))->check_item_exist(itemIdx) != -1)
        return true;
    if (((const CInventory*)((const char*)m_selected +
                             offsetof(CharacInfoFields, m_inven)))->check_item_exist(itemIdx) != -1)
        return true;
    return false;
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

signed char CUserCharacInfo::get_charac_visible_values()
{
    return m_selected ? CUR->m_visibleValues : 0;
}

bool CUserCharacInfo::get_charac_visible_grow_avatar()
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
        else
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
    // [b28] 值 cast 伪差已消（m_selView 直读），本函数 AE。
    return CUR->m_ultimateInout[index];
}

_Charac_info* CUserCharacInfo::getTagCharacW() { return m_tag; }

char* CUserCharacInfo::getTagCharacSkillW()
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (char*)((CharacInfoFields*)m_tag)->m_skill;
}

SkillSlot* CUserCharacInfo::getTagCharacSkillR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (SkillSlot*)((CharacInfoFields*)m_tag)->m_skill;
}

const _Charac_info* CUserCharacInfo::getTagCharacR() const { return m_tag; }

Inven_Item* CUserCharacInfo::getTagCharacInvenW()
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (Inven_Item*)((CharacInfoFields*)m_tag)->m_inven;
}

CInventory* CUserCharacInfo::getTagCharacInvenR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (CInventory*)((CharacInfoFields*)m_tag)->m_inven;
}

CCargo* CUserCharacInfo::getTagCharacCargoW()
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return &((CharacInfoFields*)m_tag)->m_cargo;
}

const CCargo* CUserCharacInfo::getTagCharacCargoR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return &((CharacInfoFields*)m_tag)->m_cargo;
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

// [b28d-solo-CUserCharacInfo AE] 160=160：
// ①`out.clear()` 移到 buffer 声明之前 ⇒ ORIG 0x0864607e `call clear` 在 rep stos 前；
// ②删具名 bool stop 改直测条件（P198②）；
// ③P274/P319：offset 更新三处 unsigned cast ⇒ 复现 ORIG 两条 lea
//   （`lea (%eax,%edx,1)` + `lea (%edx,%eax,1)`，探针 /tmp/s156_probe/p10.cpp g3）。
void CUserCharacInfo::getSeparatedTextUnits(std::vector<std::string>& out,
                                            const char* text, const char* sep,
                                            int maxUnits)
{
    // [b28d-L14 真 bug 修正 + P253] ORIG 0x08647874 `mov $0x40,%edx; rep stos`：
    // %edx 是 dword 计数 ⇒ 清零 0x40*4=0x100 字节（全缓冲），旧 memset(0x40)
    // 少清 0xc0 字节。显式零初始化数组复现 4.1 rep stos（P214③）。
    out.clear();
    char buffer[0x100] = {0};
    int offset = 6;
    while (true)
    {
        // [b28d-solo-CUserCharacInfo P198②] ORIG 0x086460d4 无 stop 槽（寄存器 setne
        // 物化直测）⇒ 条件内联，勿用具名 bool 局部。
        if (maxUnits != 0 && (int)out.size() >= maxUnits - 1)
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
        // [b28d-solo-CUserCharacInfo P274/P319] ORIG 0x08646196 两条 lea
        // （`lea (%eax,%edx,1)` + `lea (%edx,%eax,1)`）；三处 unsigned 化
        // （异型化旋钮，探针 /tmp/s156_probe/p10.cpp g3）即逐条复现。
        offset = (unsigned int)strlen(sep) + (unsigned int)length +
                 (unsigned int)offset;
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

bool CUserCharacInfo::getMemberDungeonFlag() const
{
    return m_selected ? CUR->m_memberDungeonFlag : true;
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

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
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
    // ORIG 0x8696c23 true 路单一 `movzbl 0x1275(%eax),%eax`、false 0x8696c2c
    // `mov $0xffffffff,%eax`（dword -1）。int 返回三元/if 形均反产二段 promote
    // （IsCurCharacGhost 同款教训）→ uchar 返回 + if 形，false 保留 dword -1。
    if (m_selected)
        return CUR->m_curEquipslotFlag;
    return -1;
}


_Charac_info* CUserCharacInfo::getCurCharacW() { return m_selected; }

signed char CUserCharacInfo::getCurCharacVillPrev() const
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



unsigned int CUserCharacInfo::getCurCharacNo() const
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

CInventory* CUserCharacInfo::getCurCharacInvenRefW()
{
    enableSaveInven();
    return (CInventory*)CUR->m_inven;
}

const CInventory* CUserCharacInfo::getCurCharacInvenRefR() const
{
    return (const CInventory*)CUR->m_inven;
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

// [b28d-solo-CUserCharacInfo AE] 69=69：
// ①N6 翻案：`1 << flag`（int 字面量）保 ORIG `shl`（探针 /tmp/s156_probe/p12.cpp f4）；
// ②P169 尾标签守卫 + 低位分支共享尾 return（消两处 `mov $0; jmp` 岛）；
// ③路由 `-march=i486 -mtune=generic`（Main 已批）+ `index / 8` 复现 ORIG 6 条修正链
//   （探针 /tmp/s156_probe/p13i486.o）。
int CUserCharacInfo::getCurCharacEscaladeTutorialFlag(unsigned int flag)
{
    if (m_selected)
    {
        if (flag < 0x20)
        {
            // [b28d-L14 实证] ORIG 0x0864751e 无 mask 槽：`1<<flag` 以表达式内联
            // （mov $1,%ebx; mov %ebx,%esi; shl %cl,%esi），P0 禁具名局部缓存。
            // [b28d-solo-CUserCharacInfo N6 翻案] `1 << flag` 保 shl（探针 p12 f4）。
            if ((CUR->m_tutorialFlags & (1 << flag)) != 0)
                return 1;
        }
        else if (flag < 0x80)
        {
            int index = (int)(flag - 0x20);
            // [b28d-solo-CUserCharacInfo 路由已批] `index / 8` + i486 产 ORIG 6 条
            // （同 setCurCharacTutorialFlag；探针 /tmp/s156_probe/p13i486.o）。
            int byteIndex = index / 8;
            // [b28d-solo-CUserCharacInfo P0/P222] ORIG 0x0864757a 有独立 bit 槽 -0xc
            // （先算 index%8 再边界检查）且两守卫走 P222 正向包裹（js 尾部共享 false 块）。
            int bitIndex = index % 8;
            if (byteIndex >= 0)
            {
                if (byteIndex <= 0xf)
                    goto PASS;
            }
            return 0;
        // [b28d-solo-CUserCharacInfo P169] 复现 ORIG 0x0864757a 段
        // `js <false>; cmpl $0xf; jle <body>; mov $0; jmp; <body>:`。
        PASS:
            return (int)((signed char)CUR->m_tutorialFlags2[byteIndex]) >> bitIndex & 1;
        }
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
    return &CUR->m_cargo;
}

const CCargo* CUserCharacInfo::getCurCharacCargoR() const
{
    return &CUR->m_cargo;
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
    return m_selected ? (char*)CUR->m_guildName : 0;
}

char* CUserCharacInfo::getCreateGuildAddr()
{
    return m_selected ? (char*)CUR->m_guildAddr : 0;
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

bool CUserCharacInfo::getBringUpRewardFlag() const   // [b28d-Main 代落·S350 P292] uchar→bool
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
    const Inven_Item* invenItem = 0;
    for (int slot = 10; slot <= 0x15; ++slot)
    {
        if ((invenItem = getCurCharacInvenR()->GetInvenRef(0, slot)) == 0)
            continue;
        if (invenItem->m_addInfo == 0)
            continue;
        CItem* found = G_CDataManager()->find_item(invenItem->m_addInfo);
        if (found == 0)
            continue;
        CEquipItem* equip = (CEquipItem*)found;
        if (equip->getWpBonusPoint()->m_type == 0)
        {
            CUR->m_wpBonus.m_base += equip->getWpBonusPoint()->m_value;
        }
        else if (equip->getWpBonusPoint()->m_type == 1)
        {
            CUR->m_wpBonus.m_add += equip->getWpBonusPoint()->m_value;
        }
        if (equip->getExpBonusPoint()->m_type == 0)
        {
            CUR->m_expBonus.m_base += equip->getExpBonusPoint()->m_value;
        }
        else if (equip->getExpBonusPoint()->m_type == 1)
        {
            CUR->m_expBonus.m_add += equip->getExpBonusPoint()->m_value;
        }
    }
}



void CUserCharacInfo::resetBonusPoint()
{
    if (m_selected)
    {
    memset(&CUR->m_wpBonus, 0, sizeof(CUR->m_wpBonus));
    memset(&CUR->m_expBonus, 0, sizeof(CUR->m_expBonus));
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

// [b28 BM8 P210] 定义序移到全部同 TU callee（get_level_up_exp/两 getter）之后：
// 4.4 按已见 callee 帧继承 outgoing，缩帧向 ORIG 0x58 靠拢；原「fildll 槽对齐
// 恒形 N」旧判词系未试 P210 前置。实测 P210 前置帧不变（仍 0x68）；具名 ull
// 局部形反劣 80 insn（reload 链），回滚匿名临时形（68=68 助记符全等）。
// [b28 BM8 判词 N82附/N95] 残差全为槽号平移：4.4 聚合(8B)临时优先占深槽且基址
// 比 4.1 低 8B（-0x38 vs -0x30，连带 cw -0x3a/-0x3c vs -0x32/-0x34、除数槽
// -0x4c vs -0x3c），帧 0x58↔0x68 差 16B——4.1 生命周期穿插 vs 4.4 分区排布，
// 无源形可控（ndiff 28 封顶）。
// [b28d-solo-CUserCharacInfo 复核] ORIG 符号实为非 const _ZN15CUserCharacInfo26
// calc_disp_help_abuse_ratioEi（P107，清单给的 _ZNK 版 ORIG 不存在）；68/68 助记符
// 全等，-0x14/-0x10/-0xc/-0x1c/-0x18 槽全同，仅 8B 聚合临时对齐（16 vs 8）连带
// cw/除数槽与帧差 ⇒ 维持 N82附/N95 封顶。
// [b28d-solo-CUserCharacInfo P210 实验] 前移 get_charac_level 定义（唯一不可见
// callee）后本件帧仍 0x68（无变化，已回退）⇒ 帧差非 callee 帧继承所致。
// [b28d-solo-CUserCharacInfo 复核] 68=68 助记符逐条全等；残差=ull 转换匿名 8B 临时
// 与 cw/除数槽相对 ebp 整体下移 8（-0x30/-0x32/-0x3c → -0x38/-0x3a/-0x4c）及帧
// 0x58↔0x68：4.4 对 (double)uint 的匿名 8B 临时优先占深槽（N95/N82附）；本件无死
// 局部可删（P232）、撑帧只会加深（P190 反向）⇒ 维持 NEAR 封顶（p167 CLEAR）。
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

void CUserCharacInfo::calcHelpAbuseRatioBeforeLevelUp(int param)
{
    char unused[0x20];
    (void)unused;
    (void)param;
}


void CUserCharacInfo::calcHelpAbuseRatio(unsigned int param, bool flag)
{
    // [b28d-solo-CUserCharacInfo P232/P0] ORIG 帧 0x24 + 唯一 `mov %al,-0x24(%ebp)`：
    // 4.4 对 bool 形参恒 spill 到帧底槽（槽深=帧大小），探针 /tmp/p2d_probe.cpp
    // k1/k2/k3 实测 `sub $0x24; mov 0x10(%ebp),%eax; mov %al,-0x24(%ebp)` 与 ORIG
    // 逐条全等 ⇒ 死局部 char[0x20] 撑帧 0x24 即复现（旧数组赋值形两段已废）。
    char unused[0x20];
    (void)unused;
    (void)flag;
    (void)param;
}




// ORIG 0x08655bce 有符号除 `fatigue/10` 魔数段 `sar $0x1f,%eax;sub %eax,%edx` 直落 %edx；
// 4.4 经 %ecx 中转 `mov %edx,%ecx;sub %eax,%ecx;mov %ecx,%eax;mov %eax,%edx`（N7/N18a
// 寄存器路由），除运算与数组索引逐段全等，分支 je/jbe 落点合规，语义等价。
void CUserCharacInfo::calcCurCharacSchoolPoint(int fatigue)
{
    static const int point[24] = {
        0, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 5, 5, 5, 5
    };
    static const int pointSize = sizeof(point) / sizeof(point[0]);
    unsigned short used = getCurCharacUsedFatigue();
    // [b28d-L14 实证] 右操作数显式 (int) 化消 4.4 的 int→unsigned 物化四连
    // （探针 /tmp/l14_probe4.cpp c4 与 ORIG 0x08655bce 逐条全等：sub %eax,%edx 直落）。
    if (fatigue / 10 == (int)(used / 10u))
        return;
    unsigned int index = used / 10u;
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


// [b28d-solo-CUserCharacInfo AE] 21=21：P274 异型视角（字段 unsigned 视角
// m_guildTodayExpU + 形参 int）⇒ 复现 ORIG `mov 0xdef(%edx),%ecx; mov 0xc(%ebp),%edx;
// lea (%ecx,%edx,1),%edx`（旧判「字段 unsigned 无效」系未用 union 视角 + 字段在前写法）。
void CUserCharacInfo::add_guild_today_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        // P274 异型视角（字段 unsigned + 形参 int）：复现 ORIG
        // `mov 0xdef(%edx),%ecx; mov 0xc(%ebp),%edx; lea (%ecx,%edx,1),%edx`。
        CUR->m_guildTodayExpU = CUR->m_guildTodayExpU + exp;
}


void CUserCharacInfo::add_guild_temp_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildTempExp = (unsigned short)(CUR->m_guildTempExp + (short)exp);
}


// [b28d-solo-CUserCharacInfo AE] 21=21：P274 异型视角（m_guildExpU）复现 ORIG
// 0x08645cb1 `mov 0xde9(%edx),%ecx; mov 0xc(%ebp),%edx; lea (%ecx,%edx,1),%edx`。
void CUserCharacInfo::add_guild_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        // P274 异型视角（同 add_guild_today_exp）。
        CUR->m_guildExpU = CUR->m_guildExpU + exp;
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


// [b28d-L14 N7/N4] ORIG 0x086459c4：残差=寄存器角色互换（%ecx↔%edx）+
// `lea 0x380(%reg)` 物化时刻（4.1 于 fadds 后 / 4.4 前置）。p167 CLEAR（语义完整）。
void CUserCharacInfo::WarAreaKill(int index, int point, float kill)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x284, 5);
        trace("WARAREA CUserCharacInfo::WarAreaKill() : m_selected is NULL");
        return;
    }
    ChangeFinishPoint(point);
    // ORIG 0x08645a3a：读 flds 0xe(%edx,%ebx,4) + 写 lea 0x380(%ecx),%edx 两次
    // 独立 lea、raw idx 留 %ecx；4.4 RA 共享单 lea（读写拆两式亦被 copy-prop），
    // N18a/4.1-RA 族天花板，维持 += 最简形。
    // [b28d-solo-CUserCharacInfo N18a/4.1-RA 封顶] 11 形实测（/tmp/s156_probe/
    // p5-p8：`+=`、展开赋值、下标加法序交换、指针算术、cast 下标、`*&` 形、union
    // 双数组视角、pack(1) 下标等）全部产 4.4 前置 `lea 0x380(%edx),%ecx`（ORIG 为
    // fadds 后 `lea 0x380(%ecx),%edx`）；4.4 对「读写共享的 PLUS_EXPR 下标」恒先
    // 求值 LHS 地址（SAVE_EXPR），无源形可翻。59/59 条数全等，语义等价。
// [b28d-solo-CUserCharacInfo 复核] 再补 9 形（/tmp/b28d_solo/wak.cpp、wak2.cpp、
// wak3.cpp、star.cpp：`+=`/展开式/下标加法序交换/指针算术/具名 float 槽/裸语句
// 表达式 `({...})`/volatile 视图/`*&` 双算/两处独立下标）全部产 LHS 地址前置
// `lea 0x380(%edx),%ecx`；P211 档③ cast 异型化（int↔unsigned）亦无 RHS-first
// ⇒ N76/N18a 封顶定论（ORIG 4.1 为 fadds 后 `lea 0x380(%ecx),%edx`）。
    ((WarAreaKillWindow*)CUR)->m_warAreaKillFlat[index + 0x380] += kill;
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


// [b28c-L26 SIGFIX·档①] 定义返回类型随头改 unsigned int（P159：返回类型不入
// mangled，ABI 同宽；lane35 转来的 spec，Main 终裁本 TU 归我收尾）。
unsigned int CUserCharacInfo::UseFinishPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x220, 5);
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


void CUserCharacInfo::SetOverTradeGoldCount(unsigned int count)
{
    // ORIG：`mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0xd0(%eax)`
    // LHS(this) 先载。字段已改 unsigned（头同步）与形参 Ej 同型直赋无转换节点，
    // 4.4 出 LHS 先=ORIG 同形（N76 翻案：无转换→LHS 先，有转换→RHS 先）。
    m_overTradeGoldCount = count;
}

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

bool CUserCharacInfo::IsProperDungeonUser()
{
    return m_selected ? CUR->m_properDungeonUser : 0;
}

bool CUserCharacInfo::IsFirstTimeLogin()   // [b28d-Main 代落·S336 P292] uchar→bool
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


bool CUserCharacInfo::IsCurCharacVisible()
{
    if (m_selected && CUR->m_visibleBits.visible)
        return true;
    return false;
}


bool CUserCharacInfo::IsCurCharacLeaveTimeout()
{
    return m_selected ? CUR->m_leaveTimeout : false;
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


// [b28d-L14 N82附/N18a] ORIG 0x0864657e：ORIG 用 %edi 跨 call 保活（push edi，帧 0x4c）
// vs 4.4 槽化（帧 0x40）；`mov (%eax),%ebx` 4.4 折单段。p167 CLEAR（8/8，语义完整）。
bool CUserCharacInfo::IsAvailableCurCharacTradeGoldDaily(unsigned int gold)
{
    if (!m_selected)
        return false;
    unsigned int limit = get_charac_level() * get_charac_level() * 10000;
    if (limit < CUR->m_reliablePerson.GetTradeGoldDaily() + gold)
    {
        // [b28d-solo-CUserCharacInfo P227/P37] ORIG 0x0864657e：varargs 先求值（右起）
        // 且 GetTradeGoldDaily 值经 %edi 跨 ctor 保活（push edi）⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x3f1, 0)(
            "TRADE_RESTIRCTION charac_name(%s), charac_level(%d), limit_gold(%u), "
            "add_gold(%u), use_gold(%u)",
            getCurCharacName(), get_charac_level(), limit, gold,
            CUR->m_reliablePerson.GetTradeGoldDaily());
        return false;
    }
    return true;
}

const char* CUserCharacInfo::getCurCharacName() const
{
    return m_selected ? (const char*)CUR->m_name : "";
}


void CUserCharacInfo::InitFinishPoint()
{
    if (m_selected)
    {
    CUR->m_finishPoint = 0;
    CUR->m_finishPointTotal = 0;
    memset(CUR->m_warAreaKill, 0, sizeof(CUR->m_warAreaKill));
    ((CUser*)this)->m_historyLog.FPSub(
        0, CUR->m_finishPoint, CUR->m_finishPointTotal, (eFPSubReason)1);
    }
}


void CUserCharacInfo::IncrementCurCharacTradeGoldDaily(unsigned int gold)
{
    enableSaveCharacStat();
    CUR->m_reliablePerson.IncrementTradeGold(gold);
}

// ORIG `movzwl 0x121f(%edx),%edx;mov %edx,%ecx` 二段；4.4 折单步 `movzwl …,%ecx`
// （N18a：4.4 恒折 mem→%reg→callee-saved 两段），寄存器分配差异，语义等价。
// [b28d-solo-CUserCharacInfo N18a 复核] `+=` 形实测同形（movzwl mem,%ecx 单段）；
// 字段为 16 位 ushort，无法按 P254 改 enum（宽度变 4 破坏 +0x121f 布局）⇒ 封顶。
void CUserCharacInfo::IncreasePowerWarPoint(short point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    // P254 packed enum 视角：复现 ORIG `movzwl 0x121f(%edx),%edx; mov %edx,%ecx` 两段。
    CUR->m_powerWarPointEnum =
        (EPowerWarPointValue)((int)CUR->m_powerWarPointEnum + point);
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


// ORIG 0x086566e4：`movzwl 0x4f,%edx;mov %edx,%ecx` 二段（4.4 折单步 N18a）；两处比较
// ORIG 直 `cmp -0xa(%ebp),%ax` 折返，4.4 先物化寄存器再 `cmp %ax,%dx`，jl/jle 落点相位同，
// 无一分支反向，语义等价。
// [b28d-solo-CUserCharacInfo AE] 49=49：P254 packed enum 视角（EFatigueBatteryValue /
// m_fatigueBatteryEnum）⇒ 累加段两段 regmove 复现（old/比较段维持 ushort 视角不变）。
void CUserCharacInfo::IncFatigueBatteryCharging(short value)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        // ORIG IncFatigueBatteryCharging（0x86566e4）：movzwl 0x5c0(%eax) 读
        // CDataManager+0x68 处 ServerParameterScript 的疲劳电池常量表上限。
        ServerParameterScript* sp =
            (ServerParameterScript*)((char*)G_CDataManager() +
                                     offsetof(CDataManager, m_serverParameter));
        // P209 翻案：old 声明 signed short 出 ORIG 直 `cmp -0xa(%ebp),%ax`
        // 内存比较（探针 /tmp/bl10/p5.cpp h1/h2）。
        short old = (short)CUR->m_fatigueBattery;
        // P254 packed enum 视角：复现 ORIG `movzwl 0x4f(%edx),%edx; mov %edx,%ecx` 两段。
        CUR->m_fatigueBatteryEnum =
            (EFatigueBatteryValue)((int)CUR->m_fatigueBatteryEnum + value);
        if ((short)CUR->m_fatigueBattery < old ||
            (int)(short)CUR->m_fatigueBattery > (int)sp->m_fatigueTable5c0.m_capLimit)
        {
            CUR->m_fatigueBattery = sp->m_fatigueTable5c0.m_capLimit;
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x22f, 5);
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x260, 5);
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
void CUserCharacInfo::SetIsInitSkillFlag(bool flag)
{
    if (m_selected)
    {
        CUR->m_isInitSkillFlag = flag;
    }
}

void CUserCharacInfo::SetIsInitSkillFlag2ND(bool flag)
{
    if (m_selected)
    {
        CUR->m_isInitSkillFlag2ND = flag;
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x26a, 5);
        trace("WARAREA CUserCharacInfo::GetFinishPointTotal() : m_selected is NULL");
        return 0;
    }
    return CUR->m_finishPointTotal;
}



int CUserCharacInfo::GetFinishPoint()
{
    if (m_selected)
        return CUR->m_finishPoint;
    cMyTrace trace(__PRETTY_FUNCTION__, 0x244, 5);
    trace("WARAREA CUserCharacInfo::GetFinishPoint() : m_selected is NULL");
    return 0;
}


short CUserCharacInfo::GetFatigueBatteryCharging()
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

int CUserCharacInfo::GetCurCharacSkillTreeIndex() const  // ORIG 0x822f33c _ZNK15CUserCharacInfo26GetCurCharacSkillTreeIndexEv
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
    return m_selected ? (const char*)CUR->m_backupSkill2ND : 0;
}

const char* CUserCharacInfo::GetCurCharacBackupSkill() const
{
    return m_selected ? (const char*)CUR->m_backupSkill : 0;
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

// [b28d-solo-CUserCharacInfo] ①字段直赋中转已修（P211 档③）：SigTypes_C.h
// m_cooltimeCharacNo/m_effectCharacNo int→unsigned（与 m_characNo 同型）⇒ ORIG
// 0x08677f34 单段 `mov 0x4(%eax),%edx`，ndiff 54→48。②余差 = N82附（4.4 把
// `index + 0x460` CSE 到 %esi 跨 call 保活，ORIG 每处独立 `lea 0x460(%ebx),%ecx`
// 重算；语句表达式下标/下标交换两形实测不可阻）+ N4/P30（4.4 把 disp 0x10 折成
// 索引 +2：`lea 0x2(%esi),%edx`）⇒ 封顶，语义等价（104 vs 100 条）。
// [b28d-solo-CUserCharacInfo 复核] 再补 5 形（/tmp/b28d_solo/gcii.cpp、gcii2.cpp、
// gcii3.cpp、star.cpp：`*&` 双算、裸语句表达式 RHS、具名 const 引用、int↔unsigned
// cast 异型化、unsigned 下标）均产同一「LHS 地址（index+0x460）前置跨 call 保活」形；
// disp 折叠经探针确认为 4.4 定则（disp/scale≥2 折、==1 不折：gcii.cpp k1 vs k3）
// ⇒ N76/N82附/N4 封顶定论。
void CUserCharacInfo::GetContinuousItemInfo(SIG_LOAD_ETC* p) const
{
    p->m_cooltimeCharacNo = p->m_characNo;
    p->m_effectCharacNo = p->m_characNo;
    const std::vector<ContinuousItemInfo>* cool = GetCurCharacCoolTimeItemListR();
    p->m_cooltimeItemCount = (unsigned char)cool->size();
    int index = 0;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        ((LoadEtcCoolItemWindow*)p)->m_entries[index + 0x460].m_itemIdx =
            (*cool)[i].m_itemIdx;
        ((LoadEtcCoolItemWindow*)p)->m_entries[index + 0x460].m_expireTime =
            (*cool)[i].m_expireTime;
        ++index;
    }
    index = 0;
    const std::vector<ContinuousItemInfo>* effect = GetCurCharacEffectItemListR();
    p->m_effectItemCount = (unsigned char)effect->size();
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        ((LoadEtcEffectItemWindow*)p)->m_entries[index + 0x494].m_itemIdx =
            (*effect)[i].m_itemIdx;
        ((LoadEtcEffectItemWindow*)p)->m_entries[index + 0x494].m_expireTime =
            (*effect)[i].m_expireTime;
        ++index;
    }
}

const std::vector<ContinuousItemInfo>* CUserCharacInfo::GetCurCharacEffectItemListR() const
{
    return &CUR->m_effectItemList.m_list;
}

const std::vector<ContinuousItemInfo>* CUserCharacInfo::GetCurCharacCoolTimeItemListR() const
{
    return &CUR->m_coolTimeItemList.m_list;
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
    // P63 正极性包裹：否定早返 `if(!sel||cp<=0)return` 使 4.4 为两条短路测试
    // 各发空 else 桩（nop;jmp 中继 ×2，+4 条）；正极 `if(sel&&cp>0){主流程}`
    // 时 je/jle 直达尾 epilogue，与 ORIG 0x08645e5e 一致。
    if (m_selected && CUR->m_chaosPoint > 0)
    {
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
}

// [b28d-L14 N107+控制流] ORIG 0x08680e04：`lea 0xdaa(%edx),%ecx` vs 4.4 `add $0xdaa,%eax`
// （N107 成员常量偏移取址六形全败）；否定早返布局（P59/P222）。p167 CLEAR（6/6）。
// [b28d-solo-CUserCharacInfo AE] 85=85：①守卫独立早返（P59）已落；②CCargo/CInventory::
// FindItemLock bool 化（Main 代落）；③N107 两处 add-vs-lea 由 P106 真实类型成员取址
// 归零 —— m_cargo 由 char[0x14] 改真实 CCargo（CUserCharacInfo.h），m_inven 经 pack(1)
// CharacInvenView 真实 CInventory 成员访问 ⇒ `lea 0xdaa(%edx),%ecx` / `lea 0xda(%edx),%ecx`
// 逐条复现（数组退化形恒产 add）。
bool CUserCharacInfo::FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot,
                                   CUser* user) const
{
    if (user == 0)
        return false;
    if (m_selected == 0)
        return false;
    if (((const CCargo&)CUR->m_cargo).FindItemLock(b, space, slot))
        return true;
    if (((const CharacInvenView*)CUR)->m_inven.FindItemLock(b, space, slot))
        return true;
    CExpandEquipslot* expand = user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    if (expand->FindItemLock(b, space, slot))
        return true;
    if (((CTitleBook*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14))
            ->findItemLock(b, space, slot))
        return true;
    return false;
}

// [b28d-L14 N18a/N95] ORIG 0x08659c50：增量段 ppEi ⇒ 源写 `it++`（P28，已改）；
// 残差 `mov (%eax),%eax; mov %eax,%ebx` 4.4 折单段 + 槽 -0x28 vs -0x24。
// [b28d-solo-CUserCharacInfo AE] 95=95：①`return 0;`→`break;`（P214② 共享尾块）已落；
// ②P254 enum 视角：ContinuousItemInfo 加匿名 union 成员 EContinuousItemIdx
// m_itemIdxEnum（4 字节等宽，原 m_itemIdx int 全仓消费点不变）⇒ 复现 ORIG
// `mov (%eax),%eax; mov %eax,%ebx` 两段中转。
bool CUserCharacInfo::EnablePeriodExpItem()
{
    std::vector<ContinuousItemInfo>* v = &CUR->m_effectItemList.m_list;
    std::vector<ContinuousItemInfo>::iterator it = v->begin();
    unsigned int now = GlobalData::s_systemTime_.getCurSec();
    for (; it != v->end();)
    {
        // P254 enum 视角：复现 ORIG `mov (%eax),%eax; mov %eax,%ebx` 两段中转。
        CItem* item = G_CDataManager()->find_item(it->m_itemIdxEnum);
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
                break;
            }
        }
        // [b28d-L14 实证] ORIG 0x08659cf0 增量段调 ppEi（后置）⇒ 源写 it++（P28）。
        it++;
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
    std::vector<ContinuousItemInfo>* v = &CUR->m_effectItemList.m_list;
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




// [b28d-solo-CUserCharacInfo AE] 三旋钮一次归零（108=108）：
// ①P0 删具名 `CUserCharacInfo* other` 局部（ORIG 0x0864633a/0x0864634d 两次重读
//   0x10(%ebp) 形参直用）；
// ②P198②/P225 两处 bool 条件整体内联（ORIG 直测 `test %al,%al; je`，无 mov$1/mov$0 物化）；
// ③P252 具名 `float penalty` 局部（ORIG 0x086462f0 `fstps -0x10(%ebp)` + 回读压栈；
//   删 other 局部后槽位自洽，旧「复测反劣」系 other 占槽所致）。
void CUserCharacInfo::DieCurCharac(int responTime, CUser* user)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_ghost = 1;
    CUR->m_chaosStateTime = GlobalData::s_systemTime_.getCurSec() - CUR->m_chaosDieTime;
    CUR->m_chaosDieTime = GlobalData::s_systemTime_.getCurSec() + responTime;
    int ghostTime = 0;
    // [b28d-solo-CUserCharacInfo P198②/P225] ORIG 0x086462b0 直测
    // `test %al,%al; je`（无 bool 物化）⇒ 条件整体内联，勿用具名/register bool。
    if (GlobalData::s_power_manager->IsPowerWarEventOn() ||
        getCurCharacVill() == 7)
    {
        // [b28d-solo-CUserCharacInfo P252] 具名 float penalty 局部复现 ORIG
        // 0x086462f0 `fstps -0x10(%ebp)` + 回读压栈（删 other 局部后槽位自洽）。
        float penalty =
            GlobalData::s_power_manager->GetPowerWarResponPenalty(user);
        ghostTime =
            GlobalData::s_power_manager->GetPowerWarGhostTime(user, penalty);
    }
    else
    {
        // [b28d-solo-CUserCharacInfo P0] 删具名 other 局部：ORIG 0x0864633a/0x0864634d
        // 两次重读 0x10(%ebp)（形参直用），命名局部多槽 + 2 insn。
        ghostTime = ((ServerParameterScript*)((char*)G_CDataManager() +
                                              offsetof(CDataManager, m_serverParameter)))
                        ->getAssaultGhostTime(((CUserCharacInfo*)user)->get_charac_level(),
                                              ((CUserCharacInfo*)user)->GetCurCharacChaosPoint());
    }
    // [b28d-solo-CUserCharacInfo P198②] 同上：ORIG 直测 `test %al,%al; je`。
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

signed char CUserCharacInfo::getCurCharacVill() const
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


bool CUserCharacInfo::IsCurCharacGhost()
{
    // ORIG：true 路单一 `movzbl 0xe66(%eax),%eax` 直返（uchar 返回 = load 即
    // 终值，无 int promote 二段 movzbl %al；bool 声明反产 test;je;mov$1 规范化）
    // ⇒ 返回类型 unsigned char（技巧 33，mangled 无返回类型）。
    if (m_selected)
        return CUR->m_ghost;
    return 0;
}

int CUserCharacInfo::GetCurCharacChaosPoint()
{
    return m_selected ? CUR->m_chaosPoint : 0;
}

void CUserCharacInfo::DecreaseWarPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x297, 5);
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


// ORIG `movzwl 0x121f(%edx),%edx;mov %edx,%ecx` 二段；4.4 折单步（N18a），语义等价。
// 同 IncreasePowerWarPoint。
// [b28d-solo-CUserCharacInfo AE] 36=36：P254 packed enum 视角（EPowerWarPointValue /
// m_powerWarPointEnum）⇒ 减算段两段 regmove 复现。
void CUserCharacInfo::DecreasePowerWarPoint(short point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    // P254 packed enum 视角（同 IncreasePowerWarPoint）。
    CUR->m_powerWarPointEnum =
        (EPowerWarPointValue)((int)CUR->m_powerWarPointEnum - point);
    if ((short)CUR->m_powerWarPoint < 0)
        CUR->m_powerWarPoint = 0;
    }
}


// ORIG 0x08656784：`movzwl 0x4f,%edx;mov %edx,%ecx` 二段（4.4 折单步 N18a）；第二条件
// `mfat>old` ORIG 直 `cmp -0xa(%ebp),%ax;jle` 折返（跳返回侧），4.4 先物化 old 到 %eax 再
// `cmp %ax,%dx`，jle 落点/相位同 ORIG，无一分支反向，语义等价。
// [b28d-solo-CUserCharacInfo AE] 45=45：P254 packed enum 视角（m_fatigueBatteryEnum）。
void CUserCharacInfo::DecFatigueBatteryCharging(short value)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        // P209 翻案：old 必须声明为 **signed short**——unsigned short 局部使 4.4
        // 恒物化 `movzwl 槽,%eax; cmp %ax,%dx` 三段；signed 局部出 ORIG 直
        // `cmp -0xa(%ebp),%ax` 内存比较（探针 /tmp/bl10/p5.cpp h1/h2 对照）。
        short old = (short)CUR->m_fatigueBattery;
        // P254 packed enum 视角（同 IncFatigueBatteryCharging）。
        CUR->m_fatigueBatteryEnum =
            (EFatigueBatteryValue)((int)CUR->m_fatigueBatteryEnum - value);
        if ((short)CUR->m_fatigueBattery < 0 || (short)CUR->m_fatigueBattery > old)
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
    std::vector<ContinuousItemInfo>* v = &CUR->m_effectItemList.m_list;
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
    return &CUR->m_effectItemList.m_list;
}

void CUserCharacInfo::ClearUselessCooltimeItem(unsigned int expireTime)
{
    std::vector<ContinuousItemInfo>* v = &CUR->m_coolTimeItemList.m_list;
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
    return &CUR->m_coolTimeItemList.m_list;
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x24d, 5);
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
