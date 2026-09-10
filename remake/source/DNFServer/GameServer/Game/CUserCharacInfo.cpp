// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserCharacInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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

#include "CUserCharacInfo/dtor_CUserCharacInfo.inc"
}

#include "CUserCharacInfo/useWinPoint.inc"
}

#include "CUserCharacInfo/updateLastItemLimitEditionBuyTime.inc"
}


#include "CUserCharacInfo/subtractCurCharacExp.inc"
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/subUltimateInoutValue.inc"
}

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/subDemensionInoutValue.inc"
}

#include "CUserCharacInfo/set_uppermember_exp_level.inc"
}


#include "CUserCharacInfo/set_member_pay_tex_money_to_upper.inc"
}


#include "CUserCharacInfo/set_member_pay_tex_fatigue_to_upper.inc"
}


#include "CUserCharacInfo/set_level_before_dungeon.inc"
}


#include "CUserCharacInfo/set_guild_exp.inc"
}


#include "CUserCharacInfo/set_guild_create_right.inc"
}


#include "CUserCharacInfo/set_growth_power_reward.inc"
}


#include "CUserCharacInfo/set_growth_power_exp_reward_right.inc"
}


#include "CUserCharacInfo/set_connect_upper_member.inc"
}


#include "CUserCharacInfo/set_charac_visible_values.inc"
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/set_charac_visible_teleport_.inc"
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/set_charac_visible_grow_avatar.inc"
}


#include "CUserCharacInfo/set_charac_seria_buf_bonus_exp.inc"
}


#include "CUserCharacInfo/set_charac_party_bonus_exp.inc"
}


#include "CUserCharacInfo/set_charac_memberkey.inc"
}


#include "CUserCharacInfo/set_charac_member_bonus_exp.inc"
}


#include "CUserCharacInfo/set_charac_guildkey.inc"
}


#include "CUserCharacInfo/set_charac_fatigue_buf_bonus_exp.inc"
}


#include "CUserCharacInfo/set_charac_createtime.inc"
}

#include "CUserCharacInfo/set_aura_avatar_option_value.inc"
}


#include "CUserCharacInfo/set_aura_avatar_option.inc"
}



#include "CUserCharacInfo/setWeekendBonusFatigue.inc"
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/setWeekendBonusExp.inc"
}


#include "CUserCharacInfo/setUserEventCharacterFlag.inc"

#include "CUserCharacInfo/setUpgradeSeparateTrueFlag.inc"
}


#include "CUserCharacInfo/setUpgradeSeparateFalseFlag.inc"
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/setUltimateInoutValue.inc"
}

#include "CUserCharacInfo/setTagCharac.inc"

#include "CUserCharacInfo/setStraightVictories.inc"
}


#include "CUserCharacInfo/setSeriaRoomAniDecoInfo.inc"
}



#include "CUserCharacInfo/setSaveCharacOption.inc"
}

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/setPremiumBonusExp.inc"
}


#include "CUserCharacInfo/setPowerWarHP.inc"
}


#include "CUserCharacInfo/setMemberDungeonFlag.inc"
}


#include "CUserCharacInfo/setLinkCharacSlotIndex.inc"
}


#include "CUserCharacInfo/setGuildMemberGrade.inc"
}


#include "CUserCharacInfo/setGuildCreateFlag.inc"
}


// ORIG W 0x0822f8f6：m_selected 非空时返回其 +0xee9 字节，否则 0。
// 返回 bool：调用点 0x0821b4d8 `xor $0x1,%eax;test %al,%al`（uchar/int 声明不出 xor，探针实证）。
#include "CUserCharacInfo/isDisguiseCharac.inc"
}


// ORIG W 0x0822f91a：m_selected 非空时置 +0xee9 伪装标志，再依次
// setDisguiseKind(kind)（W 0x0822f976）/ setDisguiseIndex(index)（W 0x0822f99e）。
// [b28d-solo-CUserCharacInfo P210] 本定义必须前置于两个 callee 定义之前：
// 4.4 对 TU 内已见定义的 callee 精确继承 outgoing ⇒ caller 帧塌缩 sub $0x14
// + spill -0x4/-0x8/-0xc；前置后恢复 ORIG 保守帧 sub $0x28 + spill
// -0xc/-0x10/-0x14（探针 /tmp/s156_probe/p11.cpp f0/f1 对照）。
#include "CUserCharacInfo/setDisguiseCharac.inc"
}


#include "CUserCharacInfo/setDisguiseKind.inc"
}


#include "CUserCharacInfo/setDisguiseIndex.inc"
}


// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/setDemensionInoutValue.inc"
}

#include "CUserCharacInfo/setCurrCharacJoinPower.inc"
}




#include "CUserCharacInfo/setCurrCharacBlackCount.inc"
}


#include "CUserCharacInfo/setCurUseBringUpMaterialTotal.inc"
}


#include "CUserCharacInfo/setCurEquipslotFlag.inc"
}


#include "CUserCharacInfo/setCurCharacVill.inc"
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
}

#include "CUserCharacInfo/setCurCharacTradeGoldDaily.inc"
}

#include "CUserCharacInfo/setCurCharacTagCharacNo.inc"
}


#include "CUserCharacInfo/setCurCharacStrikerSkillIndex.inc"
}


#include "CUserCharacInfo/setCurCharacStamina.inc"
}



#include "CUserCharacInfo/setCurCharacPremiumFatigue.inc"
}


#include "CUserCharacInfo/setCurCharacMaxPremiumFatigue.inc"
}


#include "CUserCharacInfo/setCurCharacLevel.inc"
}


#include "CUserCharacInfo/setCurCharacLastPlayTickPowerWar.inc"
}



