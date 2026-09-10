// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserCharacInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CUserCharacInfo/dtor_CUserCharacInfo.inc"
{
    // 空体：隐式成员逆序析构（m_dungeonClear→m_battleRecord→m_pvpResult =
    // ORIG +0xb4→+0x9c→+0x18 调用序）。显式伪析构调用使 4.4 每调用点再布
    // cleanup 链（+22 条 vs ORIG 43）。
}

#include "CUserCharacInfo/useWinPoint.inc"
{
    enableSavePvP();
    if (m_pvpResult.m_winPoint >= point)
        m_pvpResult.m_winPoint -= point;
    else
        m_pvpResult.m_winPoint = 0;
}

#include "CUserCharacInfo/updateLastItemLimitEditionBuyTime.inc"
{
    if (m_selected)
    {
    CUR->m_lastItemLimitEditionBuyTime = t;
    }
}


#include "CUserCharacInfo/subtractCurCharacExp.inc"
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_exp -= exp;
    }
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/subUltimateInoutValue.inc"
{
    CUR->m_saveBloodInout = 1;
    --CUR->m_ultimateInout[index];
}

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/subDemensionInoutValue.inc"
{
    if (index > 5 || index < 0)
        return;
    CUR->m_saveDemensionInout = 1;
    --CUR->m_demensionInout[index];
}

#include "CUserCharacInfo/set_uppermember_exp_level.inc"
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


#include "CUserCharacInfo/set_member_pay_tex_money_to_upper.inc"
{
    if (m_selected)
    {
    CUR->m_memberPayTexMoney = money;
    }
}


#include "CUserCharacInfo/set_member_pay_tex_fatigue_to_upper.inc"
{
    if (m_selected)
    {
    CUR->m_memberPayTexFatigue = fatigue;
    }
}


#include "CUserCharacInfo/set_level_before_dungeon.inc"
{
    if (m_selected)
    {
    CUR->m_levelBeforeDungeon = level;
    }
}


#include "CUserCharacInfo/set_guild_exp.inc"
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildExp = exp;
}


#include "CUserCharacInfo/set_guild_create_right.inc"
{
    if (m_selected)
    {
    enableSaveCharacEtc();
    CUR->m_guildCreateRight = right;
    }
}


#include "CUserCharacInfo/set_growth_power_reward.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_growthPowerReward = (unsigned char)reward;
    }
}


#include "CUserCharacInfo/set_growth_power_exp_reward_right.inc"
{
    if (m_selected)
    {
    CUR->m_growthPowerExpRewardRight = flag;
    }
}


#include "CUserCharacInfo/set_connect_upper_member.inc"
{
    if (m_selected)
    {
    CUR->m_connectUpperMember = member;
    }
}


#include "CUserCharacInfo/set_charac_visible_values.inc"
{
    if (m_selected)
    {
    CUR->m_visibleValues = (unsigned char)values;
    }
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/set_charac_visible_teleport_.inc"
{
    if (m_selected)
    {
        CUR->m_visibleBits.teleport = flag;
    }
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/set_charac_visible_grow_avatar.inc"
{
    if (m_selected)
    {
        CUR->m_visibleBits.growAvatar = flag;
    }
}


#include "CUserCharacInfo/set_charac_seria_buf_bonus_exp.inc"
{
    if (m_selected)
    {
    CUR->m_seriaBufBonusExp = exp;
    }
}


#include "CUserCharacInfo/set_charac_party_bonus_exp.inc"
{
    if (m_selected)
    {
    CUR->m_partyBonusExp = exp;
    }
}


#include "CUserCharacInfo/set_charac_memberkey.inc"
{
    if (m_selected)
    {
    CUR->m_memberkey = key;
    }
}


#include "CUserCharacInfo/set_charac_member_bonus_exp.inc"
{
    if (m_selected)
    {
    CUR->m_memberBonusExp = exp;
    }
}


#include "CUserCharacInfo/set_charac_guildkey.inc"
{
    if (m_selected)
    {
    CUR->m_guildkey = key;
    }
}


#include "CUserCharacInfo/set_charac_fatigue_buf_bonus_exp.inc"
{
    if (m_selected)
    {
    CUR->m_fatigueBufBonusExp = exp;
    }
}


#include "CUserCharacInfo/set_charac_createtime.inc"
{
    CUR->m_createTime.SetCreateTime(t);
}

#include "CUserCharacInfo/set_aura_avatar_option_value.inc"
{
    if (m_selected && index >= 0 && index <= 2)
        CUR->m_auraAvatarOptionArr[index] = value;
}


#include "CUserCharacInfo/set_aura_avatar_option.inc"
{
    if (m_selected)
        CUR->m_auraAvatarOption = option;
}



#include "CUserCharacInfo/setWeekendBonusFatigue.inc"
{
    if (m_selected)
    {
    CUR->m_maxFatigue = (unsigned short)(DEFAULT_MAX_FATIGUE + value);
    }
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/setWeekendBonusExp.inc"
{
    if (m_selected)
    {
        CUR->m_weekendBonusExp = (int)((double)exp * (percent / 100.0f));
    }
}


#include "CUserCharacInfo/setUserEventCharacterFlag.inc"

#include "CUserCharacInfo/setUpgradeSeparateTrueFlag.inc"
{
    if (m_selected)
    {
    CUR->m_upgradeSeparateTrue = (unsigned char)flag;
    }
}


#include "CUserCharacInfo/setUpgradeSeparateFalseFlag.inc"
{
    if (m_selected)
    {
    CUR->m_upgradeSeparateFalse = (unsigned char)flag;
    }
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/setUltimateInoutValue.inc"
{
    CUR->m_saveBloodInout = 1;
    CUR->m_ultimateInout[index] = (unsigned char)value;
}

#include "CUserCharacInfo/setTagCharac.inc"

#include "CUserCharacInfo/setStraightVictories.inc"
{
    if (m_selected)
    {
    CUR->m_straightVictories = count;
    }
}


#include "CUserCharacInfo/setSeriaRoomAniDecoInfo.inc"
{
    if (m_selected)
        CUR->m_seriaRoomAniDeco = info;
}



#include "CUserCharacInfo/setSaveCharacOption.inc"
{
    m_saveCharacOption = flag;
}

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/setPremiumBonusExp.inc"
{
    if (m_selected)
    {
        CUR->m_premiumBonusExp = (int)(long long)((double)exp * (percent / 100.0f));
        return CUR->m_premiumBonusExp;
    }
    return 0;
}


#include "CUserCharacInfo/setPowerWarHP.inc"
{
    if (m_selected)
    {
    CUR->m_powerWarHP = hp;
    }
}


#include "CUserCharacInfo/setMemberDungeonFlag.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_memberDungeonFlag = flag;
    }
}


#include "CUserCharacInfo/setLinkCharacSlotIndex.inc"
{
    if (m_selected)
    {
    CUR->m_linkCharacSlotIndex = index;
    }
}


#include "CUserCharacInfo/setGuildMemberGrade.inc"
{
    if (m_selected)
    {
    CUR->m_guildMemberGrade = grade;
    }
}


#include "CUserCharacInfo/setGuildCreateFlag.inc"
{
    if (m_selected)
    {
    CUR->m_guildCreateFlag = (unsigned short)(CUR->m_guildCreateFlag | flag);
    }
}


// ORIG W 0x0822f8f6：m_selected 非空时返回其 +0xee9 字节，否则 0。
// 返回 bool：调用点 0x0821b4d8 `xor $0x1,%eax;test %al,%al`（uchar/int 声明不出 xor，探针实证）。
#include "CUserCharacInfo/isDisguiseCharac.inc"
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
#include "CUserCharacInfo/setDisguiseCharac.inc"
                                        unsigned short index)
{
    if (m_selected)
    {
    CUR->m_isDisguiseCharac = flag;
    setDisguiseKind(kind);
    setDisguiseIndex(index);
    }
}


#include "CUserCharacInfo/setDisguiseKind.inc"
{
    if (m_selected)
    {
    CUR->m_disguiseKind = kind;
    }
}


#include "CUserCharacInfo/setDisguiseIndex.inc"
{
    if (m_selected)
    {
    CUR->m_disguiseIndex = index;
    }
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/setDemensionInoutValue.inc"
{
    if (index > 5 || index < 0)
        return;
    CUR->m_saveDemensionInout = 1;
    CUR->m_demensionInout[index] = (unsigned char)value;
}

#include "CUserCharacInfo/setCurrCharacJoinPower.inc"
{
    enableSaveCharacStat();
    if (m_selected)
        CUR->m_joinPower = (unsigned char)power;
}




#include "CUserCharacInfo/setCurrCharacBlackCount.inc"
{
    if (m_selected)
    {
    CUR->m_blackCount = count;
    }
}


#include "CUserCharacInfo/setCurUseBringUpMaterialTotal.inc"
{
    if (m_selected)
    {
    CUR->m_curUseBringUpMaterialTotal = total;
    }
}


#include "CUserCharacInfo/setCurEquipslotFlag.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_curEquipslotFlag = flag;
    }
}


#include "CUserCharacInfo/setCurCharacVill.inc"
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
#include "CUserCharacInfo/setCurCharacTutorialFlag.inc"
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

#include "CUserCharacInfo/setCurCharacTradeGoldDaily.inc"
{
    enableSaveCharacStat();
    CUR->m_reliablePerson.SetTradeGoldDaily(gold);
}

#include "CUserCharacInfo/setCurCharacTagCharacNo.inc"
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    CUR->m_tagCharacNo = no;
    }
}


#include "CUserCharacInfo/setCurCharacStrikerSkillIndex.inc"
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    CUR->m_strikerSkillIndex = index;
    }
}


#include "CUserCharacInfo/setCurCharacStamina.inc"
{
    enableSaveCharacStat();
    CUR->m_stamina = value;
}



#include "CUserCharacInfo/setCurCharacPremiumFatigue.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_premiumFatigue = value;
    }
}


#include "CUserCharacInfo/setCurCharacMaxPremiumFatigue.inc"
{
    if (m_selected)
    {
    enableSaveCharacEtc();
    CUR->m_maxPremiumFatigue = value;
    }
}


