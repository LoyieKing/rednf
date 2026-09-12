#!/bin/sh
# game 逐 TU 编译器分发（2026-08-16 起）。
# 初始：全部 c6（c6root 4.4.7 驱动）。
# 后续按 df_game_r 二进制各 TU 的实际 cc1plus 变体逐步切 c6446r/c6444r，
# 判定口径：diff_func.py --bin game 逐函数 IDENTICAL/NEAR/DIFF 统计。
ROOT="${DNF_TC_ROOT:-$(cd "$(dirname "$0")/../../.." && pwd)/toolchains}"
C6="${ROOT}/c6root/usr/bin/g++"
TOOLDIR="$(cd "$(dirname "$0")" && pwd)"
C6446R="$TOOLDIR/dnf_c6446r_gxx.sh"
C6444R="$TOOLDIR/dnf_c6444r_gxx.sh"
# W7_BuildHarmonize：项目共享头使用 C++0x static_assert；下方被指令级对齐
# 强制 -std=gnu++98 的 TU 需 -include 注入 C++03 兼容展开（static_assert_compat.h），
# 否则 make 全量失败（单 TU 检查链统一 gnu++0x 故不复现该差异）。
SA_COMPAT="-include"
SA_HDR="$TOOLDIR/static_assert_compat.h"

export LD_LIBRARY_PATH="${ROOT}/c6root/usr/lib64:${ROOT}/c6root/usr/lib"

src=""
prev=""
for a in "$@"; do
    case "$a" in
        *.cpp) src="$a" ;;
    esac
