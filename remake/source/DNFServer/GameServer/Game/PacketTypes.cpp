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


InstanceRentalSystem::RentOutput::RentOutput() : m_infos(), m_deleted() {}  // ORIG 0x8231c4e
InstanceRentalSystem::RentOutput::~RentOutput() {}                          // ORIG 0x8231c94

MSG_BINGO_QUIZ::MSG_BINGO_QUIZ()
{
    m_0d = 0;  // ORIG C1：+0xd int 清零
}
MSG_BLUE_MARBLE::MSG_BLUE_MARBLE()
{
    m_0d = 0;  // ORIG C1：+0xd byte 清零
}
MSG_BLUE_MARBLE_CONFIRM_INFO::MSG_BLUE_MARBLE_CONFIRM_INFO()
    : m_0d(0), m_0e(0xffff)
{  // ORIG 0x822e39a：+0xd byte=0 先、+0xe word=0xffff 后
}
MSG_BREAK_AWAY::MSG_BREAK_AWAY() : m_checkFlag(0) {}
// 字段化后保持占位版语义：+0xd 包头后全量清零（ORIG 无出线 ctor，此为本仓库构造约定）。
MSG_BUY_SKILL::MSG_BUY_SKILL()
    : m_treeKind(0), m_count(0)
{
    memset(m_entries, 0, 4);  // ORIG 0x8230956 仅清 +0xf 起 4 字节
}
MSG_CHANGE_ANOTHER_SKILL_TREE::MSG_CHANGE_ANOTHER_SKILL_TREE()
{
    m_0d = 0xff;  // ORIG C1：哨兵
}
MSG_CHANGE_AVATAR_OPTION::MSG_CHANGE_AVATAR_OPTION()
    : m_consumeSlot(0xffff), m_consumeItemIdx(-1), m_avatarSlot(0xffff),
      m_equipItemIdx(-1), m_option(0xff)
{
    // ORIG 0x0822e0cc：MSG_BASE C1 后 +0x0d word=-1/+0x0f dword=-1/+0x13
    // word=-1/+0x15 dword=-1/+0x19 byte=0xff（初始列表序一致）。
}
MSG_CHANGE_RANDOM_OPTION::MSG_CHANGE_RANDOM_OPTION() : m_0e(0), m_10(0), m_12(0) {}
  // ORIG C1：+0xe/+0x10 word、+0x12 byte 清零
MSG_CHARAC_SELECT::MSG_CHARAC_SELECT() : m_slot(-1), m_0e(0), m_12(0) {}
// MSG_CHECK_GUILD_JOIN ORIG 0x0822dd7c：+0xd int=0；+0x11 起 memset 0x17。
MSG_CHECK_GUILD_JOIN::MSG_CHECK_GUILD_JOIN()
{  // ORIG 0x0822dd7c：+0xd int=0；memset(this+0x11,0,0x17)
    m_0d = 0;
    memset(m_11, 0, sizeof(m_11));
}
// MSG_CLIENT_SPEC_STATISTIC ORIG 0x082556a0：调 Client_Spec ctor 于 +0xe。
// 本仓库 Client_Spec 为 pad 占位（HWSpec[0xff] 子对象未建模），构造为空（推断）。
MSG_CLIENT_SPEC_STATISTIC::MSG_CLIENT_SPEC_STATISTIC() {}
MSG_COMBO_SKILL::MSG_COMBO_SKILL() : m_pad0d(0)
{
    m_comboMap.clear();  // ORIG 0x82603d6 构造后显式 clear
}
MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET::MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET()
{
    m_0d = 0;  // ORIG C1
}
MSG_COMPATIBILITY_INDEX::MSG_COMPATIBILITY_INDEX()
    : m_0d(0), m_0e(0), m_10(0), m_14(0), m_16(0), m_18(0), m_1a(0),
      m_1c(0), m_1d(0) {}  // ORIG 0x822e174