#include "CUserCharacInfo/setCurCharacLevel.inc"
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    CUR->m_level = (unsigned short)level;
    }
}


#include "CUserCharacInfo/setCurCharacLastPlayTickPowerWar.inc"
{
    enableSaveCharacStat();
    CUR->m_lastPlayTickPowerWar = tick;
}



#include "CUserCharacInfo/setCurCharacLastPlayTick.inc"
{
    enableSaveCharacStat();
    CUR->m_lastPlayTick = tick;
}



#include "CUserCharacInfo/setCurCharacLastPlayDungeonIndex.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_lastPlayDungeonIndex = index;
    }
}


#include "CUserCharacInfo/setCurCharacHelpAbuseComputedRatio.inc"
{
    if (m_selected)
    {
    CUR->m_helpAbuseComputedRatio = ratio;
    }
}


#include "CUserCharacInfo/setCurCharacGuildSecede.inc"
{
    CUR->m_guildSecede = (unsigned char)flag;
}

#include "CUserCharacInfo/setCurCharacFatigueGrownUpBuff.inc"
{
    if (m_selected)
    {
        enableSaveCharacStat();
        CUR->m_fatigueGrownUpBuff = value;
    }
}




#include "CUserCharacInfo/setCurCharacFatigue.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_fatigue = (unsigned short)fatigue;
    }
}


#include "CUserCharacInfo/setCurCharacExp.inc"
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_exp = exp;
    }
}


#include "CUserCharacInfo/setCurCharacEventCharacterGrowtype.inc"
{
    enableSaveCharacEtc();
    CUR->m_eventCharacterGrowtype = (unsigned char)growtype;
}



#include "CUserCharacInfo/setCurCharacBackupStamina.inc"
{
    if (m_selected)
    {
    CUR->m_backupStamina = value;
    }
}


#include "CUserCharacInfo/setCurCharac.inc"
{
    enableSaveCharacInfo();
    enableSaveCharacStat();
    enableSaveCharacEtc();
    m_selected = p;
}

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
// （身位字节 shl/sar/and/or 逻辑逐条全等）
#include "CUserCharacInfo/setCurCharGrowthType.inc"
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
#include "CUserCharacInfo/setCurCharChangingGrowthType.inc"
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    CUR->m_growTypeChanging.first = first;
    CUR->m_growTypeChanging.second = second;
}



#include "CUserCharacInfo/setCurChannelEquipslotSwitch.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_curChannelEquipslotSwitch = (unsigned char)flag;
    }
}


#include "CUserCharacInfo/setConctractOfCubePremiumInfo.inc"
{
    if (!m_selected)
        return false;
    CUR->m_cubePremiumSaved = 1;
    CUR->m_cubePremiumContract = active;
    CUR->m_cubePremiumCount = count;
    return true;
}

#include "CUserCharacInfo/setCharacViewState.inc"
{
    if (m_selected)
    {
    CUR->m_characViewState = state;
    }
}


#include "CUserCharacInfo/setCharacOption.inc"
{
    if (m_selected)
        CUR->m_characterOption = option;
}




#include "CUserCharacInfo/setCharacLinkBonusGold.inc"
{
    if (m_selected)
    {
    CUR->m_characLinkBonusGold = gold;
    }
}


#include "CUserCharacInfo/setCharacLinkBonusExp.inc"
{
    if (m_selected)
    {
    CUR->m_characLinkBonusExp = exp;
    }
}


#include "CUserCharacInfo/setBringUpRewardFlag.inc"
{
    if (m_selected)
    {
    CUR->m_bringUpRewardFlag = flag;
    }
}


#include "CUserCharacInfo/setBlueMarbleEnterCount.inc"
{
    if (m_selected)
    {
    CUR->m_blueMarbleEnterCount = count;
    }
}


#include "CUserCharacInfo/setBloodBestRecord.inc"
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


#include "CUserCharacInfo/setBloodBestRecordUpdate.inc"
{
    if (m_selected)
    {
    CUR->m_bloodBestRecordUpdate = flag;
    }
}


#include "CUserCharacInfo/reset_level_before_dungeon.inc"
{
    if (m_selected)
    {
    CUR->m_levelBeforeDungeon = 0;
    }
}


#include "CUserCharacInfo/reset_is_end_lethe_at_login.inc"
{
    if (m_selected)
    {
    CUR->m_isEndLetheAtLogin = 0;
    }
}


#include "CUserCharacInfo/reset_guild_today_exp.inc"
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildTodayExp = 0;
}


#include "CUserCharacInfo/reset_guild_temp_exp.inc"
{
    if (m_selected)
    {
    CUR->m_guildTempExp = 0;
    }
}


#include "CUserCharacInfo/reset_guild_exp.inc"
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildExp = 0;
}


#include "CUserCharacInfo/resetWeekendBonusExp.inc"
{
    if (m_selected)
    {
    CUR->m_weekendBonusExp = 0;
    }
}


#include "CUserCharacInfo/resetTempGuildNameAddr.inc"
{
    if (m_selected)
    {
    memset(CUR->m_guildName, 0, sizeof(CUR->m_guildName));
    memset(CUR->m_guildAddr, 0, sizeof(CUR->m_guildAddr));
    }
}


#include "CUserCharacInfo/resetStraightVictories.inc"
{
    if (m_selected)
    {
    CUR->m_straightVictories = 0;
    }
}


#include "CUserCharacInfo/resetPremiumBonusExp.inc"
{
    if (m_selected)
    {
    CUR->m_premiumBonusExp = 0;
    }
}


#include "CUserCharacInfo/resetGuildCreateFlag.inc"
{
    if (m_selected)
    {
    CUR->m_guildCreateFlag = 0;
    }
}


#include "CUserCharacInfo/resetCurUseBringUpMaterialCount.inc"
{
    if (m_selected)
    {
    CUR->m_curUseBringUpMaterialCount = 0;
    }
}


#include "CUserCharacInfo/resetCurCharacUsedFatigue.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_usedFatigue = 0;
    }
}


#include "CUserCharacInfo/resetCurCharacSchoolPoint.inc"
{
    if (m_selected)
    {
    CUR->m_reliablePerson.m_schoolPoint = 0;
    }
}


#include "CUserCharacInfo/resetCurCharacOverEquipFlag.inc"
{
    if (!m_selected)
        return;
    CUR->m_overEquipFlag = 0;
}



#include "CUserCharacInfo/resetCurCharacMemberBonusFatigue.inc"
{
    enableSaveCharacStat();
    CUR->m_memberBonusFatigue = 0;
}




#include "CUserCharacInfo/resetCharacTutorialFlag.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_tutorialFlags = 0;
    memset(CUR->m_tutorialFlags2, 0, 0x10);
    }
}


#include "CUserCharacInfo/resetCharacFatigueGrownUpBuff.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_fatigueGrownUpBuff = 0;
    }
}


#include "CUserCharacInfo/resetCharGuildInfo.inc"
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


#include "CUserCharacInfo/resetBlueMarbleUserDeadCount.inc"
{
    if (m_selected)
    {
    CUR->m_blueMarbleUserDeadCount = 0;
    }
}


#include "CUserCharacInfo/resetBlueMarbleEnterCount.inc"
{
    if (m_selected)
    {
    CUR->m_blueMarbleEnterCount = 0;
    }
}


#include "CUserCharacInfo/needCurCharacCheckOverEquip.inc"
{
    if (!m_selected)
        return 0;
    return (unsigned char)CUR->m_overEquipFlag;
}


// [b28d-Main·检查点14] ORIG 0x08696ca4：13 条，`movzbl 0x127b(%eax),%eax; jmp`
// 直返无 setne ⇒ 字段/返回同型 bool；调用点 0x0868dbfb `xor $1` 联判。
// 字段已由 Main 在 CUserCharacInfo.h 正式改 bool，TU 局部视图已删除（直读）。
#include "CUserCharacInfo/is_growth_power_exp_reward_right.inc"
{
    return m_selected ? CUR->m_growthPowerExpRewardRight : 0;
}

#include "CUserCharacInfo/is_connect_upper_member.inc"
{
    if (m_selected && CUR->m_connectUpperMember != 0)
        return true;
    return false;
}




#include "CUserCharacInfo/isSetGuildCreateFlag.inc"
{
    if (m_selected)
    {
        if ((CUR->m_guildCreateFlag & flag) == flag)
            return true;
        return false;
    }
    return false;
}



#include "CUserCharacInfo/isSavedPremiumCubeInfo.inc"
{
    if (!m_selected)
        return 0;
    return CUR->m_cubePremiumSaved;
}


// [b28c-L26 档①] 返回类型随头改 bool（Main 批准，ORIG 体 movzbl 直返实证）。
#include "CUserCharacInfo/isSaveSkill.inc"

#include "CUserCharacInfo/isSaveQuest.inc"

#include "CUserCharacInfo/isSavePvP.inc"

#include "CUserCharacInfo/isSaveInven.inc"

#include "CUserCharacInfo/isSaveDungeonClear.inc"

#include "CUserCharacInfo/isSaveCharacOption.inc"

#include "CUserCharacInfo/isSaveCharacInfo.inc"

#include "CUserCharacInfo/isSaveCharacEtc.inc"

#include "CUserCharacInfo/isSaveCargo.inc"

#include "CUserCharacInfo/isSaveBattleRecord.inc"

#include "CUserCharacInfo/isJoinPowerWar.inc"
{
    return m_selected ? CUR->m_joinPower : 0;
}

#include "CUserCharacInfo/getPowerWarHP.inc"
{
    return m_selected ? CUR->m_powerWarHP : 0;
}

#include "CUserCharacInfo/isExistItem.inc"
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



#include "CUserCharacInfo/isBloodBestRecordUpdate.inc"
{
    return CUR->m_bloodBestRecordUpdate;
}

#include "CUserCharacInfo/initContractOfCubePremiumInfo.inc"
{
    if (!m_selected)
        return false;
    CUR->m_cubePremiumSaved = 0;
    CUR->m_cubePremiumContract = active;
    CUR->m_cubePremiumCount = count;
    return true;
}