done
# ---------------------------------------------------------------------------
# b28c 三态 sweep（/tmp/c46tri.tsv，796 TU 逐 TU 实测）：以下 TU 在「4.4.6 cc1plus
# + 4.4.6 C++ 头集」下与 ORIG 更接近（逐 TU 之和 AE +1231 / near −404 / diff −831）。
# 做法：把命令行里的 -isystem …/c++/4.4.7* 三项**原位**替换为 4.4.6 三项，使搜索序
# 与 sweep 一致（gcc 内建 include 之后、glibc 之前），避免 include_next 命中 4.4.7
# 同名包装头而被 include guard 空展开（fenv.h 等）。后端仍由下方 dispatch 决定。
# 排除包装器另行注入 -O2/-std=gnu++98/-fno-exceptions 的 TU（sweep 未按真实旗标测）。
# ---------------------------------------------------------------------------
H46="${ROOT}/lsd44/v4.4.6/usr/include/c++/4.4.6"
case "$src" in
    # [SemA09 slice09 · i486 轴 · P0.1 必守程序已过（Main 批准 2026-09-12）]
    #   ExpertJobLink4 原档 = 下方大队列 446h46（4.4.6 后端 + 4.4.6 头集原位替换）；
    #   本 case 置于大队列之前 ⇒ 同档再叠 `-march=i486 -mtune=generic`（N85/P230② x87 比较族）。
    #   A/B（唯一变量 = +i486；档位/头集/后端全同）：四数逐值不变
    #     （identical=96 ae=188 near=0 diff=6 missing=33）、`nm --defined-only | comm -3` = 0；
    #   改善：TU Σndiff 274→270，`expert_job::CDisjointer::complete_disjoint` ndiff 55→51 ——
    #     ORIG `fucompp;fnstsw %ax;test $0x45,%ah;sete` 复现，默认 4.4.7 档产 `fucomip/fstp/seta`。
    *Game/ExpertJobLink4.cpp)
        _newargs="-B${ROOT}/cc1plus446bin/ -march=i486 -mtune=generic"
        _swapped=""
        for _a in "$@"; do
            case "$_a" in
                *c++/4.4.7*)
                    if [ -z "$_swapped" ]; then
                        _swapped=1
                        _newargs="$_newargs -isystem${H46} -isystem${H46}/x86_64-redhat-linux -isystem${H46}/backward"
                    fi
                    continue ;;
            esac
            _newargs="$_newargs $_a"
        done
        # shellcheck disable=SC2086
        set -- $_newargs ;;
    # [R8a01 2026-09-12 · i486 轴 · P0.1 必守程序已过（Main 批准）]
    #   CBattle_Field：ORIG 档**无 CMOV** 实证 —— `CBattle_Field::set_dungeon_diff`
    #   ORIG 0x0830ed21..0x0830ed2d = `movzbl 0xc(%ebp),%edx; cmp $0x4,%edx;
    #   jbe +0x10; push $0x4; pop %edx; nop`（MIN_EXPR 的 4.1 条件跳转 lowering），
    #   而 4.4.7 默认档产 `cmova %edx,%eax`（P230①：`-march=i486` 消 cmov/MIN_EXPR 族）。
    #   本分支 = 原大队列 446h46 档（4.4.6 cc1plus + 4.4.6 头集原位替换）**再叠**
    #   `-march=i486 -mtune=generic`（两者正交；`-mtune=generic` 必须同带）。
    #   A/B（唯一变量 = +i486）与符号集/抽验见 /tmp/verify357/R8a01/report.md §④-构建档位修正。
    *Game/CBattle_Field.cpp)
        _newargs="-B${ROOT}/cc1plus446bin/ -march=i486 -mtune=generic"
        _swapped=""
        for _a in "$@"; do
            case "$_a" in
                *c++/4.4.7*)
                    if [ -z "$_swapped" ]; then
                        _swapped=1
                        _newargs="$_newargs -isystem${H46} -isystem${H46}/x86_64-redhat-linux -isystem${H46}/backward"
                    fi
                    continue ;;
            esac
            _newargs="$_newargs $_a"
        done
        # shellcheck disable=SC2086
        set -- $_newargs ;;
    *Game/ARAD.cpp | \
            *Game/Action.cpp | \
            *Game/AdditionalEffectData.cpp | \
            *Game/AradPeriodB.cpp | \
            *Game/Arad_AvatarRoulette.cpp | \
            *Game/Arad_Dispatcher.cpp | \
            *Game/Arad_OS.cpp | \
            *Game/Arad_Dispatch_Manager.cpp | \
            *Game/Arad_Dispatcher_BuyItemCheck_Impl.cpp | \
            *Game/Arad_IPGProcess.cpp | \
            *Game/Arad_MileageProcess.cpp | \
            *Game/Arad_InternalDispatcher.cpp | \
            *Game/Arad_DatabaseDispatcher.cpp | \
            *Game/Arad_ItemBonus.cpp | \
            *Game/CeraShopPurcahseCountBonus.cpp | \
            *Game/Arad_Script.cpp | \
            *Game/Arad_RDARScriptEtcParameter.cpp | \
            *Game/Arad_Verify.cpp | \
            *Game/Area.cpp | \
            *Game/AttendanceEvent.cpp | \
            *Game/AvatarFixedHiddenOptionServer.cpp | \
            *Game/BaseServerProxy.cpp | \
            *Game/BattleDepsB2.cpp | \
            *Game/BattleMonsterDepsC2.cpp | \
            *Game/BattleMonsterTailC3A.cpp | \
            *Game/BingoEventLink4.cpp | \
            *Game/BossTowerFunc.cpp | \
            *Game/CAICharacterList.cpp | \
            *Game/CAchievement.cpp | \
            *Game/CAssaultMgr.cpp | \
            *Game/CAuctionAveragePrice.cpp | \
            *Game/CBingoTest.cpp | \
            *Game/CBloodDungeonRewardEvents.cpp | \
            *Game/CBoosterGage.cpp | \
            *Game/CBossTower.cpp | \
            *Game/CCeraShopGoods.cpp | \
            *Game/CCeraShop_Special.cpp | \
            *Game/CCharacInformNotice.cpp | \
            *Game/CCharacter.cpp | \
            *Game/CChattingEmoticonList.cpp | \
            *Game/CClearReward.cpp | \
            *Game/CClientCrashDownLogStatistic.cpp | \
            *Game/CCommonStatisticsMgr.cpp | \
            *Game/CConditionEventManager.cpp | \
            *Game/CDataManager.cpp | \
            *Game/CDataManagerMethods.cpp | \
            *Game/CDataManagerScripts.cpp | \
            *Game/global_ScriptDeleteInvalidItem.cpp | \
            *Game/CDataManagerTailB.cpp | \
            *Game/CData_charac_expand.cpp | \
            *Game/CDeathTower.cpp | \
            *Game/CDeathTowerWinPointEvent.cpp | \
            *Game/CDungeon.cpp | \
            *Game/CEnvironment.cpp | \
            *Game/CEquipItem.cpp | \
            *Game/CEventStayTime.cpp | \
            *Game/CExpandEquipslot.cpp | \
            *Game/CExpertJobList.cpp | \
            *Game/CFairMatchRooms.cpp | \
            *Game/CFairPvPScore.cpp | \
            *Game/CFatigueAttendance.cpp | \
            *Game/CFieldLengthChecker.cpp | \
            *Game/CGameManager.cpp | \
            *Game/CGlobalEffectManager.cpp | \
            *Game/CGuildServerProxy.cpp | \
            *Game/CGuildWarEvent.cpp | \
            *Game/CHackAnalyzer.cpp | \
            *Game/Secu_AccountHacking.cpp | \
            *Game/CInventory.cpp | \
            *Game/CItem.cpp | \
            *Game/CItemDictionary.cpp | \
            *Game/W9Interval.cpp | \
            *Game/CDungeonClear.cpp | \
            *Game/CPartyTelePort.cpp | \
            *Game/AvatarConvertServer.cpp | \
            *Game/CItemList.cpp | \
            *Game/CItemShop.cpp | \
            *Game/CItemUpgrade.cpp | \
            *Game/CKillMonsterInfo_chain.cpp | \
            *Game/CLeagueMatch.cpp | \
            *Game/CListTables.cpp | \
            *Game/CLog.cpp | \
            *Game/CMailBoxHelper.cpp | \
            *Game/CMap.cpp | \
            *Game/CMission.cpp | \
            *Game/CMissionList_Charac.cpp | \
            *Game/CMonster.cpp | \
            *Game/CNCrypto.cpp | \
            *Game/CNotApplyBalkeunEvent.cpp | \
            *Game/COnTimeEvent.cpp | \
            *Game/CParty.cpp | \
            *Game/CPartyDepsB2.cpp | \
            *Game/CPartyDepsC2.cpp | \
            *Game/CPartyDepsD2.cpp | \
            *Game/CPartyExpBonusEvent.cpp | \
            *Game/CPowerWarVictoriousEvent.cpp | \
            *Game/CPrivateStoreMgr.cpp | \
            *Game/CQuestShop.cpp | \
            *Game/CReduceUpgradeItemPay.cpp | \
            *Game/CRidable.cpp | \
            *Game/CSecretShop.cpp | \
            *Game/SecretShopStatistic.cpp | \
            *Game/SecretShopData.cpp | \
            *Game/CSourceVersionMgr.cpp | \
            *Game/CStackableItem.cpp | \
            *Game/CTitleBook.cpp | \
            *Game/CUser.cpp | \
            *Game/CUserCharacInfo.cpp | \
            *Game/CUserDepsB.cpp | \
            *Game/CUserMissingA2.cpp | \
            *Game/CUserOnlyOrig.cpp | \
            *Game/CUserPremium.cpp | \
            *Game/CUserStatB.cpp | \
            *Game/CharacterAIA2.cpp | \
            *Game/ChecksumCrypto.cpp | \
            *Game/Cipher.cpp | \
            *Game/ContainerTemplateInstantiations.cpp | \
            *Game/CraneMinigameManager.cpp | \
            *Game/CreateDnfEventSciprt.cpp | \
            *Game/CreatureScript.cpp | \
            *Game/CtorProducers_1.cpp | \
            *Game/CtorProducers_2.cpp | \
            *Game/DB_AradDatabaseDispatcher.cpp | \
            *Game/DB_DispatcherTaiwan.cpp | \
            *Game/DB_LoadInventory.cpp | \
            *Game/DisPatcherCore.cpp | \
            *Game/DispLinkA1.cpp | \
            *Game/DispLinkA2.cpp | \
            *Game/Dispatcher_CompoundEmblem_impl.cpp | \
            *Game/Dispatcher_UseBoosterItem_impl.cpp | \
            *Game/Dispatcher_UseLimitCube_impl.cpp | \
            *Game/Dispatchers.cpp | \
            *Game/DomainMiscDeps.cpp | \
            *Game/DropLayoutDeps.cpp | \
            *Game/EmblemCompoundServer.cpp | \
            *Game/EncryptTool.cpp | \
            *Game/EpisodeScript.cpp | \
            *Game/EquipmentParameterInfo.cpp | \
            *Game/EventCharacterParameterScript.cpp | \
            *Game/EventClassify_Action.cpp | \
            *Game/EventNewCharacterReward.cpp | \
            *Game/EventRewardQuickPartyB2.cpp | \
            *Game/EventScriptsCompleteB2.cpp | \
            *Game/EventUpgradeJar.cpp | \
            *Game/ExchangeServer_Session.cpp | \
            *Game/ExpertExtraction.cpp | \
            *Game/ExpertJobLink4.cpp | \
            *Game/ProfessionDisjointer.cpp | \
            *Game/ExpertJobOnlyEnchanter.cpp | \
            *Game/ExpertJobOnlyMaterial.cpp | \
            *Game/ExpertJobOnlyMgr.cpp | \
            *Game/GameBasicsScripts.cpp | \
            *Game/GameWorld.cpp | \
            *Game/GlobalData.cpp | \
            *Game/DBConnector.cpp | \
            *Game/Teleport.cpp | \
            *Game/ItemVendingMachine.cpp | \
            *Game/GlobalScriptTutorial.cpp | \
            *Game/GmCmds_D.cpp | \
            *Game/GmCmds_E.cpp | \
            *Game/GmCore.cpp | \
            *Game/GmMacro.cpp | \
            *Game/HeroMissionEvent.cpp | \
            *Game/HeroMissionLink4.cpp | \
            *Game/InitScriptProducers_1.cpp | \
            *Game/InitScriptProducers_2.cpp | \
            *Game/InitScriptProducers_3.cpp | \
            *Game/InitScriptProducers_4.cpp | \
            *Game/InitScriptProducers_6.cpp | \
            *Game/InitScriptProducers_7.cpp | \
            *Game/InitScriptProducers_8.cpp | \
            *Game/InterDispatcher.cpp | \
            *Game/ItemUpgradeVtableB2.cpp | \
            *Game/LC_Misc.cpp | \
            *Game/LevelupSupportLeafC3.cpp | \
            *Game/LogManager.cpp | \
            *Game/LwCallback.cpp | \
            *Game/MonsterAbilityLeafC3.cpp | \
            *Game/MonsterIndependentDrop.cpp | \
            *Game/MonsterWrapLCB.cpp | \
            *Game/MultiBoxLotteryInfo.cpp | \
            *Game/NPCScript.cpp | \
            *Game/NewAccountLevelUpEventToJob.cpp | \
            *Game/OnlinePreliminaryMatch.cpp | \
            *Game/OnlyOrigBatch11.cpp | \
            *Game/RDARScriptEventUpgradeJar.cpp | \
            *Game/greg_month.cpp | \
            *Game/RentalSystem.cpp | \
            *Game/RDARScriptDisjoint.cpp | \
            *Game/RDARScriptSecretShop.cpp | \
            *Game/RDARScriptFairPvPStat.cpp | \
            *Game/RDARScriptTrainingQuest.cpp | \
            *Game/RDARScriptGuildParameter.cpp | \
            *Game/RDARScriptPowerParameter.cpp | \
            *Game/RDARScriptQuestParameter.cpp | \
            *Game/RDARScriptInGameAdvertisement.cpp | \
            *Game/PacketDispatcher_Secu.cpp | \
            *Game/localchina_PacketDispatcher.cpp | \
            *Game/HackLog_InvalidUsingSkill.cpp | \
            *Game/RegenerationROI.cpp | \
            *Game/Utility.cpp | \
            *Game/CaptchaGen.cpp | \
            *Game/RDARScriptQuickParty.cpp | \
            *Game/DispatchThread.cpp | \
            *Game/Channel.cpp | \
            *Game/RDARScriptCeraShop.cpp | \
            *Game/log.cpp | \
            *Game/CSCommonFunction.cpp | \
            *Game/OneADayItemShopEvent.cpp | \
            *Game/RDARScriptChattingEmoticon.cpp | \
            *Game/RDARScriptRegeneration_ROI.cpp | \
            *Game/NetworkThread.cpp | \
            *Game/RDARScriptQuest.cpp | \
            *Game/RDARScriptDungeon.cpp | \
            *Game/RDARScriptSkill.cpp | \
            *Game/RDARScriptObject.cpp | \
            *Game/RDARScriptCharacManage.cpp | \
            *Game/SimpleSSO.cpp | \
            *Game/SkillChanger.cpp | \
            *Game/OnlyOrigBatch12_rand_r.cpp | \
            *Game/OnlyOrigBatch12_security_server.cpp | \
            *Game/OnlyOrigBatch12_server_utility.cpp | \
            *Game/OnlyOrigBatch14.cpp | \
            *Game/PackSet.cpp | \
            *Game/PacketDispatcher.cpp | \
            *Game/PacketTypes.cpp | \
            *Game/PartyClosureC2.cpp | \
            *Game/PvP_MissionSystem.cpp | \
            *Game/PvP_Room.cpp | \
            *Game/Quest.cpp | \
            *Game/RDARScriptEtcParameter.cpp | \
            *Game/RDARScriptStringManager.cpp | \
            *Game/RandomOptionScript.cpp | \
            *Game/RandomRentalA2.cpp | \
            *Game/RestrictNameDeps.cpp | \
            *Game/STAssignMonster.cpp | \
            *Game/STDeadTowerDialog.cpp | \
            *Game/STDungeonScript.cpp | \
            *Game/STEquipmentScript.cpp | \
            *Game/STItemScriptNested.cpp | \
            *Game/STMapScript.cpp | \
            *Game/STMonsterScript.cpp | \
            *Game/STQuestScript.cpp | \
            *Game/STSkillScript.cpp | \
            *Game/STStackableScript.cpp | \
            *Game/ScriptDepsA.cpp | \
            *Game/ScriptDepsB.cpp | \
            *Game/ScriptExtras.cpp | \
            *Game/ScriptLeafA2.cpp | \
            *Game/Secu_DungeonAverageCheck.cpp | \
            *Game/SeriaRoomDecoObjectList.cpp | \
            *Game/ServerParameterScript.cpp | \
            *Game/ServiceRestrictManager.cpp | \
            *Game/SkillShopGuideTextData.cpp | \
            *Game/SkillSlot.cpp | \
            *Game/SlangFlexLexerEngine.cpp | \
            *Game/StScriptImporters.cpp | \
            *Game/StackablePackage.cpp | \
            *Game/StageMapScriptC2.cpp | \
            *Game/Store.cpp | \
            *Game/StrikerWorldDropB2.cpp | \
            *Game/StringTypes.cpp | \
            *Game/TaiwanDBDispatcher.cpp | \
            *Game/TaiwanInternalDispatcher.cpp | \
            *Game/TimerDispatcher.cpp | \
            *Game/TimerQueue.cpp | \
            *Game/TournamentFinalC3.cpp | \
            *Game/TournamentTailC3C.cpp | \
            *Game/TownAreaScript.cpp | \
            *Game/UsedFatigueRewardItem.cpp | \
            *Game/UserPartyLink4.cpp | \
            *Game/UserQuest.cpp | \
            *Game/ValueCtors.cpp | \
            *Game/W6_CeraShopDeps.cpp | \
            *Game/W7_UserOnlyC.cpp | \
            *Game/W8LT_DBTask.cpp | \
            *Game/GiveGrowCreatureEvent.cpp | \
            *Game/SeriaRoomAniDecoEvent.cpp | \
            *Game/W8_CU_G.cpp | \
            *Game/W8_EventClassifyB.cpp | \
            *Game/WarRoom.cpp | \
            *Game/WongWorkDrop_A.cpp | \
            *Game/WongWorkDrop_B.cpp | \
            *Game/WongWorkEvents_A.cpp | \
            *Game/WongWorkMiscB_A.cpp | \
            *Game/advancealtar_Manager.cpp | \
            *Game/advancealtar_ProcStage.cpp | \
            *Game/advancealtar_StageControl.cpp | \
            *Game/advancealtar_StageTimeline.cpp | \
            *Game/advancealtar_misc.cpp | \
            *Game/cUserHistoryLog.cpp | \
            *Game/randomMessageBalloonInfo.cpp)
        _newargs="-B${ROOT}/cc1plus446bin/"
        _swapped=""
        for _a in "$@"; do
            case "$_a" in
                *c++/4.4.7*)
                    if [ -z "$_swapped" ]; then
                        _swapped=1
                        _newargs="$_newargs -isystem${H46} -isystem${H46}/x86_64-redhat-linux -isystem${H46}/backward"
                    fi
                    continue ;;
            esac
            _newargs="$_newargs $_a"
        done
        # shellcheck disable=SC2086
        set -- $_newargs ;;