MSG_ENCHANT::MSG_ENCHANT()
{
    // ORIG 0x822dc04：先写 +0x13/+0x15，再拷贝到 +0x0d/+0x0f，末尾 +0x19
    m_13 = 0;
    m_0d = m_13;
    m_15 = 0;
    m_0f = m_15;
    m_19 = 0;
}
MSG_EXCHANGE_RANDOM_ITEM_REWARD::MSG_EXCHANGE_RANDOM_ITEM_REWARD() {}
MSG_EXPERT_EXTRACTION::MSG_EXPERT_EXTRACTION()
{
    // ORIG C1：+0xd byte=0、+0x10 word=0、+0xe ← (+0x10)
    m_0d = 0;
    m_10 = 0;
    m_0e = m_10;
}
MSG_FATIGUE_ATTENDANCE::MSG_FATIGUE_ATTENDANCE()
{
    m_0d = 0;  // ORIG C1
}
MSG_GROWTH_EQUIPMENT::MSG_GROWTH_EQUIPMENT()
{
    // ORIG C1：+0xd/+0x11 int 清零
    m_0d = 0;
    m_11 = 0;
}
MSG_GROWTH_POWER::MSG_GROWTH_POWER()
{
    // ORIG C1：+0xd byte 清零、memset(this+0xe,0,5)
    m_0d = 0;
    memset(m_0e, 0, sizeof(m_0e));
}
// ORIG 0x822b802：+0=0xff、+1=0xff、+2=0、memset(this+3,0,0x1e)
STGuildMemberCharacData::STGuildMemberCharacData()
{
    m_0 = 0xff;
    m_1 = 0xff;
    m_2 = 0;
    memset(m_pad3, 0, 0x1e);
}

// ORIG 0x822b83c：成员 C1（m_members@0x84）→ ints +0x78/+0x7c/+0x80 清零 →
// memset(this,0,0x78)（+0..+0x77 全为 m_pad78，与 memset 首参等价）。
// 唯一定义处归本 TU：消除 MSG_GUILD_BOARD 构造对 _ZN18STGuildBoardDBInfoC1Ev 的 undefined。
STGuildBoardDBInfo::STGuildBoardDBInfo()
    : m_members()
{
    m_78 = 0;
    m_7c = 0;
    m_80 = 0;
    memset(m_pad78, 0, 0x78);
}