#include "CUserCharacInfo/increaseBlueMarbleUserDeadCount.inc"
{
    if (m_selected)
    {
    ++CUR->m_blueMarbleUserDeadCount;
    }
}


#include "CUserCharacInfo/increaseBlueMarbleEnterCount.inc"
{
    if (m_selected)
    {
    ++CUR->m_blueMarbleEnterCount;
    }
}


#include "CUserCharacInfo/incStraightVictories.inc"
{
    if (m_selected)
    {
    ++CUR->m_straightVictories;
    }
}


#include "CUserCharacInfo/incCurUseBringUpMaterialCount.inc"
{
    if (m_selected)
    {
    ++CUR->m_curUseBringUpMaterialCount;
    ++CUR->m_curUseBringUpMaterialTotal;
    }
}


#include "CUserCharacInfo/incCurCharacUsedFatigue.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_usedFatigue = (unsigned short)(CUR->m_usedFatigue + value);
    }
}


#include "CUserCharacInfo/incCurCharacMemberBonusFatigue.inc"
{
    enableSaveCharacStat();
    CUR->m_memberBonusFatigue =
        (unsigned short)(CUR->m_memberBonusFatigue + value);
}



#include "CUserCharacInfo/incCurCharacLevel.inc"
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    ++CUR->m_level;
    }
}


#include "CUserCharacInfo/get_uppermember_exp_level.inc"
{
    return m_selected ? CUR->m_uppermemberExpLevel : 0;
}

#include "CUserCharacInfo/get_pvp_result_oldexp_point.inc"

#include "CUserCharacInfo/get_pvp_result_exp_point.inc"

#include "CUserCharacInfo/get_pvp_grade.inc"

#include "CUserCharacInfo/get_member_pay_tex_money_to_upper.inc"
{
    return m_selected ? CUR->m_memberPayTexMoney : 0;
}

#include "CUserCharacInfo/get_member_pay_tex_fatigue_to_upper.inc"
{
    return m_selected ? CUR->m_memberPayTexFatigue : 0;
}

#include "CUserCharacInfo/get_guild_today_exp.inc"
{
    return m_selected ? CUR->m_guildTodayExp : 0;
}

#include "CUserCharacInfo/get_guild_temp_exp.inc"
{
    return m_selected ? CUR->m_guildTempExp : 0;
}

#include "CUserCharacInfo/get_guild_exp.inc"
{
    return m_selected ? CUR->m_guildExp : 0;
}

#include "CUserCharacInfo/get_guild_create_right.inc"
{
    return m_selected ? CUR->m_guildCreateRight : 0;
}

#include "CUserCharacInfo/get_growth_power_reward.inc"
{
    return m_selected ? CUR->m_growthPowerReward : 0;
}

#include "CUserCharacInfo/get_connect_upper_member.inc"
{
    return m_selected ? CUR->m_connectUpperMember : 0;
}

#include "CUserCharacInfo/get_charac_visible_values.inc"
{
    return m_selected ? CUR->m_visibleValues : 0;
}

#include "CUserCharacInfo/get_charac_visible_grow_avatar.inc"
{
    if (m_selected && CUR->m_visibleBits.growAvatar)
        return 1;
    return 0;
}


#include "CUserCharacInfo/get_charac_seria_buf_bonus_exp.inc"
{
    if (m_selected)
    {
        int value = CUR->m_seriaBufBonusExp;
        CUR->m_seriaBufBonusExp = 0;
        return value;
    }
    return 0;
}


#include "CUserCharacInfo/get_charac_party_bonus_exp.inc"
{
    return m_selected ? CUR->m_partyBonusExp : 0;
}

#include "CUserCharacInfo/get_charac_memberkey.inc"
{
    return m_selected ? CUR->m_memberkey : 0;
}

#include "CUserCharacInfo/get_charac_member_bonus_exp.inc"
{
    return m_selected ? CUR->m_memberBonusExp : 0;
}

#include "CUserCharacInfo/get_charac_level_before_dungeon.inc"
{
    if (m_selected)
    {
        if (CUR->m_levelBeforeDungeon != 0)
            return CUR->m_levelBeforeDungeon;
        return CUR->m_level;
    }
    return 0;
}


#include "CUserCharacInfo/get_charac_job.inc"
{
    return m_selected ? (signed char)CUR->m_job : 0;
}

#include "CUserCharacInfo/get_charac_guildkey.inc"
{
    return m_selected ? CUR->m_guildkey : 0;
}

#include "CUserCharacInfo/get_charac_fatigue_buf_bonus_exp.inc"
{
    return m_selected ? CUR->m_fatigueBufBonusExp : 0;
}

#include "CUserCharacInfo/get_charac_exp.inc"
{
    return m_selected ? CUR->m_exp : 0;
}

#include "CUserCharacInfo/get_charac_10_level_section.inc"
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

#include "CUserCharacInfo/getWeekendBonusExp.inc"
{
    return m_selected ? CUR->m_weekendBonusExp : 0;
}

#include "CUserCharacInfo/getUserEventCharacterFlag.inc"

#include "CUserCharacInfo/getUpgradeSeparateTrueFlag.inc"
{
    return m_selected ? (char)CUR->m_upgradeSeparateTrue : 0;
}

#include "CUserCharacInfo/getUpgradeSeparateFlaseFlag.inc"
{
    return m_selected ? (char)CUR->m_upgradeSeparateFalse : 0;
}

#include "CUserCharacInfo/getUltimateInoutValue.inc"
{
    // [b28] 值 cast 伪差已消（m_selView 直读），本函数 AE。
    return CUR->m_ultimateInout[index];
}

#include "CUserCharacInfo/getTagCharacW.inc"

#include "CUserCharacInfo/getTagCharacSkillW.inc"
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (char*)((CharacInfoFields*)m_tag)->m_skill;
}

#include "CUserCharacInfo/getTagCharacSkillR.inc"
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (SkillSlot*)((CharacInfoFields*)m_tag)->m_skill;
}

#include "CUserCharacInfo/getTagCharacR.inc"

#include "CUserCharacInfo/getTagCharacInvenW.inc"
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (Inven_Item*)((CharacInfoFields*)m_tag)->m_inven;
}

#include "CUserCharacInfo/getTagCharacInvenR.inc"
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (CInventory*)((CharacInfoFields*)m_tag)->m_inven;
}

#include "CUserCharacInfo/getTagCharacCargoW.inc"
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return &((CharacInfoFields*)m_tag)->m_cargo;
}

#include "CUserCharacInfo/getTagCharacCargoR.inc"
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return &((CharacInfoFields*)m_tag)->m_cargo;
}

#include "CUserCharacInfo/getTagCharacAddInfoR.inc"
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (const void*)((CharacInfoFields*)m_tag)->m_addInfo;
}

#include "CUserCharacInfo/getStraightVictories.inc"
{
    return m_selected ? CUR->m_straightVictories : 0;
}

#include "CUserCharacInfo/getSkillCommand.inc"
{
    return m_selected ? (const char*)CUR->m_skillCommand : 0;
}

#include "CUserCharacInfo/getSeriaRoomAniDecoInfo.inc"
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
#include "CUserCharacInfo/getSeparatedTextUnits.inc"
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

#include "CUserCharacInfo/getRequestRenameState.inc"
{
    return m_selected ? CUR->m_characViewState : 0;
}

#include "CUserCharacInfo/getPremiumBonusExp.inc"
{
    return m_selected ? CUR->m_premiumBonusExp : 0;
}

#include "CUserCharacInfo/getPVPResultW.inc"
{
    enableSavePvP();
    return &m_pvpResult;
}

#include "CUserCharacInfo/getPVPResultRefW.inc"
{
    enableSavePvP();
    return &m_pvpResult;
}
#include "CUserCharacInfo/getPVPResultRefR.inc"
{
    return &m_pvpResult;
}

#include "CUserCharacInfo/getPVPResultR.inc"
{
    return const_cast<PvpResultType*>(&m_pvpResult);
}

#include "CUserCharacInfo/getMemberDungeonFlag.inc"
{
    return m_selected ? CUR->m_memberDungeonFlag : true;
}


#include "CUserCharacInfo/getLinkCharacSlotIndex.inc"
{
    return m_selected ? (int)(unsigned char)CUR->m_linkCharacSlotIndex : -1;
}


#include "CUserCharacInfo/getLastItemLimitEditionBuyTime.inc"
{
    return m_selected ? CUR->m_lastItemLimitEditionBuyTime : 0;
}

#include "CUserCharacInfo/getGuildMemberGrade.inc"
{
    return m_selected ? CUR->m_guildMemberGrade : 0;
}

#include "CUserCharacInfo/getDungeonClearRefW.inc"
{
    enableSaveDungeonClear();
    return &m_dungeonClear;
}

#include "CUserCharacInfo/getDungeonClearRefR.inc"
{
    return &m_dungeonClear;
}
#include "CUserCharacInfo/getDisguiseKind.inc"
{
    return m_selected ? CUR->m_disguiseKind : 0;
}

#include "CUserCharacInfo/getDisguiseIndex.inc"
{
    return m_selected ? CUR->m_disguiseIndex : 0;
}

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/getDemensionInoutValue.inc"
{
    if (index > 5 || index < 0)
        return 0;
    return CUR->m_demensionInout[index];
}

#include "CUserCharacInfo/getCurrCharacBlackCount.inc"
{
    return m_selected ? CUR->m_blackCount : 0;
}

#include "CUserCharacInfo/getCurUseBringUpMaterialTotal.inc"
{
    return m_selected ? CUR->m_curUseBringUpMaterialTotal : 0;
}

#include "CUserCharacInfo/getCurUseBringUpMaterialCount.inc"
{
    return m_selected ? CUR->m_curUseBringUpMaterialCount : 0;
}

#include "CUserCharacInfo/getCurExpandEquipslotSwitch.inc"
{
    return m_selected ? (int)(unsigned char)CUR->m_curExpandEquipslotSwitch : -1;
}