#include "CUserCharacInfo/setCurCharacLastPlayTick.inc"
}



#include "CUserCharacInfo/setCurCharacLastPlayDungeonIndex.inc"
}


#include "CUserCharacInfo/setCurCharacHelpAbuseComputedRatio.inc"
}


#include "CUserCharacInfo/setCurCharacGuildSecede.inc"
}

#include "CUserCharacInfo/setCurCharacFatigueGrownUpBuff.inc"
}




#include "CUserCharacInfo/setCurCharacFatigue.inc"
}


#include "CUserCharacInfo/setCurCharacExp.inc"
}


#include "CUserCharacInfo/setCurCharacEventCharacterGrowtype.inc"
}



#include "CUserCharacInfo/setCurCharacBackupStamina.inc"
}


#include "CUserCharacInfo/setCurCharac.inc"
}

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
// （身位字节 shl/sar/and/or 逻辑逐条全等）
#include "CUserCharacInfo/setCurCharGrowthType.inc"
}



// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
// 同 setCurCharGrowthType（身位字节写逻辑逐条一致）。
#include "CUserCharacInfo/setCurCharChangingGrowthType.inc"
}



#include "CUserCharacInfo/setCurChannelEquipslotSwitch.inc"
}


#include "CUserCharacInfo/setConctractOfCubePremiumInfo.inc"
}

#include "CUserCharacInfo/setCharacViewState.inc"
}


#include "CUserCharacInfo/setCharacOption.inc"
}




#include "CUserCharacInfo/setCharacLinkBonusGold.inc"
}


#include "CUserCharacInfo/setCharacLinkBonusExp.inc"
}


#include "CUserCharacInfo/setBringUpRewardFlag.inc"
}


#include "CUserCharacInfo/setBlueMarbleEnterCount.inc"
}


#include "CUserCharacInfo/setBloodBestRecord.inc"
}


#include "CUserCharacInfo/setBloodBestRecordUpdate.inc"
}


#include "CUserCharacInfo/reset_level_before_dungeon.inc"
}


#include "CUserCharacInfo/reset_is_end_lethe_at_login.inc"
}


#include "CUserCharacInfo/reset_guild_today_exp.inc"
}


#include "CUserCharacInfo/reset_guild_temp_exp.inc"
}


#include "CUserCharacInfo/reset_guild_exp.inc"
}


#include "CUserCharacInfo/resetWeekendBonusExp.inc"
}


#include "CUserCharacInfo/resetTempGuildNameAddr.inc"
}


#include "CUserCharacInfo/resetStraightVictories.inc"
}


#include "CUserCharacInfo/resetPremiumBonusExp.inc"
}


#include "CUserCharacInfo/resetGuildCreateFlag.inc"
}


#include "CUserCharacInfo/resetCurUseBringUpMaterialCount.inc"
}


#include "CUserCharacInfo/resetCurCharacUsedFatigue.inc"
}


#include "CUserCharacInfo/resetCurCharacSchoolPoint.inc"
}


#include "CUserCharacInfo/resetCurCharacOverEquipFlag.inc"
}



#include "CUserCharacInfo/resetCurCharacMemberBonusFatigue.inc"
}




#include "CUserCharacInfo/resetCharacTutorialFlag.inc"
}


#include "CUserCharacInfo/resetCharacFatigueGrownUpBuff.inc"
}


#include "CUserCharacInfo/resetCharGuildInfo.inc"
}


#include "CUserCharacInfo/resetBlueMarbleUserDeadCount.inc"
}


#include "CUserCharacInfo/resetBlueMarbleEnterCount.inc"
}


#include "CUserCharacInfo/needCurCharacCheckOverEquip.inc"
}


// [b28d-Main·检查点14] ORIG 0x08696ca4：13 条，`movzbl 0x127b(%eax),%eax; jmp`
// 直返无 setne ⇒ 字段/返回同型 bool；调用点 0x0868dbfb `xor $1` 联判。
// 字段已由 Main 在 CUserCharacInfo.h 正式改 bool，TU 局部视图已删除（直读）。
#include "CUserCharacInfo/is_growth_power_exp_reward_right.inc"
}

#include "CUserCharacInfo/is_connect_upper_member.inc"
}




#include "CUserCharacInfo/isSetGuildCreateFlag.inc"
}



#include "CUserCharacInfo/isSavedPremiumCubeInfo.inc"
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
}

#include "CUserCharacInfo/getPowerWarHP.inc"
}

#include "CUserCharacInfo/isExistItem.inc"
}



#include "CUserCharacInfo/isBloodBestRecordUpdate.inc"
}

#include "CUserCharacInfo/initContractOfCubePremiumInfo.inc"
}

#include "CUserCharacInfo/increaseBlueMarbleUserDeadCount.inc"
}


#include "CUserCharacInfo/increaseBlueMarbleEnterCount.inc"
}


#include "CUserCharacInfo/incStraightVictories.inc"
}


#include "CUserCharacInfo/incCurUseBringUpMaterialCount.inc"
}


#include "CUserCharacInfo/incCurCharacUsedFatigue.inc"
}


#include "CUserCharacInfo/incCurCharacMemberBonusFatigue.inc"
}



#include "CUserCharacInfo/incCurCharacLevel.inc"
}


#include "CUserCharacInfo/get_uppermember_exp_level.inc"
}

#include "CUserCharacInfo/get_pvp_result_oldexp_point.inc"

#include "CUserCharacInfo/get_pvp_result_exp_point.inc"

#include "CUserCharacInfo/get_pvp_grade.inc"

#include "CUserCharacInfo/get_member_pay_tex_money_to_upper.inc"
}

#include "CUserCharacInfo/get_member_pay_tex_fatigue_to_upper.inc"
}

