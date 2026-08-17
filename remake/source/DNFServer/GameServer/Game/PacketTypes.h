#ifndef DNF_GAME_PACKET_TYPES_H_
#define DNF_GAME_PACKET_TYPES_H_

// ============================================================================
// df_game_r 还原 —— MSG / Param 类型定义（批量生成）
// 各类型成员布局待细化；当前用 char 数组占位以生成符号。
// 所有 MSG 继承 MSG_BASE，所有 Param 继承 ParamBase。
// ============================================================================

#include "PacketDispatcher.h"

// ---------------------------------------------------------------------------
// MSG 类型（占位大小 0x100，后续按 ORIG 构造函数修正）
// ---------------------------------------------------------------------------
class MSG_ABNORMAL_USE_STACKABLE : public MSG_BASE
{
public:
    MSG_ABNORMAL_USE_STACKABLE();
    char m_pad[0x100];
};

class MSG_ACHIEVEMENT_TRIGGER : public MSG_BASE
{
public:
    MSG_ACHIEVEMENT_TRIGGER();
    char m_pad[0x100];
};

class MSG_ADD_FRIEND : public MSG_BASE
{
public:
    MSG_ADD_FRIEND();
    char m_pad[0x100];
};

class MSG_APPROVE_JOIN_GUILD : public MSG_BASE
{
public:
    MSG_APPROVE_JOIN_GUILD();
    char m_pad[0x100];
};

class MSG_AVATAR_DISJOINT_RANDOM_REWARD : public MSG_BASE
{
public:
    MSG_AVATAR_DISJOINT_RANDOM_REWARD();
    char m_pad[0x100];
};

class MSG_BINGO_QUIZ : public MSG_BASE
{
public:
    MSG_BINGO_QUIZ();
    char m_pad[0x100];
};

class MSG_BLUE_MARBLE : public MSG_BASE
{
public:
    MSG_BLUE_MARBLE();
    char m_pad[0x100];
};

class MSG_BLUE_MARBLE_CONFIRM_INFO : public MSG_BASE
{
public:
    MSG_BLUE_MARBLE_CONFIRM_INFO();
    char m_pad[0x100];
};

class MSG_BOSS_DIE_CHECK : public MSG_BASE
{
public:
    MSG_BOSS_DIE_CHECK();
    char m_pad[0x100];
};

class MSG_BREAK_AWAY : public MSG_BASE
{
public:
    MSG_BREAK_AWAY();
    char m_pad[0x100];
};

class MSG_BUY_ITEM : public MSG_BASE
{
public:
    MSG_BUY_ITEM();
    char m_pad[0x100];
};

class MSG_BUY_SKILL : public MSG_BASE
{
public:
    MSG_BUY_SKILL();
    char m_pad[0x100];
};

class MSG_CANCEL_JOIN_GUILD : public MSG_BASE
{
public:
    MSG_CANCEL_JOIN_GUILD();
    char m_pad[0x100];
};

class MSG_CHANGE_ANOTHER_SKILL_TREE : public MSG_BASE
{
public:
    MSG_CHANGE_ANOTHER_SKILL_TREE();
    char m_pad[0x100];
};

class MSG_CHANGE_AVATAR_OPTION : public MSG_BASE
{
public:
    MSG_CHANGE_AVATAR_OPTION();
    char m_pad[0x100];
};

class MSG_CHANGE_EMOTION : public MSG_BASE
{
public:
    MSG_CHANGE_EMOTION();
    char m_pad[0x100];
};

class MSG_CHANGE_HOST_WARROOM : public MSG_BASE
{
public:
    MSG_CHANGE_HOST_WARROOM();
    char m_pad[0x100];
};

class MSG_CHANGE_LETTER_STAT : public MSG_BASE
{
public:
    MSG_CHANGE_LETTER_STAT();
    char m_pad[0x100];
};

class MSG_CHANGE_RANDOM_OPTION : public MSG_BASE
{
public:
    MSG_CHANGE_RANDOM_OPTION();
    char m_pad[0x100];
};

class MSG_CHANGE_SKILLSLOT : public MSG_BASE
{
public:
    MSG_CHANGE_SKILLSLOT();
    char m_pad[0x100];
};

class MSG_CHARAC_SELECT : public MSG_BASE
{
public:
    MSG_CHARAC_SELECT();
    char m_pad[0x100];
};

class MSG_CHECK_GUILD_JOIN : public MSG_BASE
{
public:
    MSG_CHECK_GUILD_JOIN();
    char m_pad[0x100];
};

class MSG_CLIENT_SPEC_STATISTIC : public MSG_BASE
{
public:
    MSG_CLIENT_SPEC_STATISTIC();
    char m_pad[0x100];
};

class MSG_COLLECT_ITEMS : public MSG_BASE
{
public:
    MSG_COLLECT_ITEMS();
    char m_pad[0x100];
};

class MSG_COMBO_SKILL : public MSG_BASE
{
public:
    MSG_COMBO_SKILL();
    char m_pad[0x100];
};

class MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET : public MSG_BASE
{
public:
    MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET();
    char m_pad[0x100];
};

class MSG_COMPATIBILITY_INDEX : public MSG_BASE
{
public:
    MSG_COMPATIBILITY_INDEX();
    char m_pad[0x100];
};