#include "CUserCharacInfo/getCurEquipslotFlag.inc"
{
    // ORIG 0x8696c23 true 路单一 `movzbl 0x1275(%eax),%eax`、false 0x8696c2c
    // `mov $0xffffffff,%eax`（dword -1）。int 返回三元/if 形均反产二段 promote
    // （IsCurCharacGhost 同款教训）→ uchar 返回 + if 形，false 保留 dword -1。
    if (m_selected)
        return CUR->m_curEquipslotFlag;
    return -1;
}


#include "CUserCharacInfo/getCurCharacW.inc"

#include "CUserCharacInfo/getCurCharacVillPrev.inc"
{
    return m_selected ? CUR->m_villPrev : 0;
}

#include "CUserCharacInfo/getCurCharacVill4Save.inc"
{
    return CUR->m_vill;
}

#include "CUserCharacInfo/getCurCharacTutorialFlag.inc"
{
    return m_selected ? CUR->m_tutorialFlags : 0;
}

#include "CUserCharacInfo/getCurCharacTradeGoldTotalBillion.inc"
{
    return m_selected ? CUR->m_reliablePerson.GetTradeGoldTotalBillion() : 0;
}

#include "CUserCharacInfo/getCurCharacTradeGoldTotal.inc"
{
    return m_selected ? CUR->m_reliablePerson.GetTradeGoldTotal() : 0;
}

#include "CUserCharacInfo/getCurCharacTradeGoldDaily.inc"
{
    return m_selected ? CUR->m_reliablePerson.GetTradeGoldDaily() : 0;
}

#include "CUserCharacInfo/getCurCharacTagCharacNo.inc"
{
    return m_selected ? CUR->m_tagCharacNo : 0;
}

#include "CUserCharacInfo/getCurCharacStrikerSkillIndex.inc"
{
    return m_selected ? CUR->m_strikerSkillIndex : 0;
}

#include "CUserCharacInfo/getCurCharacStamina.inc"
{
    return CUR->m_stamina;
}

#include "CUserCharacInfo/getCurCharacSkillW.inc"
{
    enableSaveSkill();
    return (char*)CUR->m_skill;
}

#include "CUserCharacInfo/getCurCharacSkillR.inc"
{
    return (const char*)CUR->m_skill;
}

#include "CUserCharacInfo/getCurCharacSchoolPoint.inc"
{
    return m_selected ? CUR->m_reliablePerson.m_schoolPoint : 0;
}

#include "CUserCharacInfo/getCurCharacR.inc"

#include "CUserCharacInfo/getCurCharacPremiumFatigue.inc"
{
    return m_selected ? CUR->m_premiumFatigue : 0;
}

#include "CUserCharacInfo/getCurCharacPayCoin.inc"
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->GetPayCoin();
    return 0;
}



#include "CUserCharacInfo/getCurCharacNo.inc"
{
    return m_selected ? CUR->m_characNo : 0;
}

#include "CUserCharacInfo/getCurCharacMoney.inc"
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->get_money();
    return 0;
}



#include "CUserCharacInfo/getCurCharacMemberBonusFatigue.inc"
{
    return CUR->m_memberBonusFatigue;
}

#include "CUserCharacInfo/getCurCharacMaxPremiumFatigue.inc"
{
    return m_selected ? CUR->m_maxPremiumFatigue : 0;
}

#include "CUserCharacInfo/getCurCharacMaxFatigue.inc"
{
    return m_selected ? CUR->m_maxFatigue : 0;
}

#include "CUserCharacInfo/getCurCharacLastPlayTickPowerWar.inc"
{
    return CUR->m_lastPlayTickPowerWar;
}

#include "CUserCharacInfo/getCurCharacLastPlayTick.inc"
{
    return CUR->m_lastPlayTick;
}

#include "CUserCharacInfo/getCurCharacLastPlayDungeonIndex.inc"
{
    return m_selected ? CUR->m_lastPlayDungeonIndex : 0;
}

#include "CUserCharacInfo/getCurCharacInvenW.inc"
{
    enableSaveInven();
    return (CInventory*)CUR->m_inven;
}

#include "CUserCharacInfo/getCurCharacInvenRefW.inc"
{
    enableSaveInven();
    return (CInventory*)CUR->m_inven;
}

#include "CUserCharacInfo/getCurCharacInvenRefR.inc"
{
    return (const CInventory*)CUR->m_inven;
}

#include "CUserCharacInfo/getCurCharacInvenR.inc"
{
    return (const CInventory*)CUR->m_inven;
}

#include "CUserCharacInfo/getCurCharacHelpAbuseComputedRatio.inc"
{
    return m_selected ? CUR->m_helpAbuseComputedRatio : 0;
}

#include "CUserCharacInfo/getCurCharacGuildSecede.inc"
{
    return m_selected ? CUR->m_guildSecede : 0;
}

#include "CUserCharacInfo/getCurCharacGrowType.inc"
{
    return m_selected ? CUR->m_growType.byte : 0;
}

#include "CUserCharacInfo/getCurCharacFatigueGrownUpBuff.inc"
{
    return m_selected ? CUR->m_fatigueGrownUpBuff : 0;
}

#include "CUserCharacInfo/getCurCharacFatigue.inc"
{
    return m_selected ? CUR->m_fatigue : 0;
}

#include "CUserCharacInfo/getCurCharacExp.inc"
{
    return m_selected ? CUR->m_exp : 0;
}

#include "CUserCharacInfo/getCurCharacEventCoin.inc"
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->GetEventCoin();
    return 0;
}



#include "CUserCharacInfo/getCurCharacEventCharacterGrowtype.inc"
{
    return m_selected ? CUR->m_eventCharacterGrowtype : 0;
}

// [b28d-solo-CUserCharacInfo AE] 69=69：
// ①N6 翻案：`1 << flag`（int 字面量）保 ORIG `shl`（探针 /tmp/s156_probe/p12.cpp f4）；
// ②P169 尾标签守卫 + 低位分支共享尾 return（消两处 `mov $0; jmp` 岛）；
// ③路由 `-march=i486 -mtune=generic`（Main 已批）+ `index / 8` 复现 ORIG 6 条修正链
//   （探针 /tmp/s156_probe/p13i486.o）。
#include "CUserCharacInfo/getCurCharacEscaladeTutorialFlag.inc"
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

#include "CUserCharacInfo/getCurCharacEscaladeTutorialFlag_1.inc"
{
    return m_selected ? (unsigned char*)CUR->m_tutorialFlags2 : 0;
}

#include "CUserCharacInfo/getCurCharacCreateTime.inc"
{
    return CUR->m_createTime.operator long();
}

#include "CUserCharacInfo/getCurCharacCoin.inc"
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->GetCoin();
    return 0;
}



#include "CUserCharacInfo/getCurCharacCargoW.inc"
{
    enableSaveCargo();
    return &CUR->m_cargo;
}

#include "CUserCharacInfo/getCurCharacCargoR.inc"
{
    return &CUR->m_cargo;
}

#include "CUserCharacInfo/getCurCharacAddInfoW.inc"
{
    enableSaveCharacInfo();
    return (void*)CUR->m_addInfo;
}

#include "CUserCharacInfo/getCurCharacAddInfoRefW.inc"
{
    enableSaveCharacInfo();
    return (void*)CUR->m_addInfo;
}

#include "CUserCharacInfo/getCurCharacAddInfoRefR.inc"
{
    return (const void*)CUR->m_addInfo;
}

#include "CUserCharacInfo/getCurCharacAddInfoR.inc"
{
    return (const void*)CUR->m_addInfo;
}

#include "CUserCharacInfo/getCurCharSecondGrowType.inc"
{
    return m_selected ? (char)CUR->m_growType.second : 0;
}

#include "CUserCharacInfo/getCurCharFirstGrowType.inc"
{
    return m_selected ? (char)CUR->m_growType.first : 0;
}

#include "CUserCharacInfo/getCurChannelEquipslotSwitch.inc"
{
    return m_selected ? (int)(unsigned char)CUR->m_curChannelEquipslotSwitch : -1;
}


#include "CUserCharacInfo/getCreateGuildName.inc"
{
    return m_selected ? (char*)CUR->m_guildName : 0;
}

#include "CUserCharacInfo/getCreateGuildAddr.inc"
{
    return m_selected ? (char*)CUR->m_guildAddr : 0;
}

#include "CUserCharacInfo/getContractOfCubePremiumInfo.inc"
{
    if (!m_selected)
        return false;
    *(&active) = CUR->m_cubePremiumContract != 0 ? 1 : 0;
    *(&count) = CUR->m_cubePremiumCount;
    return true;
}


#include "CUserCharacInfo/getCharacLinkType.inc"
{
    return m_selected ? (char)CUR->m_characLinkType : 0;
}

#include "CUserCharacInfo/getCharacLinkBonusGold.inc"
{
    return m_selected ? CUR->m_characLinkBonusGold : 0;
}

#include "CUserCharacInfo/getCharacLinkBonusExp.inc"
{
    return m_selected ? CUR->m_characLinkBonusExp : 0;
}

#include "CUserCharacInfo/getBringUpRewardFlag.inc"
{
    return m_selected ? CUR->m_bringUpRewardFlag : 0;
}

#include "CUserCharacInfo/getBlueMarbleUserDeadCount.inc"
{
    return m_selected ? CUR->m_blueMarbleUserDeadCount : 0;
}

#include "CUserCharacInfo/getBlueMarbleEnterCount.inc"
{
    return m_selected ? CUR->m_blueMarbleEnterCount : 0;
}

#include "CUserCharacInfo/getBloodBestTimeRecord.inc"
{
    return CUR->m_bloodBestTime;
}

#include "CUserCharacInfo/getBloodBestRoundRecord.inc"
{
    return CUR->m_bloodBestRound;
}

#include "CUserCharacInfo/getBattleRecordRefW.inc"
{
    enableSaveBattleRecord();
    return &m_battleRecord;
}

#include "CUserCharacInfo/getBattleRecordRefR.inc"
{
    return &m_battleRecord;
}

#include "CUserCharacInfo/enableSaveSkill.inc"

#include "CUserCharacInfo/enableSaveQuest.inc"

#include "CUserCharacInfo/enableSavePvP.inc"

#include "CUserCharacInfo/enableSaveInven.inc"