MSG_GUILD_BOARD::MSG_GUILD_BOARD() : m_info()
{
    m_b2 = 0;  // ORIG 0x822dbfa
}
MSG_INFORM_NOTICE::MSG_INFORM_NOTICE()
{
    m_noticeFlag = 0x80;  // ORIG C1：notice 类型哨兵（字段化改名，值语义不变）
}
MSG_INFORM_NOTICE_2ND::MSG_INFORM_NOTICE_2ND()
{
    m_noticeFlag = 0x80;  // ORIG C1（字段化改名）
}
MSG_ITEM_UPGRADE::MSG_ITEM_UPGRADE()
{
    // ORIG C1：先 +0x25 word=0，memset(this+0x28,0,0x81)，末尾 +0x27 byte=0
    m_25 = 0;
    memset(m_28, 0, sizeof(m_28));
    m_27 = 0;
}
MSG_LOG_INGAME_ADVERTISEMENT::MSG_LOG_INGAME_ADVERTISEMENT() {}
MSG_MONSTERCARD_BIND::MSG_MONSTERCARD_BIND()
{
    // ORIG C1：拷贝链 m_11←0 → m_0f ← m_11 → m_0d ← m_0f
    m_11 = 0;
    m_0f = m_11;
    m_0d = m_0f;
}
MSG_OPEN_CERAPACKAGE::MSG_OPEN_CERAPACKAGE()
{
    // ORIG C1：+0xe word 清零、memset(this+0x14,0,0x118)
    m_0e = 0;
    memset(m_14, 0, sizeof(m_14));
}
MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE::MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE()
{
    m_0d = 0;  // ORIG C1：+0xd byte、+0xe int
    m_0e = 0;
}
MSG_P2P_STATISTICS::MSG_P2P_STATISTICS()
    // ORIG 0x822e1d2：逐字段写入序 d,e,10,12,14,18,1c,20,24,28,2a,2c,30,34,38,3c（字段声明升序）。
    // -O0 下 ctor 先发射 init-list 成员（声明序）；全部移入 init-list 使发射序与 ORIG 升序一致。
    : m_0d(0), m_0e(0), m_10(0), m_12(0), m_14(0), m_18(0), m_1c(0),
      m_20(0), m_24(0), m_28(0), m_2a(0), m_2c(0), m_30(0), m_34(0), m_38(0), m_3c(0)
{}
MSG_PARTY_TELEPORT::MSG_PARTY_TELEPORT()
{
    // ORIG C1：byte/byte/word/word/byte 全零
    m_field0d = 0;
    m_field0e = 0;
    m_field0f = 0;
    m_field11 = 0;
    m_field13 = 0;
}
MSG_PARTY_TELEPORT_CONFIRM::MSG_PARTY_TELEPORT_CONFIRM()
{
    m_confirmFlag = 0;  // ORIG C1（字段化改名）
}
MSG_PURIFY_ITEM::MSG_PURIFY_ITEM()
{
    // ORIG C1：四组标量清零
    m_0d = 0;
    m_0f = 0;
    m_13 = 0;
    m_15 = 0;
}
MSG_PVP_MISSION_HP_PERCENT::MSG_PVP_MISSION_HP_PERCENT() {}  // ORIG 0x8250c7c：仅两个 vector C1（+0xe/+0x1a），无标量初始化
MSG_PVP_MISSION_HP_PERCENT::~MSG_PVP_MISSION_HP_PERCENT() {}           // ORIG 0x8250cd2：逆序析构两个 vector
MSG_QUEST_SHOP::MSG_QUEST_SHOP()
{
    // ORIG C1：+0xd/+0xe byte 清零
    m_0d = 0;
    m_0e = 0;
}
MSG_QUICK_CHAT::MSG_QUICK_CHAT() : m_0d(0), m_11(0)
{  // ORIG C1：标量初始化列表先于 m_quick 构造（声明序）
}
MSG_QUICK_PARTY::MSG_QUICK_PARTY()
{
    // ORIG C1：+0xe word=0、+0x10 byte=0xff
    m_0e = 0;
    m_10 = 0xff;
}
MSG_REFUND_SKILL::MSG_REFUND_SKILL()
{
    // ORIG C1：+0xd int 与 +0x11..+0x13 bytes 清零
    m_0d = 0;
    m_11 = 0;
    m_12 = 0;
    m_13 = 0;
}
MSG_RENAME_CREATURE::MSG_RENAME_CREATURE()
{
    memset(m_name, 0, sizeof(m_name));  // ORIG C1：memset(this+0x14,0,0xd)（字段化改名）
}
MSG_RENT_ITEM::MSG_RENT_ITEM()
{  // ORIG C1：+0xd/+0xe byte 清零、memset(this+0xf,0,2)
    m_0d = 0;
    m_0e = 0;
    memset(m_0f, 0, sizeof(m_0f));
}
MSG_REQUEST_CHARAC_SKILL_INFO::MSG_REQUEST_CHARAC_SKILL_INFO()
{
    m_0d = 0xff;  // ORIG C1：哨兵
}
MSG_REQUEST_INGAME_ADVERTISEMENT::MSG_REQUEST_INGAME_ADVERTISEMENT() {}
MSG_REQUEST_GUILD_JOIN::MSG_REQUEST_GUILD_JOIN()
{  // ORIG C1：+0xd int=0；memset(+0x11)；+0x28 int=0；memset(+0x2c)
    m_0d = 0;
    m_28 = 0;
    memset(m_11, 0, sizeof(m_11));
    memset(m_2c, 0, sizeof(m_2c));
}
MSG_RESET_RANDOM_OPTION::MSG_RESET_RANDOM_OPTION()
    : m_equipSlot(0), m_scrollSlot(0), m_rarityChoice(0)
{
}
MSG_RIGHT_OF_CHANGE_GROW_TYPE::MSG_RIGHT_OF_CHANGE_GROW_TYPE()
{
    m_0d = 0;  // ORIG C1：word、byte
    m_0f = 0;
}
MSG_SAVE_CHARACTER_OPTION::MSG_SAVE_CHARACTER_OPTION() {}
MSG_SAVE_GAME_OPTION_1::MSG_SAVE_GAME_OPTION_1() {}
MSG_SAVE_GAME_OPTION_2::MSG_SAVE_GAME_OPTION_2() {}
MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON() {}
MSG_SAVE_QUEST_NOTIFY::MSG_SAVE_QUEST_NOTIFY()
{
    memset(m_0d, 0, sizeof(m_0d));  // ORIG C1：memset(this+0xd,0,0x28)
}
MSG_SECURITY_LOG_PACKET::MSG_SECURITY_LOG_PACKET()
{  // ORIG C1：memset(this,0,0x25)
    memset(this, 0, 0x25);    // ORIG C1：constants 先装(先 arg3/arg2 再 this)——去掉 (char*) 强转
}
MSG_SELECT_STRIKER::MSG_SELECT_STRIKER()
{
    m_0d = 0xff;  // ORIG C1：哨兵
    m_0e = 0;
}
MSG_SKILL_COMMAND::MSG_SKILL_COMMAND()
{  // ORIG 0x8230b?? memset(this,0,4)：仅包头前 4 字节（整体清零语义，CQuickChattingOption::clear 同型）
    memset(this, 0, 4);    // ORIG C1：constants 先装(先 arg3/arg2 再 this)——去掉 (char*) 强转(其致 this 先装,与 ORIG 常数先不符)
}
MSG_SKILL_INIT::MSG_SKILL_INIT()
{
    m_0d = 0;  // ORIG C1：+0xd byte 清零
}
MSG_SOLO_TELEPORT::MSG_SOLO_TELEPORT()
{  // ORIG 0x0822d764 调用链实证
    memset(m_0d, 0, sizeof(m_0d));  // +0xd..+0x2a
    m_2c = 0;
    m_30 = 0;
    m_34 = -1;      // +0x34 dword = 0xffffffff
    m_tp.reset();   // stPartyTelePort::reset @ +0x38（ORIG 出线调用）
    m_44 = 0;
}
MSG_STACKABLE_ACTION_USE::MSG_STACKABLE_ACTION_USE()
    : m_0d(0), m_0f(0), m_10(0), m_14(0), m_15(0), m_19(0) {}  // ORIG 0x826033e