class MSG_CREATE_CHARACTER : public MSG_BASE
{
public:
    MSG_CREATE_CHARACTER();
    char m_pad[0x100];
};

class MSG_CREATE_DISJOINT_STORE : public MSG_BASE
{
public:
    MSG_CREATE_DISJOINT_STORE();
    char m_pad[0x100];
};

class MSG_DECREASE_DURABILITY : public MSG_BASE
{
public:
    MSG_DECREASE_DURABILITY();
    char m_pad[0x100];
};

class MSG_DELETE_CHARACTER : public MSG_BASE
{
public:
    MSG_DELETE_CHARACTER();
    char m_pad[0x100];
};

class MSG_DELETE_ITEM : public MSG_BASE
{
public:
    MSG_DELETE_ITEM();
    char m_pad[0x100];
};

class MSG_DELETE_TO_BLACKLIST : public MSG_BASE
{
public:
    MSG_DELETE_TO_BLACKLIST();
    char m_pad[0x100];
};

class MSG_DENY_JOIN_GUILD : public MSG_BASE
{
public:
    MSG_DENY_JOIN_GUILD();
    char m_pad[0x100];
};

class MSG_DIE_CHARACTER : public MSG_BASE
{
public:
    MSG_DIE_CHARACTER();
    char m_pad[0x100];
};

class MSG_DONATE_GUILD_FUND : public MSG_BASE
{
public:
    MSG_DONATE_GUILD_FUND();
    char m_pad[0x100];
};

class MSG_DROP_ITEM : public MSG_BASE
{
public:
    MSG_DROP_ITEM();
    char m_pad[0x100];
};

class MSG_ENCHANT : public MSG_BASE
{
public:
    MSG_ENCHANT();
    char m_pad[0x100];
};

class MSG_ENCHANT_BY_BEAD : public MSG_BASE
{
public:
    MSG_ENCHANT_BY_BEAD();
    char m_pad[0x100];
};

class MSG_ENTER_DISJOINT_STORE : public MSG_BASE
{
public:
    MSG_ENTER_DISJOINT_STORE();
    char m_pad[0x100];
};

class MSG_EQUIPSLOT_SWITCH : public MSG_BASE
{
public:
    MSG_EQUIPSLOT_SWITCH();
    char m_pad[0x100];
};

class MSG_EXCHANGE_RANDOM_ITEM_REWARD : public MSG_BASE
{
public:
    MSG_EXCHANGE_RANDOM_ITEM_REWARD();
    char m_pad[0x100];
};

class MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE : public MSG_BASE
{
public:
    MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE();
    char m_pad[0x100];
};

class MSG_EXPERT_EXTRACTION : public MSG_BASE
{
public:
    MSG_EXPERT_EXTRACTION();
    char m_pad[0x100];
};

class MSG_FATIGUE_ATTENDANCE : public MSG_BASE
{
public:
    MSG_FATIGUE_ATTENDANCE();
    char m_pad[0x100];
};

class MSG_FINISH_LOADING : public MSG_BASE
{
public:
    MSG_FINISH_LOADING();
    char m_pad[0x100];
};

class MSG_FLOAT_RDATA_MODULATION : public MSG_BASE
{
public:
    MSG_FLOAT_RDATA_MODULATION();
    char m_pad[0x100];
};

class MSG_GET_ITEM : public MSG_BASE
{
public:
    MSG_GET_ITEM();
    char m_pad[0x100];
};

class MSG_GOLD_TAKE_INCREASING_AMOUNT : public MSG_BASE
{
public:
    MSG_GOLD_TAKE_INCREASING_AMOUNT();
    char m_pad[0x100];
};

class MSG_GROWTH_EQUIPMENT : public MSG_BASE
{
public:
    MSG_GROWTH_EQUIPMENT();
    char m_pad[0x100];
};

class MSG_GROWTH_POWER : public MSG_BASE
{
public:
    MSG_GROWTH_POWER();
    char m_pad[0x100];
};

class MSG_GUILD_ATTENDANCE : public MSG_BASE
{
public:
    MSG_GUILD_ATTENDANCE();
    char m_pad[0x100];
};

class MSG_GUILD_BOARD : public MSG_BASE
{
public:
    MSG_GUILD_BOARD();
    char m_pad[0x100];
};

class MSG_GUILD_CARGO_MOVE_ITEM : public MSG_BASE
{
public:
    MSG_GUILD_CARGO_MOVE_ITEM();
    char m_pad[0x100];
};

class MSG_GUILD_CARGO_POP_ITEM : public MSG_BASE
{
public:
    MSG_GUILD_CARGO_POP_ITEM();
    char m_pad[0x100];
};

class MSG_GUILD_CARGO_PUSH_ITEM : public MSG_BASE
{
public:
    MSG_GUILD_CARGO_PUSH_ITEM();
    char m_pad[0x100];
};

class MSG_HATCH_CREATURE : public MSG_BASE
{
public:
    MSG_HATCH_CREATURE();
    char m_pad[0x100];
};

class MSG_INFORM_NOTICE : public MSG_BASE
{
public:
    MSG_INFORM_NOTICE();
    char m_pad[0x100];
};