#include "CUserCharacInfo/get_guild_today_exp.inc"
}

#include "CUserCharacInfo/get_guild_temp_exp.inc"
}

#include "CUserCharacInfo/get_guild_exp.inc"
}

#include "CUserCharacInfo/get_guild_create_right.inc"
}

#include "CUserCharacInfo/get_growth_power_reward.inc"
}

#include "CUserCharacInfo/get_connect_upper_member.inc"
}

#include "CUserCharacInfo/get_charac_visible_values.inc"
}

#include "CUserCharacInfo/get_charac_visible_grow_avatar.inc"
}


#include "CUserCharacInfo/get_charac_seria_buf_bonus_exp.inc"
}


#include "CUserCharacInfo/get_charac_party_bonus_exp.inc"
}

#include "CUserCharacInfo/get_charac_memberkey.inc"
}

#include "CUserCharacInfo/get_charac_member_bonus_exp.inc"
}

#include "CUserCharacInfo/get_charac_level_before_dungeon.inc"
}


#include "CUserCharacInfo/get_charac_job.inc"
}

#include "CUserCharacInfo/get_charac_guildkey.inc"
}

#include "CUserCharacInfo/get_charac_fatigue_buf_bonus_exp.inc"
}

#include "CUserCharacInfo/get_charac_exp.inc"
}

#include "CUserCharacInfo/get_charac_10_level_section.inc"
}

#include "CUserCharacInfo/getWeekendBonusExp.inc"
}

#include "CUserCharacInfo/getUserEventCharacterFlag.inc"

#include "CUserCharacInfo/getUpgradeSeparateTrueFlag.inc"
}

#include "CUserCharacInfo/getUpgradeSeparateFlaseFlag.inc"
}

#include "CUserCharacInfo/getUltimateInoutValue.inc"
}

#include "CUserCharacInfo/getTagCharacW.inc"

#include "CUserCharacInfo/getTagCharacSkillW.inc"
}

#include "CUserCharacInfo/getTagCharacSkillR.inc"
}

#include "CUserCharacInfo/getTagCharacR.inc"

#include "CUserCharacInfo/getTagCharacInvenW.inc"
}

#include "CUserCharacInfo/getTagCharacInvenR.inc"
}

#include "CUserCharacInfo/getTagCharacCargoW.inc"
}

#include "CUserCharacInfo/getTagCharacCargoR.inc"
}

#include "CUserCharacInfo/getTagCharacAddInfoR.inc"
}

#include "CUserCharacInfo/getStraightVictories.inc"
}

#include "CUserCharacInfo/getSkillCommand.inc"
}

#include "CUserCharacInfo/getSeriaRoomAniDecoInfo.inc"
}

// [b28d-solo-CUserCharacInfo AE] 160=160：
// ①`out.clear()` 移到 buffer 声明之前 ⇒ ORIG 0x0864607e `call clear` 在 rep stos 前；
// ②删具名 bool stop 改直测条件（P198②）；
// ③P274/P319：offset 更新三处 unsigned cast ⇒ 复现 ORIG 两条 lea
//   （`lea (%eax,%edx,1)` + `lea (%edx,%eax,1)`，探针 /tmp/s156_probe/p10.cpp g3）。
#include "CUserCharacInfo/getSeparatedTextUnits.inc"
}

#include "CUserCharacInfo/getRequestRenameState.inc"
}

#include "CUserCharacInfo/getPremiumBonusExp.inc"
}

#include "CUserCharacInfo/getPVPResultW.inc"
}

#include "CUserCharacInfo/getPVPResultRefW.inc"
}
#include "CUserCharacInfo/getPVPResultRefR.inc"
}

#include "CUserCharacInfo/getPVPResultR.inc"
}

#include "CUserCharacInfo/getMemberDungeonFlag.inc"
}


#include "CUserCharacInfo/getLinkCharacSlotIndex.inc"
}


#include "CUserCharacInfo/getLastItemLimitEditionBuyTime.inc"
}

#include "CUserCharacInfo/getGuildMemberGrade.inc"
}

#include "CUserCharacInfo/getDungeonClearRefW.inc"
}

#include "CUserCharacInfo/getDungeonClearRefR.inc"
}
#include "CUserCharacInfo/getDisguiseKind.inc"
}

#include "CUserCharacInfo/getDisguiseIndex.inc"
}

// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
#include "CUserCharacInfo/getDemensionInoutValue.inc"
}

#include "CUserCharacInfo/getCurrCharacBlackCount.inc"
}

#include "CUserCharacInfo/getCurUseBringUpMaterialTotal.inc"
}

#include "CUserCharacInfo/getCurUseBringUpMaterialCount.inc"
}

#include "CUserCharacInfo/getCurExpandEquipslotSwitch.inc"
}


#include "CUserCharacInfo/getCurEquipslotFlag.inc"
}


#include "CUserCharacInfo/getCurCharacW.inc"

#include "CUserCharacInfo/getCurCharacVillPrev.inc"
}

#include "CUserCharacInfo/getCurCharacVill4Save.inc"
}

#include "CUserCharacInfo/getCurCharacTutorialFlag.inc"
}

#include "CUserCharacInfo/getCurCharacTradeGoldTotalBillion.inc"
}

#include "CUserCharacInfo/getCurCharacTradeGoldTotal.inc"
}

#include "CUserCharacInfo/getCurCharacTradeGoldDaily.inc"
}

#include "CUserCharacInfo/getCurCharacTagCharacNo.inc"
}

#include "CUserCharacInfo/getCurCharacStrikerSkillIndex.inc"
}

#include "CUserCharacInfo/getCurCharacStamina.inc"
}

#include "CUserCharacInfo/getCurCharacSkillW.inc"
}