MSG_TEMP_STRUCT::MSG_TEMP_STRUCT() {}
MSG_UPDATE_CONTRACT_OF_CUBE_INFO::MSG_UPDATE_CONTRACT_OF_CUBE_INFO()
{
    m_useFlag = 0;    // ORIG C1（字段化改名，值语义不变）
    m_selectIdx = 0;
}
MSG_USER_HISTORY_LOG::MSG_USER_HISTORY_LOG()
{
    // ORIG C1：+0xd byte、+0x10 int 清零、memset(this+0x14,0,0x100)
    m_0d = 0;
    m_10 = 0;
    memset(m_14, 0, sizeof(m_14));
}
MSG_USE_DYE::MSG_USE_DYE()
{
    // ORIG C1：+0xe/+0x10 word 清零
    m_0e = 0;
    m_10 = 0;
}
MSG_USE_LIMIT_CUBE::MSG_USE_LIMIT_CUBE()
{  // ORIG C1：memset(this,0,4)（同 MSG_SKILL_COMMAND）
    memset(this, 0, 4);    // ORIG C1：constants 先装(先 arg3/arg2 再 this)——去掉 (char*) 强转(其致 this 先装,与 ORIG 常数先不符)
}
MSG_USE_RENAME_CARD::MSG_USE_RENAME_CARD()
{
    m_0e = 0;  // ORIG C1：+0xe word
}
MsgClearQuestIndex::MsgClearQuestIndex() : m_questIdxs() {}  // ORIG 0x8231a3a：仅 vector C1
// ORIG 0x08235266（W）：逐标量拷贝 +0x00..+0x08 / +0x0c / +0x10，跳过 +0x09..+0x0b
// 与 +0x0d..+0x0f（编译器逐成员拷贝语义，padding 不拷贝），m_questIdxs 走
// std::vector<int>::operator=。
MsgClearQuestIndex& MsgClearQuestIndex::operator=(const MsgClearQuestIndex& other)
{
    // ORIG 0x8235266：基类子对象整体赋值（3×dword + 尾字节），随后 count/vector
    static_cast<MSG_BASE&>(*this) = static_cast<const MSG_BASE&>(other);
    m_count = other.m_count;
    m_questIdxs = other.m_questIdxs;
    return *this;
}
MsgRequestAssaultPrice::MsgRequestAssaultPrice()
{
    m_0d = -1;  // ORIG C1：word=0xffff
}
MsgTournamentRewardSelect::MsgTournamentRewardSelect()
{
    m_0d = 0xff;  // ORIG C1：双 byte 哨兵
    m_0e = 0xff;
}

AbnormalUseStackable::AbnormalUseStackable()
{
    m_4 = 0;  // ORIG C1：byte
}
BuyItemParam::BuyItemParam()
    : m_item(), m_resultItem(), m_80(0), m_84(0), m_88(0), m_8c(0),
      m_90(0), m_94(0), m_pair(), m_a0(-1), m_a4(-1), m_a8(0), m_ac(0), m_b0(0)
{  // ORIG 0x8230906：初始化列表按声明序 emit —— 两 Inven_Item C1、六连 0 写
}