class MSG_INFORM_NOTICE_2ND : public MSG_BASE
{
public:
    MSG_INFORM_NOTICE_2ND();
    char m_pad[0x100];
};

class MSG_ITEM_DICTIONARY : public MSG_BASE
{
public:
    MSG_ITEM_DICTIONARY();
    char m_pad[0x100];
};

class MSG_ITEM_HYPER_LINK_MESSAGE : public MSG_BASE
{
public:
    MSG_ITEM_HYPER_LINK_MESSAGE();
    char m_pad[0x100];
};

class MSG_ITEM_UPGRADE : public MSG_BASE
{
public:
    MSG_ITEM_UPGRADE();
    char m_pad[0x100];
};

class MSG_LOG_INGAME_ADVERTISEMENT : public MSG_BASE
{
public:
    MSG_LOG_INGAME_ADVERTISEMENT();
    char m_pad[0x100];
};

class MSG_MAILBOX_EXTRACT_ITEM : public MSG_BASE
{
public:
    MSG_MAILBOX_EXTRACT_ITEM();
    char m_pad[0x100];
};

class MSG_MAILBOX_SEND : public MSG_BASE
{
public:
    MSG_MAILBOX_SEND();
    char m_pad[0x100];
};

class MSG_MEMBER_ENTER_REPLY : public MSG_BASE
{
public:
    MSG_MEMBER_ENTER_REPLY();
    char m_pad[0x100];
};

class MSG_MEMBER_SECEDE : public MSG_BASE
{
public:
    MSG_MEMBER_SECEDE();
    char m_pad[0x100];
};

class MSG_MONSTERCARD_BIND : public MSG_BASE
{
public:
    MSG_MONSTERCARD_BIND();
    char m_pad[0x100];
};

class MSG_MOTION_HACK : public MSG_BASE
{
public:
    MSG_MOTION_HACK();
    char m_pad[0x100];
};

class MSG_MOVE_ITEMSPACE : public MSG_BASE
{
public:
    MSG_MOVE_ITEMSPACE();
    char m_pad[0x100];
};

class MSG_MOVE_MAP : public MSG_BASE
{
public:
    MSG_MOVE_MAP();
    char m_pad[0x100];
};

class MSG_OBJECT_BRING_UP : public MSG_BASE
{
public:
    MSG_OBJECT_BRING_UP();
    char m_pad[0x100];
};

class MSG_OPEN_CERAPACKAGE : public MSG_BASE
{
public:
    MSG_OPEN_CERAPACKAGE();
    char m_pad[0x100];
};

class MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE : public MSG_BASE
{
public:
    MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE();
    char m_pad[0x100];
};

class MSG_P2P_STATISTICS : public MSG_BASE
{
public:
    MSG_P2P_STATISTICS();
    char m_pad[0x100];
};

class MSG_PARTY_SET_INFO : public MSG_BASE
{
public:
    MSG_PARTY_SET_INFO();
    char m_pad[0x100];
};

class MSG_PARTY_TELEPORT : public MSG_BASE
{
public:
    MSG_PARTY_TELEPORT();
    char m_pad[0x100];
};

class MSG_PARTY_TELEPORT_CONFIRM : public MSG_BASE
{
public:
    MSG_PARTY_TELEPORT_CONFIRM();
    char m_pad[0x100];
};

class MSG_PURIFY_ITEM : public MSG_BASE
{
public:
    MSG_PURIFY_ITEM();
    char m_pad[0x100];
};

class MSG_PVP_MISSION_COMBO_CLEAR : public MSG_BASE
{
public:
    MSG_PVP_MISSION_COMBO_CLEAR();
    char m_pad[0x100];
};

class MSG_PVP_MISSION_HP_PERCENT : public MSG_BASE
{
public:
    MSG_PVP_MISSION_HP_PERCENT();
    char m_pad[0x100];
};

class MSG_PVP_TIMEOUT : public MSG_BASE
{
public:
    MSG_PVP_TIMEOUT();
    char m_pad[0x100];
};

class MSG_QUEST_SHOP : public MSG_BASE
{
public:
    MSG_QUEST_SHOP();
    char m_pad[0x100];
};

class MSG_QUICK_CHAT : public MSG_BASE
{
public:
    MSG_QUICK_CHAT();
    char m_pad[0x100];
};

class MSG_QUICK_PARTY : public MSG_BASE
{
public:
    MSG_QUICK_PARTY();
    char m_pad[0x100];
};

class MSG_REFUND_SKILL : public MSG_BASE
{
public:
    MSG_REFUND_SKILL();
    char m_pad[0x100];
};

class MSG_REGENERATION_RANDOM_OPTION : public MSG_BASE
{
public:
    MSG_REGENERATION_RANDOM_OPTION();
    char m_pad[0x100];
};

class MSG_REGISTER_TO_BLACKLIST : public MSG_BASE
{
public:
    MSG_REGISTER_TO_BLACKLIST();
    char m_pad[0x100];
};

class MSG_REMOVE_FRIEND : public MSG_BASE
{
public:
    MSG_REMOVE_FRIEND();
    char m_pad[0x100];
};

class MSG_RENAME_CREATURE : public MSG_BASE
{
public:
    MSG_RENAME_CREATURE();
    char m_pad[0x100];
};