#include "CUserCharacInfo/getCurCharacSkillR.inc"
}

#include "CUserCharacInfo/getCurCharacSchoolPoint.inc"
}

#include "CUserCharacInfo/getCurCharacR.inc"

#include "CUserCharacInfo/getCurCharacPremiumFatigue.inc"
}

#include "CUserCharacInfo/getCurCharacPayCoin.inc"
}



#include "CUserCharacInfo/getCurCharacNo.inc"
}

#include "CUserCharacInfo/getCurCharacMoney.inc"
}



#include "CUserCharacInfo/getCurCharacMemberBonusFatigue.inc"
}

#include "CUserCharacInfo/getCurCharacMaxPremiumFatigue.inc"
}

#include "CUserCharacInfo/getCurCharacMaxFatigue.inc"
}

#include "CUserCharacInfo/getCurCharacLastPlayTickPowerWar.inc"
}

#include "CUserCharacInfo/getCurCharacLastPlayTick.inc"
}

#include "CUserCharacInfo/getCurCharacLastPlayDungeonIndex.inc"
}

#include "CUserCharacInfo/getCurCharacInvenW.inc"
}

#include "CUserCharacInfo/getCurCharacInvenRefW.inc"
}

#include "CUserCharacInfo/getCurCharacInvenRefR.inc"
}

#include "CUserCharacInfo/getCurCharacInvenR.inc"
}

#include "CUserCharacInfo/getCurCharacHelpAbuseComputedRatio.inc"
}

#include "CUserCharacInfo/getCurCharacGuildSecede.inc"
}

#include "CUserCharacInfo/getCurCharacGrowType.inc"
}

#include "CUserCharacInfo/getCurCharacFatigueGrownUpBuff.inc"
}

#include "CUserCharacInfo/getCurCharacFatigue.inc"
}

#include "CUserCharacInfo/getCurCharacExp.inc"
}

#include "CUserCharacInfo/getCurCharacEventCoin.inc"
}



#include "CUserCharacInfo/getCurCharacEventCharacterGrowtype.inc"
}

// [b28d-solo-CUserCharacInfo AE] 69=69：
// ①N6 翻案：`1 << flag`（int 字面量）保 ORIG `shl`（探针 /tmp/s156_probe/p12.cpp f4）；
// ②P169 尾标签守卫 + 低位分支共享尾 return（消两处 `mov $0; jmp` 岛）；
// ③路由 `-march=i486 -mtune=generic`（Main 已批）+ `index / 8` 复现 ORIG 6 条修正链
//   （探针 /tmp/s156_probe/p13i486.o）。
#include "CUserCharacInfo/getCurCharacEscaladeTutorialFlag.inc"
}

#include "CUserCharacInfo/getCurCharacEscaladeTutorialFlag_1.inc"
}

#include "CUserCharacInfo/getCurCharacCreateTime.inc"
}

#include "CUserCharacInfo/getCurCharacCoin.inc"
}



#include "CUserCharacInfo/getCurCharacCargoW.inc"
}

#include "CUserCharacInfo/getCurCharacCargoR.inc"
}

#include "CUserCharacInfo/getCurCharacAddInfoW.inc"
}

#include "CUserCharacInfo/getCurCharacAddInfoRefW.inc"
}

#include "CUserCharacInfo/getCurCharacAddInfoRefR.inc"
}

#include "CUserCharacInfo/getCurCharacAddInfoR.inc"
}

#include "CUserCharacInfo/getCurCharSecondGrowType.inc"
}

#include "CUserCharacInfo/getCurCharFirstGrowType.inc"
}

#include "CUserCharacInfo/getCurChannelEquipslotSwitch.inc"
}


#include "CUserCharacInfo/getCreateGuildName.inc"
}

#include "CUserCharacInfo/getCreateGuildAddr.inc"
}

#include "CUserCharacInfo/getContractOfCubePremiumInfo.inc"
}


#include "CUserCharacInfo/getCharacLinkType.inc"
}

#include "CUserCharacInfo/getCharacLinkBonusGold.inc"
}

#include "CUserCharacInfo/getCharacLinkBonusExp.inc"
}

#include "CUserCharacInfo/getBringUpRewardFlag.inc"
}

#include "CUserCharacInfo/getBlueMarbleUserDeadCount.inc"
}

#include "CUserCharacInfo/getBlueMarbleEnterCount.inc"
}

#include "CUserCharacInfo/getBloodBestTimeRecord.inc"
}

#include "CUserCharacInfo/getBloodBestRoundRecord.inc"
}

#include "CUserCharacInfo/getBattleRecordRefW.inc"
}

#include "CUserCharacInfo/getBattleRecordRefR.inc"
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
}


#include "CUserCharacInfo/decCurCharacFatigueGrownUpBuff.inc"
}


#include "CUserCharacInfo/checkBonusPoint.inc"
}



#include "CUserCharacInfo/resetBonusPoint.inc"
}


#include "CUserCharacInfo/getWpBonusPoint.inc"
}




#include "CUserCharacInfo/getExpBonusPoint.inc"
}




#include "CUserCharacInfo/get_level_up_exp.inc"
}

#include "CUserCharacInfo/getCurCharacDungeonHelpAbuseRatio.inc"
}

#include "CUserCharacInfo/getCurCharacDungeonHelpAbuseExp.inc"
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
#include "CUserCharacInfo/calc_disp_help_abuse_ratio.inc"
}

#include "CUserCharacInfo/calcHelpAbuseRatioBeforeLevelUp.inc"
}


#include "CUserCharacInfo/calcHelpAbuseRatio.inc"
}