#include "CUserCharacInfo/enableSaveDungeonClear.inc"

#include "CUserCharacInfo/enableSaveCharacEtc.inc"

#include "CUserCharacInfo/enableSaveCargo.inc"

#include "CUserCharacInfo/enableSaveBattleRecord.inc"

#include "CUserCharacInfo/disableSavedPremiumCubeInfo.inc"
{
    if (!m_selected)
        return;
    CUR->m_cubePremiumSaved = 0;
}



#include "CUserCharacInfo/disableSaveSkill.inc"

#include "CUserCharacInfo/disableSaveQuest.inc"

#include "CUserCharacInfo/disableSavePvP.inc"

#include "CUserCharacInfo/disableSaveInven.inc"

#include "CUserCharacInfo/disableSaveDungeonClear.inc"

#include "CUserCharacInfo/disableSaveCharacStat.inc"

#include "CUserCharacInfo/disableSaveCharacManageInfo.inc"

#include "CUserCharacInfo/disableSaveCharacInfo.inc"

#include "CUserCharacInfo/disableSaveCharacEtc.inc"

#include "CUserCharacInfo/disableSaveCargo.inc"

#include "CUserCharacInfo/disableSaveBattleRecord.inc"

#include "CUserCharacInfo/decCurCharacLevel.inc"
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    --CUR->m_level;
    }
}


#include "CUserCharacInfo/decCurCharacFatigueGrownUpBuff.inc"
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


#include "CUserCharacInfo/checkBonusPoint.inc"
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



#include "CUserCharacInfo/resetBonusPoint.inc"
{
    if (m_selected)
    {
    memset(&CUR->m_wpBonus, 0, sizeof(CUR->m_wpBonus));
    memset(&CUR->m_expBonus, 0, sizeof(CUR->m_expBonus));
    }
}


#include "CUserCharacInfo/getWpBonusPoint.inc"
{
    if (m_selected)
        out = CUR->m_wpBonus;
}




#include "CUserCharacInfo/getExpBonusPoint.inc"
{
    if (m_selected)
        out = CUR->m_expBonus;
}




#include "CUserCharacInfo/get_level_up_exp.inc"
{
    return G_CDataManager()->get_level_exp(level + 1) - G_CDataManager()->get_level_exp(level);
}

#include "CUserCharacInfo/getCurCharacDungeonHelpAbuseRatio.inc"
{
    return m_selected ? CUR->m_dungeonHelpAbuseRatio : 0;
}

#include "CUserCharacInfo/getCurCharacDungeonHelpAbuseExp.inc"
{
    return m_selected ? CUR->m_dungeonHelpAbuseExp : 0;
}

// [S1Charac 2026-09-11 复核] 68/68、助记符逐条全等；真残差=匿名 8B 转换临时基准比 ORIG
// 低 8B（我方 -0x38/-0x34、cw -0x3a/-0x3c、除数槽 -0x4c；ORIG -0x30/-0x2c、-0x32/-0x34、
// -0x3c）连带帧 0x68↔0x58 ⇒ 4.4 聚合临时优先占深槽的分区排布（N95），无死局部可删。
// 本波 §6.5 串核查：本函数无 rodata 串引用（报告内那条字符串系解析噪声）⇒ CLEAR。
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
#include "CUserCharacInfo/calc_disp_help_abuse_ratio.inc"
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

#include "CUserCharacInfo/calcHelpAbuseRatioBeforeLevelUp.inc"
{
    char unused[0x20];
    (void)unused;
    (void)param;
    // [b28d-solo-CUserCharacInfo 复核] ORIG 该空体函数仍发射局部静态
    // _ZZN15CUserCharacInfo31calcHelpAbuseRatioBeforeLevelUpEiE19__PRETTY_FUNCTION__
    // （r 段 0x3b，全仓仅此一件 ORIG-only 数据符号；ORIG 体 0x7 条无引用）
    // ⇒ 原码有对该串的死引用，照抄（P171 死写入族；不产生指令）。
    (void)__PRETTY_FUNCTION__;
}


#include "CUserCharacInfo/calcHelpAbuseRatio.inc"
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
#include "CUserCharacInfo/calcCurCharacSchoolPoint.inc"
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


#include "CUserCharacInfo/getCurCharacUsedFatigue.inc"
{
    return m_selected ? CUR->m_usedFatigue : 0;
}

#include "CUserCharacInfo/add_member_pay_tex_money_to_upper.inc"
{
    if (m_selected)
    {
    CUR->m_memberPayTexMoney += money;
    }
}


#include "CUserCharacInfo/add_member_pay_tex_fatigue_to_upper.inc"
{
    if (m_selected)
    {
    CUR->m_memberPayTexFatigue += fatigue;
    }
}


// [b28d-solo-CUserCharacInfo AE] 21=21：P274 异型视角（字段 unsigned 视角
// m_guildTodayExpU + 形参 int）⇒ 复现 ORIG `mov 0xdef(%edx),%ecx; mov 0xc(%ebp),%edx;
// lea (%ecx,%edx,1),%edx`（旧判「字段 unsigned 无效」系未用 union 视角 + 字段在前写法）。
#include "CUserCharacInfo/add_guild_today_exp.inc"
{
    if (m_selected && CUR->m_guildkey != 0)
        // P274 异型视角（字段 unsigned + 形参 int）：复现 ORIG
        // `mov 0xdef(%edx),%ecx; mov 0xc(%ebp),%edx; lea (%ecx,%edx,1),%edx`。
        CUR->m_guildTodayExpU = CUR->m_guildTodayExpU + exp;
}


#include "CUserCharacInfo/add_guild_temp_exp.inc"
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildTempExp = (unsigned short)(CUR->m_guildTempExp + (short)exp);
}


// [b28d-solo-CUserCharacInfo AE] 21=21：P274 异型视角（m_guildExpU）复现 ORIG
// 0x08645cb1 `mov 0xde9(%edx),%ecx; mov 0xc(%ebp),%edx; lea (%ecx,%edx,1),%edx`。
#include "CUserCharacInfo/add_guild_exp.inc"
{
    if (m_selected && CUR->m_guildkey != 0)
        // P274 异型视角（同 add_guild_today_exp）。
        CUR->m_guildExpU = CUR->m_guildExpU + exp;
}


#include "CUserCharacInfo/addCurcharacStamina.inc"
{
    enableSaveCharacStat();
    CUR->m_stamina = (unsigned char)(CUR->m_stamina + value);
}


#include "CUserCharacInfo/addCurCharacExp.inc"
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_exp += exp;
    }
}


// [b28d-L14 N7/N4] ORIG 0x086459c4：残差=寄存器角色互换（%ecx↔%edx）+
// `lea 0x380(%reg)` 物化时刻（4.1 于 fadds 后 / 4.4 前置）。p167 CLEAR（语义完整）。
// [S1Charac 2026-09-11 复核] 59/59、p167 CLEAR。真残差 2 处仅寄存器角色：
// ORIG 0x08645a28 `mov 0xc(%ebp),%ecx` / 0x08645a3a `flds 0xe(%edx,%ebx,4)` /
// 0x08645a41 `lea 0x380(%ecx),%edx` / `fstps 0xe(%eax,%edx,4)`；我方同段为
// `mov 0xc(%ebp),%edx` / `lea 0x380(%edx),%ecx` / `fstps 0xe(%eax,%ecx,4)`
// ⇒ 4.1/4.4 RA 命名差（前面两轮 20 形已证无源形可翻）。维持 N82/N18a。
#include "CUserCharacInfo/WarAreaKill.inc"
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

#include "CUserCharacInfo/VillageAttack_NotDBUpdate.inc"
{
    if (m_selected)
    {
    CUR->m_villageAttackDBUpdate = 0;
    }
}


#include "CUserCharacInfo/VillageAttack_DBUpdate.inc"
{
    if (m_selected)
    {
    CUR->m_villageAttackDBUpdate = 1;
    }
}


// [b28c-L26 SIGFIX·档①] 定义返回类型随头改 unsigned int（P159：返回类型不入
// mangled，ABI 同宽；lane35 转来的 spec，Main 终裁本 TU 归我收尾）。
#include "CUserCharacInfo/UseFinishPoint.inc"
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

#include "CUserCharacInfo/SubCurCharacChaosPoint.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosPoint -= point;
    if (CUR->m_chaosPoint < 0)
        CUR->m_chaosPoint = 0;
    }
}


#include "CUserCharacInfo/SetVisible.inc"
{
    CUR->m_visibleBits.visible = 1;
}

#include "CUserCharacInfo/SetSaveDemensionInoutValue.inc"
{
    CUR->m_saveDemensionInout = flag;
}




#include "CUserCharacInfo/SetSaveBloodInoutValue.inc"
{
    CUR->m_saveBloodInout = flag;
}




#include "CUserCharacInfo/SetReliablePerson.inc"
{
    m_reliablePerson = flag;
}

#include "CUserCharacInfo/SetProperDungeonUser.inc"
{
    if (m_selected)
    {
    CUR->m_properDungeonUser = 1;
    }
}


#include "CUserCharacInfo/SetPowerWarProcessInfoLastUpdateTime.inc"
{
    if (m_selected)
    {
    CUR->m_powerWarProcessLastUpdateTime = t;
    }
}


#include "CUserCharacInfo/SetPowerWarLastPlayTime.inc"
{
    if (m_selected)
    {
    CUR->m_powerWarLastPlayTime = t;
    }
}


#include "CUserCharacInfo/SetOverTradeGoldCount.inc"
{
    // ORIG：`mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0xd0(%eax)`
    // LHS(this) 先载。字段已改 unsigned（头同步）与形参 Ej 同型直赋无转换节点，
    // 4.4 出 LHS 先=ORIG 同形（N76 翻案：无转换→LHS 先，有转换→RHS 先）。
    m_overTradeGoldCount = count;
}

#include "CUserCharacInfo/SetOpencoin.inc"
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




#include "CUserCharacInfo/SetOneDayLetheFlag2ND.inc"
{
    if (m_selected)
    {
    CUR->m_oneDayLetheFlag2ND = flag;
    }
}


