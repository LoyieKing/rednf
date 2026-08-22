// df_game_r 还原 —— MSG / Param 构造函数 + IPacketDispatcher 显式实例化
#include "PacketTypes.h"
#include <cstring>


MSG_ABNORMAL_USE_STACKABLE::MSG_ABNORMAL_USE_STACKABLE() {}
MSG_ACHIEVEMENT_TRIGGER::MSG_ACHIEVEMENT_TRIGGER() {}
MSG_ADD_FRIEND::MSG_ADD_FRIEND() {}
MSG_APPROVE_JOIN_GUILD::MSG_APPROVE_JOIN_GUILD() {}
MSG_AVATAR_DISJOINT_RANDOM_REWARD::MSG_AVATAR_DISJOINT_RANDOM_REWARD() {}
MSG_BINGO_QUIZ::MSG_BINGO_QUIZ() {}
MSG_BLUE_MARBLE::MSG_BLUE_MARBLE() {}
MSG_BLUE_MARBLE_CONFIRM_INFO::MSG_BLUE_MARBLE_CONFIRM_INFO() {}
MSG_BOSS_DIE_CHECK::MSG_BOSS_DIE_CHECK() {}
MSG_BREAK_AWAY::MSG_BREAK_AWAY() {}
MSG_BUY_ITEM::MSG_BUY_ITEM() {}
MSG_BUY_SKILL::MSG_BUY_SKILL() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_CANCEL_JOIN_GUILD::MSG_CANCEL_JOIN_GUILD() {}
MSG_CHANGE_ANOTHER_SKILL_TREE::MSG_CHANGE_ANOTHER_SKILL_TREE() {}
MSG_CHANGE_AVATAR_OPTION::MSG_CHANGE_AVATAR_OPTION() {}
MSG_CHANGE_EMOTION::MSG_CHANGE_EMOTION() {}
MSG_CHANGE_HOST_WARROOM::MSG_CHANGE_HOST_WARROOM() {}
MSG_CHANGE_LETTER_STAT::MSG_CHANGE_LETTER_STAT() {}
MSG_CHANGE_RANDOM_OPTION::MSG_CHANGE_RANDOM_OPTION() {}
MSG_CHANGE_SKILLSLOT::MSG_CHANGE_SKILLSLOT() {}
MSG_CHARAC_SELECT::MSG_CHARAC_SELECT() {}
MSG_CHECK_GUILD_JOIN::MSG_CHECK_GUILD_JOIN() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_CLIENT_SPEC_STATISTIC::MSG_CLIENT_SPEC_STATISTIC() {}
MSG_COLLECT_ITEMS::MSG_COLLECT_ITEMS() {}
MSG_COMBO_SKILL::MSG_COMBO_SKILL() {}
MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET::MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET() {}
MSG_COMPATIBILITY_INDEX::MSG_COMPATIBILITY_INDEX() {}
MSG_CREATE_CHARACTER::MSG_CREATE_CHARACTER() {}
MSG_CREATE_DISJOINT_STORE::MSG_CREATE_DISJOINT_STORE() {}
MSG_DECREASE_DURABILITY::MSG_DECREASE_DURABILITY() {}
MSG_DELETE_CHARACTER::MSG_DELETE_CHARACTER() {}
MSG_DELETE_ITEM::MSG_DELETE_ITEM() {}
MSG_DELETE_TO_BLACKLIST::MSG_DELETE_TO_BLACKLIST() {}
MSG_DENY_JOIN_GUILD::MSG_DENY_JOIN_GUILD() {}
MSG_DIE_CHARACTER::MSG_DIE_CHARACTER() {}
MSG_DONATE_GUILD_FUND::MSG_DONATE_GUILD_FUND() {}
MSG_DROP_ITEM::MSG_DROP_ITEM() {}
MSG_ENCHANT::MSG_ENCHANT() {}
MSG_ENCHANT_BY_BEAD::MSG_ENCHANT_BY_BEAD() {}
MSG_ENTER_DISJOINT_STORE::MSG_ENTER_DISJOINT_STORE() {}
MSG_EQUIPSLOT_SWITCH::MSG_EQUIPSLOT_SWITCH() {}
MSG_EXCHANGE_RANDOM_ITEM_REWARD::MSG_EXCHANGE_RANDOM_ITEM_REWARD() {}
MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE::MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE() {}
MSG_EXPERT_EXTRACTION::MSG_EXPERT_EXTRACTION() {}
MSG_FATIGUE_ATTENDANCE::MSG_FATIGUE_ATTENDANCE() {}
MSG_FINISH_LOADING::MSG_FINISH_LOADING() {}
MSG_FLOAT_RDATA_MODULATION::MSG_FLOAT_RDATA_MODULATION() {}
MSG_GET_ITEM::MSG_GET_ITEM() {}
MSG_GOLD_TAKE_INCREASING_AMOUNT::MSG_GOLD_TAKE_INCREASING_AMOUNT() {}
MSG_GROWTH_EQUIPMENT::MSG_GROWTH_EQUIPMENT() {}
MSG_GROWTH_POWER::MSG_GROWTH_POWER() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_GUILD_ATTENDANCE::MSG_GUILD_ATTENDANCE() {}
MSG_GUILD_BOARD::MSG_GUILD_BOARD() {}
MSG_GUILD_CARGO_MOVE_ITEM::MSG_GUILD_CARGO_MOVE_ITEM() {}
MSG_GUILD_CARGO_POP_ITEM::MSG_GUILD_CARGO_POP_ITEM() {}
MSG_GUILD_CARGO_PUSH_ITEM::MSG_GUILD_CARGO_PUSH_ITEM() {}
MSG_HATCH_CREATURE::MSG_HATCH_CREATURE() {}
MSG_INFORM_NOTICE::MSG_INFORM_NOTICE() {}
MSG_INFORM_NOTICE_2ND::MSG_INFORM_NOTICE_2ND() {}
MSG_ITEM_DICTIONARY::MSG_ITEM_DICTIONARY() {}
MSG_ITEM_HYPER_LINK_MESSAGE::MSG_ITEM_HYPER_LINK_MESSAGE() {}
MSG_ITEM_UPGRADE::MSG_ITEM_UPGRADE() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_LOG_INGAME_ADVERTISEMENT::MSG_LOG_INGAME_ADVERTISEMENT() {}
MSG_MAILBOX_EXTRACT_ITEM::MSG_MAILBOX_EXTRACT_ITEM() {}
MSG_MAILBOX_SEND::MSG_MAILBOX_SEND() {}
MSG_MEMBER_ENTER_REPLY::MSG_MEMBER_ENTER_REPLY() {}
MSG_MEMBER_SECEDE::MSG_MEMBER_SECEDE() {}
MSG_MONSTERCARD_BIND::MSG_MONSTERCARD_BIND() {}
MSG_MOTION_HACK::MSG_MOTION_HACK() {}
MSG_MOVE_ITEMSPACE::MSG_MOVE_ITEMSPACE() {}
MSG_MOVE_MAP::MSG_MOVE_MAP() {}
MSG_OBJECT_BRING_UP::MSG_OBJECT_BRING_UP() {}
MSG_OPEN_CERAPACKAGE::MSG_OPEN_CERAPACKAGE() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE::MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE() {}
MSG_P2P_STATISTICS::MSG_P2P_STATISTICS() {}
MSG_PARTY_SET_INFO::MSG_PARTY_SET_INFO() {}
MSG_PARTY_TELEPORT::MSG_PARTY_TELEPORT() {}
MSG_PARTY_TELEPORT_CONFIRM::MSG_PARTY_TELEPORT_CONFIRM() {}
MSG_PURIFY_ITEM::MSG_PURIFY_ITEM() {}
MSG_PVP_MISSION_COMBO_CLEAR::MSG_PVP_MISSION_COMBO_CLEAR() {}
MSG_PVP_MISSION_HP_PERCENT::MSG_PVP_MISSION_HP_PERCENT() {}
MSG_PVP_TIMEOUT::MSG_PVP_TIMEOUT() {}
MSG_QUEST_SHOP::MSG_QUEST_SHOP() {}
MSG_QUICK_CHAT::MSG_QUICK_CHAT() {}
MSG_QUICK_PARTY::MSG_QUICK_PARTY() {}
MSG_REFUND_SKILL::MSG_REFUND_SKILL() {}
MSG_REGENERATION_RANDOM_OPTION::MSG_REGENERATION_RANDOM_OPTION() {}
MSG_REGISTER_TO_BLACKLIST::MSG_REGISTER_TO_BLACKLIST() {}
MSG_REMOVE_FRIEND::MSG_REMOVE_FRIEND() {}
MSG_RENAME_CREATURE::MSG_RENAME_CREATURE() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_RENT_ITEM::MSG_RENT_ITEM() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_REPAIR_EQUIP::MSG_REPAIR_EQUIP() {}
MSG_REQUEST_CHARAC_SKILL_INFO::MSG_REQUEST_CHARAC_SKILL_INFO() {}
MSG_REQUEST_DISJOINT_ITEM::MSG_REQUEST_DISJOINT_ITEM() {}
MSG_REQUEST_INGAME_ADVERTISEMENT::MSG_REQUEST_INGAME_ADVERTISEMENT() {}
MSG_REQUEST_GUILD_JOIN::MSG_REQUEST_GUILD_JOIN() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_REQUEST_MEMBER_ENTER::MSG_REQUEST_MEMBER_ENTER() {}
MSG_RESET_RANDOM_OPTION::MSG_RESET_RANDOM_OPTION() {}
MSG_RIGHT_OF_CHANGE_GROW_TYPE::MSG_RIGHT_OF_CHANGE_GROW_TYPE() {}
MSG_SAVE_CHARACTER_OPTION::MSG_SAVE_CHARACTER_OPTION() {}
MSG_SAVE_GAME_OPTION_1::MSG_SAVE_GAME_OPTION_1() {}
MSG_SAVE_GAME_OPTION_2::MSG_SAVE_GAME_OPTION_2() {}
MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON() {}
MSG_SAVE_QUEST_NOTIFY::MSG_SAVE_QUEST_NOTIFY() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_SCAN_DLL::MSG_SCAN_DLL() {}
MSG_SCAN_DRV::MSG_SCAN_DRV() {}
MSG_SCRIPT_HASH::MSG_SCRIPT_HASH() {}
MSG_SECRET_SHOP_BUY_ITEM::MSG_SECRET_SHOP_BUY_ITEM() {}
MSG_SECRET_SHOP_OPEN_CLOSE::MSG_SECRET_SHOP_OPEN_CLOSE() {}
MSG_SECURITY_LOG_PACKET::MSG_SECURITY_LOG_PACKET() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_SECU_RPY::MSG_SECU_RPY() {}
MSG_SELECT_DUNGEON::MSG_SELECT_DUNGEON() {}
MSG_SELECT_STRIKER::MSG_SELECT_STRIKER() {}
MSG_SERIAROOM_DECO_EVENT::MSG_SERIAROOM_DECO_EVENT() {}
MSG_SET_READY_STATE::MSG_SET_READY_STATE() {}
MSG_SET_USER_AREA::MSG_SET_USER_AREA() {}
MSG_SKILL_COMMAND::MSG_SKILL_COMMAND() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_SKILL_INIT::MSG_SKILL_INIT() {}
MSG_SOLO_TELEPORT::MSG_SOLO_TELEPORT() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_SORT_ITEM::MSG_SORT_ITEM() {}
MSG_STACKABLE_ACTION_USE::MSG_STACKABLE_ACTION_USE() {}
MSG_TELEPORT::MSG_TELEPORT() {}
MSG_TEMP_STRUCT::MSG_TEMP_STRUCT() {}
MSG_TITLEBOOK_GET::MSG_TITLEBOOK_GET() {}
MSG_TITLEBOOK_OTHERUSER::MSG_TITLEBOOK_OTHERUSER() {}
MSG_TITLEBOOK_PUT::MSG_TITLEBOOK_PUT() {}
MSG_UPDATE_CONTRACT_OF_CUBE_INFO::MSG_UPDATE_CONTRACT_OF_CUBE_INFO() {}
MSG_USERINFO_REQ::MSG_USERINFO_REQ() {}
MSG_USER_HISTORY_LOG::MSG_USER_HISTORY_LOG() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_USER_SET_POS::MSG_USER_SET_POS() {}
MSG_USE_COIN::MSG_USE_COIN() {}
MSG_USE_DYE::MSG_USE_DYE() {}
MSG_USE_LIMIT_CUBE::MSG_USE_LIMIT_CUBE() { memset(m_pad, 0, sizeof(m_pad)); }
MSG_USE_RENAME_CARD::MSG_USE_RENAME_CARD() {}
MSG_USE_SKILL::MSG_USE_SKILL() {}
MSG_WALKOUT_PARTY::MSG_WALKOUT_PARTY() {}
MSG_WARROOM_WP_PER_MONSTER::MSG_WARROOM_WP_PER_MONSTER() {}
MsgClearQuestIndex::MsgClearQuestIndex() {}
MsgLimitNpcBuyItem::MsgLimitNpcBuyItem() {}
MsgMercenaryCompetition::MsgMercenaryCompetition() {}
MsgMercenaryReturn::MsgMercenaryReturn() {}
MsgRequestAssaultPrice::MsgRequestAssaultPrice() {}
MsgTournamentRewardSelect::MsgTournamentRewardSelect() {}

