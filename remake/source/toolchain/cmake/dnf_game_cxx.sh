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
    *Game/ARAD.cpp | \
            *Game/Action.cpp | \
            *Game/AdditionalEffectData.cpp | \
            *Game/AradPeriodB.cpp | \
            *Game/Arad_AvatarRoulette.cpp | \
            *Game/Arad_Dispatcher.cpp | \
            *Game/Arad_IPGProcess.cpp | \
            *Game/Arad_InternalDispatcher.cpp | \
            *Game/Arad_ItemBonus.cpp | \
            *Game/Arad_Script.cpp | \
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
            *Game/CBattle_Field.cpp | \
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
            *Game/CInventory.cpp | \
            *Game/CItem.cpp | \
            *Game/CItemDictionary.cpp | \
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
            *Game/CSourceVersionMgr.cpp | \
            *Game/CStackableItem.cpp | \
            *Game/CTitleBook.cpp | \
            *Game/CUser.cpp | \
            *Game/CUserCharacInfo.cpp | \
            *Game/CUserDepsB.cpp | \
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
            *Game/ExpertJobOnlyEnchanter.cpp | \
            *Game/ExpertJobOnlyMaterial.cpp | \
            *Game/ExpertJobOnlyMgr.cpp | \
            *Game/GameBasicsScripts.cpp | \
            *Game/GameWorld.cpp | \
            *Game/GlobalData.cpp | \
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
            *Game/Inter_LoadEtc_impl.cpp | \
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
            *Game/OnlyOrigBatch12.cpp | \
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
            *Game/W8LT_EventTail.cpp | \
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
            *Game/W9_EventSmall.cpp)
        set -- -B"${ROOT}/cc1plus446bin/" "$@" ;;
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
                exec "$C6446R" "$@" ;;
            *Game/STDungeonScript.cpp)
                # STDungeonScript（Import/exportDungeonScript）：ORIG 为 4.4.6。
                # 字节级 identical 需 4.4.6 产生与 ORIG 相同栈布局/寄存器分配。
                exec "$C6446R" "$@" ;;
            *Game/CUser.cpp)
                # CUser：ORIG 为 4.4.6（构造/EH 着陆垫链、子对象寄存器形态；
                # CUser agent 验证 c6446r 更接近，建议与 CItemList 同路由）
                exec "$C6446R" "$@" ;;
            *Game/CLeagueMatch.cpp)
                # b28d-6：ORIG CompareRating 的 x87 比较为 fucompp/fnstsw/sahf；4.4 需
                # -march=i486 才出该 lowering（P230 补）。实测 TU ae 51→52 / diff 24→23。
                exec "$C6" "$@" -march=i486 -mtune=generic ;;
            *Game/CExpandEquipslot.cpp)
                # CExpandEquipslot：loadData 虚调用寄存器形态需 4.4.6
                # （c6 NEAR / c6446r AE；G1-3 refine agent 验证）
                exec "$C6446R" "$@" ;;
            # ---- 脚本 Import/Export 族 + 相关：ORIG GameServer 为 GCC 4.4.6（DWARF producer 实证），
            # 切 4.4.6 后端使寄存器/栈槽分配逐操作数对齐，翻 NEAR→IDENTICAL。 ----
            *Game/STMonsterScript.cpp | *Game/STEquipmentScript.cpp | *Game/STStackableScript.cpp | \
            *Game/STMapScript.cpp | *Game/OnlyOrigBatch14.cpp | *Game/CreatureScript.cpp | \
            *Game/NPCScript.cpp | *Game/GameScriptEnumHelpers.cpp | *Game/EventScriptsCompleteB2.cpp | \
            *Game/W8LT_PassiveObjectScript.cpp | *Game/InitScriptProducers_6.cpp | *Game/sync_script.cpp | \
            *Game/EquipmentParameterInfo.cpp | *Game/DispLinkA2.cpp | *Game/CAuction.cpp)
                exec "$C6446R" "$@" ;;
            *Game/FlexLexerEngine.cpp)
                # flex 2.5.35 生成 TU：ORIG 与 4.4.6 虚调用/寄存器分配一致
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
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/Arad_Script.cpp)
                # ORIG Arad_Script is -O0 (reload this from 0x8(%ebp) each
                # field access). -O2 made every getter DIFF.
                # -std=gnu++0x：ORIG mangled `St10shared_ptr`（std::shared_ptr）
                # 证明 ORIG 为 C++0x 模式；gnu++98 下 <memory> 无 std::shared_ptr，
                # GameTypes.h→LogManager.h 编译失败。
                exec "$C6446R" "$@" -std=gnu++0x ;;
            *Game/ScriptExtras.cpp)
                exec "$C6446R" "$@" ;;
            *Game/RDARScriptStringManager.cpp)
                exec "$C6446R" "$@" ;;
            *Game/ScriptStringManager.cpp)
                # ORIG ctor inlines empty vector/map (vptr+zeros+header).
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/DNFLexCore.cpp)
                # SRC_OPTS 强制 -O2 -std=gnu++98（见 game/CMakeLists.txt），同样注入兼容头
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" ;;
            *ServerLab/ServerLib/*)
                # nsl 基础库：ORIG 为 Red Hat GCC 4.4.6-3（cc1plus446bin）编译
                # （E/D/C 组独立验证：4.4.6 下虚调用/寄存器分配与 point ORIG 逐操作数一致）
                exec "$C6446R" "$@" ;;
            *)
                exec "$C6" "$@" ;;
        esac ;;
esac