class MSG_RENT_ITEM : public MSG_BASE
{
public:
    MSG_RENT_ITEM();
    char m_pad[0x100];
};

class MSG_REPAIR_EQUIP : public MSG_BASE
{
public:
    MSG_REPAIR_EQUIP();
    char m_pad[0x100];
};

class MSG_REQUEST_CHARAC_SKILL_INFO : public MSG_BASE
{
public:
    MSG_REQUEST_CHARAC_SKILL_INFO();
    char m_pad[0x100];
};

class MSG_REQUEST_DISJOINT_ITEM : public MSG_BASE
{
public:
    MSG_REQUEST_DISJOINT_ITEM();
    char m_pad[0x100];
};

class MSG_REQUEST_GUILD_JOIN : public MSG_BASE
{
public:
    MSG_REQUEST_GUILD_JOIN();
    char m_pad[0x100];
};

class MSG_REQUEST_INGAME_ADVERTISEMENT : public MSG_BASE
{
public:
    MSG_REQUEST_INGAME_ADVERTISEMENT();
    char m_pad[0x100];
};

class MSG_REQUEST_MEMBER_ENTER : public MSG_BASE
{
public:
    MSG_REQUEST_MEMBER_ENTER();
    char m_pad[0x100];
};

class MSG_RESET_RANDOM_OPTION : public MSG_BASE
{
public:
    MSG_RESET_RANDOM_OPTION();
    char m_pad[0x100];
};

class MSG_RIGHT_OF_CHANGE_GROW_TYPE : public MSG_BASE
{
public:
    MSG_RIGHT_OF_CHANGE_GROW_TYPE();
    char m_pad[0x100];
};

class MSG_SAVE_CHARACTER_OPTION : public MSG_BASE
{
public:
    MSG_SAVE_CHARACTER_OPTION();
    char m_pad[0x100];
};

class MSG_SAVE_GAME_OPTION_1 : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_1();
    char m_pad[0x100];
};

class MSG_SAVE_GAME_OPTION_2 : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_2();
    char m_pad[0x100];
};

class MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON();
    char m_pad[0x100];
};

class MSG_SAVE_QUEST_NOTIFY : public MSG_BASE
{
public:
    MSG_SAVE_QUEST_NOTIFY();
    char m_pad[0x100];
};

class MSG_SCAN_DLL : public MSG_BASE
{
public:
    MSG_SCAN_DLL();
    char m_pad[0x100];
};

class MSG_SCAN_DRV : public MSG_BASE
{
public:
    MSG_SCAN_DRV();
    char m_pad[0x100];
};

class MSG_SCRIPT_HASH : public MSG_BASE
{
public:
    MSG_SCRIPT_HASH();
    char m_pad[0x100];
};

class MSG_SECRET_SHOP_BUY_ITEM : public MSG_BASE
{
public:
    MSG_SECRET_SHOP_BUY_ITEM();
    char m_pad[0x100];
};

class MSG_SECRET_SHOP_OPEN_CLOSE : public MSG_BASE
{
public:
    MSG_SECRET_SHOP_OPEN_CLOSE();
    char m_pad[0x100];
};

class MSG_SECURITY_LOG_PACKET : public MSG_BASE
{
public:
    MSG_SECURITY_LOG_PACKET();
    char m_pad[0x100];
};

class MSG_SECU_RPY : public MSG_BASE
{
public:
    MSG_SECU_RPY();
    char m_pad[0x100];
};

class MSG_SELECT_DUNGEON : public MSG_BASE
{
public:
    MSG_SELECT_DUNGEON();
    char m_pad[0x100];
};

class MSG_SELECT_STRIKER : public MSG_BASE
{
public:
    MSG_SELECT_STRIKER();
    char m_pad[0x100];
};

class MSG_SERIAROOM_DECO_EVENT : public MSG_BASE
{
public:
    MSG_SERIAROOM_DECO_EVENT();
    char m_pad[0x100];
};

class MSG_SET_READY_STATE : public MSG_BASE
{
public:
    MSG_SET_READY_STATE();
    char m_pad[0x100];
};

class MSG_SET_USER_AREA : public MSG_BASE
{
public:
    MSG_SET_USER_AREA();
    char m_pad[0x100];
};

class MSG_SKILL_COMMAND : public MSG_BASE
{
public:
    MSG_SKILL_COMMAND();
    char m_pad[0x100];
};

class MSG_SKILL_INIT : public MSG_BASE
{
public:
    MSG_SKILL_INIT();
    char m_pad[0x100];
};

class MSG_SOLO_TELEPORT : public MSG_BASE
{
public:
    MSG_SOLO_TELEPORT();
    char m_pad[0x100];
};

class MSG_SORT_ITEM : public MSG_BASE
{
public:
    MSG_SORT_ITEM();
    char m_pad[0x100];
};

class MSG_STACKABLE_ACTION_USE : public MSG_BASE
{
public:
    MSG_STACKABLE_ACTION_USE();
    char m_pad[0x100];
};

class MSG_TELEPORT : public MSG_BASE
{
public:
    MSG_TELEPORT();
    char m_pad[0x100];
};

class MSG_TEMP_STRUCT : public MSG_BASE
{
public:
    MSG_TEMP_STRUCT();
    char m_pad[0x100];
};

