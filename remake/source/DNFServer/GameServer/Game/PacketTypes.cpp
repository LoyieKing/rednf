// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PacketTypes/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— MSG / Param 构造函数 + IPacketDispatcher 显式实例化
#include "PacketTypes.h"
#include <cstring>
#include <ctype.h>      // isalnum（is_base64，ORIG _ZL9is_base64h）

// ---------------------------------------------------------------------------
// base64 族（ORIG 归属本 TU：__static_init @0x0807ee76 = ios_base::Init +
// 本 string 动态构造/operator[](const 重载 _ZNKSsixEj 以 $0x93f56f4 为 this)、
// _GLOBAL__I 键名 _GLOBAL__I__Z13base64_encodePKhjRSs @0x0807ef30、
// base64_encode T 0x0807ea2e / base64_decode T 0x0807ebf4 地址紧邻、
// _ZL12base64_chars b 0x093f56f4）。声明序：文件首个动态初始化对象。
// ---------------------------------------------------------------------------
static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


#include "PacketTypes/base64_encode.inc"


#include "PacketTypes/RentOutput.inc"
#include "PacketTypes/dtor_RentOutput.inc"

#include "PacketTypes/MSG_BINGO_QUIZ.inc"

#include "PacketTypes/MSG_BLUE_MARBLE.inc"

#include "PacketTypes/MSG_BLUE_MARBLE_CONFIRM_INFO.inc"

#include "PacketTypes/MSG_BREAK_AWAY.inc"
#include "PacketTypes/MSG_BUY_SKILL.inc"

#include "PacketTypes/MSG_CHANGE_ANOTHER_SKILL_TREE.inc"

#include "PacketTypes/MSG_CHANGE_AVATAR_OPTION.inc"

#include "PacketTypes/MSG_CHANGE_RANDOM_OPTION.inc"
#include "PacketTypes/MSG_CHARAC_SELECT.inc"
#include "PacketTypes/MSG_CHECK_GUILD_JOIN.inc"

#include "PacketTypes/MSG_CLIENT_SPEC_STATISTIC.inc"
#include "PacketTypes/MSG_COMBO_SKILL.inc"

#include "PacketTypes/MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET.inc"

#include "PacketTypes/MSG_COMPATIBILITY_INDEX.inc"
  // ORIG 0x822e174
#include "PacketTypes/MSG_ENCHANT.inc"

#include "PacketTypes/MSG_EXCHANGE_RANDOM_ITEM_REWARD.inc"
#include "PacketTypes/MSG_EXPERT_EXTRACTION.inc"

#include "PacketTypes/MSG_FATIGUE_ATTENDANCE.inc"

#include "PacketTypes/MSG_GROWTH_EQUIPMENT.inc"

#include "PacketTypes/MSG_GROWTH_POWER.inc"

#include "PacketTypes/STGuildMemberCharacData.inc"


#include "PacketTypes/STGuildBoardDBInfo.inc"


#include "PacketTypes/MSG_GUILD_BOARD.inc"

#include "PacketTypes/MSG_INFORM_NOTICE.inc"

#include "PacketTypes/MSG_INFORM_NOTICE_2ND.inc"

#include "PacketTypes/MSG_ITEM_UPGRADE.inc"

#include "PacketTypes/MSG_LOG_INGAME_ADVERTISEMENT.inc"
#include "PacketTypes/MSG_MONSTERCARD_BIND.inc"

#include "PacketTypes/MSG_OPEN_CERAPACKAGE.inc"

#include "PacketTypes/MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE.inc"

#include "PacketTypes/MSG_P2P_STATISTICS.inc"

#include "PacketTypes/MSG_PARTY_TELEPORT.inc"

#include "PacketTypes/MSG_PARTY_TELEPORT_CONFIRM.inc"

#include "PacketTypes/MSG_PURIFY_ITEM.inc"

#include "PacketTypes/MSG_PVP_MISSION_HP_PERCENT.inc"
#include "PacketTypes/dtor_MSG_PVP_MISSION_HP_PERCENT.inc"
#include "PacketTypes/MSG_QUEST_SHOP.inc"

#include "PacketTypes/MSG_QUICK_CHAT.inc"

#include "PacketTypes/MSG_QUICK_PARTY.inc"

#include "PacketTypes/MSG_REFUND_SKILL.inc"

#include "PacketTypes/MSG_RENAME_CREATURE.inc"

#include "PacketTypes/MSG_RENT_ITEM.inc"