CheckSecurityProtection::CheckSecurityProtection()
{
    m_4 = 0;  // ORIG C1：byte
}
InformNoticeData::InformNoticeData() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
InformNoticeData2nd::InformNoticeData2nd() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
LimitNpcBuyItem::LimitNpcBuyItem() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
PcRoomPlayTimeRewardParam::PcRoomPlayTimeRewardParam() {}
ReqDebugClearQuestResult::ReqDebugClearQuestResult() : m_index() {}  // ORIG 0x8231a72
ReqResult::ReqResult() : m_found(0), m_questIdx(-1) {}
STQuestShopBuyParam::STQuestShopBuyParam()
{  // ORIG C1：+4 byte、+6 word、+8 int
    m_4 = 0;
    m_6 = 0;
    m_8 = 0;
}
STQuestShopParam::STQuestShopParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
STUdateItemParam::STUdateItemParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
ScanBotByDllParam::ScanBotByDllParam()
{  // ORIG C1：+4 int、+8 byte、+a word
    m_4 = 0;
    m_8 = 0;
    m_a = 0;
}
ScanBotByDrvParam::ScanBotByDrvParam()
{  // ORIG C1：+4 int、+8 byte
    m_4 = 0;
    m_8 = 0;
}
ScriptHashParam::ScriptHashParam()
{  // ORIG C1：+4/+5 byte、+8 int 清零
    m_4 = 0;
    m_5 = 0;
    m_8 = 0;
}
SecuCardRpyParam::SecuCardRpyParam()
{
    m_4 = 0;  // ORIG C1：两 byte
    m_5 = 0;
}
SendMercenaryCompetitionResult::SendMercenaryCompetitionResult() {}
// [b28c-32] verdict=DIFF：N91/P216（competition_reward_result ctor nothrow 待 PacketTypes.h 代落）。
SendMercenaryReturnResult::SendMercenaryReturnResult()
{  // ORIG C1：competition_reward_result 出线构造于 +0xc（成员自动构造承担）
}

// ORIG _slot_String 成员函数（0x8239e66/0x8239f16 实证）：word@+0、string@+2。
//   copy ctor / op= 首 2 字节均走一次 16 位字拷贝（movzwl (%eax),%edx; mov %dx,(%eax)），
//   m_slot 现为单一 unsigned short（PacketTypes.h 代落），两连并行字拷贝即 byte-identical。
MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String::_slot_String() {}
MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String::_slot_String(const _slot_String& o)
    : m_slot(o.m_slot), m_str(o.m_str)
{
}
MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String&
MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String::operator=(const _slot_String& o)
{
    m_slot = o.m_slot;
    m_str = o.m_str;
    return *this;
}
MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String::~_slot_String() {}
TempParam::TempParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
saveQuestNotifyParam::saveQuestNotifyParam() {}
stAddFriendParam::stAddFriendParam() { m_field4 = 0; }  // ORIG 0x8230f64
stAvatarDisjointRandomReward::stAvatarDisjointRandomReward() : m_4(0), m_8(0) {}  // ORIG 0x82319aa
stBingoQuiz::stBingoQuiz() {}
stBingoReward::stBingoReward() {}
stBlueMarble::stBlueMarble() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stBlueMarbleConfirmInfo::stBlueMarbleConfirmInfo() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stBossDieCheckParam::stBossDieCheckParam()
{  // ORIG C1：+4 int、+8 byte、+a word、+c int
    m_4 = 0;
    m_8 = 0;
    m_a = 0;
    m_c = 0;
}
stBreakAwayParam::stBreakAwayParam() : m_resultCode(0) {}
stCallMemberListParam::stCallMemberListParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stChangeAnotherSkillTree::stChangeAnotherSkillTree() {}
stChangeAvatarOption::stChangeAvatarOption() {}
stChangeEmotionParam::stChangeEmotionParam()
{
    m_4 = 0;  // ORIG C1：word
}
stChangeHostWarRoom::stChangeHostWarRoom() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stChangeLetterStatParam::stChangeLetterStatParam() : m_result(0) {}
  // ORIG C1：+0x04 int 清零
stCharacSlotExtendEffect::stCharacSlotExtendEffect()
{  // ORIG C1：+4 int、+8/+9 byte
    m_4 = 0;
    m_8 = 0;
    m_9 = 0;
}
stClientSpecStatisticParam::stClientSpecStatisticParam()
{
    m_spec = 0;  // ORIG C1：byte（字段化改名）
}
stCloseDisjointStoreParam::stCloseDisjointStoreParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stCollectItems::stCollectItems()
{  // ORIG C1：+4 int、+8/+a word 清零
    m_4 = 0;
    m_8 = 0;
    m_a = 0;
}
stComboSkill::stComboSkill() : m_result(0) {}
  // ORIG C1：+0x04 int 清零