class MSG_TITLEBOOK_GET : public MSG_BASE
{
public:
    MSG_TITLEBOOK_GET();
    char m_pad[0x100];
};

class MSG_TITLEBOOK_OTHERUSER : public MSG_BASE
{
public:
    MSG_TITLEBOOK_OTHERUSER();
    char m_pad[0x100];
};

class MSG_TITLEBOOK_PUT : public MSG_BASE
{
public:
    MSG_TITLEBOOK_PUT();
    char m_pad[0x100];
};

class MSG_UPDATE_CONTRACT_OF_CUBE_INFO : public MSG_BASE
{
public:
    MSG_UPDATE_CONTRACT_OF_CUBE_INFO();
    char m_pad[0x100];
};

class MSG_USERINFO_REQ : public MSG_BASE
{
public:
    MSG_USERINFO_REQ();
    char m_pad[0x100];
};

class MSG_USER_HISTORY_LOG : public MSG_BASE
{
public:
    MSG_USER_HISTORY_LOG();
    char m_pad[0x100];
};

class MSG_USER_SET_POS : public MSG_BASE
{
public:
    MSG_USER_SET_POS();
    char m_pad[0x100];
};

class MSG_USE_COIN : public MSG_BASE
{
public:
    MSG_USE_COIN();
    char m_pad[0x100];
};

class MSG_USE_DYE : public MSG_BASE
{
public:
    MSG_USE_DYE();
    char m_pad[0x100];
};

class MSG_USE_LIMIT_CUBE : public MSG_BASE
{
public:
    MSG_USE_LIMIT_CUBE();
    char m_pad[0x100];
};

class MSG_USE_RENAME_CARD : public MSG_BASE
{
public:
    MSG_USE_RENAME_CARD();
    char m_pad[0x100];
};

class MSG_USE_SKILL : public MSG_BASE
{
public:
    MSG_USE_SKILL();
    char m_pad[0x100];
};

class MSG_WALKOUT_PARTY : public MSG_BASE
{
public:
    MSG_WALKOUT_PARTY();
    char m_pad[0x100];
};

class MSG_WARROOM_WP_PER_MONSTER : public MSG_BASE
{
public:
    MSG_WARROOM_WP_PER_MONSTER();
    char m_pad[0x100];
};

class MsgClearQuestIndex : public MSG_BASE
{
public:
    MsgClearQuestIndex();
    char m_pad[0x100];
};

class MsgLimitNpcBuyItem : public MSG_BASE
{
public:
    MsgLimitNpcBuyItem();
    char m_pad[0x100];
};

class MsgMercenaryCompetition : public MSG_BASE
{
public:
    MsgMercenaryCompetition();
    char m_pad[0x100];
};

class MsgMercenaryReturn : public MSG_BASE
{
public:
    MsgMercenaryReturn();
    char m_pad[0x100];
};

class MsgRequestAssaultPrice : public MSG_BASE
{
public:
    MsgRequestAssaultPrice();
    char m_pad[0x100];
};

class MsgTournamentRewardSelect : public MSG_BASE
{
public:
    MsgTournamentRewardSelect();
    char m_pad[0x100];
};

// ---------------------------------------------------------------------------
// Param 类型（占位大小 0x40，后续按 ORIG 构造函数修正）
// ---------------------------------------------------------------------------
class AbnormalUseStackable : public ParamBase
{
public:
    AbnormalUseStackable();
    char m_pad[0x40];
};

class BuyItemParam : public ParamBase
{
public:
    BuyItemParam();
    char m_pad[0x40];
};

class CheckSecurityProtection : public ParamBase
{
public:
    CheckSecurityProtection();
    char m_pad[0x40];
};

class InformNoticeData : public ParamBase
{
public:
    InformNoticeData();
    char m_pad[0x40];
};

class InformNoticeData2nd : public ParamBase
{
public:
    InformNoticeData2nd();
    char m_pad[0x40];
};

class LimitNpcBuyItem : public ParamBase
{
public:
    LimitNpcBuyItem();
    char m_pad[0x40];
};

class PcRoomPlayTimeRewardParam : public ParamBase
{
public:
    PcRoomPlayTimeRewardParam();
    char m_pad[0x40];
};

class ReqDebugClearQuestResult : public ParamBase
{
public:
    ReqDebugClearQuestResult();
    char m_pad[0x40];
};

class ReqResult : public ParamBase
{
public:
    ReqResult();
    char m_pad[0x40];
};

class STQuestShopBuyParam : public ParamBase
{
public:
    STQuestShopBuyParam();
    char m_pad[0x40];
};

class STQuestShopParam : public ParamBase
{
public:
    STQuestShopParam();
    char m_pad[0x40];
};

class STUdateItemParam : public ParamBase
{
public:
    STUdateItemParam();
    char m_pad[0x40];
};

class ScanBotByDllParam : public ParamBase
{
public:
    ScanBotByDllParam();
    char m_pad[0x40];
};

class ScanBotByDrvParam : public ParamBase
{
public:
    ScanBotByDrvParam();
    char m_pad[0x40];
};

class ScriptHashParam : public ParamBase
{
public:
    ScriptHashParam();
    char m_pad[0x40];
};

class SecuCardRpyParam : public ParamBase
{
public:
    SecuCardRpyParam();
    char m_pad[0x40];
};