#include "CUserCharacInfo/SetOneDayLetheFlag.inc"
{
    if (m_selected)
    {
    CUR->m_oneDayLetheFlag = flag;
    }
}


#include "CUserCharacInfo/SetLoginTick.inc"
{
    CUR->m_loginTick = tick;
}

#include "CUserCharacInfo/SetIsEndLetheAtLogin.inc"
{
    if (m_selected)
    {
    CUR->m_isEndLetheAtLogin = 1;
    }
}


#include "CUserCharacInfo/SetInvisible.inc"
{
    CUR->m_visibleBits.visible = 0;
}

#include "CUserCharacInfo/SetFirstTimeLogin.inc"
{
    CUR->m_firstTimeLogin = flag;
}

#include "CUserCharacInfo/SetCurRevengeDungeonCount.inc"
{
    if (m_selected)
    {
    CUR->m_revengeDungeonCount = count;
    }
}


#include "CUserCharacInfo/SetCurExpEquipSlotStat.inc"
{
    if (m_selected)
    {
    CUR->m_curExEquipSlotStat = stat;
    }
}


#include "CUserCharacInfo/SetCurCharacSuperState.inc"
{
    if (m_selected)
    {
    CUR->m_superState = flag;
    }
}


#include "CUserCharacInfo/SetCurCharacSkillTreeIndex.inc"
{
    if (m_selected)
    {
    enableSaveCharacInfo();
    CUR->m_skillTreeIndex = index;
    }
}


#include "CUserCharacInfo/SetCurCharacRevengeData.inc"
{
    if (m_selected)
    {
    CUR->m_revengeCount = count;
    CUR->m_revengeDungeonCount = ticket;
    }
}


#include "CUserCharacInfo/SetCurCharacOption.inc"
{
    if (m_selected)
        CUR->m_characterOption = option;
}




#include "CUserCharacInfo/SetCurCharacOnlinePreliminaryTeam.inc"
    online_preliminary::COnlinePreliminaryTeam* team)
{
    if (m_selected)
    {
    CUR->m_onlinePreliminaryTeam = team;
    }
}


#include "CUserCharacInfo/SetCurCharacMaxEquipLevel.inc"
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    CUR->m_maxEquipLevel = level;
    }
}


#include "CUserCharacInfo/enableSaveCharacManageInfo.inc"

#include "CUserCharacInfo/SetCurCharacLuckPointUpdateTimeCount.inc"
{
    if (m_selected)
    {
    CUR->m_luckPointUpdateTimeCount = count;
    }
}


#include "CUserCharacInfo/SetCurCharacLeaveTimeout.inc"
{
    if (m_selected)
    {
    CUR->m_leaveTimeout = flag;
    }
}


#include "CUserCharacInfo/SetCurCharacGhost.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_ghost = flag;
    }
}


#include "CUserCharacInfo/SetCurCharacExpertJobType.inc"
{
    if (m_selected)
    {
    enableSaveCharacInfo();
    CUR->m_expertJobType = type;
    }
}


#include "CUserCharacInfo/enableSaveCharacInfo.inc"

#include "CUserCharacInfo/SetCurCharacExpertJobExp.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_expertJobExp = exp;
    }
}


#include "CUserCharacInfo/SetCurCharacExpertJob.inc"
{
    if (m_selected)
    {
    CUR->m_expertJob = job;
    }
}


#include "CUserCharacInfo/SetCurCharacEmotion.inc"
{
    enableSaveCharacStat();
    CUR->m_curCharacEmotion = (unsigned short)emotion;
}

#include "CUserCharacInfo/SetCurCharacChaosPoint.inc"
{
    if (m_selected)
    {
    CUR->m_chaosPoint = point;
    }
}


#include "CUserCharacInfo/SetCurCharacChaosKillTime.inc"
{
    if (m_selected)
    {
    CUR->m_chaosKillTime = t;
    }
}


#include "CUserCharacInfo/SetCurCharacAssaultedUserId.inc"
{
    if (m_selected)
    {
    CUR->m_assaultedUserId = userId;
    }
}


#include "CUserCharacInfo/SetCurCharacAssaultMoney.inc"
{
    if (m_selected)
    {
    CUR->m_assaultMoney = money;
    }
}


#include "CUserCharacInfo/SetCurCharacAssaultFatigueCount.inc"
{
    if (m_selected)
    {
    CUR->m_assaultFatigueCount = count;
    }
}


#include "CUserCharacInfo/SetCharacStartPlayTick.inc"

#include "CUserCharacInfo/SetAssaultPlace.inc"
{
    if (m_selected)
    {
    CUR->m_assaultPlace = place;
    }
}


#include "CUserCharacInfo/ResetRevengeDungeonClear.inc"
{
    if (m_selected)
    {
    CUR->m_revengeDungeonCount = 0;
    CUR->m_revengeCount = 0;
    }
}


#include "CUserCharacInfo/ResetPowerWarAssaultRecord.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_powerWarAssaultCount = 0;
    CUR->m_powerWarAssaultVictoryCount = 0;
    }
}


#include "CUserCharacInfo/ResetCurCharacEventItemCount.inc"
{
    if (m_selected)
    {
    CUR->m_curCharacEventItemCount = 0;
    }
}


#include "CUserCharacInfo/ResetCurCharacEventAddItemCount.inc"
{
    if (m_selected)
    {
    CUR->m_curCharacEventAddItemCount = 0;
    }
}


#include "CUserCharacInfo/ResetCurCharacDungeonPlayCount.inc"
{
    enableSaveCharacStat();
    SetCurCharacDungeonPlayCount(0);
}

#include "CUserCharacInfo/LiveCurCharac.inc"
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


// b28d-CUser P292：ORIG 0x08696976 `test;je;movzbl 0x1244;ret`（无 setne 归一化）
//   ⇒ 返回 bool 且三目两臂同型（字段已 bool 化）。
#include "CUserCharacInfo/IsVillageAttackDBUpdate.inc"
{
    return m_selected ? CUR->m_villageAttackDBUpdate : false;
}

#include "CUserCharacInfo/IsSaveDemensionInoutValue.inc"
{
    return CUR->m_saveDemensionInout;
}

#include "CUserCharacInfo/IsSaveCharacStat.inc"

#include "CUserCharacInfo/IsSaveCharacManageInfo.inc"

#include "CUserCharacInfo/IsSaveBloodInoutValue.inc"
{
    return CUR->m_saveBloodInout;
}

#include "CUserCharacInfo/IsProperDungeonUser.inc"
{
    return m_selected ? CUR->m_properDungeonUser : 0;
}

#include "CUserCharacInfo/IsFirstTimeLogin.inc"
{
    return CUR->m_firstTimeLogin;
}

#include "CUserCharacInfo/IsExpItemAffected.inc"
{
    return m_selected ? CUR->m_isExpItemAffected : 0;
}


#include "CUserCharacInfo/IsEndLetheAtLogin.inc"
{
    return m_selected ? CUR->m_isEndLetheAtLogin : 0;
}


#include "CUserCharacInfo/IsCurCharacVisible.inc"
{
    if (m_selected && CUR->m_visibleBits.visible)
        return true;
    return false;
}


#include "CUserCharacInfo/IsCurCharacLeaveTimeout.inc"
{
    return m_selected ? CUR->m_leaveTimeout : false;
}

#include "CUserCharacInfo/IsBusyDisjointStore.inc"
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
#include "CUserCharacInfo/IsAvailableCurCharacTradeGoldDaily.inc"
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

#include "CUserCharacInfo/getCurCharacName.inc"
{
    return m_selected ? (const char*)CUR->m_name : "";
}


#include "CUserCharacInfo/InitFinishPoint.inc"
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


#include "CUserCharacInfo/IncrementCurCharacTradeGoldDaily.inc"
{
    enableSaveCharacStat();
    CUR->m_reliablePerson.IncrementTradeGold(gold);
}

// ORIG `movzwl 0x121f(%edx),%edx;mov %edx,%ecx` 二段；4.4 折单步 `movzwl …,%ecx`
// （N18a：4.4 恒折 mem→%reg→callee-saved 两段），寄存器分配差异，语义等价。
// [b28d-solo-CUserCharacInfo N18a 复核] `+=` 形实测同形（movzwl mem,%ecx 单段）；
// 字段为 16 位 ushort，无法按 P254 改 enum（宽度变 4 破坏 +0x121f 布局）⇒ 封顶。
#include "CUserCharacInfo/IncreasePowerWarPoint.inc"
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


#include "CUserCharacInfo/IncProperDungeonClearCount.inc"
{
    if (m_selected)
    {
    ++CUR->m_properDungeonClearCount;
    }
}


#include "CUserCharacInfo/IncPowerWarAssaultVictoryCount.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    ++CUR->m_powerWarAssaultVictoryCount;
    }
}


#include "CUserCharacInfo/IncPowerWarAssaultCount.inc"
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
#include "CUserCharacInfo/IncFatigueBatteryCharging.inc"
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




#include "CUserCharacInfo/IncCurConditionEventStep.inc"
{
    if (m_selected)
    {
        ++CUR->m_curConditionEventStep;
        if ((short)CUR->m_curConditionEventStep > 0xff)
            CUR->m_curConditionEventStep = 0xff;
    }
}




#include "CUserCharacInfo/IncCurConditionEventRewardStep.inc"
{
    if (m_selected)
    {
        ++CUR->m_curConditionEventRewardStep;
        if ((short)CUR->m_curConditionEventRewardStep > 0xff)
            CUR->m_curConditionEventRewardStep = 0xff;
    }
}




#include "CUserCharacInfo/IncCurCharacLuckPointUpdateTimeCount.inc"
{
    if (m_selected)
        CUR->m_luckPointUpdateTimeCount += 1;
}




#include "CUserCharacInfo/IncCurCharacLuckPoint.inc"
{
    enableSaveCharacStat();
    if (GetCurCharacLuckPoint() == 100000)
        return;
    SetCurCharacLuckPoint(GetCurCharacLuckPoint() + point);
}

#include "CUserCharacInfo/IncCurCharacEventAddItemCount.inc"
{
    if (m_selected)
    {
    CUR->m_curCharacEventAddItemCount =
        (unsigned short)(CUR->m_curCharacEventAddItemCount + count);
    }
}