stComboSkillExtensionQuickSlotReset::stComboSkillExtensionQuickSlotReset() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stCreateCharac::stCreateCharac() : m_errNo(0) {}
stCreateDisjointStoreParam::stCreateDisjointStoreParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stDeleteCharac::stDeleteCharac() : m_errNo(0) {}
stDeleteItemParam::stDeleteItemParam()
    : m_4(0), m_8(0), m_9(0)
{  // ORIG 0x8230b38：i=0..100 清零三个数组
    for (int i = 0; i < 100; ++i)
    {
        m_slotsA[i] = 0;
        m_slotsD2[i] = 0;
        m_counts19c[i] = 0;
    }
}
stDeleteToBlackListParam::stDeleteToBlackListParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stDieCharacInDungeon::stDieCharacInDungeon() : m_errNo(0) {}
stDropItemParam::stDropItemParam()
    : m_errNo(0), m_8(0)
{  // ORIG C1 另清 +0xc int（字段已补入头文件）
    m_c = 0;
}
// NOTE(ORIG 0x8230d02)：还存在 +0xc int 清零，待头文件补 int m_c 后加入初始化列表。
stEnchantByBeadParam::stEnchantByBeadParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stEnchantParam::stEnchantParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stEnterDisjointStoreParam::stEnterDisjointStoreParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stEquipslotSwitch::stEquipslotSwitch()
{
    m_result = 0;  // ORIG C1：byte（字段化改名）
}
stExchangeRandomItemReward::stExchangeRandomItemReward() : m_4(0), m_8(0), m_9(0) {}  // ORIG 0x823191e
stExpandEquipslotFlagUpdate::stExpandEquipslotFlagUpdate()
{
    m_4 = 0;  // ORIG C1：byte
}
stExpertExtraction::stExpertExtraction() : m_list()
{  // ORIG 0x82317fe
    m_4 = 0;
    m_8 = 0;
    m_list.clear();
}
stFatigueAttendance::stFatigueAttendance() {}
stFinishLoadParam::stFinishLoadParam() : m_errNo(0) {}
stGetItemParam::stGetItemParam() : m_errNo(0) {}
stGoldTakeIncreasingAmount::stGoldTakeIncreasingAmount()
{  // ORIG C1：+4 int、+8 byte
    m_4 = 0;
    m_8 = 0;
}
stGrowthCreature::stGrowthCreature() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stGrowthEquipment::stGrowthEquipment() : m_4(0), m_8(0), m_c(0), m_10(0), m_14(0) {}  // ORIG 0x8231e56
stGrowthPowerParam::stGrowthPowerParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stGuildBoardParam::stGuildBoardParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stGuildCargoHistoryParam::stGuildCargoHistoryParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stGuildCargoMoveItemParam::stGuildCargoMoveItemParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stGuildCargoParam::stGuildCargoParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stGuildCargoPopItemParam::stGuildCargoPopItemParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stGuildCargoPushItemParam::stGuildCargoPushItemParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stHatchCreature::stHatchCreature() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stImageCommunicationEquipmentUse::stImageCommunicationEquipmentUse()
{
    m_4 = 0;  // ORIG C1：三连 int
    m_8 = 0;
    m_c = 0;
}
stItemDictionary::stItemDictionary() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stItemHyperLinkMessage::stItemHyperLinkMessage() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stLeavePartyParam::stLeavePartyParam() : m_errNo(0) {}
stLogIngameAdvertisement::stLogIngameAdvertisement() {}
stMailboxExtractParam::stMailboxExtractParam()
{
    m_4 = 0;  // ORIG C1：双 int
    m_8 = 0;
}
stMailboxOpenParam::stMailboxOpenParam()
{
    m_4 = 0;  // ORIG C1：双 int
    m_8 = 0;
}
stMailboxSendParam::stMailboxSendParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stMemberEnterReplyParam::stMemberEnterReplyParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stMemberSecedeParam::stMemberSecedeParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stMonstercardBind::stMonstercardBind()
{  // ORIG 0x823175a：vector C1 先于标量（成员自动构造），随后拷贝链与 clear
    m_4 = 0;
    m_c = 0;
    m_a = m_c;
    m_8 = m_a;
    m_list.clear();
}
stMoveItemParam::stMoveItemParam() : m_4(0), m_8(0), m_a(0), m_c(0), m_10(0), m_14(0) {}  // ORIG 0x8230cd4
stMoveMapParam::stMoveMapParam() : m_errNo(0) {}
stObjectBringUp::stObjectBringUp() : m_4(0), m_8(0), m_a(0), m_b(0) {}  // ORIG 0x8231ae6
stOnBuySkillParam::stOnBuySkillParam()
    : m_4(0), m_350(-1)
{  // ORIG 0x8230a04：m_infos[70] 隐式逐元素 buySkillInfoC1（GCC 循环），尾部标量与
   // memset(this+8,0,0x348)。唯一指令级差异：ORIG 将 m_4 清零置于数组构造前。
    m_354 = 0;
    m_355 = 0;
    memset(m_infos, 0, sizeof(m_infos));
}
stOnChangeSkillParam::stOnChangeSkillParam() : m_curSlot(-1), m_newSlot(-1) {}
stOnGetUserInfoParam::stOnGetUserInfoParam()
{  // ORIG C1：+4 word、+6 byte
    m_4 = 0;
    m_6 = 0;
}
stOnPvPTimeOut::stOnPvPTimeOut() : m_timeOutResult(-1) {}
stOnRecoverStaminaParam::stOnRecoverStaminaParam() : m_errNo(-1) {}
stOnSetPvPReadyParam::stOnSetPvPReadyParam() : m_errNo(0) {}
stOnSortItemParam::stOnSortItemParam() : m_invenType(0), m_result(0) {}
stOnUseEquipParam::stOnUseEquipParam() : m_errNo(0), m_8(0) {}
stOpenCeraPackage::stOpenCeraPackage() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stPartyTeleport::stPartyTeleport() {}
stPartyTeleportConfirm::stPartyTeleportConfirm() {}
stQuickChatParam::stQuickChatParam() {}
stQuickPartyParam::stQuickPartyParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stRefundSkill::stRefundSkill() {}
stRegenROIResult::stRegenROIResult() : m_errNo(0), m_insertedSlot(0) {}
stRegisterToBlackListParam::stRegisterToBlackListParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stRemoveFriendParam::stRemoveFriendParam() : m_errNo(0) {}
stRenameCreature::stRenameCreature() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stRentItemParam::stRentItemParam() : m_4(0) {}  // ORIG 0x8231cda；RentOutput 成员自动构造
stRepairDisjointMachineParam::stRepairDisjointMachineParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stRepairEquipParam::stRepairEquipParam() : m_errNo(0), m_8(0), m_a(0), m_c(0) {}
stRequestAssaultPrice::stRequestAssaultPrice() {}
stRequestBlackListParam::stRequestBlackListParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stRequestCharacSkillInfo::stRequestCharacSkillInfo() {}
stRequestDisjointItemParam::stRequestDisjointItemParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stRequestIngameAdvertisement::stRequestIngameAdvertisement() {}
stRequestMemberEnterParam::stRequestMemberEnterParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stRightOfChangeGrowType::stRightOfChangeGrowType() {}
stSaveCharacOption::stSaveCharacOption() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stSecretShopBuyItemParam::stSecretShopBuyItemParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stSecretShopOpenCloseParam::stSecretShopOpenCloseParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stSecurityLogParam::stSecurityLogParam() {}
stSelectCharac::stSelectCharac() : m_errNo(0) {}
// NOTE(stSelectDungeonParam ORIG 0x8230dd8：+4/+8/+c=0、+10=2、+14=0 —— 头文件字段就绪后改写)
stSelectDungeonParam::stSelectDungeonParam()
{  // ORIG 0x8230dd8：写入序 +4/+8/+c=0、+10=2、+14=0
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
    m_10 = 2;
    m_14 = 0;
}
stSelectStriker::stSelectStriker() {}
stSendClearedMission::stSendClearedMission() : m_missionIdxs() {}  // ORIG 0x8250d1e
stSeriaRoomDecoEvent::stSeriaRoomDecoEvent()
{  // ORIG C1：+4 int、+8/+a word
    m_4 = 0;
    m_8 = 0;
    m_a = 0;
}
stSetAreaParam::stSetAreaParam()
{  // ORIG C1：写入序 +4 int、+8/+9 byte
    m_errNo = 0;
    m_8 = 0;
    m_9 = 0;
}
// NOTE(ORIG 0x8230e5e)：另有 +8/+9 两字节清零，待头文件补字段。
stSetPartyInfo::stSetPartyInfo() : m_errNo(0) {}
stSkillCommandParam::stSkillCommandParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stSkillInit::stSkillInit()
{
    // ORIG C1：+4 int、+8 byte 清零
    m_4 = 0;
    m_8 = 0;
}
stSoloTeleportParam::stSoloTeleportParam()
{
    m_4 = 0;  // ORIG C1：三连 int
    m_8 = 0;
    m_c = 0;
}
stStackableActionUse::stStackableActionUse() {}
stStartGameParam::stStartGameParam()
{  // ORIG C1：+4=0、+8=0、+0xc=2
    m_errNo = 0;
    m_8 = 0;
    m_c = 2;
}
// NOTE(ORIG 0x8230f2a)：另清 +8 int 与 +c int=2，待头文件补字段。
stTeleportParam::stTeleportParam()
{  // ORIG 0x8231362：写入序 +4、+8(word)、+10、+11、+14
    m_4 = 0;
    m_8 = 0;
    m_10 = 0;
    m_11 = 0;
    m_14 = 0;
}
stTournamentRewardSelect::stTournamentRewardSelect() {}
stTournamentRewardSelectState::stTournamentRewardSelectState() {}
stUpdateContractOfCubeInfo::stUpdateContractOfCubeInfo()
{  // ORIG C1：+4 int、+8/+9 byte
    m_4 = 0;
    m_8 = 0;
    m_9 = 0;
}
stUpgradeDisjointMachineParam::stUpgradeDisjointMachineParam() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stUseCoinParam::stUseCoinParam() : m_errNo(0)
{
    m_8 = 0;  // ORIG C1：+4 int、+8 word 清零
}
stUseDye::stUseDye() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stUseLimitCube::stUseLimitCube()
{  // ORIG C1：+4 byte、+8/+c int、+10 word
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
}
stUseRenameCard::stUseRenameCard() : m_4(0) {}
  // ORIG C1：+0x04 int 清零