class SendMercenaryCompetitionResult : public ParamBase
{
public:
    SendMercenaryCompetitionResult();
    char m_pad[0x40];
};

class SendMercenaryReturnResult : public ParamBase
{
public:
    SendMercenaryReturnResult();
    char m_pad[0x40];
};

class TempParam : public ParamBase
{
public:
    TempParam();
    char m_pad[0x40];
};

class saveQuestNotifyParam : public ParamBase
{
public:
    saveQuestNotifyParam();
    char m_pad[0x40];
};

class stAddFriendParam : public ParamBase
{
public:
    stAddFriendParam();
    char m_pad[0x40];
};

class stAvatarDisjointRandomReward : public ParamBase
{
public:
    stAvatarDisjointRandomReward();
    char m_pad[0x40];
};

class stBingoQuiz : public ParamBase
{
public:
    stBingoQuiz();
    char m_pad[0x40];
};

class stBingoReward : public ParamBase
{
public:
    stBingoReward();
    char m_pad[0x40];
};

class stBlueMarble : public ParamBase
{
public:
    stBlueMarble();
    char m_pad[0x40];
};

class stBlueMarbleConfirmInfo : public ParamBase
{
public:
    stBlueMarbleConfirmInfo();
    char m_pad[0x40];
};

class stBossDieCheckParam : public ParamBase
{
public:
    stBossDieCheckParam();
    char m_pad[0x40];
};

class stBreakAwayParam : public ParamBase
{
public:
    stBreakAwayParam();
    char m_pad[0x40];
};

class stCallMemberListParam : public ParamBase
{
public:
    stCallMemberListParam();
    char m_pad[0x40];
};

class stChangeAnotherSkillTree : public ParamBase
{
public:
    stChangeAnotherSkillTree();
    char m_pad[0x40];
};

class stChangeAvatarOption : public ParamBase
{
public:
    stChangeAvatarOption();
    char m_pad[0x40];
};

class stChangeEmotionParam : public ParamBase
{
public:
    stChangeEmotionParam();
    char m_pad[0x40];
};

class stChangeHostWarRoom : public ParamBase
{
public:
    stChangeHostWarRoom();
    char m_pad[0x40];
};

class stChangeLetterStatParam : public ParamBase
{
public:
    stChangeLetterStatParam();
    char m_pad[0x40];
};

class stCharacSlotExtendEffect : public ParamBase
{
public:
    stCharacSlotExtendEffect();
    char m_pad[0x40];
};

class stClientSpecStatisticParam : public ParamBase
{
public:
    stClientSpecStatisticParam();
    char m_pad[0x40];
};

class stCloseDisjointStoreParam : public ParamBase
{
public:
    stCloseDisjointStoreParam();
    char m_pad[0x40];
};

class stCollectItems : public ParamBase
{
public:
    stCollectItems();
    char m_pad[0x40];
};

class stComboSkill : public ParamBase
{
public:
    stComboSkill();
    char m_pad[0x40];
};

class stComboSkillExtensionQuickSlotReset : public ParamBase
{
public:
    stComboSkillExtensionQuickSlotReset();
    char m_pad[0x40];
};

class stCreateCharac : public ParamBase
{
public:
    stCreateCharac();
    char m_pad[0x40];
};

class stCreateDisjointStoreParam : public ParamBase
{
public:
    stCreateDisjointStoreParam();
    char m_pad[0x40];
};

class stDeleteCharac : public ParamBase
{
public:
    stDeleteCharac();
    char m_pad[0x40];
};

class stDeleteItemParam : public ParamBase
{
public:
    stDeleteItemParam();
    char m_pad[0x40];
};

class stDeleteToBlackListParam : public ParamBase
{
public:
    stDeleteToBlackListParam();
    char m_pad[0x40];
};

class stDieCharacInDungeon : public ParamBase
{
public:
    stDieCharacInDungeon();
    char m_pad[0x40];
};

class stDropItemParam : public ParamBase
{
public:
    stDropItemParam();
    char m_pad[0x40];
};

class stEnchantByBeadParam : public ParamBase
{
public:
    stEnchantByBeadParam();
    char m_pad[0x40];
};

class stEnchantParam : public ParamBase
{
public:
    stEnchantParam();
    char m_pad[0x40];
};

class stEnterDisjointStoreParam : public ParamBase
{
public:
    stEnterDisjointStoreParam();
    char m_pad[0x40];
};

class stEquipslotSwitch : public ParamBase
{
public:
    stEquipslotSwitch();
    char m_pad[0x40];
};

class stExchangeRandomItemReward : public ParamBase
{
public:
    stExchangeRandomItemReward();
    char m_pad[0x40];
};

class stExpandEquipslotFlagUpdate : public ParamBase
{
public:
    stExpandEquipslotFlagUpdate();
    char m_pad[0x40];
};

class stExpertExtraction : public ParamBase
{
public:
    stExpertExtraction();
    char m_pad[0x40];
};

class stFatigueAttendance : public ParamBase
{
public:
    stFatigueAttendance();
    char m_pad[0x40];
};

class stFinishLoadParam : public ParamBase
{
public:
    stFinishLoadParam();
    char m_pad[0x40];
};

class stGetItemParam : public ParamBase
{
public:
    stGetItemParam();
    char m_pad[0x40];
};