esac

# b28c 三态 sweep：以下 TU 仅换 4.4.6 后端（头集仍 4.4.7）最优（合计 AE +26）。
case "$src" in
    *Game/CPowerWarEvent.cpp | \
    *Game/Inter_DispatcherTaiwan.cpp | \
    *Game/Arad_MomijiEvent.cpp | \
            *Game/CAradRyosikaEvent.cpp | \
            *Game/CAssaultOnOffEvent.cpp | \
            *Game/CBurnigGoldMonsterEvent.cpp | \
            *Game/CCollectArchieveEventLog.cpp | \
            *Game/CConditionEvent.cpp | \
            *Game/CDimensionActivationEvent.cpp | \
            *Game/CDnFLeaguePromoteFirstEvent.cpp | \
            *Game/CDnFLeaguePromoteSecondEvent.cpp | \
            *Game/CDoubleGoldCardEvent.cpp | \
            *Game/CEventCeraShopRewardPoint.cpp | \
            *Game/CFatigueGiveItemEvent.cpp | \
            *Game/CPCRoomPlayTimeEvent.cpp | \
            *Game/CPCRoomWorldDropEvent.cpp | \
            *Game/CPCRoomWorldDropEvent2nd.cpp | \
            *Game/CPremiumGoldCard.cpp | \
            *Game/CSecretShopEvent.cpp | \
            *Game/CStopOverlabExpEvent.cpp | \
            *Game/CUXGameLogEvent.cpp | \
            *Game/CVendingMachineBonusEvent.cpp | \
            *Game/CVillageObjectMgr.cpp | \
            *Game/LevelupSupport2ndEvent.cpp | \
            *Game/LevelupSupportEvent.cpp | \
            *Game/OnlinePreliminaryTeam.cpp | \
            *Game/Inter_DispatcherAradOnly.cpp | \
            *Game/AvatarConvertServer.cpp | \
            *Game/DB_LoadEtc.cpp | \
            *Game/GmCmds_A.cpp | \
            *Game/Encryption.cpp | \
            *Game/DB_Login.cpp | \
            *Game/UserMiscLink.cpp | \
            *Game/CharacBloodDungeon.cpp | \
            *Game/AutoMarketConditionsControlEvent.cpp | \
            *Game/W9_EventSmall.cpp)
                # b28d-S419：DB_LoadEtc 的间接 call 目标寄存器同族（ORIG 0x08405c9e
                # `mov (%eax),%edx; mov -0x14(%ebp),%eax; call *%edx` vs 4.4.7 %eax）。
                # 实测 4.4.7 identical=4/ae=46/near=2/diff=14 → 4.4.6 identical=4/
                # ae=48/near=0/diff=14 ⇒ 零回归。
                # b28d-S314：Inter_DispatcherAradOnly::dispatch_sig 的 N7（间接 call
                # 目标 %eax↔%edx）属后端敏感族；4.4.7 下 NEAR（助记符全等），
                # 换 446 后端（-B cc1plus446bin）即 AE 归零。
                # 实测 447 identical=0/ae=4/near=1/diff=2 → 446 identical=0/ae=5/
                # near=0/diff=1（唯一 diff 为 N123 撞名噪音）⇒ 零回归。
                # b28d-S353：AvatarConvertServer::StatisticsAvatarDisjoint 的间接 call
                # 目标寄存器同族（ORIG %edx vs 4.4.7 %eax）。实测 4.4.7
                # identical=97/ae=181/near=1/diff=6 → 4.4.6 identical=97/ae=182/
                # near=0/diff=6 ⇒ 零回归。
        set -- -B"${ROOT}/cc1plus446bin/" "$@" ;;
esac

# [LaneC Round13 · §P0.1 五步落地] h46-only 块：仅把 3 项 `c++/4.4.7` isystem **原位**替换为
#   4.4.6 头集，**不加** `-B cc1plus446bin`（后端保持默认 4.4.7）。用于「头集要 46、后端要 47」的 TU。
#   BlueMarble（Main 已放行）：现档 446h46 = identical=90 ae=196 near=4 diff=2（.o missing=1）
#   → 447h46 = **90/197/4/1**（.o missing=1）⇒ +1 AE / diff −1 且 missing 不增。
#   翻正件 `_ZNSt20__uninitialized_copyILb0EE18uninitialized_copyISt13move_iteratorIP18BlueMarbleUserInfoES4_EET0_T_S7_S6_`
#   （ORIG 侧 DIFF→AE）；该件虽为 W，但全 build 目录 802 个 .o 中**仅 BlueMarble.cpp.o 发射**（逐 .o nm 核证）
#   ⇒ 链接口径可兑现（无 weak/COMDAT 高估）。
#   A/B 唯一变量=后端：`nm --defined-only | comm -3` 逐名零差（457=457）、`nm -u` 零差。
#   反证：447h47（同去头集）missing 1→2 属劣化；446h47（去头集保 446 后端）90/195/4/3 亦劣 ⇒ 头集必须保留。
case "$src" in
    *Game/BlueMarble.cpp)
        _newargs=""
        _swapped=""
        for _a in "$@"; do
            case "$_a" in
                *c++/4.4.7*)
                    if [ -z "$_swapped" ]; then
                        _swapped=1
                        _newargs="$_newargs -isystem${H46} -isystem${H46}/x86_64-redhat-linux -isystem${H46}/backward"
                    fi
                    continue ;;
            esac
            _newargs="$_newargs $_a"
        done
        set -- $_newargs ;;