#include "PacketTypes/MSG_REQUEST_CHARAC_SKILL_INFO.inc"

#include "PacketTypes/MSG_REQUEST_INGAME_ADVERTISEMENT.inc"
#include "PacketTypes/MSG_REQUEST_GUILD_JOIN.inc"

#include "PacketTypes/MSG_RESET_RANDOM_OPTION.inc"

#include "PacketTypes/MSG_RIGHT_OF_CHANGE_GROW_TYPE.inc"

#include "PacketTypes/MSG_SAVE_CHARACTER_OPTION.inc"
#include "PacketTypes/MSG_SAVE_GAME_OPTION_1.inc"
#include "PacketTypes/MSG_SAVE_GAME_OPTION_2.inc"
#include "PacketTypes/MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON.inc"
#include "PacketTypes/MSG_SAVE_QUEST_NOTIFY.inc"

#include "PacketTypes/MSG_SECURITY_LOG_PACKET.inc"

#include "PacketTypes/MSG_SELECT_STRIKER.inc"

#include "PacketTypes/MSG_SKILL_COMMAND.inc"

#include "PacketTypes/MSG_SKILL_INIT.inc"

#include "PacketTypes/MSG_SOLO_TELEPORT.inc"

#include "PacketTypes/MSG_STACKABLE_ACTION_USE.inc"
  // ORIG 0x826033e
#include "PacketTypes/MSG_TEMP_STRUCT.inc"
#include "PacketTypes/MSG_UPDATE_CONTRACT_OF_CUBE_INFO.inc"

#include "PacketTypes/MSG_USER_HISTORY_LOG.inc"

#include "PacketTypes/MSG_USE_DYE.inc"

#include "PacketTypes/MSG_USE_LIMIT_CUBE.inc"

#include "PacketTypes/MSG_USE_RENAME_CARD.inc"

#include "PacketTypes/MsgClearQuestIndex.inc"
// ORIG 0x08235266（W）：逐标量拷贝 +0x00..+0x08 / +0x0c / +0x10，跳过 +0x09..+0x0b
// 与 +0x0d..+0x0f（编译器逐成员拷贝语义，padding 不拷贝），m_questIdxs 走
// std::vector<int>::operator=。
MsgClearQuestIndex& MsgClearQuestIndex::operator=(const MsgClearQuestIndex& other)
{
    // ORIG 0x8235266：基类子对象整体赋值（3×dword + 尾字节），随后 count/vector
    reinterpret_cast<MSG_BASE&>(*this) = reinterpret_cast<const MSG_BASE&>(other);
    m_count = other.m_count;
    m_questIdxs = other.m_questIdxs;
    return *this;
}
#include "PacketTypes/MsgRequestAssaultPrice.inc"

#include "PacketTypes/MsgTournamentRewardSelect.inc"


#include "PacketTypes/AbnormalUseStackable.inc"

#include "PacketTypes/BuyItemParam.inc"


#include "PacketTypes/CheckSecurityProtection.inc"

#include "PacketTypes/InformNoticeData.inc"
#include "PacketTypes/InformNoticeData2nd.inc"
#include "PacketTypes/LimitNpcBuyItem.inc"
#include "PacketTypes/PcRoomPlayTimeRewardParam.inc"
#include "PacketTypes/ReqDebugClearQuestResult.inc"
#include "PacketTypes/ReqResult.inc"
#include "PacketTypes/STQuestShopBuyParam.inc"

#include "PacketTypes/STQuestShopParam.inc"
#include "PacketTypes/STUdateItemParam.inc"
#include "PacketTypes/ScanBotByDllParam.inc"

#include "PacketTypes/ScanBotByDrvParam.inc"

#include "PacketTypes/ScriptHashParam.inc"

#include "PacketTypes/SecuCardRpyParam.inc"

#include "PacketTypes/SendMercenaryCompetitionResult.inc"
#include "PacketTypes/SendMercenaryReturnResult.inc"


#include "PacketTypes/_slot_String.inc"
#include "PacketTypes/_slot_String_1.inc"

MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String&
MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String::operator=(const _slot_String& o)
{
    m_slot = o.m_slot;
    m_str = o.m_str;
    return *this;
}
#include "PacketTypes/dtor__slot_String.inc"
#include "PacketTypes/TempParam.inc"
#include "PacketTypes/saveQuestNotifyParam.inc"
#include "PacketTypes/stAddFriendParam.inc"
#include "PacketTypes/stAvatarDisjointRandomReward.inc"
#include "PacketTypes/stBingoQuiz.inc"
#include "PacketTypes/stBingoReward.inc"
#include "PacketTypes/stBlueMarble.inc"
#include "PacketTypes/stBlueMarbleConfirmInfo.inc"
#include "PacketTypes/stBossDieCheckParam.inc"