class stGoldTakeIncreasingAmount : public ParamBase
{
public:
    stGoldTakeIncreasingAmount();
    char m_pad[0x40];
};

class stGrowthCreature : public ParamBase
{
public:
    stGrowthCreature();
    char m_pad[0x40];
};

class stGrowthEquipment : public ParamBase
{
public:
    stGrowthEquipment();
    char m_pad[0x40];
};

class stGrowthPowerParam : public ParamBase
{
public:
    stGrowthPowerParam();
    char m_pad[0x40];
};

class stGuildBoardParam : public ParamBase
{
public:
    stGuildBoardParam();
    char m_pad[0x40];
};

class stGuildCargoHistoryParam : public ParamBase
{
public:
    stGuildCargoHistoryParam();
    char m_pad[0x40];
};

class stGuildCargoMoveItemParam : public ParamBase
{
public:
    stGuildCargoMoveItemParam();
    char m_pad[0x40];
};

class stGuildCargoParam : public ParamBase
{
public:
    stGuildCargoParam();
    char m_pad[0x40];
};

class stGuildCargoPopItemParam : public ParamBase
{
public:
    stGuildCargoPopItemParam();
    char m_pad[0x40];
};

class stGuildCargoPushItemParam : public ParamBase
{
public:
    stGuildCargoPushItemParam();
    char m_pad[0x40];
};

class stHatchCreature : public ParamBase
{
public:
    stHatchCreature();
    char m_pad[0x40];
};

class stImageCommunicationEquipmentUse : public ParamBase
{
public:
    stImageCommunicationEquipmentUse();
    char m_pad[0x40];
};

class stItemDictionary : public ParamBase
{
public:
    stItemDictionary();
    char m_pad[0x40];
};

class stItemHyperLinkMessage : public ParamBase
{
public:
    stItemHyperLinkMessage();
    char m_pad[0x40];
};

class stLeavePartyParam : public ParamBase
{
public:
    stLeavePartyParam();
    char m_pad[0x40];
};

class stLogIngameAdvertisement : public ParamBase
{
public:
    stLogIngameAdvertisement();
    char m_pad[0x40];
};

class stMailboxExtractParam : public ParamBase
{
public:
    stMailboxExtractParam();
    char m_pad[0x40];
};

class stMailboxOpenParam : public ParamBase
{
public:
    stMailboxOpenParam();
    char m_pad[0x40];
};

class stMailboxSendParam : public ParamBase
{
public:
    stMailboxSendParam();
    char m_pad[0x40];
};

class stMemberEnterReplyParam : public ParamBase
{
public:
    stMemberEnterReplyParam();
    char m_pad[0x40];
};

class stMemberSecedeParam : public ParamBase
{
public:
    stMemberSecedeParam();
    char m_pad[0x40];
};

class stMonstercardBind : public ParamBase
{
public:
    stMonstercardBind();
    char m_pad[0x40];
};

class stMoveItemParam : public ParamBase
{
public:
    stMoveItemParam();
    char m_pad[0x40];
};

class stMoveMapParam : public ParamBase
{
public:
    stMoveMapParam();
    char m_pad[0x40];
};

class stObjectBringUp : public ParamBase
{
public:
    stObjectBringUp();
    char m_pad[0x40];
};

class stOnBuySkillParam : public ParamBase
{
public:
    stOnBuySkillParam();
    char m_pad[0x40];
};

class stOnChangeSkillParam : public ParamBase
{
public:
    stOnChangeSkillParam();
    char m_pad[0x40];
};

class stOnGetUserInfoParam : public ParamBase
{
public:
    stOnGetUserInfoParam();
    char m_pad[0x40];
};

class stOnPvPTimeOut : public ParamBase
{
public:
    stOnPvPTimeOut();
    char m_pad[0x40];
};

class stOnRecoverStaminaParam : public ParamBase
{
public:
    stOnRecoverStaminaParam();
    char m_pad[0x40];
};

class stOnSetPvPReadyParam : public ParamBase
{
public:
    stOnSetPvPReadyParam();
    char m_pad[0x40];
};

class stOnSortItemParam : public ParamBase
{
public:
    stOnSortItemParam();
    char m_pad[0x40];
};

class stOnUseEquipParam : public ParamBase
{
public:
    stOnUseEquipParam();
    char m_pad[0x40];
};

class stOpenCeraPackage : public ParamBase
{
public:
    stOpenCeraPackage();
    char m_pad[0x40];
};

class stPartyTeleport : public ParamBase
{
public:
    stPartyTeleport();
    char m_pad[0x40];
};

class stPartyTeleportConfirm : public ParamBase
{
public:
    stPartyTeleportConfirm();
    char m_pad[0x40];
};

class stQuickChatParam : public ParamBase
{
public:
    stQuickChatParam();
    char m_pad[0x40];
};

class stQuickPartyParam : public ParamBase
{
public:
    stQuickPartyParam();
    char m_pad[0x40];
};

class stRefundSkill : public ParamBase
{
public:
    stRefundSkill();
    char m_pad[0x40];
};

class stRegenROIResult : public ParamBase
{
public:
    stRegenROIResult();
    char m_pad[0x40];
};