AbnormalUseStackable::AbnormalUseStackable() {}
BuyItemParam::BuyItemParam() {}
CheckSecurityProtection::CheckSecurityProtection() {}
InformNoticeData::InformNoticeData() {}
InformNoticeData2nd::InformNoticeData2nd() {}
LimitNpcBuyItem::LimitNpcBuyItem() {}
PcRoomPlayTimeRewardParam::PcRoomPlayTimeRewardParam() {}
ReqDebugClearQuestResult::ReqDebugClearQuestResult() {}
ReqResult::ReqResult() {}
STQuestShopBuyParam::STQuestShopBuyParam() {}
STQuestShopParam::STQuestShopParam() {}
STUdateItemParam::STUdateItemParam() {}
ScanBotByDllParam::ScanBotByDllParam() {}
ScanBotByDrvParam::ScanBotByDrvParam() {}
ScriptHashParam::ScriptHashParam() {}
SecuCardRpyParam::SecuCardRpyParam() {}
SendMercenaryCompetitionResult::SendMercenaryCompetitionResult() {}
SendMercenaryReturnResult::SendMercenaryReturnResult() {}
TempParam::TempParam() {}
saveQuestNotifyParam::saveQuestNotifyParam() {}
stAddFriendParam::stAddFriendParam() {}
stAvatarDisjointRandomReward::stAvatarDisjointRandomReward() {}
stBingoQuiz::stBingoQuiz() {}
stBingoReward::stBingoReward() {}
stBlueMarble::stBlueMarble() {}
stBlueMarbleConfirmInfo::stBlueMarbleConfirmInfo() {}
stBossDieCheckParam::stBossDieCheckParam() {}
stBreakAwayParam::stBreakAwayParam() {}
stCallMemberListParam::stCallMemberListParam() {}
stChangeAnotherSkillTree::stChangeAnotherSkillTree() {}
stChangeAvatarOption::stChangeAvatarOption() {}
stChangeEmotionParam::stChangeEmotionParam() {}
stChangeHostWarRoom::stChangeHostWarRoom() {}
stChangeLetterStatParam::stChangeLetterStatParam() {}
stCharacSlotExtendEffect::stCharacSlotExtendEffect() {}
stClientSpecStatisticParam::stClientSpecStatisticParam() {}
stCloseDisjointStoreParam::stCloseDisjointStoreParam() {}
stCollectItems::stCollectItems() {}
stComboSkill::stComboSkill() {}
stComboSkillExtensionQuickSlotReset::stComboSkillExtensionQuickSlotReset() {}
stCreateCharac::stCreateCharac() {}
stCreateDisjointStoreParam::stCreateDisjointStoreParam() {}
stDeleteCharac::stDeleteCharac() {}
stDeleteItemParam::stDeleteItemParam() {}
stDeleteToBlackListParam::stDeleteToBlackListParam() {}
stDieCharacInDungeon::stDieCharacInDungeon() {}
stDropItemParam::stDropItemParam() {}
stEnchantByBeadParam::stEnchantByBeadParam() {}
stEnchantParam::stEnchantParam() {}
stEnterDisjointStoreParam::stEnterDisjointStoreParam() {}
stEquipslotSwitch::stEquipslotSwitch() {}
stExchangeRandomItemReward::stExchangeRandomItemReward() {}
stExpandEquipslotFlagUpdate::stExpandEquipslotFlagUpdate() {}
stExpertExtraction::stExpertExtraction() {}
stFatigueAttendance::stFatigueAttendance() {}
stFinishLoadParam::stFinishLoadParam() {}
stGetItemParam::stGetItemParam() {}
stGoldTakeIncreasingAmount::stGoldTakeIncreasingAmount() {}
stGrowthCreature::stGrowthCreature() {}
stGrowthEquipment::stGrowthEquipment() {}
stGrowthPowerParam::stGrowthPowerParam() {}
stGuildBoardParam::stGuildBoardParam() {}
stGuildCargoHistoryParam::stGuildCargoHistoryParam() {}
stGuildCargoMoveItemParam::stGuildCargoMoveItemParam() {}
stGuildCargoParam::stGuildCargoParam() {}
stGuildCargoPopItemParam::stGuildCargoPopItemParam() {}
stGuildCargoPushItemParam::stGuildCargoPushItemParam() {}
stHatchCreature::stHatchCreature() {}
stImageCommunicationEquipmentUse::stImageCommunicationEquipmentUse() {}
stItemDictionary::stItemDictionary() {}
stItemHyperLinkMessage::stItemHyperLinkMessage() {}
stLeavePartyParam::stLeavePartyParam() {}
stLogIngameAdvertisement::stLogIngameAdvertisement() {}
stMailboxExtractParam::stMailboxExtractParam() {}
stMailboxOpenParam::stMailboxOpenParam() {}
stMailboxSendParam::stMailboxSendParam() {}
stMemberEnterReplyParam::stMemberEnterReplyParam() {}
stMemberSecedeParam::stMemberSecedeParam() {}
stMonstercardBind::stMonstercardBind() {}
stMoveItemParam::stMoveItemParam() {}
stMoveMapParam::stMoveMapParam() {}
stObjectBringUp::stObjectBringUp() {}
stOnBuySkillParam::stOnBuySkillParam() {}
stOnChangeSkillParam::stOnChangeSkillParam() {}
stOnGetUserInfoParam::stOnGetUserInfoParam() {}
stOnPvPTimeOut::stOnPvPTimeOut() {}
stOnRecoverStaminaParam::stOnRecoverStaminaParam() {}
stOnSetPvPReadyParam::stOnSetPvPReadyParam() {}
stOnSortItemParam::stOnSortItemParam() {}
stOnUseEquipParam::stOnUseEquipParam() {}
stOpenCeraPackage::stOpenCeraPackage() {}
stPartyTeleport::stPartyTeleport() {}
stPartyTeleportConfirm::stPartyTeleportConfirm() {}
stQuickChatParam::stQuickChatParam() {}
stQuickPartyParam::stQuickPartyParam() {}
stRefundSkill::stRefundSkill() {}
stRegenROIResult::stRegenROIResult() {}
stRegisterToBlackListParam::stRegisterToBlackListParam() {}
stRemoveFriendParam::stRemoveFriendParam() {}
stRenameCreature::stRenameCreature() {}
stRentItemParam::stRentItemParam() {}
stRepairDisjointMachineParam::stRepairDisjointMachineParam() {}
stRepairEquipParam::stRepairEquipParam() {}
stRequestAssaultPrice::stRequestAssaultPrice() {}
stRequestBlackListParam::stRequestBlackListParam() {}
stRequestCharacSkillInfo::stRequestCharacSkillInfo() {}
stRequestDisjointItemParam::stRequestDisjointItemParam() {}
stRequestIngameAdvertisement::stRequestIngameAdvertisement() {}
stRequestMemberEnterParam::stRequestMemberEnterParam() {}
stRightOfChangeGrowType::stRightOfChangeGrowType() {}
stSaveCharacOption::stSaveCharacOption() {}
stSecretShopBuyItemParam::stSecretShopBuyItemParam() {}
stSecretShopOpenCloseParam::stSecretShopOpenCloseParam() {}
stSecurityLogParam::stSecurityLogParam() {}
stSelectCharac::stSelectCharac() {}
stSelectDungeonParam::stSelectDungeonParam() {}
stSelectStriker::stSelectStriker() {}
stSendClearedMission::stSendClearedMission() {}
stSeriaRoomDecoEvent::stSeriaRoomDecoEvent() {}
stSetAreaParam::stSetAreaParam() {}
stSetPartyInfo::stSetPartyInfo() {}
stSkillCommandParam::stSkillCommandParam() {}
stSkillInit::stSkillInit() {}
stSoloTeleportParam::stSoloTeleportParam() {}
stStackableActionUse::stStackableActionUse() {}
stStartGameParam::stStartGameParam() {}
stTeleportParam::stTeleportParam() {}
stTournamentRewardSelect::stTournamentRewardSelect() {}
stTournamentRewardSelectState::stTournamentRewardSelectState() {}
stUpdateContractOfCubeInfo::stUpdateContractOfCubeInfo() {}
stUpgradeDisjointMachineParam::stUpgradeDisjointMachineParam() {}
stUseCoinParam::stUseCoinParam() {}
stUseDye::stUseDye() {}
stUseLimitCube::stUseLimitCube() {}
stUseRenameCard::stUseRenameCard() {}
stUseSkillParam::stUseSkillParam() {}
stWalkoutPartyParam::stWalkoutPartyParam() {}
stWarRoomWp::stWarRoomWp() {}

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