// ORIG 0x08655bce 有符号除 `fatigue/10` 魔数段 `sar $0x1f,%eax;sub %eax,%edx` 直落 %edx；
// 4.4 经 %ecx 中转 `mov %edx,%ecx;sub %eax,%ecx;mov %ecx,%eax;mov %eax,%edx`（N7/N18a
// 寄存器路由），除运算与数组索引逐段全等，分支 je/jbe 落点合规，语义等价。
#include "CUserCharacInfo/calcCurCharacSchoolPoint.inc"
}


#include "CUserCharacInfo/getCurCharacUsedFatigue.inc"
}

#include "CUserCharacInfo/add_member_pay_tex_money_to_upper.inc"
}


#include "CUserCharacInfo/add_member_pay_tex_fatigue_to_upper.inc"
}


// [b28d-solo-CUserCharacInfo AE] 21=21：P274 异型视角（字段 unsigned 视角
// m_guildTodayExpU + 形参 int）⇒ 复现 ORIG `mov 0xdef(%edx),%ecx; mov 0xc(%ebp),%edx;
// lea (%ecx,%edx,1),%edx`（旧判「字段 unsigned 无效」系未用 union 视角 + 字段在前写法）。
#include "CUserCharacInfo/add_guild_today_exp.inc"
}


#include "CUserCharacInfo/add_guild_temp_exp.inc"
}


// [b28d-solo-CUserCharacInfo AE] 21=21：P274 异型视角（m_guildExpU）复现 ORIG
// 0x08645cb1 `mov 0xde9(%edx),%ecx; mov 0xc(%ebp),%edx; lea (%ecx,%edx,1),%edx`。
#include "CUserCharacInfo/add_guild_exp.inc"
}


#include "CUserCharacInfo/addCurcharacStamina.inc"
}


#include "CUserCharacInfo/addCurCharacExp.inc"
}


// [b28d-L14 N7/N4] ORIG 0x086459c4：残差=寄存器角色互换（%ecx↔%edx）+
// `lea 0x380(%reg)` 物化时刻（4.1 于 fadds 后 / 4.4 前置）。p167 CLEAR（语义完整）。
#include "CUserCharacInfo/WarAreaKill.inc"
}

#include "CUserCharacInfo/VillageAttack_NotDBUpdate.inc"
}


#include "CUserCharacInfo/VillageAttack_DBUpdate.inc"
}


// [b28c-L26 SIGFIX·档①] 定义返回类型随头改 unsigned int（P159：返回类型不入
// mangled，ABI 同宽；lane35 转来的 spec，Main 终裁本 TU 归我收尾）。
#include "CUserCharacInfo/UseFinishPoint.inc"
}

#include "CUserCharacInfo/SubCurCharacChaosPoint.inc"
}


#include "CUserCharacInfo/SetVisible.inc"
}

#include "CUserCharacInfo/SetSaveDemensionInoutValue.inc"
}




#include "CUserCharacInfo/SetSaveBloodInoutValue.inc"
}




#include "CUserCharacInfo/SetReliablePerson.inc"
}

#include "CUserCharacInfo/SetProperDungeonUser.inc"
}


#include "CUserCharacInfo/SetPowerWarProcessInfoLastUpdateTime.inc"
}


#include "CUserCharacInfo/SetPowerWarLastPlayTime.inc"
}


#include "CUserCharacInfo/SetOverTradeGoldCount.inc"
}

#include "CUserCharacInfo/SetOpencoin.inc"
}




#include "CUserCharacInfo/SetOneDayLetheFlag2ND.inc"
}


#include "CUserCharacInfo/SetOneDayLetheFlag.inc"
}


#include "CUserCharacInfo/SetLoginTick.inc"
}

#include "CUserCharacInfo/SetIsEndLetheAtLogin.inc"
}


#include "CUserCharacInfo/SetInvisible.inc"
}

#include "CUserCharacInfo/SetFirstTimeLogin.inc"
}

#include "CUserCharacInfo/SetCurRevengeDungeonCount.inc"
}


#include "CUserCharacInfo/SetCurExpEquipSlotStat.inc"
}


#include "CUserCharacInfo/SetCurCharacSuperState.inc"
}


#include "CUserCharacInfo/SetCurCharacSkillTreeIndex.inc"
}


#include "CUserCharacInfo/SetCurCharacRevengeData.inc"
}


#include "CUserCharacInfo/SetCurCharacOption.inc"
}




#include "CUserCharacInfo/SetCurCharacOnlinePreliminaryTeam.inc"
}


#include "CUserCharacInfo/SetCurCharacMaxEquipLevel.inc"
}


#include "CUserCharacInfo/enableSaveCharacManageInfo.inc"

#include "CUserCharacInfo/SetCurCharacLuckPointUpdateTimeCount.inc"
}


#include "CUserCharacInfo/SetCurCharacLeaveTimeout.inc"
}


#include "CUserCharacInfo/SetCurCharacGhost.inc"
}


#include "CUserCharacInfo/SetCurCharacExpertJobType.inc"
}


#include "CUserCharacInfo/enableSaveCharacInfo.inc"

#include "CUserCharacInfo/SetCurCharacExpertJobExp.inc"
}


#include "CUserCharacInfo/SetCurCharacExpertJob.inc"
}


#include "CUserCharacInfo/SetCurCharacEmotion.inc"
}

#include "CUserCharacInfo/SetCurCharacChaosPoint.inc"
}


#include "CUserCharacInfo/SetCurCharacChaosKillTime.inc"
}


#include "CUserCharacInfo/SetCurCharacAssaultedUserId.inc"
}


#include "CUserCharacInfo/SetCurCharacAssaultMoney.inc"
}


#include "CUserCharacInfo/SetCurCharacAssaultFatigueCount.inc"
}


#include "CUserCharacInfo/SetCharacStartPlayTick.inc"