class stRegisterToBlackListParam : public ParamBase
{
public:
    stRegisterToBlackListParam();
    char m_pad[0x40];
};

class stRemoveFriendParam : public ParamBase
{
public:
    stRemoveFriendParam();
    char m_pad[0x40];
};

class stRenameCreature : public ParamBase
{
public:
    stRenameCreature();
    char m_pad[0x40];
};

class stRentItemParam : public ParamBase
{
public:
    stRentItemParam();
    char m_pad[0x40];
};

class stRepairDisjointMachineParam : public ParamBase
{
public:
    stRepairDisjointMachineParam();
    char m_pad[0x40];
};

class stRepairEquipParam : public ParamBase
{
public:
    stRepairEquipParam();
    char m_pad[0x40];
};

class stRequestAssaultPrice : public ParamBase
{
public:
    stRequestAssaultPrice();
    char m_pad[0x40];
};

class stRequestBlackListParam : public ParamBase
{
public:
    stRequestBlackListParam();
    char m_pad[0x40];
};

class stRequestCharacSkillInfo : public ParamBase
{
public:
    stRequestCharacSkillInfo();
    char m_pad[0x40];
};

class stRequestDisjointItemParam : public ParamBase
{
public:
    stRequestDisjointItemParam();
    char m_pad[0x40];
};

class stRequestIngameAdvertisement : public ParamBase
{
public:
    stRequestIngameAdvertisement();
    char m_pad[0x40];
};

class stRequestMemberEnterParam : public ParamBase
{
public:
    stRequestMemberEnterParam();
    char m_pad[0x40];
};

class stRightOfChangeGrowType : public ParamBase
{
public:
    stRightOfChangeGrowType();
    char m_pad[0x40];
};

class stSaveCharacOption : public ParamBase
{
public:
    stSaveCharacOption();
    char m_pad[0x40];
};

class stSecretShopBuyItemParam : public ParamBase
{
public:
    stSecretShopBuyItemParam();
    char m_pad[0x40];
};

class stSecretShopOpenCloseParam : public ParamBase
{
public:
    stSecretShopOpenCloseParam();
    char m_pad[0x40];
};

class stSecurityLogParam : public ParamBase
{
public:
    stSecurityLogParam();
    char m_pad[0x40];
};

class stSelectCharac : public ParamBase
{
public:
    stSelectCharac();
    char m_pad[0x40];
};

class stSelectDungeonParam : public ParamBase
{
public:
    stSelectDungeonParam();
    char m_pad[0x40];
};

class stSelectStriker : public ParamBase
{
public:
    stSelectStriker();
    char m_pad[0x40];
};

class stSendClearedMission : public ParamBase
{
public:
    stSendClearedMission();
    char m_pad[0x40];
};

class stSeriaRoomDecoEvent : public ParamBase
{
public:
    stSeriaRoomDecoEvent();
    char m_pad[0x40];
};

class stSetAreaParam : public ParamBase
{
public:
    stSetAreaParam();
    char m_pad[0x40];
};

class stSetPartyInfo : public ParamBase
{
public:
    stSetPartyInfo();
    char m_pad[0x40];
};

class stSkillCommandParam : public ParamBase
{
public:
    stSkillCommandParam();
    char m_pad[0x40];
};

class stSkillInit : public ParamBase
{
public:
    stSkillInit();
    char m_pad[0x40];
};

class stSoloTeleportParam : public ParamBase
{
public:
    stSoloTeleportParam();
    char m_pad[0x40];
};

class stStackableActionUse : public ParamBase
{
public:
    stStackableActionUse();
    char m_pad[0x40];
};

class stStartGameParam : public ParamBase
{
public:
    stStartGameParam();
    char m_pad[0x40];
};

class stTeleportParam : public ParamBase
{
public:
    stTeleportParam();
    char m_pad[0x40];
};

class stTournamentRewardSelect : public ParamBase
{
public:
    stTournamentRewardSelect();
    char m_pad[0x40];
};

class stTournamentRewardSelectState : public ParamBase
{
public:
    stTournamentRewardSelectState();
    char m_pad[0x40];
};

class stUpdateContractOfCubeInfo : public ParamBase
{
public:
    stUpdateContractOfCubeInfo();
    char m_pad[0x40];
};

class stUpgradeDisjointMachineParam : public ParamBase
{
public:
    stUpgradeDisjointMachineParam();
    char m_pad[0x40];
};

class stUseCoinParam : public ParamBase
{
public:
    stUseCoinParam();
    char m_pad[0x40];
};

class stUseDye : public ParamBase
{
public:
    stUseDye();
    char m_pad[0x40];
};

class stUseLimitCube : public ParamBase
{
public:
    stUseLimitCube();
    char m_pad[0x40];
};

class stUseRenameCard : public ParamBase
{
public:
    stUseRenameCard();
    char m_pad[0x40];
};

class stUseSkillParam : public ParamBase
{
public:
    stUseSkillParam();
    char m_pad[0x40];
};

class stWalkoutPartyParam : public ParamBase
{
public:
    stWalkoutPartyParam();
    char m_pad[0x40];
};

class stWarRoomWp : public ParamBase
{
public:
    stWarRoomWp();
    char m_pad[0x40];
};

#endif  // DNF_GAME_PACKET_TYPES_H_