#include "PacketTypes/stBreakAwayParam.inc"
#include "PacketTypes/stCallMemberListParam.inc"
#include "PacketTypes/stChangeAnotherSkillTree.inc"
#include "PacketTypes/stChangeAvatarOption.inc"
#include "PacketTypes/stChangeEmotionParam.inc"

#include "PacketTypes/stChangeHostWarRoom.inc"
#include "PacketTypes/stChangeLetterStatParam.inc"
#include "PacketTypes/stCharacSlotExtendEffect.inc"

#include "PacketTypes/stClientSpecStatisticParam.inc"

#include "PacketTypes/stCloseDisjointStoreParam.inc"
#include "PacketTypes/stCollectItems.inc"

#include "PacketTypes/stComboSkill.inc"
#include "PacketTypes/stComboSkillExtensionQuickSlotReset.inc"
#include "PacketTypes/stCreateCharac.inc"
#include "PacketTypes/stCreateDisjointStoreParam.inc"
#include "PacketTypes/stDeleteCharac.inc"
#include "PacketTypes/stDeleteItemParam.inc"

#include "PacketTypes/stDeleteToBlackListParam.inc"
#include "PacketTypes/stDieCharacInDungeon.inc"
#include "PacketTypes/stDropItemParam.inc"

#include "PacketTypes/stEnchantByBeadParam.inc"
#include "PacketTypes/stEnchantParam.inc"
#include "PacketTypes/stEnterDisjointStoreParam.inc"
#include "PacketTypes/stEquipslotSwitch.inc"

#include "PacketTypes/stExchangeRandomItemReward.inc"
#include "PacketTypes/stExpandEquipslotFlagUpdate.inc"

#include "PacketTypes/stExpertExtraction.inc"

#include "PacketTypes/stFatigueAttendance.inc"
#include "PacketTypes/stFinishLoadParam.inc"
#include "PacketTypes/stGetItemParam.inc"
#include "PacketTypes/stGoldTakeIncreasingAmount.inc"

#include "PacketTypes/stGrowthCreature.inc"
#include "PacketTypes/stGrowthEquipment.inc"
#include "PacketTypes/stGrowthPowerParam.inc"
#include "PacketTypes/stGuildBoardParam.inc"
#include "PacketTypes/stGuildCargoHistoryParam.inc"
#include "PacketTypes/stGuildCargoMoveItemParam.inc"
#include "PacketTypes/stGuildCargoParam.inc"
#include "PacketTypes/stGuildCargoPopItemParam.inc"
#include "PacketTypes/stGuildCargoPushItemParam.inc"
#include "PacketTypes/stHatchCreature.inc"
#include "PacketTypes/stImageCommunicationEquipmentUse.inc"

#include "PacketTypes/stItemDictionary.inc"
#include "PacketTypes/stItemHyperLinkMessage.inc"
#include "PacketTypes/stLeavePartyParam.inc"
#include "PacketTypes/stLogIngameAdvertisement.inc"
#include "PacketTypes/stMailboxExtractParam.inc"

#include "PacketTypes/stMailboxOpenParam.inc"

#include "PacketTypes/stMailboxSendParam.inc"
#include "PacketTypes/stMemberEnterReplyParam.inc"
#include "PacketTypes/stMemberSecedeParam.inc"
#include "PacketTypes/stMonstercardBind.inc"

#include "PacketTypes/stMoveItemParam.inc"
#include "PacketTypes/stMoveMapParam.inc"
#include "PacketTypes/stObjectBringUp.inc"
#include "PacketTypes/stOnBuySkillParam.inc"

#include "PacketTypes/stOnChangeSkillParam.inc"
#include "PacketTypes/stOnGetUserInfoParam.inc"