esac

case "$(basename "$src" .cpp)" in
    *)
        case "$src" in
            *Game/CEnvironment.cpp)
                # CEnvironment：ORIG 为 4.4.6（虚调用/寄存器分配验证：4.4.7 下
                # load 的虚调用目标寄存器不同 → near；4.4.6 逐操作数一致）
                exec "$C6446R" "$@" ;;
            *Game/LogManager.cpp)
                # LogManager：ORIG DWARF producer = Red Hat GCC 4.4.6-3
                # （P2 组验证：4.4.6 下 logString 等虚调用寄存器逐操作数一致）
                exec "$C6446R" "$@" ;;
            *Game/BaseServerProxy.cpp)
                # BaseServerProxy：ORIG 为 4.4.6（虚调用目标寄存器 edx 形态 +
                # OnTcpRecv/OnTcpSend/Disconnect 寄存器分配：4.4.7 下 3 个 near；
                # 4.4.6 下 11/12 AE，仅 InitTcp 差 errnoVal/ret 寄存器互换）
                exec "$C6446R" "$@" ;;
            *Game/CHadesServerProxy.cpp)
                # CHadesServerProxy：本 TU 无虚调用，c6/c6446r 均 16/16 AE；
                # 与 BaseServerProxy 同批（E1），统一走 c6446r 保持一致性
                exec "$C6446R" "$@" ;;
            *Game/CItemList.cpp)
                # CItemList：ORIG 为 4.4.6（destroy/create_item 虚调用寄存器形态
                # + FindGoods 迭代器槽位：4.4.6 下 identical=147 ae=675（合计 822）
                # near 3→1，diff 保持 8；4.4.7 下合计 820）
                # [Main·2026-09-11 Round9 · 444 全量 sweep 候选] 换 4.4.4 后端：
                # RouteC A/B（同 argv 仅换 -B）：695/1359/6/13 → **695/1362/6/10**（ae+3/diff−3，
                # 3×DIFF→AE @ORIG 0832de18/0832dd1c/0832d15e），符号集 comm-3 零差。
                exec "$C6444R" "$@" ;;
            *Game/STDungeonScript.cpp)
                # STDungeonScript（Import/exportDungeonScript）：ORIG 为 4.4.6。
                # 字节级 identical 需 4.4.6 产生与 ORIG 相同栈布局/寄存器分配。
                exec "$C6446R" "$@" ;;
            # [Main·2026-09-11 Round8 路由 A/B 实测 · c46h7sweep A3 次候选兑现]
            # 7 TU：仅切 4.4.6 后端（头保持 4.4.7）⇒ 各 +1 AE（NEAR→AE）且符号集
            # `comm -3` 逐名一致、missing 不增 ⇒ 零回归（RouteB 报告 /tmp/lane_routeB/report.md）。
            #   CBattle_Field_KillChain 45/76/1/2 → 45/77/0/2
            #   CMaxFatigueFactorEvent   4/12/1/1 → 4/13/0/1
            #   CUserStatA               9/16/1/1 → 9/17/0/1
            #   DispLinkA3               4/14/1/1 → 4/15/0/1
            #   EventBattleLink          27/29/1/2 → 27/30/0/2
            #   LinkClosureA_Misc        1/2/1/1  → 1/3/0/1
            #   GameMain                 62/212/3/3 → 62/213/1/4（non-identical 6→5；
            #     round5 曾判回退，本波源码已变后重测为净改善）
            *Game/CBattle_Field_KillChain.cpp)
                # [Main·2026-09-11 Round10 · i486 轴] +`-march=i486 -mtune=generic`：
                #   1 件 DIFF→AE，四数 45/77/0/2 → **45/78/0/1**（comm-3=0、missing 不变）。
                exec "$C6446R" "$@" -march=i486 -mtune=generic ;;
            *Game/CMaxFatigueFactorEvent.cpp | \
            *Game/MaxFatigueFactorEvent.cpp | \
            *Game/CUserStatA.cpp | \
            *Game/DispLinkA3.cpp | \
            *Game/EventBattleLink.cpp | \
            *Game/LinkClosureA_Misc.cpp | \
            *Game/GameMain.cpp | \
            *Game/RDARScriptNpc.cpp)
                exec "$C6446R" "$@" ;;
            *Game/Inter_LoadEtc_impl.cpp)
                # [Main·2026-09-11 Round8 路由 A/B] c46h7sweep 主候选②「撤头替换、留后端」：
                # 从 H46 块移出（头保持 4.4.7）+ 切 4.4.6 cc1plus（446h47）⇒
                # 手工两态口径 identical 66→**67**、ae 102 不变（missing .o 级 1→2，
                # 属头/后端带出的模板实例，整库口径由收口核账覆核；劣化即回滚）。
                exec "$C6446R" "$@" ;;
            *Game/Arad_Misc.cpp)
                # [Main·2026-09-11 Round7 路由 A/B 实测] c46h7sweep 全量三态主候选之一，
                # Main 真实口径复测（flags.make 旗标 + 4.4.7 头 + 4.4.6 cc1plus）：
                #   默认(C6/4.4.7)   = identical=1 ae=7 near=3 diff=1 missing=0
                #   446h47/446h46    = identical=1 **ae=9 near=1** diff=1 missing=0
                #   ⇒ `ARAD::ARAD_CRYPT::encryptAES` 与 `ARAD::encryptAES` 两件 NEAR→AE，
                #     零回归（其余件读数不变）。
                exec "$C6446R" "$@" ;;
            # [Main·2026-09-11 Round10 · FeasB 全量真口径 3 轴扫描 · i486 轴（+18 件，零 down）]
            #   机理 = 恢复 ORIG 4.1 的 x87 比较 lowering（N85）：`fucompp;fnstsw %ax;test $0x45,%ah;sete`
            #   vs 4.4.7 默认 `fucomip %st(1),%st;fstp;seta`。arch 对照 i386≡i486≡i586 最优、i686 无收益；
            #   `-mtune=generic` 必须同带（历史：单独 -march=i486 会崩）。
            #   各 TU 增益（全部 comm-3=0、missing 不变、零 down）：CNRDScriptData +3｜GmCore +3｜CSkill +2(NEAR)｜
            #   BattleMonsterTailC3B +1｜CItemGenerator +1｜ContainerTemplateInstantiations +1｜
            #   Dispatchers +1(NEAR)｜RestrictGeolocation +1。
            *Game/CNRDScriptData.cpp | *Game/CSkill.cpp | *Game/BattleMonsterTailC3B.cpp | \
            *Game/CSkillCanLearn.cpp | *Game/CSkillMaxSkillLevel.cpp | \
            *Game/CItemGenerator.cpp | *Game/RestrictGeolocation.cpp | \
            *Game/ContainerTemplateInstantiations.cpp | *Game/Dispatchers.cpp | *Game/GmCore.cpp)
                exec "$C6" "$@" -march=i486 -mtune=generic ;;
            # [Main·2026-09-11 Round9 · 444 后端 sweep 候选（RouteC 全量四态 + 逐条 A/B 确认；
            #  同 argv 仅换 -B cc1plus444bin/，符号集 comm-3 零差、missing 不变）]
            #   CLeagueMatch    44/63/1/16  → 44/76/1/3   （+13 AE，13×DIFF→AE）
            #   STEquipmentScript 710/1439/2/7 → 710/1440/2/6（+1）
            #   TimerDispatcher 60/703/5/9  → 60/706/5/6  （+3）
            #   CCreatureMgr    180/415/3/6 → 180/416/2/6 （+1）
            #   （以上四条须同带 -march=i486 -mtune=generic，x87 族既有需求）
            *Game/CLeagueMatch.cpp | *Game/STEquipmentScript.cpp | \
            *Game/TimerDispatcher.cpp | *Game/CCreatureMgr.cpp)
                exec "$C6444R" "$@" -march=i486 -mtune=generic ;;
            #   CPowerManager   6/22/1/2    → 6/23/1/1    （+1；默认 447 档，无 i486 需求）
            *Game/CPowerManager.cpp)
                exec "$C6444R" "$@" ;;
            # [SplitC batch6 · BScan B 类真增量] MatchingSystem 虚析构拆壳 + 444 后端 ⇒ NEAR→AE（ndiff 6→0）。
            #   反证：整 TU 换 444/446 会把 GetMatchingSystem AE→DIFF（净负）⇒ 拆壳是必要条件（BScan/BClassOne 实测）。
            *Game/MatchingSystemDtor.cpp)
                exec "$C6444R" "$@" ;;
            *Game/CUser.cpp)
                # CUser：ORIG 为 4.4.6（构造/EH 着陆垫链、子对象寄存器形态；
                # CUser agent 验证 c6446r 更接近，建议与 CItemList 同路由）
                # [Main·检查点23 PoolCUser 三组实测] 再叠 `-march=i486 -mtune=generic`
                #   （N85 x87 lowering 族解药，同 437 行 WStringInst/56 行先例）：
                #   (a) 现路由 c6446r            = 703/1508/near4/diff115（合计 2211）
                #   (b) c6446r + i486+mtune=gen  = 703/1509/near4/diff114（合计 **2212**）
                #       · _processSpeicalContinuousTimeItem ndiff 5 → **AE 归零**
                #       · reqHumanCertify4ClearMap 97 → 92（残差主体转 N82 槽阶梯）
                #   (c) c6446r + 仅 -march=i486（无 -mtune=generic）= 674/142/2/1512 **崩**
                #       ⇒ **-mtune=generic 必须同带**（默认 tune 改调度，整 TU 劣化）。
                #   符号集两态逐名一致（PoolCUser `nm --defined-only` 复核），不触铁律1。
                exec "$C6446R" "$@" -march=i486 -mtune=generic ;;
            *Game/CreatureOnlyMgrRest.cpp)
                # [Main·检查点23 PoolMisc2 实测] 切 4.4.6 后端（头集保持 4.4.7）：
                #   基线(c6/4.4.7) 40/75/near5/diff3 → **40/77/near3/diff3**（ae+2、near−2）
                #   · user_creature::CCreatureMgr::IsSaveCreatureItem(ORIG 0x0833ba30) NEAR 4→AE
                #   · user_creature::CCreatureMgr::LoadCreatureItem(ORIG 0x0833b68c)  NEAR 13→AE
                #   · SaveCreatureItem ndiff 131→107；ChangeOwner/ChangeSlotNo/
                #     InsertCreatureItemResult/SetOverSkillTime 逐件读数不变（零回退）
                #   · 两态 `nm --defined-only` 符号集**逐名一致** ⇒ 不触铁律1
                #   同族先例：W6_AssaultPlaceDepsB / MatchRoomList / CExpandEquipslot；
                #   本 TU 原判词自陈「P230 换 4.4.6 可治、未路由」，本次兑现。
                exec "$C6446R" "$@" ;;
            *Game/CItemAmplifier.cpp)
                # b28d-S106：N7 间接 call 目标 4.4.7 恒 %eax / 4.4.6 与 ORIG 同用 %edx
                # （checkInvestableItem 3 处 GetItemType 虚调 + generateAmplifyItem
                # isAmplifyPollutionItem）。实测 TU ae 191→193 / near 4→2 / diff 不变，
                # 其余 4 件两后端读数一致 ⇒ 零回归。
                exec "$C6446R" "$@" ;;
            *Game/CItemUpgrade_Separate.cpp)
                # b28d-S126：item_upgrade_system 虚调目标 %edx + sete/test/je 物化在
                # 4.4.6 下与 ORIG 全等（NEAR→AE）。实测 TU ae 94→95 / near 2→1 /
                # diff 8 不变 ⇒ 零回归。
                exec "$C6446R" "$@" ;;
            *Game/CEventScriptMng_entities.cpp)
                # b28d-S157：set_event_data ×6 + CPcRoomPlayTime::verifyUserPcRoomPlayEvent
                # 的 near 差在 4.4.6 下全消。实测 TU 4.4.7 identical=29/ae=46/near=7/diff=2
                # vs 4.4.6 identical=35/ae=47/near=0/diff=2 ⇒ near 7→0 零回归。
                exec "$C6446R" "$@" ;;
            *Game/CEventScriptMng_entitiesB.cpp)
                # b28d-S188：5 件 set_event_data 的 N7（vfn %eax/this %edx vs ORIG %ecx/%eax）
                # 在 4.4.6 下全消。实测 4.4.7 identical=30/ae=42/near=5/diff=1
                # vs 4.4.6 identical=35/ae=42/near=0/diff=1 ⇒ near 5→0 零回归。
                exec "$C6446R" "$@" ;;
            *Game/CEventManager.cpp)
                # b28d-S195：7 件间接 call 目标 %ecx/%edx vs 4.4.7 恒 %eax 的 N7 族在
                # 4.4.6 下翻正。实测 4.4.7 identical=129/ae=233/near=8/diff=3
                # vs 4.4.6 identical=130/ae=239/near=1/diff=3 ⇒ 零回归。
                exec "$C6446R" "$@" ;;
            *Game/W6_IPGDeps.cpp | \
            *Game/IPGMgr.cpp)
                # b28d-S194：Finalize/dispatch/IPGOutputSimulation 的 N7 间接 call 目标
                # 寄存器在 4.4.6 下翻正。实测 4.4.7 identical=61/ae=155/near=3/diff=4
                # vs 4.4.6 identical=61/ae=158/near=0/diff=4 ⇒ 零回归。
                exec "$C6446R" "$@" ;;
            *Game/DisPatcher_DebugCommand_cmds.cpp)
                # b28d-S193：AddItem/AddAvatar 的 N7 在 4.4.6 下翻 AE。实测
                # 4.4.7 identical=20/ae=68/near=3/diff=2 vs 4.4.6 合计 90AE/near=1/diff=2
                # ⇒ 零回归。
                exec "$C6446R" "$@" ;;
            *Game/W6_AssaultPlaceDepsB.cpp)
                # b28d-S237：CAssaultPlace::_MoveUser(CDisposition*,int,int) 的 N7
                # 间接 call 目标寄存器（ORIG=4.4.6 形 %edx/%eax，4.4.7 恒 %eax/%edx）
                # 在 4.4.6 下翻 AE。实测 4.4.7 identical=6/ae=37/near=2/diff=6
                # vs 4.4.6 identical=6/ae=38/near=1/diff=6 ⇒ 零回归。
                exec "$C6446R" "$@" ;;
            *Game/CLeagueMatch.cpp | *Game/LinkMiscSecuTimerSolo.cpp | *Game/STEquipmentScript.cpp | *Game/CCreatureMgr.cpp | *Game/OnlyOrigBatch12_rand_r.cpp | *Game/OnlyOrigBatch12_security_server.cpp | *Game/OnlyOrigBatch12_server_utility.cpp | *Game/STMonsterScript.cpp | *Game/PvP_Room.cpp | *Game/ItemVendingMachine_Auction.cpp | *Game/GameWorld.cpp | \
            *Game/CFairMatch.cpp | *Game/CEquipItem.cpp | *Game/CAccountCargo.cpp | \
            *Game/CUserCharacInfo.cpp | *Game/ScriptDepsA.cpp | \
            *Game/CExpertJob.cpp)
                # b28d-6：ORIG CompareRating 的 x87 比较为 fucompp/fnstsw/sahf；4.4 需
                # -march=i486 才出该 lowering（P230 补）。实测 TU ae 51→52 / diff 24→23。
                # b28d-D44：LinkMiscSecuTimerSolo::GetMarketConditionRate 同族 x87 比较，
                # 该旗标下 ndiff 8→0。
                # b28d-S07：CCreatureMgr 的 x87 比较族（CStomach::IncrementValue/COverSkillTime::checkTime/
                # CCreature::IsDieCreature），实测 ae 393→396 / diff 30→27 零回归。
                # b28d-D56：STEquipmentScript 的 float 比较需 fucompp/fnstsw/sahf 形
                # （-march=i486 才关 CMOV/SSE 比较 lowering；探针 lane56/x87.cpp 逐条命中）。
                # b28d-S79：CFairMatch::TeamInfoThird::GetTeamRating 的 x87 比较同族，
                # 该旗标下 TU identical=37 不变 / ae 24→25 / diff 3→2 零回归。
                # b28d-S115：CEquipItem 的 getLevelLinearAbility ORIG 为
                # fucompp;fnstsw;sahf;seta / test $0x45,%ah;sete（4.4 默认产 fucomip）
                # ⇒ 该旗标下 DIFF→NEAR（identical 191 / ae 375 不变）零回归。
                # b28d-S124：CAccountCargo 的 GetEmptySlot/GetSpecificItemSlot 为
                # MIN_EXPR cmov（ORIG `cmp; jbe; mov $0x38`），仅 i486 可消；
                # CheckMoneyLimit 64 位比较段同族。预期 3 件转 AE 零回归。
                # b28d-S156：CUserCharacInfo 的 `index / 8`（有符号除 2^k）在 i486 下
                # 产 ORIG 精确 5 条（sar/shr/lea/sar），默认旗标产 cmovs（N97）。
                # 实测 ae 374→376 / diff 5→3 零回归。
                # b28d-S248：ScriptDepsA 的 GetRoundInt 为 x87 两向比较
                # （ORIG fucompp/fnstsw/sahf，默认 4.4.7 产 fucomip/fstp，N85）；
                # 实测 identical=375 不变 / ae 707→708 / diff 17→16 零回归。
                # b28d-BigMonsterScript：STMonsterScript 的 9 处 x87 比较段
                # （exportMonsterScript 内）需 fucompp/fnstsw/sahf/test $0x45,%ah/sete；
                # 该旗标下 export gap +8→−1、Import gap 不变（−4）、
                # TU identical=794 ae=1890 near=2 diff=18 与默认逐值相同（零回归）。
                # b28d-SweepPvPRoom：PvP_Room 的 CalculateVictoryPointOfSinglePlay
                # 有符号 /2^k 的 cmovs 需 i486 才关掉（P230/S156 同族）；
                # 实测 TU identical=32/ae=128/near=5/diff=22 → 32/129/5/21（+1 AE，零回归），
                # 该函数 99→105=ORIG、多重集相等、ndiff=0。
                # b28d-SweepVending：ItemVendingMachine_Auction 的 BuyAuctionItem 有 8 次
                # x87 比较（ORIG fucompp/fnstsw/sahf）；该旗标下 BuyAuctionItem 413→461
                # （ORIG 462，gap 49→1）、20 处 fucompp 同形，TU 四数 1/5/1/2 不变（零回归）。
                # b28d-Main 旗标实验：GameWorld 在该旗标下 ae 451→452 / diff 14→13（零回归）。
                # b28d-SweepCExpertJob（Main 独立核验）：CExpertJob 的 IsSuccess 需
                # fucompp/fnstsw/sahf（x87 比较）+ get_recipe/set_recipe 的 index/8 有符号
                # 除 2^k 需 i486 关 cmovs。三函数尺寸 0x12d/0x65/0xa2 与 ORIG 全等，
                # set_recipe 全 62 条字节相同；TU 36/45/0/5 → **37/47/0/2** 零回归。
                exec "$C6" "$@" -march=i486 -mtune=generic ;;
            *Game/CExpandEquipslot.cpp)
                # CExpandEquipslot：loadData 虚调用寄存器形态需 4.4.6
                # （c6 NEAR / c6446r AE；G1-3 refine agent 验证）
                exec "$C6446R" "$@" ;;
            *Game/MatchRoomList.cpp)
                # b28d-S349：CMultiMatchRoomList::doMatch/MultiMatch 的虚调用目标
                # 寄存器属 N7 后端敏感族（4.4.7 %eax / 4.4.6 %edx）。实测
                # 4.4.7 identical=130/ae=239/near=3/diff=4 →
                # 4.4.6 identical=130/ae=241/near=1/diff=4（doMatch/MultiMatch
                # NEAR→AE，两后端 p167 均 5/6 CLEAR）⇒ 零回归。
                # [Main·2026-09-10 round5] 再叠 `-march=i486 -mtune=generic`（x87 比较
                # lowering 族，P230/S275 同族）。.o 口径实测 identical=130/ae=241(371)/
                # near=1/diff=4 → identical=131/ae=241(372)/near=1/diff=3。
                # ⚠ 但该唯一翻正件是 **weak/COMDAT 模板实例** `_ZNKSt4lessIfEclERKfS2_`
                # （多 TU 重复发射、链接器只保留一份）⇒ **全局核账实测增量为 0**。
                # 保留该旗标仅因「全局零回归」（与新增 6 TU 臂同批验证）；若日后需精简，
                # 此臂可先删且无全局影响。`-mtune=generic` 必须同带。
                # [Main·2026-09-11 Round10 · FeasB 444 轴] 换 4.4.4 后端（保持 i486）：
                #   `CMultiMatchRoomList::GetWaitingRoomCombination` DIFF→AE，
                #   四数 131/242/1/2 → **131/243/1/1**；`comm -3`=0（nsym 599→599、symmd5 相同）。
                #   反证：单去 i486 ⇒ down=2（CLeagueMatchRoomList::Match AE→DIFF）⇒ i486 必须保留。
                exec "$C6444R" "$@" -march=i486 -mtune=generic ;;
            # ---- 脚本 Import/Export 族 + 相关：ORIG GameServer 为 GCC 4.4.6（DWARF producer 实证），
            # 切 4.4.6 后端使寄存器/栈槽分配逐操作数对齐，翻 NEAR→IDENTICAL。 ----
            *Game/CreatureScript.cpp)
                # b28d-S275：AttackInfo::isIntendDamage 的 x87 比较 ORIG 为
                # fucompp/fnstsw/sahf/seta；4.4.6 默认产 fucomip/fstp（N85），需
                # -march=i486 关 SSE/CMOV 比较 lowering（P230）。实测默认
                # identical=222/ae=455/near=1/diff=6 → i486 identical=223/ae=455/
                # near=1/diff=5 ⇒ 零回归。
                exec "$C6446R" "$@" -march=i486 -mtune=generic ;;
            *Game/STMonsterScript.cpp | *Game/STEquipmentScript.cpp | \
            *Game/STMapScript.cpp | \
            *Game/NPCScript.cpp | *Game/GameScriptEnumHelpers.cpp | *Game/EventScriptsCompleteB2.cpp | \
            *Game/W8LT_PassiveObjectScript.cpp | *Game/InitScriptProducers_6.cpp | *Game/sync_script.cpp | \
            *Game/EquipmentParameterInfo.cpp | *Game/DispLinkA2.cpp | *Game/CAuction.cpp)
                exec "$C6446R" "$@" ;;
            *Game/OnlyOrigBatch14.cpp)
                # [Main·2026-09-11 round5 第十批] 从上面共享 446 分支拆出，叠
                #   `-march=i486 -mtune=generic`（N85 x87 lowering 族解药，同
                #   `STStackableScript`/`CUser`/`CreatureScript` 先例）。
                #   申请与举证由 P1Free284to303 完成，Main 复测后落地：
                #     非 weak 件 `_Z29exportCharacterEtcStatsScriptP8_IO_FILEP17CharacterEtcStats`
                #     （ORIG 0x08a100f3 size 0x1b1）DIFF 119/118 gap=+1 → **AE 119=119 gap=0**
                #     （命中 ORIG 的 `fld [eax+edx*4+0xc]; fldz; fucompp; fnstsw %ax; sahf; jne; jp`）。
                #   TU 四数（真实构建口径 = C6446R + 本旗标）：396/800/1/11/197 →
                #     396/801/1/10/197（**ae +1 / diff −1，零劣化**）。
                #   ⚠ 前置读数纠错：曾有一份「带臂 396/786/1/24（13 个 weak 劣化）」的测量，
                #     那是在 **默认 C6/4.4.7 基线上叠 i486** 所致，与真实构建口径
                #     （已在 C6446R 之上）不同 ⇒ 正确口径下**无 weak 劣化**。
                #   旁证零回归：同批 C6446R 路由的 `LogManager` / `InitScriptProducers_8`
                #     加臂后逐项零变化 ⇒ 收益仅本 TU 这 1 件。
                #   ⚠ `-mtune=generic` 必须同带（单独 -march=i486 会崩，既有记录）。
                exec "$C6446R" "$@" -march=i486 -mtune=generic ;;
            *Game/STStackableScript.cpp)
                # [Main·检查点302] 从上面共享 446 分支拆出，叠 `-march=i486 -mtune=generic`
                #   （N85 x87 lowering 族解药；同 CUser.cpp 第 384 行 / WStringInst 先例）。
                #   Lane41 三组 A/B 实测：默认 797/1683/4/8 → 同带 797/1684/4/7
                #   （`GetExpertJobValueMaterialVariation` DIFF→AE，逐条命中 ORIG
                #   0x089bfc69 的 `fucompp;fnstsw %ax;test $0x5,%ah;sete;test;je`）；
                #   仅 `-march=i486`（不带 mtune）= 771/11/0/1710 **崩** ⇒ mtune 必须同带。
                #   其余 2480 件读数逐项不变；符号集待 relink 用 nm 逐名复核（铁律1）。
                exec "$C6446R" "$@" -march=i486 -mtune=generic ;;
            *Game/FlexLexerEngine.cpp)
                # flex 2.5.35 生成 TU：ORIG 与 4.4.6 虚调用/寄存器分配一致
                # [Main·2026-09-11 Round10 · i486 轴] +`-march=i486 -mtune=generic`：
                #   `yy_get_next_buffer` DIFF→AE，四数 0/29/0/5 → **0/30/0/4**（comm-3=0、missing 不变）。
                exec "$C6446R" "$@" -march=i486 -mtune=generic ;;
            *Game/CBossStage.cpp | *Game/CFairMatchRooms.cpp)
                # [Main·2026-09-10 round1 路由 A/B 实证] 切 4.4.6 后端（头集保持 4.4.7）：
                #   CBossStage        c6 228/368(596)/near3/diff6 → c6446r 228/369(597)/near2/diff6
                #                     （近 N7 族 `call *%edx` vs 4.4.7 `call *%eax`）；
                #   CFairMatchRooms   c6  36/ 81(117)/near11/diff6 → c6446r 36/ 88(124)/near4/diff6
                #                     （near 11→4、ae +7，本波单 TU 最大路由收益）。
                #   两 TU 均零语义风险（仅后端选择），消费面自持。
                exec "$C6446R" "$@" ;;
            *Game/SlangFlexLexerEngine.cpp)
                exec "$C6446R" "$@" ;;
            *Game/SlangFilter.cpp)
                exec "$C6446R" "$@" ;;
            *Game/CSyncSlangFilter.cpp)
                exec "$C6446R" "$@" ;;
            *Game/DNFLexWrapperHelpers.cpp)
                exec "$C6446R" "$@" ;;
            *Game/DNFLexWrapper.cpp)
                # ORIG wrapper getters are -O2 (pop-ebp-before-load, tail-call tok_str).
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/DNFLexWrapperStream.cpp)
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/DNFLexWrapperLoadStream.cpp)
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/CompiledDNFLexCore.cpp)
                # b28d-S245：ORIG PushInputStream 0x8ad0b8 调
                # `_ZNSt5dequeIN14CompiledDNFLex8stream_tESaIS1_EE16_M_push_back_auxIIRKS1_EEEvDpOT_`
                # （C++0x 变参版）⇒ 该 TU 原为 C++0x。实测 gnu++98 identical=0/ae=24/diff=8/
                # missing=9 → gnu++0x identical=0/ae=25/diff=8/missing=8，逐函数 ndiff 合计 471
                # 不变（8 件 DIFF 集合与读数完全一致）⇒ 零回归。
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++0x ;;
            *Game/Arad_Script.cpp)
                # ORIG Arad_Script is -O0 (reload this from 0x8(%ebp) each
                # field access). -O2 made every getter DIFF.
                # -std=gnu++0x：ORIG mangled `St10shared_ptr`（std::shared_ptr）
                # 证明 ORIG 为 C++0x 模式；gnu++98 下 <memory> 无 std::shared_ptr，
                # GameTypes.h→LogManager.h 编译失败。
                exec "$C6446R" "$@" -std=gnu++0x ;;
            *Game/ScriptExtras.cpp)
                # [Main·2026-09-11 Round10 · i486 轴] +`-march=i486 -mtune=generic`：
                #   3 件 DIFF→AE，四数 103/192/0/7 → **103/195/0/4**（comm-3=0、missing 不变）。
                exec "$C6446R" "$@" -march=i486 -mtune=generic ;;
            *Game/RDARScriptStringManager.cpp)
                exec "$C6446R" "$@" ;;
            *Game/ScriptStringManager.cpp)
                # ORIG ctor inlines empty vector/map (vptr+zeros+header).
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/DNFLexCore.cpp)
                # SRC_OPTS 强制 -O2 -std=gnu++98（见 game/CMakeLists.txt），同样注入兼容头
                # [R7a17 2026-09-12] 追加 -std=gnu++0x（覆盖 SRC_OPTS 的 gnu++98）：ORIG 该 TU
                # 的 PushInputStream（0x08acd7d0，15 calls）调
                # `_ZNSt6vectorIN6DNFLex8stream_tESaIS1_EE13_M_insert_auxIIRKS1_EEEvN9__gnu_cxx17__normal_iteratorIPS1_S3_EEDpOT_`
                # （C++0x 变参版）；gnu++98 头集无该重载 ⇒ 调 C++98 `...13_M_insert_auxE…ERKS1_`（P167 MISSING 1）。
                # A/B（唯一变量=std）：identical 0/0、ae 22→23、near 0/0、diff 2/2、missing 4→3；
                # nm --defined-only 35=35，comm -3 仅 C++98↔C++0x 两版互换（后者与 ORIG 同名）。
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -std=gnu++0x ;;
            *ServerLab/ServerLib/*)
                # nsl 基础库：ORIG 为 Red Hat GCC 4.4.6-3（cc1plus446bin）编译
                # （E/D/C 组独立验证：4.4.6 下虚调用/寄存器分配与 point ORIG 逐操作数一致）
                exec "$C6446R" "$@" ;;
            *Game/TimerDispatcher.cpp)
                # b28d-Main 旗标实验（SweepTimerDispatcher-2 上报 + Main 独立复编核验）：
                # TimerGeneratePowerUp::dispatch_sig 的 x87 比较段（fucompp/fnstsw/sahf/sete）
                # 需 -march=i486 才出 ORIG lowering（P230/S156 同族；默认旗标产 fucomip/fstp）。
                # 实测（同源码仅旗标差）：函数尺寸 0x3f8 → **0x400 = ORIG 0x400**（nm -S 双侧），
                # ndiff 36 → 12（余 clamp 段 setl 物化），TU 四数 60/694/near4/diff17 逐项不变、
                # 其余 8 件读数不变 ⇒ 零回归。注意 -mtune=generic 必须同带（默认 tune 改调度）。
                exec "$C6" "$@" -march=i486 -mtune=generic ;;
            *Game/TaiwanQueries.cpp)
                # [Main·2026-09-10 round5] 路由 A/B 实证（D02 上报 + 源码按 ORIG 三目形对齐）：
                #   `COpensslRSA::Decrypt/Encrypt`（ORIG 0x0816c532 / 0x0816c3a2）的
                #   `chunk=min(len-pos,rsa_size)` 为 MIN_EXPR+i486 形
                #   （`mov -0x28,%edx; cmp %edx,%eax; jle +2; mov %edx,%eax; mov %eax,-0x20`，无 cmov、单 store）；
                #   默认 4.4.7 把三目折成 `cmovle`。实测：默认 33/116(149)/0/6 →
                #   同旗标 33/118(151)/0/4（**两件 DIFF→AE，净 −2 非identical**）；
                #   149 个可比函数中仅该 2 件变化、**无任何 AE/NEAR 回退**（cmov 计数 4→0）。
                #   同 SDCLink：`-mtune=generic` 必须同带。
                exec "$C6" "$@" -march=i486 -mtune=generic ;;
            *Game/SDCLink.cpp)
                # [Main·2026-09-10 round5] 路由 A/B 实证（C03 上报 + Main 独立复编核验）：
                #   `_ZSt3maxIfERKT_S2_S2_`（ORIG 0x08080609）为 4.1 x87 比较 lowering
                #   `fucompp; fnstsw %ax; test $0x45,%ah; sete %al`；默认 4.4.7 产
                #   `fucomip %st(1),%st; fstp %st(0); seta %al`（N85 族）。
                #   实测：默认 28/20(48)/near0/diff7 → 同旗标 28/21(49)/near0/diff6
                #   （ae +1 / diff −1，其余 5 件 DIFF 读数不变，零回归）。
                #   注意 `-mtune=generic` 必须同带（单独 -march=i486 会崩，既有记录）。
                exec "$C6" "$@" -march=i486 -mtune=generic ;;
            *Game/ServerParameterScript.cpp)
                # [Main·2026-09-10 round5] 路由 A/B 实证（C04 上报 + Main 独立复编核验）：
                #   x87 比较 lowering 族（ORIG `fucompp; fnstsw %ax; sahf` vs 4.4.7 `fucomip/fstp`）。
                #   实测（同口径两次编译，仅差该旗标）：623/1125(1748)/near4/diff20 →
                #   623/1126(1749)/near4/diff19（ae +1 / diff −1，净 −1）。
                exec "$C6" "$@" -march=i486 -mtune=generic ;;
            *Game/CHackLog_OverDamage.cpp)
                # [Main·2026-09-11 round5 第九批] 路由申请由 N6Free231to250 提出，附完整证据链：
                #   x87 比较 lowering 族（P230/S156/N85 同族）——
                #   `CHackLog_OverDamage::GetDungeonLimitDamageAndHackType` ORIG 0x082868cc 为
                #   `fucompp; fnstsw %ax; test $0x45,%ah; sete %al`（**无 cmov 的 i386 族** lowering），
                #   默认 4.4.7 产 `fucomip %st(1),%st; fstp; seta`。
                #   N6S3PosA 探针扫 -march：i386/i486/i586 ⇒ fnstsw 序列；i686/pentium4 ⇒ fucomip。
                # ⚠ **臂的收益经 Main 两次对照实测确认（勿再移除）**：N6 曾复审撤回该申请
                #   （理由是「收益来自源码改动、默认路由也是 ae=5/diff=1」），Main 随后实测否证：
                #     有臂：`identical=3 ae=5 near=0 diff=1`（唯一 DIFF = N133 工具假缺口 ⇒ 实质全绿）
                #     无臂：`identical=3 ae=4 near=0 diff=2`
                #   ⇒ **`GetDungeonLimitDamageAndHackType` 的 DIFF→AE 确实由该旗标带来**，
                #   已恢复本臂。**教训：旗标 A/B 必须在「对方声称的当前源码状态」下自己做一次**，
                #   不能只采信转述的读数（本件即两侧都自称测过、结论相反）。
                # ⚠ `-mtune=generic` 必须同带（单独 -march=i486 会崩，既有记录）。
                exec "$C6" "$@" -march=i486 -mtune=generic ;;
            *Game/PvpUserTable.cpp | \
            *Game/ScriptLeafA2.cpp | \
            *Game/GmCmds_E.cpp | \
            *Game/CBattle_Field_kill_monster.cpp | \
            *Game/CPrivateStoreMgr.cpp | \
            *Game/CCreature.cpp)
                # [Main·2026-09-10 round5] x87 比较 lowering 族路由（P230/S275 同族）。
                # 逐 TU .o 口径 A/B（同 wrapper 仅差 `-march=i486 -mtune=generic`）：
                #   PvpUserTable 16→18 / diff 4→2、ScriptLeafA2 1732→1735 / diff 17→14、
                #   GmCmds_E 379→380 / diff 6→5、CBattle_Field_kill_monster 166→167 / diff 3→2、
                #   CPrivateStoreMgr identical 171→172、CCreature 21→22 / diff 2→1
                #   ⇒ .o 口径合计 id+ae +9 / diff −9。
                # ⚠ **口径警告（必读）**：该 +9 中 6 处是 **weak/COMDAT 模板实例**
                #   （`_ZSt3minIfERKT_S2_S2_` / `_ZSt3maxIf…` / `_ZNKSt4lessIfEcl…` /
                #    `_ZNSt18_Bit_iterator_base7_M_incrEi` / `_ZNSt8__detail…_M_need_rehash` /
                #    `_ZNSt8__detail13__lower_bound…`）—— 这些符号在**多个 TU 重复发射**，
                #    链接器只保留一份 ⇒ 改一个 TU 的编译旗标**不会**改变最终二进制里那份。
                #   逐函数核对后的**真实全局**增益只有 TU 专属件：
                #   `PvpUserTable::GetResult` (+1)、`user_creature::CStomach::IsSaveStomach` (+1)、
                #   `private_store::CPrivateStore::VerifyPrivateStoreItem` thunk（ae→id, identical +1）。
                # ⇒ **实测全局：identical 34123→34125（+2）、diff 1749→1747（−2）**，零回退。
                # ⇒ **判据（写入模板）**：编译旗标 A/B **必须用全局核账（compare_game_full.py）
                #   定稿**；per-TU 四数只作方向筛选，凡命中 weak/COMDAT（`St`/`__detail`/`_Bit_`
                #   前缀或 nm 显示 `W`/`w`）的「翻正」一律**不计入预期**。
                # `-mtune=generic` 必须同带（单独 -march=i486 会崩，既有记录）。
                exec "$C6" "$@" -march=i486 -mtune=generic ;;
            *Game/CEventCreateDnf.cpp | \
            *Game/EventRewardWrite.cpp | \
            *Game/StubS3A_deps.cpp | \
            *Game/BestClearTime.cpp | \
            *Game/DB_InsertAccountFirstLoginDailyFlag.cpp | \
            *Game/DB_InsertAccountFirstLoginFlag.cpp | \
            *Game/Dispatcher_AuctionRegistItem_impl.cpp | \
            *Game/LinkClosureD.cpp | \
            *Game/MouseRegisterEJ.cpp | \
            *Game/MySQL.cpp | \
            *Game/W9_StructMisc.cpp | \
            *Game/CEventScriptMng.cpp | \
            *Game/RDARScriptInGameEvent.cpp)
                # [Main·2026-09-10 round5] **446 后端杠杆**（`-B cc1plus446bin/`）路由 A/B。
                # 该族画像是「间接调用目标寄存器 ORIG `call *%edx` vs 4.4.7 `call *%eax`」（N7），
                # 机器筛出 88 件/56 TU，本批为逐 TU A/B 有增益者（其余 34 TU 零变化、
                # 3 TU 回退已剔除：MatchingSystem id+ae −2、CompoundDeathLCB/GameMain near→diff）。
                # .o 口径合计：Δident **+14** / Δnear **−18** / Δdiff 0（非 identical 净 −17）。
                # ⚠ 与 x87 臂同规矩：**全局核账定稿**（weak/COMDAT 重复发射会高估）。
                exec "$C6" -B"${ROOT}/cc1plus446bin/" "$@" ;;
            # [Main·2026-09-10 round5] **444 后端杠杆**（`dnf_c6444r_gxx.sh` = 4.4.7 驱动 +
            # `-B cc1plus444bin/`，ORIG `.comment` 主体编译器候选）。该杠杆此前从未在任何
            # 分支使用；对 diff 最大的 30 个 TU 逐个 A/B 后**仅本件命中**（其余 29 个零变化，
            # 扩展扫 78 TU 亦无新增收益）。实测 .o 口径 identical 49 / ae 0 / near 0 / diff 11
            # → identical **58** / ae 0 / diff **2**；全局核账 identical 34141→**34150（+9）**、
            # diff 1745→**1736（−9）**。
            # ⚠ 本 TU 已按 ① 部件包含式拆分（聚合器 + Game/CTournamentDungeonReward/*.inc）
            #   ⇒ TU 路径不变，此处即自动继承给全部部件。
            *Game/CTournamentDungeonReward.cpp)
                exec "$C6444R" "$@" ;;
            # [Main·2026-09-10 round5] 非 Game 库域的 446 后端杠杆（G12 上报 + Main 复核）：
            #   全库 38 个非 Game TU 做过 447↔446 双编对照，**仅本件受益**。
            #   实测 .o 口径 identical 69 / ae 88 / near 4 / diff 1 →
            #                identical 69 / ae 92 / near **0** / diff 1
            #   （4 件 NEAR→AE：CharsetInit 100/100、EventLoad 110/110、RGBALoad 93/93、
            #     ProcessLoad 83/83；差异全部是 N7 —— 间接 call 目标
            #     %eax(4.4.7) vs %edx(ORIG 4.1 / 4.4.6)，源码无解）。
            #   ⚠ 与其它旗标臂同规矩：**全局核账定稿**。
            *ServerCommon/ServerXml.cpp)
                exec "$C6446R" "$@" ;;

            # [SemA09 slice09 · i486 轴 · P0.1 必守程序已过（Main 批准 2026-09-12）]
            #   N85/P230② x87 比较 lowering：ORIG 为 `fucompp;fnstsw %ax;test $0x5/%0x45,%ah;sete`，
            #   默认 4.4.7 档产 `fucomip %st(1),%st;fstp %st(0);setae/seta`；+i486 后逐条复现 ORIG。
            #   A/B（唯一变量 = +`-march=i486 -mtune=generic`，同源码同旗标）：
            #     W6_AssaultMgrDeps  identical=36 ae=51 near=0 diff=2 missing=7（逐值不变）
            #       · GetRequestAssaultNeedMoney ndiff 51→34（5 处 x87 段，TU Σndiff 200→183）
            #   注：ExpertJobLink4 同理，但因它已在下方大队列（446h46）中匹配，专属 case 置于
            #   文件上方 `case $src` 首块（见 36 行处）。
            #   `nm --defined-only | comm -3` = 0 行；`-mtune=generic` 必须同带（P230 历史）。
            # [SemA16 申请 · SemA09 落地 2026-09-12] CTradeSpace.cpp 同族（N85 x87）：
            #   A/B（SemA16 提供，唯一变量 = +i486）：TU 四数逐值不变、comm-3=0（328 符号）、
            #   missing 45→45、目标件 `change_trade_state` ndiff **715→701（−14）**、x87 段复现 ORIG。
            # [SemA17 申请 · SemA09 落地 2026-09-12] CParty.cpp 同族（N85 x87）：
            #   A/B（SemA17 提供，唯一变量 = +i486）：TU 四数逐值不变（identical 146 / ae 334 /
            #   near 0 / diff 27 / missing 86）、comm-3=0、逐件 verdict（593 符号）零差、
            #   TU Σnd 14061→14024（−37）；`CParty::MakeTournamentReward` 449/448 gap+1 → 449/449 gap0、
            #   nd 51→44；x87 段 `fucomip/fstp/seta` → `fnstsw %ax;test $0x45,%ah;sete` = ORIG。
            *Game/W6_AssaultMgrDeps.cpp | *Game/CTradeSpace.cpp | *Game/CParty.cpp)
                exec "$C6" "$@" -march=i486 -mtune=generic ;;

            *)
                exec "$C6" "$@" ;;
        esac ;;
esac