#include "CUserCharacInfo/SetAssaultPlace.inc"
}


#include "CUserCharacInfo/ResetRevengeDungeonClear.inc"
}


#include "CUserCharacInfo/ResetPowerWarAssaultRecord.inc"
}


#include "CUserCharacInfo/ResetCurCharacEventItemCount.inc"
}


#include "CUserCharacInfo/ResetCurCharacEventAddItemCount.inc"
}


#include "CUserCharacInfo/ResetCurCharacDungeonPlayCount.inc"
}

#include "CUserCharacInfo/LiveCurCharac.inc"
}


// b28d-CUser P292：ORIG 0x08696976 `test;je;movzbl 0x1244;ret`（无 setne 归一化）
//   ⇒ 返回 bool 且三目两臂同型（字段已 bool 化）。
#include "CUserCharacInfo/IsVillageAttackDBUpdate.inc"
}

#include "CUserCharacInfo/IsSaveDemensionInoutValue.inc"
}

#include "CUserCharacInfo/IsSaveCharacStat.inc"

#include "CUserCharacInfo/IsSaveCharacManageInfo.inc"

#include "CUserCharacInfo/IsSaveBloodInoutValue.inc"
}

#include "CUserCharacInfo/IsProperDungeonUser.inc"
}

#include "CUserCharacInfo/IsFirstTimeLogin.inc"
}

#include "CUserCharacInfo/IsExpItemAffected.inc"
}


#include "CUserCharacInfo/IsEndLetheAtLogin.inc"
}


#include "CUserCharacInfo/IsCurCharacVisible.inc"
}


#include "CUserCharacInfo/IsCurCharacLeaveTimeout.inc"
}

#include "CUserCharacInfo/IsBusyDisjointStore.inc"
}


// [b28d-L14 N82附/N18a] ORIG 0x0864657e：ORIG 用 %edi 跨 call 保活（push edi，帧 0x4c）
// vs 4.4 槽化（帧 0x40）；`mov (%eax),%ebx` 4.4 折单段。p167 CLEAR（8/8，语义完整）。
#include "CUserCharacInfo/IsAvailableCurCharacTradeGoldDaily.inc"
}

#include "CUserCharacInfo/getCurCharacName.inc"
}


#include "CUserCharacInfo/InitFinishPoint.inc"
}


#include "CUserCharacInfo/IncrementCurCharacTradeGoldDaily.inc"
}

// ORIG `movzwl 0x121f(%edx),%edx;mov %edx,%ecx` 二段；4.4 折单步 `movzwl …,%ecx`
// （N18a：4.4 恒折 mem→%reg→callee-saved 两段），寄存器分配差异，语义等价。
// [b28d-solo-CUserCharacInfo N18a 复核] `+=` 形实测同形（movzwl mem,%ecx 单段）；
// 字段为 16 位 ushort，无法按 P254 改 enum（宽度变 4 破坏 +0x121f 布局）⇒ 封顶。
#include "CUserCharacInfo/IncreasePowerWarPoint.inc"
}


#include "CUserCharacInfo/IncProperDungeonClearCount.inc"
}


#include "CUserCharacInfo/IncPowerWarAssaultVictoryCount.inc"
}


#include "CUserCharacInfo/IncPowerWarAssaultCount.inc"
}


// ORIG 0x086566e4：`movzwl 0x4f,%edx;mov %edx,%ecx` 二段（4.4 折单步 N18a）；两处比较
// ORIG 直 `cmp -0xa(%ebp),%ax` 折返，4.4 先物化寄存器再 `cmp %ax,%dx`，jl/jle 落点相位同，
// 无一分支反向，语义等价。
// [b28d-solo-CUserCharacInfo AE] 49=49：P254 packed enum 视角（EFatigueBatteryValue /
// m_fatigueBatteryEnum）⇒ 累加段两段 regmove 复现（old/比较段维持 ushort 视角不变）。
#include "CUserCharacInfo/IncFatigueBatteryCharging.inc"
}




#include "CUserCharacInfo/IncCurConditionEventStep.inc"
}




#include "CUserCharacInfo/IncCurConditionEventRewardStep.inc"
}




#include "CUserCharacInfo/IncCurCharacLuckPointUpdateTimeCount.inc"
}




#include "CUserCharacInfo/IncCurCharacLuckPoint.inc"
}

#include "CUserCharacInfo/IncCurCharacEventAddItemCount.inc"
}


#include "CUserCharacInfo/IncCurCharacDungeonPlayCount.inc"
}

#include "CUserCharacInfo/SetCurCharacDungeonPlayCount.inc"
}


#include "CUserCharacInfo/IncCurCharacChaosModeCount.inc"
}

#include "CUserCharacInfo/SetCurCharacChaosModeCount.inc"
}


#include "CUserCharacInfo/IncCurCharacChaosKillCount.inc"
}

#include "CUserCharacInfo/SetCurCharacChaosKillCount.inc"
}


#include "CUserCharacInfo/IncCurCharacChaosDieCount.inc"
}

#include "CUserCharacInfo/SetCurCharacChaosDieCount.inc"
}


#include "CUserCharacInfo/IncCurCharacAssaultFatigueCount.inc"
}


#include "CUserCharacInfo/IncCurCharacAssaultCount.inc"
}

#include "CUserCharacInfo/SetAssaultCount.inc"
}


#include "CUserCharacInfo/GmSetCurCharacChaosPoint.inc"
}


// [b28d-CPrivateStoreMgr] 返回类型判据（ORIG 0x085cbd38 + 探针 f15/f16）：unsigned int。
#include "CUserCharacInfo/GetWinPoint.inc"

#include "CUserCharacInfo/GetWarAreaKillCountTotal.inc"
}