#include "PacketTypes/stOnPvPTimeOut.inc"
#include "PacketTypes/stOnRecoverStaminaParam.inc"
#include "PacketTypes/stOnSetPvPReadyParam.inc"
#include "PacketTypes/stOnSortItemParam.inc"
#include "PacketTypes/stOnUseEquipParam.inc"
#include "PacketTypes/stOpenCeraPackage.inc"
#include "PacketTypes/stPartyTeleport.inc"
#include "PacketTypes/stPartyTeleportConfirm.inc"
#include "PacketTypes/stQuickChatParam.inc"
#include "PacketTypes/stQuickPartyParam.inc"
#include "PacketTypes/stRefundSkill.inc"
#include "PacketTypes/stRegenROIResult.inc"
#include "PacketTypes/stRegisterToBlackListParam.inc"
#include "PacketTypes/stRemoveFriendParam.inc"
#include "PacketTypes/stRenameCreature.inc"
#include "PacketTypes/stRentItemParam.inc"
#include "PacketTypes/stRepairDisjointMachineParam.inc"
#include "PacketTypes/stRepairEquipParam.inc"
#include "PacketTypes/stRequestAssaultPrice.inc"
#include "PacketTypes/stRequestBlackListParam.inc"
#include "PacketTypes/stRequestCharacSkillInfo.inc"
#include "PacketTypes/stRequestDisjointItemParam.inc"
#include "PacketTypes/stRequestIngameAdvertisement.inc"
#include "PacketTypes/stRequestMemberEnterParam.inc"
#include "PacketTypes/stRightOfChangeGrowType.inc"
#include "PacketTypes/stSaveCharacOption.inc"
#include "PacketTypes/stSecretShopBuyItemParam.inc"
#include "PacketTypes/stSecretShopOpenCloseParam.inc"
#include "PacketTypes/stSecurityLogParam.inc"
#include "PacketTypes/stSelectCharac.inc"
#include "PacketTypes/stSelectDungeonParam.inc"

#include "PacketTypes/stSelectStriker.inc"
#include "PacketTypes/stSendClearedMission.inc"
#include "PacketTypes/stSeriaRoomDecoEvent.inc"

#include "PacketTypes/stSetAreaParam.inc"

#include "PacketTypes/stSetPartyInfo.inc"
#include "PacketTypes/stSkillCommandParam.inc"
#include "PacketTypes/stSkillInit.inc"

#include "PacketTypes/stSoloTeleportParam.inc"

#include "PacketTypes/stStackableActionUse.inc"
#include "PacketTypes/stStartGameParam.inc"

#include "PacketTypes/stTeleportParam.inc"

#include "PacketTypes/stTournamentRewardSelect.inc"
#include "PacketTypes/stTournamentRewardSelectState.inc"
#include "PacketTypes/stUpdateContractOfCubeInfo.inc"

#include "PacketTypes/stUpgradeDisjointMachineParam.inc"
#include "PacketTypes/stUseCoinParam.inc"
{
    m_8 = 0;  // ORIG C1：+4 int、+8 word 清零
}
#include "PacketTypes/stUseDye.inc"
#include "PacketTypes/stUseLimitCube.inc"
{  // ORIG C1：+4 byte、+8/+c int、+10 word
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
}
#include "PacketTypes/stUseRenameCard.inc"
#include "PacketTypes/stUseSkillParam.inc"
#include "PacketTypes/stWalkoutPartyParam.inc"
#include "PacketTypes/stWarRoomWp.inc"
  // ORIG C1：+0x04 int 清零