stUseSkillParam::stUseSkillParam() : m_errNo(0) {}  // ORIG C1：+4 int 清零
stWalkoutPartyParam::stWalkoutPartyParam() : m_errNo(0) {}
stWarRoomWp::stWarRoomWp() : m_4(0) {}
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

void base64_encode(const unsigned char* data, unsigned int len,
                   std::string& out)
{
    int i = 0;
    unsigned char b3[3];
    while (len != 0)
    {
        --len;
        b3[i] = *data;
        ++i;
        ++data;
        if (i == 3)
        {
            unsigned char b4[4];
            b4[0] = static_cast<unsigned char>(b3[0] >> 2);
            b4[1] = static_cast<unsigned char>((b3[0] & 3) * 16 + (b3[1] >> 4));
            b4[2] = static_cast<unsigned char>((b3[1] & 0xf) * 4 + (b3[2] >> 6));
            b4[3] = static_cast<unsigned char>(b3[2] & 0x3f);
            for (int k = 0; k < 4; ++k)
                out += base64_chars[b4[k]];
            i = 0;
        }
    }
    if (i > 0)
    {
        for (int k = i; k < 3; ++k)
            b3[k] = 0;
        unsigned char b4[4];
        b4[0] = static_cast<unsigned char>(b3[0] >> 2);
        b4[1] = static_cast<unsigned char>((b3[0] & 3) * 16 + (b3[1] >> 4));
        b4[2] = static_cast<unsigned char>((b3[1] & 0xf) * 4 + (b3[2] >> 6));
        b4[3] = static_cast<unsigned char>(b3[2] & 0x3f);
        for (int k = 0; k < i + 1; ++k)
            out += base64_chars[b4[k]];
        for (int k = i + 1; k < 4; ++k)
            out += '=';
    }
}