#include "CUserCharacInfo/GetWarAreaKillCount.inc"
}

#include "CUserCharacInfo/GetUserStateByAssault.inc"
}


#include "CUserCharacInfo/GetUserPowerWarPoint.inc"
}

// [b28d-CPrivateStoreMgr] 返回类型判据（ORIG 0x085c8acf xor 形 + P292/T3）：bool。
#include "CUserCharacInfo/GetReliablePerson.inc"

#include "CUserCharacInfo/GetProperDungeonClearCount.inc"
}

#include "CUserCharacInfo/GetPowerWarProcessInfoLastUpdateTime.inc"
}

#include "CUserCharacInfo/GetPowerWarLastPlayTime.inc"
}
#include "CUserCharacInfo/SetIsInitSkillFlag.inc"
}

#include "CUserCharacInfo/SetIsInitSkillFlag2ND.inc"
}

#include "CUserCharacInfo/IncVillageAttackCount.inc"
}

#include "CUserCharacInfo/GetPowerWarAssaultVictoryCount.inc"
}

#include "CUserCharacInfo/GetPowerWarAssaultCount.inc"
}

#include "CUserCharacInfo/GetOverTradeGoldCount.inc"

#include "CUserCharacInfo/GetOpencoin.inc"
}

#include "CUserCharacInfo/GetOneDayLetheFlag2ND.inc"
}

#include "CUserCharacInfo/GetOneDayLetheFlag.inc"
}

#include "CUserCharacInfo/GetLoginTick.inc"
}


#include "CUserCharacInfo/GetFinishPointTotal.inc"
}



#include "CUserCharacInfo/GetFinishPoint.inc"
}


#include "CUserCharacInfo/GetFatigueBatteryCharging.inc"
}

#include "CUserCharacInfo/GetFatigue.inc"
}

#include "CUserCharacInfo/GetExpAffectRate.inc"
}


#include "CUserCharacInfo/GetCurVillageAttackCount.inc"
}

#include "CUserCharacInfo/GetCurRevengeDungeonCount.inc"
}

#include "CUserCharacInfo/GetCurExEquipSlotStat.inc"
}

#include "CUserCharacInfo/GetCurConditionEventStep.inc"
}

#include "CUserCharacInfo/GetCurConditionEventRewardStep.inc"
}

#include "CUserCharacInfo/GetCurCharacSuperState.inc"
}

#include "CUserCharacInfo/GetCurCharacSkillTreeIndex.inc"
}

#include "CUserCharacInfo/GetCurCharacOnlinePreliminaryTeamName.inc"
}



#include "CUserCharacInfo/GetCurCharacOnlinePreliminaryTeamId.inc"
}



online_preliminary::COnlinePreliminaryTeam*
#include "CUserCharacInfo/GetCurCharacOnlinePreliminaryTeam.inc"
}


#include "CUserCharacInfo/GetCurCharacMaxEquipLevel.inc"
}

#include "CUserCharacInfo/GetCurCharacLuckPointUpdateTimeCount.inc"
}

#include "CUserCharacInfo/GetCurCharacExpertJobType.inc"
}

#include "CUserCharacInfo/GetCurCharacExpertJobExp.inc"
}

#include "CUserCharacInfo/GetCurCharacExpertJob.inc"
}

#include "CUserCharacInfo/GetCurCharacEventItemCount.inc"
}

#include "CUserCharacInfo/GetCurCharacEventAddItemCount.inc"
}

#include "CUserCharacInfo/GetCurCharacEmotion.inc"
}

#include "CUserCharacInfo/GetCurCharacDungeonPlayCount.inc"
}

#include "CUserCharacInfo/GetCurCharacChaosStateTime.inc"
}

#include "CUserCharacInfo/GetCurCharacChaosResponTime.inc"
}

#include "CUserCharacInfo/GetCurCharacChaosModeCount.inc"
}

#include "CUserCharacInfo/GetCurCharacChaosKillTime.inc"
}

#include "CUserCharacInfo/GetCurCharacChaosKillCount.inc"
}

#include "CUserCharacInfo/GetCurCharacChaosExp.inc"
}

#include "CUserCharacInfo/GetCurCharacChaosDieTime.inc"
}

#include "CUserCharacInfo/GetCurCharacChaosDieCount.inc"
}

#include "CUserCharacInfo/GetCurCharacBackupSkill2ND.inc"
}

#include "CUserCharacInfo/GetCurCharacBackupSkill.inc"
}

#include "CUserCharacInfo/GetCurCharacAssaultedUserId.inc"
}

#include "CUserCharacInfo/GetCurCharacAssaultMoney.inc"
}

#include "CUserCharacInfo/GetCurCharacAssaultFatigueCount.inc"
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
#include "CUserCharacInfo/GetContinuousItemInfo.inc"
}

#include "CUserCharacInfo/GetCurCharacEffectItemListR.inc"
}

#include "CUserCharacInfo/GetCurCharacCoolTimeItemListR.inc"
}
#include "CUserCharacInfo/GetCharacPlayTick.inc"
}

#include "CUserCharacInfo/GetCharacStartPlayTick.inc"

#include "CUserCharacInfo/GetAssaultPlace.inc"
}

#include "CUserCharacInfo/GetAssaultCount.inc"
}

#include "CUserCharacInfo/GainCurCharacChaosExp.inc"
}