// 显式实例化 IPacketDispatcher 模板
template class IPacketDispatcher<MSG_ENCHANT, stEnchantParam, ch_state0>;
template class IPacketDispatcher<MSG_USE_DYE, stUseDye, ch_state0>;
template class IPacketDispatcher<MSG_BUY_ITEM, BuyItemParam, ch_state3>;
template class IPacketDispatcher<MSG_GET_ITEM, stGetItemParam, ch_state5>;
template class IPacketDispatcher<MSG_MOVE_MAP, stMoveMapParam, ch_state0>;
template class IPacketDispatcher<MSG_SCAN_DLL, ScanBotByDllParam, ch_state0>;
template class IPacketDispatcher<MSG_SCAN_DRV, ScanBotByDrvParam, ch_state0>;
template class IPacketDispatcher<MSG_SECU_RPY, SecuCardRpyParam, ch_state2>;
template class IPacketDispatcher<MSG_TELEPORT, stTeleportParam, ch_state3>;
template class IPacketDispatcher<MSG_USE_COIN, stUseCoinParam, ch_state0>;
template class IPacketDispatcher<MSG_BUY_SKILL, stOnBuySkillParam, ch_state3>;
template class IPacketDispatcher<MSG_DROP_ITEM, stDropItemParam, ch_state3>;
template class IPacketDispatcher<MSG_RENT_ITEM, stRentItemParam, ch_state0>;
template class IPacketDispatcher<MSG_SORT_ITEM, stOnSortItemParam, ch_state3>;
template class IPacketDispatcher<MSG_USE_SKILL, stUseSkillParam, ch_state0>;
template class IPacketDispatcher<MSG_ADD_FRIEND, stAddFriendParam, ch_state3>;
template class IPacketDispatcher<MSG_BINGO_QUIZ, stBingoQuiz, ch_state3>;
template class IPacketDispatcher<MSG_BREAK_AWAY, stBreakAwayParam, ch_state3>;
template class IPacketDispatcher<MSG_QUEST_SHOP, STQuestShopBuyParam, ch_state0>;
template class IPacketDispatcher<MSG_QUICK_CHAT, stQuickChatParam, ch_state2>;
template class IPacketDispatcher<MSG_SKILL_INIT, stSkillInit, ch_state0>;
template class IPacketDispatcher<MSG_BLUE_MARBLE, stBlueMarble, ch_state3>;
template class IPacketDispatcher<MSG_COMBO_SKILL, stComboSkill, ch_state3>;
template class IPacketDispatcher<MSG_DELETE_ITEM, stDeleteItemParam, ch_state3>;
template class IPacketDispatcher<MSG_GUILD_BOARD, stGuildBoardParam, ch_state3>;
template class IPacketDispatcher<MSG_MOTION_HACK, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_PURIFY_ITEM, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_PVP_TIMEOUT, stOnPvPTimeOut, ch_state0>;
template class IPacketDispatcher<MSG_QUICK_PARTY, stQuickPartyParam, ch_state5>;
template class IPacketDispatcher<MSG_SCRIPT_HASH, ScriptHashParam, ch_state0>;
template class IPacketDispatcher<MSG_TEMP_STRUCT, TempParam, ch_state2>;
template class IPacketDispatcher<MSG_GROWTH_POWER, stGrowthPowerParam, ch_state3>;
template class IPacketDispatcher<MSG_ITEM_UPGRADE, STUdateItemParam, ch_state0>;
template class IPacketDispatcher<MSG_MAILBOX_SEND, stMailboxSendParam, ch_state0>;
template class IPacketDispatcher<MSG_REFUND_SKILL, stRefundSkill, ch_state0>;
template class IPacketDispatcher<MSG_REPAIR_EQUIP, stRepairEquipParam, ch_state0>;
template class IPacketDispatcher<MSG_USERINFO_REQ, stOnGetUserInfoParam, ch_state1>;
template class IPacketDispatcher<MSG_USERINFO_REQ, stOnRecoverStaminaParam, ch_state3>;
template class IPacketDispatcher<MSG_USER_SET_POS, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_CHARAC_SELECT, stSelectCharac, ch_state2>;
template class IPacketDispatcher<MSG_COLLECT_ITEMS, stCollectItems, ch_state0>;
template class IPacketDispatcher<MSG_DIE_CHARACTER, stDieCharacInDungeon, ch_state0>;
template class IPacketDispatcher<MSG_INFORM_NOTICE, InformNoticeData, ch_state3>;
template class IPacketDispatcher<MSG_MEMBER_SECEDE, stMemberSecedeParam, ch_state3>;
template class IPacketDispatcher<MSG_REMOVE_FRIEND, stRemoveFriendParam, ch_state3>;
template class IPacketDispatcher<MSG_SET_USER_AREA, stSetAreaParam, ch_state3>;
template class IPacketDispatcher<MSG_SKILL_COMMAND, stSkillCommandParam, ch_state3>;
template class IPacketDispatcher<MSG_SOLO_TELEPORT, stSoloTeleportParam, ch_state0>;
template class IPacketDispatcher<MSG_TITLEBOOK_GET, ParamBase, ch_state3>;
template class IPacketDispatcher<MSG_TITLEBOOK_PUT, ParamBase, ch_state3>;
template class IPacketDispatcher<MSG_WALKOUT_PARTY, stWalkoutPartyParam, ch_state3>;
template class IPacketDispatcher<MSG_BOSS_DIE_CHECK, stBossDieCheckParam, ch_state5>;
template class IPacketDispatcher<MSG_CHANGE_EMOTION, stChangeEmotionParam, ch_state3>;
template class IPacketDispatcher<MSG_FINISH_LOADING, stFinishLoadParam, ch_state0>;
template class IPacketDispatcher<MSG_HATCH_CREATURE, stHatchCreature, ch_state3>;
template class IPacketDispatcher<MSG_MOVE_ITEMSPACE, stMoveItemParam, ch_state3>;
template class IPacketDispatcher<MSG_P2P_STATISTICS, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_PARTY_SET_INFO, stSetPartyInfo, ch_state3>;
template class IPacketDispatcher<MSG_PARTY_TELEPORT, stPartyTeleport, ch_state0>;
template class IPacketDispatcher<MSG_SELECT_DUNGEON, stSelectDungeonParam, ch_state0>;
template class IPacketDispatcher<MSG_SELECT_STRIKER, stSelectStriker, ch_state3>;
template class IPacketDispatcher<MSG_USE_LIMIT_CUBE, stUseLimitCube, ch_state3>;
template class IPacketDispatcher<MsgClearQuestIndex, ReqDebugClearQuestResult, ch_state0>;
template class IPacketDispatcher<MsgLimitNpcBuyItem, LimitNpcBuyItem, ch_state3>;
template class IPacketDispatcher<MsgMercenaryReturn, SendMercenaryReturnResult, ch_state0>;
template class IPacketDispatcher<MSG_DENY_JOIN_GUILD, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_ENCHANT_BY_BEAD, stEnchantByBeadParam, ch_state3>;
template class IPacketDispatcher<MSG_ITEM_DICTIONARY, stItemDictionary, ch_state3>;
template class IPacketDispatcher<MSG_OBJECT_BRING_UP, stObjectBringUp, ch_state3>;
template class IPacketDispatcher<MSG_RENAME_CREATURE, stRenameCreature, ch_state0>;
template class IPacketDispatcher<MSG_SET_READY_STATE, stOnSetPvPReadyParam, ch_state0>;
template class IPacketDispatcher<MSG_USE_RENAME_CARD, stUseRenameCard, ch_state0>;
template class IPacketDispatcher<MSG_CHANGE_SKILLSLOT, stOnChangeSkillParam, ch_state3>;
template class IPacketDispatcher<MSG_CHECK_GUILD_JOIN, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_CREATE_CHARACTER, stCreateCharac, ch_state2>;
template class IPacketDispatcher<MSG_DELETE_CHARACTER, stDeleteCharac, ch_state2>;
template class IPacketDispatcher<MSG_EQUIPSLOT_SWITCH, stEquipslotSwitch, ch_state3>;
template class IPacketDispatcher<MSG_GROWTH_EQUIPMENT, stGrowthEquipment, ch_state3>;
template class IPacketDispatcher<MSG_GUILD_ATTENDANCE, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_MONSTERCARD_BIND, stMonstercardBind, ch_state3>;
template class IPacketDispatcher<MSG_OPEN_CERAPACKAGE, stOpenCeraPackage, ch_state0>;
template class IPacketDispatcher<MSG_USER_HISTORY_LOG, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_CANCEL_JOIN_GUILD, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_DONATE_GUILD_FUND, ParamBase, ch_state3>;
template class IPacketDispatcher<MSG_EXPERT_EXTRACTION, stExpertExtraction, ch_state3>;
template class IPacketDispatcher<MSG_INFORM_NOTICE_2ND, InformNoticeData2nd, ch_state3>;
template class IPacketDispatcher<MSG_SAVE_QUEST_NOTIFY, saveQuestNotifyParam, ch_state3>;
template class IPacketDispatcher<MSG_APPROVE_JOIN_GUILD, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_CHANGE_LETTER_STAT, stChangeLetterStatParam, ch_state3>;
template class IPacketDispatcher<MSG_FATIGUE_ATTENDANCE, stFatigueAttendance, ch_state0>;
template class IPacketDispatcher<MSG_MEMBER_ENTER_REPLY, stMemberEnterReplyParam, ch_state3>;
template class IPacketDispatcher<MSG_REQUEST_GUILD_JOIN, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_SAVE_GAME_OPTION_1, ParamBase, ch_state2>;
template class IPacketDispatcher<MSG_SAVE_GAME_OPTION_2, ParamBase, ch_state2>;
template class IPacketDispatcher<MsgRequestAssaultPrice, stRequestAssaultPrice, ch_state3>;
template class IPacketDispatcher<MSG_ACHIEVEMENT_TRIGGER, ParamBase, ch_state3>;
template class IPacketDispatcher<MSG_CHANGE_HOST_WARROOM, stChangeHostWarRoom, ch_state0>;
template class IPacketDispatcher<MSG_COMPATIBILITY_INDEX, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_DECREASE_DURABILITY, stOnUseEquipParam, ch_state0>;
template class IPacketDispatcher<MSG_DELETE_TO_BLACKLIST, stDeleteToBlackListParam, ch_state3>;
template class IPacketDispatcher<MSG_RESET_RANDOM_OPTION, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_SECURITY_LOG_PACKET, stSecurityLogParam, ch_state0>;
template class IPacketDispatcher<MSG_TITLEBOOK_OTHERUSER, ParamBase, ch_state3>;
template class IPacketDispatcher<MsgMercenaryCompetition, SendMercenaryCompetitionResult, ch_state0>;
template class IPacketDispatcher<MSG_CHANGE_AVATAR_OPTION, stChangeAvatarOption, ch_state3>;
template class IPacketDispatcher<MSG_CHANGE_RANDOM_OPTION, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_ENTER_DISJOINT_STORE, stEnterDisjointStoreParam, ch_state3>;
template class IPacketDispatcher<MSG_GUILD_CARGO_POP_ITEM, stGuildCargoPopItemParam, ch_state0>;
template class IPacketDispatcher<MSG_MAILBOX_EXTRACT_ITEM, stMailboxExtractParam, ch_state0>;
template class IPacketDispatcher<MSG_REQUEST_MEMBER_ENTER, stRequestMemberEnterParam, ch_state3>;
template class IPacketDispatcher<MSG_SECRET_SHOP_BUY_ITEM, stSecretShopBuyItemParam, ch_state0>;
template class IPacketDispatcher<MSG_SERIAROOM_DECO_EVENT, stSeriaRoomDecoEvent, ch_state0>;
template class IPacketDispatcher<MSG_SERIAROOM_DECO_EVENT, stSeriaRoomDecoEvent, ch_state3>;
template class IPacketDispatcher<MSG_STACKABLE_ACTION_USE, stStackableActionUse, ch_state3>;
template class IPacketDispatcher<MSG_CLIENT_SPEC_STATISTIC, stClientSpecStatisticParam, ch_state2>;
template class IPacketDispatcher<MSG_CREATE_DISJOINT_STORE, stCreateDisjointStoreParam, ch_state3>;
template class IPacketDispatcher<MSG_GUILD_CARGO_MOVE_ITEM, stGuildCargoMoveItemParam, ch_state0>;
template class IPacketDispatcher<MSG_GUILD_CARGO_PUSH_ITEM, stGuildCargoPushItemParam, ch_state0>;
template class IPacketDispatcher<MSG_REGISTER_TO_BLACKLIST, stRegisterToBlackListParam, ch_state3>;
template class IPacketDispatcher<MSG_REQUEST_DISJOINT_ITEM, stRequestDisjointItemParam, ch_state3>;
template class IPacketDispatcher<MSG_SAVE_CHARACTER_OPTION, stSaveCharacOption, ch_state2>;
template class IPacketDispatcher<MsgTournamentRewardSelect, stTournamentRewardSelect, ch_state0>;
template class IPacketDispatcher<MSG_ABNORMAL_USE_STACKABLE, AbnormalUseStackable, ch_state0>;
template class IPacketDispatcher<MSG_FLOAT_RDATA_MODULATION, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_PARTY_TELEPORT_CONFIRM, stPartyTeleportConfirm, ch_state0>;
template class IPacketDispatcher<MSG_PVP_MISSION_HP_PERCENT, stSendClearedMission, ch_state3>;
template class IPacketDispatcher<MSG_SECRET_SHOP_OPEN_CLOSE, stSecretShopOpenCloseParam, ch_state0>;
template class IPacketDispatcher<MSG_WARROOM_WP_PER_MONSTER, stWarRoomWp, ch_state0>;
template class IPacketDispatcher<MSG_ITEM_HYPER_LINK_MESSAGE, stItemHyperLinkMessage, ch_state3>;
template class IPacketDispatcher<MSG_PVP_MISSION_COMBO_CLEAR, ParamBase, ch_state3>;
template class IPacketDispatcher<MSG_BLUE_MARBLE_CONFIRM_INFO, stBlueMarbleConfirmInfo, ch_state3>;
template class IPacketDispatcher<MSG_LOG_INGAME_ADVERTISEMENT, stLogIngameAdvertisement, ch_state2>;
template class IPacketDispatcher<MSG_CHANGE_ANOTHER_SKILL_TREE, stChangeAnotherSkillTree, ch_state0>;
template class IPacketDispatcher<MSG_REQUEST_CHARAC_SKILL_INFO, stRequestCharacSkillInfo, ch_state3>;
template class IPacketDispatcher<MSG_RIGHT_OF_CHANGE_GROW_TYPE, stRightOfChangeGrowType, ch_state3>;
template class IPacketDispatcher<MSG_REGENERATION_RANDOM_OPTION, stRegenROIResult, ch_state0>;
template class IPacketDispatcher<MSG_EXCHANGE_RANDOM_ITEM_REWARD, stExchangeRandomItemReward, ch_state0>;
template class IPacketDispatcher<MSG_GOLD_TAKE_INCREASING_AMOUNT, stGoldTakeIncreasingAmount, ch_state0>;
template class IPacketDispatcher<MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE, stExpandEquipslotFlagUpdate, ch_state3>;
template class IPacketDispatcher<MSG_REQUEST_INGAME_ADVERTISEMENT, stRequestIngameAdvertisement, ch_state2>;
template class IPacketDispatcher<MSG_UPDATE_CONTRACT_OF_CUBE_INFO, stUpdateContractOfCubeInfo, ch_state0>;
template class IPacketDispatcher<MSG_AVATAR_DISJOINT_RANDOM_REWARD, stAvatarDisjointRandomReward, ch_state0>;
template class IPacketDispatcher<MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON, ParamBase, ch_state2>;
template class IPacketDispatcher<MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET, stComboSkillExtensionQuickSlotReset, ch_state3>;
template class IPacketDispatcher<MSG_BASE, stBingoReward, ch_state3>;
template class IPacketDispatcher<MSG_BASE, stEnchantParam, ch_state0>;
template class IPacketDispatcher<MSG_BASE, STQuestShopParam, ch_state0>;
template class IPacketDispatcher<MSG_BASE, stGrowthCreature, ch_state3>;
template class IPacketDispatcher<MSG_BASE, stStartGameParam, ch_state0>;
template class IPacketDispatcher<MSG_BASE, stGuildCargoParam, ch_state0>;
template class IPacketDispatcher<MSG_BASE, stLeavePartyParam, ch_state0>;
template class IPacketDispatcher<MSG_BASE, stQuickPartyParam, ch_state5>;
template class IPacketDispatcher<MSG_BASE, stGrowthPowerParam, ch_state0>;
template class IPacketDispatcher<MSG_BASE, stMailboxOpenParam, ch_state0>;
template class IPacketDispatcher<MSG_BASE, stCallMemberListParam, ch_state3>;
template class IPacketDispatcher<MSG_BASE, CheckSecurityProtection, ch_state0>;
template class IPacketDispatcher<MSG_BASE, stRequestBlackListParam, ch_state3>;
template class IPacketDispatcher<MSG_BASE, stCharacSlotExtendEffect, ch_state0>;
template class IPacketDispatcher<MSG_BASE, stGuildCargoHistoryParam, ch_state0>;
template class IPacketDispatcher<MSG_BASE, PcRoomPlayTimeRewardParam, ch_state3>;
template class IPacketDispatcher<MSG_BASE, stCloseDisjointStoreParam, ch_state3>;
template class IPacketDispatcher<MSG_BASE, stRepairDisjointMachineParam, ch_state3>;
template class IPacketDispatcher<MSG_BASE, stTournamentRewardSelectState, ch_state0>;
template class IPacketDispatcher<MSG_BASE, stUpgradeDisjointMachineParam, ch_state3>;
template class IPacketDispatcher<MSG_BASE, stImageCommunicationEquipmentUse, ch_state3>;
template class IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>;
template class IPacketDispatcher<MSG_BASE, ParamBase, ch_state2>;
template class IPacketDispatcher<MSG_BASE, ParamBase, ch_state3>;
template class IPacketDispatcher<MSG_BASE, ReqResult, ch_state3>;

// ---------------------------------------------------------------------------
// base64_encode / base64_decode（ORIG T 0x0807ea2e / 0x0807ebf4）。
// 查表经 const std::string base64_chars：encode 用 operator[]（ORIG 调用点
// `movl $0x93f56f4,(%esp); call _ZNKSsixEj`），decode 用 find(char,0)；
// 追加经 `out += char`（_ZNSspLEc），'=' 补位（$0x3d）。encode 无 out.clear
// （直接 +=）。is_base64 为 TU 内部（_ZL9is_base64h t 0x0807e9f8）。
// ---------------------------------------------------------------------------
static bool is_base64(unsigned char c)
{
    return isalnum(c) || (c == '+') || (c == '/');
}

#include "PacketTypes/base64_decode.inc"