void base64_decode(std::string const& in, std::vector<unsigned char>& out)
{
    int in_len = static_cast<int>(in.size());
    int i = 0;
    int in_ = 0;
    unsigned char c4[4];
    unsigned char c3[3];
    while (in_len-- != 0)
    {
        // b28d-lane25：P167 差集实证 ORIG 每次迭代读 in[in_] 多次（不缓存 ch）。
        if (in[in_] == '=')
            break;
        if (!is_base64(static_cast<unsigned char>(in[in_])))
            break;
        c4[i++] = static_cast<unsigned char>(in[in_++]);
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                c4[i] = static_cast<unsigned char>(
                    base64_chars.find(static_cast<char>(c4[i]), 0));
            c3[0] = static_cast<unsigned char>((c4[0] << 2) + ((c4[1] & 0x30) >> 4));
            c3[1] = static_cast<unsigned char>((c4[1] << 4) + ((c4[2] & 0x3c) >> 2));
            c3[2] = static_cast<unsigned char>((c4[2] << 6) + c4[3]);
            for (i = 0; i < 3; i++)
                out.push_back(c3[i]);
            i = 0;
        }
    }
    if (i != 0)
    {
        int j;
        for (j = i; j <= 3; j++)
            c4[j] = 0;
        for (j = 0; j < 4; j++)
            c4[j] = static_cast<unsigned char>(
                base64_chars.find(static_cast<char>(c4[j]), 0));
        c3[0] = static_cast<unsigned char>((c4[0] << 2) + ((c4[1] & 0x30) >> 4));
        c3[1] = static_cast<unsigned char>((c4[1] << 4) + ((c4[2] & 0x3c) >> 2));
        c3[2] = static_cast<unsigned char>((c4[2] << 6) + c4[3]);
        for (j = 0; j < i - 1; j++)
            out.push_back(c3[j]);
    }
}