#include "CUserCharacInfo/IncCurCharacDungeonPlayCount.inc"
{
    enableSaveCharacStat();
    SetCurCharacDungeonPlayCount(GetCurCharacDungeonPlayCount() + 1);
}

#include "CUserCharacInfo/SetCurCharacDungeonPlayCount.inc"
{
    if (m_selected)
    {
    CUR->m_dungeonPlayCount = count;
    }
}


#include "CUserCharacInfo/IncCurCharacChaosModeCount.inc"
{
    enableSaveCharacStat();
    SetCurCharacChaosModeCount(GetCurCharacChaosModeCount() + 1);
}

#include "CUserCharacInfo/SetCurCharacChaosModeCount.inc"
{
    if (m_selected)
    {
    CUR->m_chaosModeCount = count;
    }
}


#include "CUserCharacInfo/IncCurCharacChaosKillCount.inc"
{
    enableSaveCharacStat();
    SetCurCharacChaosKillCount(GetCurCharacChaosKillCount() + 1);
}

#include "CUserCharacInfo/SetCurCharacChaosKillCount.inc"
{
    if (m_selected)
    {
    CUR->m_chaosKillCount = count;
    }
}


#include "CUserCharacInfo/IncCurCharacChaosDieCount.inc"
{
    enableSaveCharacStat();
    SetCurCharacChaosDieCount(GetCurCharacChaosDieCount() + 1);
}

#include "CUserCharacInfo/SetCurCharacChaosDieCount.inc"
{
    if (m_selected)
    {
    CUR->m_chaosDieCount = count;
    }
}


#include "CUserCharacInfo/IncCurCharacAssaultFatigueCount.inc"
{
    if (m_selected)
    {
    ++CUR->m_assaultFatigueCount;
    }
}


#include "CUserCharacInfo/IncCurCharacAssaultCount.inc"
{
    enableSaveCharacStat();
    SetAssaultCount(GetAssaultCount() + 1);
}

#include "CUserCharacInfo/SetAssaultCount.inc"
{
    if (m_selected)
    {
    CUR->m_assaultCount = count;
    }
}


#include "CUserCharacInfo/GmSetCurCharacChaosPoint.inc"
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


// [b28d-CPrivateStoreMgr] 返回类型判据（ORIG 0x085cbd38 + 探针 f15/f16）：unsigned int。
#include "CUserCharacInfo/GetWinPoint.inc"

#include "CUserCharacInfo/GetWarAreaKillCountTotal.inc"
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


#include "CUserCharacInfo/GetWarAreaKillCount.inc"
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x260, 5);
        trace("WARAREA CUserCharacInfo::GetWarAreaKillCount() : m_selected is NULL");
        return 0.0f;
    }
    return CUR->m_warAreaKill[index];
}

#include "CUserCharacInfo/GetUserStateByAssault.inc"
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


#include "CUserCharacInfo/GetUserPowerWarPoint.inc"
{
    return m_selected ? CUR->m_powerWarPoint : 0;
}

// [b28d-CPrivateStoreMgr] 返回类型判据（ORIG 0x085c8acf xor 形 + P292/T3）：bool。
#include "CUserCharacInfo/GetReliablePerson.inc"

#include "CUserCharacInfo/GetProperDungeonClearCount.inc"
{
    return m_selected ? CUR->m_properDungeonClearCount : 0;
}

#include "CUserCharacInfo/GetPowerWarProcessInfoLastUpdateTime.inc"
{
    return m_selected ? CUR->m_powerWarProcessLastUpdateTime : 0;
}

#include "CUserCharacInfo/GetPowerWarLastPlayTime.inc"
{
    return m_selected ? CUR->m_powerWarLastPlayTime : 0;
}
#include "CUserCharacInfo/SetIsInitSkillFlag.inc"
{
    if (m_selected)
    {
        CUR->m_isInitSkillFlag = flag;
    }
}

#include "CUserCharacInfo/SetIsInitSkillFlag2ND.inc"
{
    if (m_selected)
    {
        CUR->m_isInitSkillFlag2ND = flag;
    }
}

#include "CUserCharacInfo/IncVillageAttackCount.inc"
{
    if (m_selected)
    {
        ++CUR->m_revengeCount;
        CUR->m_villageAttackDBUpdate = 1;
    }
}

#include "CUserCharacInfo/GetPowerWarAssaultVictoryCount.inc"
{
    return m_selected ? CUR->m_powerWarAssaultVictoryCount : 0;
}

#include "CUserCharacInfo/GetPowerWarAssaultCount.inc"
{
    return m_selected ? CUR->m_powerWarAssaultCount : 0;
}

#include "CUserCharacInfo/GetOverTradeGoldCount.inc"

#include "CUserCharacInfo/GetOpencoin.inc"
{
    return CUR->m_opencoin;
}

#include "CUserCharacInfo/GetOneDayLetheFlag2ND.inc"
{
    return m_selected ? CUR->m_oneDayLetheFlag2ND : 0;
}

#include "CUserCharacInfo/GetOneDayLetheFlag.inc"
{
    return m_selected ? CUR->m_oneDayLetheFlag : 0;
}

#include "CUserCharacInfo/GetLoginTick.inc"
{
    if (!m_selected)
        return 0;
    return CUR->m_loginTick;
}


#include "CUserCharacInfo/GetFinishPointTotal.inc"
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x26a, 5);
        trace("WARAREA CUserCharacInfo::GetFinishPointTotal() : m_selected is NULL");
        return 0;
    }
    return CUR->m_finishPointTotal;
}



#include "CUserCharacInfo/GetFinishPoint.inc"
{
    if (m_selected)
        return CUR->m_finishPoint;
    cMyTrace trace(__PRETTY_FUNCTION__, 0x244, 5);
    trace("WARAREA CUserCharacInfo::GetFinishPoint() : m_selected is NULL");
    return 0;
}


#include "CUserCharacInfo/GetFatigueBatteryCharging.inc"
{
    return m_selected ? CUR->m_fatigueBattery : 0;
}

#include "CUserCharacInfo/GetFatigue.inc"
{
    return m_selected ? CUR->m_fatigue : 0;
}

#include "CUserCharacInfo/GetExpAffectRate.inc"
{
    return m_selected ? CUR->m_expAffectRate : 0.0f;
}


#include "CUserCharacInfo/GetCurVillageAttackCount.inc"
{
    return m_selected ? CUR->m_revengeCount : 0;
}

#include "CUserCharacInfo/GetCurRevengeDungeonCount.inc"
{
    return m_selected ? CUR->m_revengeDungeonCount : 0;
}

#include "CUserCharacInfo/GetCurExEquipSlotStat.inc"
{
    return m_selected ? (unsigned char)CUR->m_curExEquipSlotStat : 0;
}

#include "CUserCharacInfo/GetCurConditionEventStep.inc"
{
    return m_selected ? CUR->m_curConditionEventStep : 0;
}

#include "CUserCharacInfo/GetCurConditionEventRewardStep.inc"
{
    return m_selected ? CUR->m_curConditionEventRewardStep : 0;
}

// [S1Charac 2026-09-11 类型轴复核 · 非本写域可修] ORIG 0x0822f6f4 段共 13 条：
//   0x0822f707 movzbl 0xe65(%eax),%eax → 0x0822f70e jmp → 直接返回字段原值，
//   **无 test/setne 归一**；我方 16 条多出 `test %al,%al; je; mov $0x1,%eax`
//   （bool 返回在 -O0 必做 0/1 归一）。4.4.6 cc1plus 探针（/tmp/s1_ss_probe.cpp，
//   -O0 -m32）逐条复现：`bool A` → 16 条含 testb/je/movl $1；`unsigned char B`
//   → 13 条与 ORIG 同形。⇒ 根因 = 返回类型 bool（CUserCharacInfo.h:964，K1Dispatchers
//   本波 uchar→bool），而字段 m_superState 本身即 unsigned char（h:484）。
//   函数体层面无解（签名轴，改必须动 .h）⇒ N（类型轴），已上报父泳道。
#include "CUserCharacInfo/GetCurCharacSuperState.inc"
{
    return m_selected ? CUR->m_superState : 0;
}

#include "CUserCharacInfo/GetCurCharacSkillTreeIndex.inc"
{
    return m_selected ? (signed char)CUR->m_skillTreeIndex : 0;
}

#include "CUserCharacInfo/GetCurCharacOnlinePreliminaryTeamName.inc"
{
    if (m_selected)
    {
        if (CUR->m_onlinePreliminaryTeam != 0)
            return CUR->m_onlinePreliminaryTeam->GetTeamName();
    }
    return "";
}



#include "CUserCharacInfo/GetCurCharacOnlinePreliminaryTeamId.inc"
{
    if (m_selected)
    {
        if (CUR->m_onlinePreliminaryTeam != 0)
            return CUR->m_onlinePreliminaryTeam->GetTeamId();
    }
    return 0;
}



online_preliminary::COnlinePreliminaryTeam*
#include "CUserCharacInfo/GetCurCharacOnlinePreliminaryTeam.inc"
{
    return m_selected ? CUR->m_onlinePreliminaryTeam : 0;
}


#include "CUserCharacInfo/GetCurCharacMaxEquipLevel.inc"
{
    return m_selected ? CUR->m_maxEquipLevel : 0;
}

#include "CUserCharacInfo/GetCurCharacLuckPointUpdateTimeCount.inc"
{
    return m_selected ? CUR->m_luckPointUpdateTimeCount : 0;
}

#include "CUserCharacInfo/GetCurCharacExpertJobType.inc"
{
    return m_selected ? CUR->m_expertJobType : 0;
}

#include "CUserCharacInfo/GetCurCharacExpertJobExp.inc"
{
    return m_selected ? CUR->m_expertJobExp : 0;
}

#include "CUserCharacInfo/GetCurCharacExpertJob.inc"
{
    return m_selected ? CUR->m_expertJob : 0;
}

#include "CUserCharacInfo/GetCurCharacEventItemCount.inc"
{
    return m_selected ? CUR->m_curCharacEventItemCount : 0;
}