// [b28d-L14 N107+控制流] ORIG 0x08680e04：`lea 0xdaa(%edx),%ecx` vs 4.4 `add $0xdaa,%eax`
// （N107 成员常量偏移取址六形全败）；否定早返布局（P59/P222）。p167 CLEAR（6/6）。
// [b28d-solo-CUserCharacInfo AE] 85=85：①守卫独立早返（P59）已落；②CCargo/CInventory::
// FindItemLock bool 化（Main 代落）；③N107 两处 add-vs-lea 由 P106 真实类型成员取址
// 归零 —— m_cargo 由 char[0x14] 改真实 CCargo（CUserCharacInfo.h），m_inven 经 pack(1)
// CharacInvenView 真实 CInventory 成员访问 ⇒ `lea 0xdaa(%edx),%ecx` / `lea 0xda(%edx),%ecx`
// 逐条复现（数组退化形恒产 add）。
#include "CUserCharacInfo/FindItemLock.inc"
}

// [b28d-L14 N18a/N95] ORIG 0x08659c50：增量段 ppEi ⇒ 源写 `it++`（P28，已改）；
// 残差 `mov (%eax),%eax; mov %eax,%ebx` 4.4 折单段 + 槽 -0x28 vs -0x24。
// [b28d-solo-CUserCharacInfo AE] 95=95：①`return 0;`→`break;`（P214② 共享尾块）已落；
// ②P254 enum 视角：ContinuousItemInfo 加匿名 union 成员 EContinuousItemIdx
// m_itemIdxEnum（4 字节等宽，原 m_itemIdx int 全仓消费点不变）⇒ 复现 ORIG
// `mov (%eax),%eax; mov %eax,%ebx` 两段中转。
#include "CUserCharacInfo/EnablePeriodExpItem.inc"
}



#include "CUserCharacInfo/SetExpAffectItemEffect.inc"
}


#include "CUserCharacInfo/EnablePeriodExpDouble.inc"
}



#include "CUserCharacInfo/SetIsAffectedExpDouble.inc"
}




// [b28d-solo-CUserCharacInfo AE] 三旋钮一次归零（108=108）：
// ①P0 删具名 `CUserCharacInfo* other` 局部（ORIG 0x0864633a/0x0864634d 两次重读
//   0x10(%ebp) 形参直用）；
// ②P198②/P225 两处 bool 条件整体内联（ORIG 直测 `test %al,%al; je`，无 mov$1/mov$0 物化）；
// ③P252 具名 `float penalty` 局部（ORIG 0x086462f0 `fstps -0x10(%ebp)` + 回读压栈；
//   删 other 局部后槽位自洽，旧「复测反劣」系 other 占槽所致）。
#include "CUserCharacInfo/DieCurCharac.inc"
}


#include "CUserCharacInfo/get_charac_level.inc"
}

#include "CUserCharacInfo/getCurCharacVill.inc"
}


#include "CUserCharacInfo/SetCurCharacChaosResponTime.inc"
}


#include "CUserCharacInfo/SetCurCharacChaosDieTime.inc"
}


#include "CUserCharacInfo/IsCurCharacGhost.inc"
}

#include "CUserCharacInfo/GetCurCharacChaosPoint.inc"
}

#include "CUserCharacInfo/DecreaseWarPoint.inc"
}

#include "CUserCharacInfo/DecreaseRevengeDungeonTicket.inc"
}


// ORIG `movzwl 0x121f(%edx),%edx;mov %edx,%ecx` 二段；4.4 折单步（N18a），语义等价。
// 同 IncreasePowerWarPoint。
// [b28d-solo-CUserCharacInfo AE] 36=36：P254 packed enum 视角（EPowerWarPointValue /
// m_powerWarPointEnum）⇒ 减算段两段 regmove 复现。
#include "CUserCharacInfo/DecreasePowerWarPoint.inc"
}


// ORIG 0x08656784：`movzwl 0x4f,%edx;mov %edx,%ecx` 二段（4.4 折单步 N18a）；第二条件
// `mfat>old` ORIG 直 `cmp -0xa(%ebp),%ax;jle` 折返（跳返回侧），4.4 先物化 old 到 %eax 再
// `cmp %ax,%dx`，jle 落点/相位同 ORIG，无一分支反向，语义等价。
// [b28d-solo-CUserCharacInfo AE] 45=45：P254 packed enum 视角（m_fatigueBatteryEnum）。
#include "CUserCharacInfo/DecFatigueBatteryCharging.inc"
}




#include "CUserCharacInfo/DecCurCharacLuckPoint.inc"
}

#include "CUserCharacInfo/SetCurCharacLuckPoint.inc"
}


#include "CUserCharacInfo/GetCurCharacLuckPoint.inc"
}

#include "CUserCharacInfo/ClearUselessEffectItem.inc"
}

#include "CUserCharacInfo/GetCurCharacEffectItemList.inc"
}

#include "CUserCharacInfo/ClearUselessCooltimeItem.inc"
}

#include "CUserCharacInfo/GetCurCharacCoolTimeItemList.inc"
}

#include "CUserCharacInfo/ClearProperDungeonUser.inc"
}


#include "CUserCharacInfo/ClearProperDungeonClearCount.inc"
}

#include "CUserCharacInfo/SetProperDungeonClearCount.inc"
}


#include "CUserCharacInfo/ClearCurConditionEventStep.inc"
}

#include "CUserCharacInfo/SetCurConditionEventStep.inc"
}


#include "CUserCharacInfo/ClearCurConditionEventRewardStep.inc"
}

#include "CUserCharacInfo/SetCurConditionEventRewardStep.inc"
}


#include "CUserCharacInfo/ChangeFinishPoint.inc"
}

#include "CUserCharacInfo/CUserCharacInfo.inc"
}

#include "CUserCharacInfo/resetSaveFlags.inc"
}

#include "CUserCharacInfo/AddCurCharacChaosPoint.inc"
}


#include "CUserCharacInfo/enableSaveCharacStat.inc"
#include "CUserCharacInfo/GetTenThousandPercentage.inc"
}