#include "CUserCharacInfo/GetCurCharacEventAddItemCount.inc"
{
    return m_selected ? CUR->m_curCharacEventAddItemCount : 0;
}

#include "CUserCharacInfo/GetCurCharacEmotion.inc"
{
    return m_selected ? CUR->m_curCharacEmotion : 0;
}

#include "CUserCharacInfo/GetCurCharacDungeonPlayCount.inc"
{
    return m_selected ? CUR->m_dungeonPlayCount : 0;
}

#include "CUserCharacInfo/GetCurCharacChaosStateTime.inc"
{
    return m_selected ? CUR->m_chaosStateTime : 0;
}

#include "CUserCharacInfo/GetCurCharacChaosResponTime.inc"
{
    return m_selected ? CUR->m_chaosResponTime : 0;
}

#include "CUserCharacInfo/GetCurCharacChaosModeCount.inc"
{
    return m_selected ? CUR->m_chaosModeCount : 0;
}

#include "CUserCharacInfo/GetCurCharacChaosKillTime.inc"
{
    return m_selected ? CUR->m_chaosKillTime : 0;
}

#include "CUserCharacInfo/GetCurCharacChaosKillCount.inc"
{
    return m_selected ? CUR->m_chaosKillCount : 0;
}

#include "CUserCharacInfo/GetCurCharacChaosExp.inc"
{
    return m_selected ? CUR->m_chaosExp : 0;
}

#include "CUserCharacInfo/GetCurCharacChaosDieTime.inc"
{
    return m_selected ? CUR->m_chaosDieTime : 0;
}

#include "CUserCharacInfo/GetCurCharacChaosDieCount.inc"
{
    return m_selected ? CUR->m_chaosDieCount : 0;
}

#include "CUserCharacInfo/GetCurCharacBackupSkill2ND.inc"
{
    return m_selected ? (const char*)CUR->m_backupSkill2ND : 0;
}

#include "CUserCharacInfo/GetCurCharacBackupSkill.inc"
{
    return m_selected ? (const char*)CUR->m_backupSkill : 0;
}

#include "CUserCharacInfo/GetCurCharacAssaultedUserId.inc"
{
    return m_selected ? CUR->m_assaultedUserId : 0;
}

#include "CUserCharacInfo/GetCurCharacAssaultMoney.inc"
{
    return m_selected ? CUR->m_assaultMoney : 0;
}

#include "CUserCharacInfo/GetCurCharacAssaultFatigueCount.inc"
{
    return m_selected ? CUR->m_assaultFatigueCount : 0;
}

// [S1Charac 2026-09-11 复核] 100/104：真残差=4.4 把 LHS 地址（index+0x460）CSE 到
// %esi 跨 call 保活（+push/pop %esi），并按「disp/scale≥2 折、==1 不折」定则把 disp
// 0x10 折成 `lea 0x2(%esi),%edx`；ORIG 0x08677f97/0x08677fbc 每处独立
// `lea 0x460(%ebx),%ecx` + disp 0x10 ⇒ N82附/N4 封顶。§6.5 串核查：本函数无 rodata
// 串引用，且本 TU 全量 .rodata 串 24/24 均存在于 ORIG（无发明串）⇒ CLEAR。
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
#include "CUserCharacInfo/GetContinuousItemInfo.inc"
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

#include "CUserCharacInfo/GetCurCharacEffectItemListR.inc"
{
    return &CUR->m_effectItemList.m_list;
}

#include "CUserCharacInfo/GetCurCharacCoolTimeItemListR.inc"
{
    return &CUR->m_coolTimeItemList.m_list;
}
#include "CUserCharacInfo/GetCharacPlayTick.inc"
{
    return GlobalData::s_systemTime_.getCurSec() - GetCharacStartPlayTick();
}

#include "CUserCharacInfo/GetCharacStartPlayTick.inc"

#include "CUserCharacInfo/GetAssaultPlace.inc"
{
    return m_selected ? CUR->m_assaultPlace : 0;
}

#include "CUserCharacInfo/GetAssaultCount.inc"
{
    return m_selected ? CUR->m_assaultCount : 0;
}

#include "CUserCharacInfo/GainCurCharacChaosExp.inc"
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
#include "CUserCharacInfo/FindItemLock.inc"
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
#include "CUserCharacInfo/EnablePeriodExpItem.inc"
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



#include "CUserCharacInfo/SetExpAffectItemEffect.inc"
{
    if (m_selected)
    {
    CUR->m_isExpItemAffected = flag;
    CUR->m_expAffectRate = rate;
    }
}


#include "CUserCharacInfo/EnablePeriodExpDouble.inc"
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



#include "CUserCharacInfo/SetIsAffectedExpDouble.inc"
{
    CUR->m_isAffectedExpDouble = flag;
}




// [b28d-solo-CUserCharacInfo AE] 三旋钮一次归零（108=108）：
// ①P0 删具名 `CUserCharacInfo* other` 局部（ORIG 0x0864633a/0x0864634d 两次重读
//   0x10(%ebp) 形参直用）；
// ②P198②/P225 两处 bool 条件整体内联（ORIG 直测 `test %al,%al; je`，无 mov$1/mov$0 物化）；
// ③P252 具名 `float penalty` 局部（ORIG 0x086462f0 `fstps -0x10(%ebp)` + 回读压栈；
//   删 other 局部后槽位自洽，旧「复测反劣」系 other 占槽所致）。
#include "CUserCharacInfo/DieCurCharac.inc"
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


#include "CUserCharacInfo/get_charac_level.inc"
{
    return m_selected ? CUR->m_level : 0;
}

#include "CUserCharacInfo/getCurCharacVill.inc"
{
    if (!m_selected)
        return 0;
    if (G_GameWorld()->IsPvPVillageMapChannel())
        return 10;
    if (G_GameWorld()->IsPVPChannel())
        return CUR->m_villPvP;
    return CUR->m_vill;
}


#include "CUserCharacInfo/SetCurCharacChaosResponTime.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosResponTime = t;
    }
}


#include "CUserCharacInfo/SetCurCharacChaosDieTime.inc"
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_chaosDieTime = t;
    }
}


#include "CUserCharacInfo/IsCurCharacGhost.inc"
{
    // ORIG：true 路单一 `movzbl 0xe66(%eax),%eax` 直返（uchar 返回 = load 即
    // 终值，无 int promote 二段 movzbl %al；bool 声明反产 test;je;mov$1 规范化）
    // ⇒ 返回类型 unsigned char（技巧 33，mangled 无返回类型）。
    if (m_selected)
        return CUR->m_ghost;
    return 0;
}

#include "CUserCharacInfo/GetCurCharacChaosPoint.inc"
{
    return m_selected ? CUR->m_chaosPoint : 0;
}

#include "CUserCharacInfo/DecreaseWarPoint.inc"
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

#include "CUserCharacInfo/DecreaseRevengeDungeonTicket.inc"
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
#include "CUserCharacInfo/DecreasePowerWarPoint.inc"
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
#include "CUserCharacInfo/DecFatigueBatteryCharging.inc"
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




#include "CUserCharacInfo/DecCurCharacLuckPoint.inc"
{
    enableSaveCharacStat();
    SetCurCharacLuckPoint(GetCurCharacLuckPoint() - GetTenThousandPercentage(GetCurCharacLuckPoint(), percent));
}

#include "CUserCharacInfo/SetCurCharacLuckPoint.inc"
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


#include "CUserCharacInfo/GetCurCharacLuckPoint.inc"
{
    return m_selected ? CUR->m_luckPoint : 0;
}

#include "CUserCharacInfo/ClearUselessEffectItem.inc"
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

#include "CUserCharacInfo/GetCurCharacEffectItemList.inc"
{
    return &CUR->m_effectItemList.m_list;
}

#include "CUserCharacInfo/ClearUselessCooltimeItem.inc"
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

#include "CUserCharacInfo/GetCurCharacCoolTimeItemList.inc"
{
    return &CUR->m_coolTimeItemList.m_list;
}

#include "CUserCharacInfo/ClearProperDungeonUser.inc"
{
    if (m_selected)
    {
    CUR->m_properDungeonUser = 0;
    }
}


#include "CUserCharacInfo/ClearProperDungeonClearCount.inc"
{
    SetProperDungeonClearCount(0);
}

#include "CUserCharacInfo/SetProperDungeonClearCount.inc"
{
    if (m_selected)
    {
    CUR->m_properDungeonClearCount = (unsigned short)count;
    }
}


#include "CUserCharacInfo/ClearCurConditionEventStep.inc"
{
    SetCurConditionEventStep(0);
}

#include "CUserCharacInfo/SetCurConditionEventStep.inc"
{
    if (m_selected)
    {
    CUR->m_curConditionEventStep = (unsigned short)step;
    }
}


#include "CUserCharacInfo/ClearCurConditionEventRewardStep.inc"
{
    SetCurConditionEventRewardStep(0);
}

#include "CUserCharacInfo/SetCurConditionEventRewardStep.inc"
{
    if (m_selected)
    {
    CUR->m_curConditionEventRewardStep = (unsigned short)step;
    }
}


#include "CUserCharacInfo/ChangeFinishPoint.inc"
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

#include "CUserCharacInfo/CUserCharacInfo.inc"
    : m_pvpResult(), m_battleRecord(), m_dungeonClear()
{
    m_selected = 0;
    m_tag = 0;
    resetSaveFlags();
}

#include "CUserCharacInfo/resetSaveFlags.inc"
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

#include "CUserCharacInfo/AddCurCharacChaosPoint.inc"
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_chaosPoint += point;
    }
}


#include "CUserCharacInfo/enableSaveCharacStat.inc"

// ============================================================================
// GetTenThousandPercentage（ORIG 0x8550fc9 W）
// 万分比计算：value * percent / 10000（编译器以 0x68db8bad 魔数除法实现，
// 反汇编确认 = imul 后魔数除法，语义即 value*percent/10000）。
// 声明见 CUserCharacInfo.h（SetCurCharacLuckPoint 使用）。
// ============================================================================
#include "CUserCharacInfo/GetTenThousandPercentage.inc"
{
    return value * percent / 10000;
}
