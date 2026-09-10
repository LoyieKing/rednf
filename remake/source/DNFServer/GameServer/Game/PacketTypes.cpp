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
                   std::string& out)
{
    int i = 0;
    int j = 0;                       // ORIG 0x0807ea3b `movl $0,-0xc`（函数级 j=0）
    unsigned char b3[3];
    // ORIG 0x0807eafd..0x0807eb0a：`cmpl $0,0xc(%ebp); setne %al; subl $0x1,0xc(%ebp);
    // test %al,%al; jne <body>` ⇒ 原码是 `while (len-- != 0)`（形参自减，退出后为 -1）。
    while (len-- != 0)
    {
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
            // ORIG 0x0807eaae：清零/自增均作用于 -0x10(i) ⇒ 循环变量复用 i。
            for (i = 0; i < 4; ++i)
                out += base64_chars[b4[i]];
            i = 0;
        }
    }
    if (i > 0)
    {
        // ORIG 0x0807eb1a..0x0807eb37：`mov i,%eax; mov %eax,-0xc(j)` + j<=2 计数
        // ⇒ 原码用函数级 j 作循环变量（非块内新变量）。
        for (j = i; j < 3; ++j)
            b3[j] = 0;
        unsigned char b4[4];
        b4[0] = static_cast<unsigned char>(b3[0] >> 2);
        b4[1] = static_cast<unsigned char>((b3[0] & 3) * 16 + (b3[1] >> 4));
        b4[2] = static_cast<unsigned char>((b3[1] & 0xf) * 4 + (b3[2] >> 6));
        b4[3] = static_cast<unsigned char>(b3[2] & 0x3f);
        // ORIG 0x0807ebbe：`mov i,%eax; add $0x1,%eax; cmp j,%eax; setg` ⇒ j < i+1
        for (j = 0; j < i + 1; ++j)
            out += base64_chars[b4[j]];
        // ORIG 0x0807ebe3：`cmpl $0x2,-0x10(i); setle %al; addl $0x1,-0x10;
        // test %al,%al; jne <body>`（比较先于自增）⇒ 原码 `while (i++ <= 2)`；
        // 自增后 i 出函数即被修改为 4（照抄原码行为，P171）。
        while (i++ <= 2)
            out += '=';
    }
}

#include "PacketTypes/RentOutput.inc"
#include "PacketTypes/dtor_RentOutput.inc"

#include "PacketTypes/MSG_BINGO_QUIZ.inc"
{
    m_0d = 0;  // ORIG C1：+0xd int 清零
}
#include "PacketTypes/MSG_BLUE_MARBLE.inc"
{
    m_0d = 0;  // ORIG C1：+0xd byte 清零
}
#include "PacketTypes/MSG_BLUE_MARBLE_CONFIRM_INFO.inc"
    : m_0d(0), m_0e(0xffff)
{  // ORIG 0x822e39a：+0xd byte=0 先、+0xe word=0xffff 后
}
#include "PacketTypes/MSG_BREAK_AWAY.inc"
// 字段化后保持占位版语义：+0xd 包头后全量清零（ORIG 无出线 ctor，此为本仓库构造约定）。
#include "PacketTypes/MSG_BUY_SKILL.inc"
    : m_treeKind(0), m_count(0)
{
    memset(m_entries, 0, 4);  // ORIG 0x8230956 仅清 +0xf 起 4 字节
}
#include "PacketTypes/MSG_CHANGE_ANOTHER_SKILL_TREE.inc"
{
    m_0d = 0xff;  // ORIG C1：哨兵
}
#include "PacketTypes/MSG_CHANGE_AVATAR_OPTION.inc"
    : m_consumeSlot(0xffff), m_consumeItemIdx(-1), m_avatarSlot(0xffff),
      m_equipItemIdx(-1), m_option(0xff)
{
    // ORIG 0x0822e0cc：MSG_BASE C1 后 +0x0d word=-1/+0x0f dword=-1/+0x13
    // word=-1/+0x15 dword=-1/+0x19 byte=0xff（初始列表序一致）。
}
#include "PacketTypes/MSG_CHANGE_RANDOM_OPTION.inc"
  // ORIG C1：+0xe/+0x10 word、+0x12 byte 清零
#include "PacketTypes/MSG_CHARAC_SELECT.inc"
// MSG_CHECK_GUILD_JOIN ORIG 0x0822dd7c：+0xd int=0；+0x11 起 memset 0x17。
#include "PacketTypes/MSG_CHECK_GUILD_JOIN.inc"
{  // ORIG 0x0822dd7c：+0xd int=0；memset(this+0x11,0,0x17)
    m_0d = 0;
    memset(m_11, 0, sizeof(m_11));
}
// MSG_CLIENT_SPEC_STATISTIC ORIG 0x082556a0：MSG_BASE::C1(this) + Client_Spec::C1(this+0xe)。
// [b28d-Main·检查点14] Client_Spec 现为 0xbf8（HWSpec[0xff] 已建模，见 CUser.h）。
#include "PacketTypes/MSG_CLIENT_SPEC_STATISTIC.inc"
#include "PacketTypes/MSG_COMBO_SKILL.inc"
{
    m_comboMap.clear();  // ORIG 0x82603d6 构造后显式 clear
}
#include "PacketTypes/MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET.inc"
{
    m_0d = 0;  // ORIG C1
}
#include "PacketTypes/MSG_COMPATIBILITY_INDEX.inc"
    : m_0d(0), m_0e(0), m_10(0), m_14(0), m_16(0), m_18(0), m_1a(0),
      m_1c(0), m_1d(0) {}  // ORIG 0x822e174
#include "PacketTypes/MSG_ENCHANT.inc"
{
    // ORIG 0x822dc04：先写 +0x13/+0x15，再拷贝到 +0x0d/+0x0f，末尾 +0x19
    m_13 = 0;
    m_0d = m_13;
    m_15 = 0;
    m_0f = m_15;
    m_19 = 0;
}
#include "PacketTypes/MSG_EXCHANGE_RANDOM_ITEM_REWARD.inc"
#include "PacketTypes/MSG_EXPERT_EXTRACTION.inc"
{
    // ORIG C1：+0xd byte=0、+0x10 word=0、+0xe ← (+0x10)
    m_0d = 0;
    m_10 = 0;
    m_0e = m_10;
}
#include "PacketTypes/MSG_FATIGUE_ATTENDANCE.inc"
{
    m_0d = 0;  // ORIG C1
}
#include "PacketTypes/MSG_GROWTH_EQUIPMENT.inc"
{
    // ORIG C1：+0xd/+0x11 int 清零
    m_0d = 0;
    m_11 = 0;
}
#include "PacketTypes/MSG_GROWTH_POWER.inc"
{
    // ORIG C1：+0xd byte 清零、memset(this+0xe,0,5)
    m_0d = 0;
    memset(m_0e, 0, sizeof(m_0e));
}
// ORIG 0x822b802：+0=0xff、+1=0xff、+2=0、memset(this+3,0,0x1e)
#include "PacketTypes/STGuildMemberCharacData.inc"
{
    m_0 = 0xff;
    m_1 = 0xff;
    m_2 = 0;
    memset(m_pad3, 0, 0x1e);
}

// ORIG 0x822b83c：成员 C1（m_members@0x84）→ ints +0x78/+0x7c/+0x80 清零 →
// memset(this,0,0x78)（+0..+0x77 全为 m_pad78，与 memset 首参等价）。
// 唯一定义处归本 TU：消除 MSG_GUILD_BOARD 构造对 _ZN18STGuildBoardDBInfoC1Ev 的 undefined。
#include "PacketTypes/STGuildBoardDBInfo.inc"
    : m_members()
{
    m_78 = 0;
    m_7c = 0;
    m_80 = 0;
    memset(m_pad78, 0, 0x78);
}

#include "PacketTypes/MSG_GUILD_BOARD.inc"
{
    m_b2 = 0;  // ORIG 0x822dbfa
}
#include "PacketTypes/MSG_INFORM_NOTICE.inc"
{
    m_noticeFlag = 0x80;  // ORIG C1：notice 类型哨兵（字段化改名，值语义不变）
}
#include "PacketTypes/MSG_INFORM_NOTICE_2ND.inc"
{
    m_noticeFlag = 0x80;  // ORIG C1（字段化改名）
}
#include "PacketTypes/MSG_ITEM_UPGRADE.inc"
{
    // ORIG C1：先 +0x25 word=0，memset(this+0x28,0,0x81)，末尾 +0x27 byte=0
    m_25 = 0;
    memset(m_28, 0, sizeof(m_28));
    m_27 = 0;
}
#include "PacketTypes/MSG_LOG_INGAME_ADVERTISEMENT.inc"
#include "PacketTypes/MSG_MONSTERCARD_BIND.inc"
{
    // ORIG C1：拷贝链 m_11←0 → m_0f ← m_11 → m_0d ← m_0f
    m_11 = 0;
    m_0f = m_11;
    m_0d = m_0f;
}
#include "PacketTypes/MSG_OPEN_CERAPACKAGE.inc"
{
    // ORIG C1：+0xe word 清零、memset(this+0x14,0,0x118)
    m_0e = 0;
    memset(m_14, 0, sizeof(m_14));
}
#include "PacketTypes/MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE.inc"
{
    m_0d = 0;  // ORIG C1：+0xd byte、+0xe int
    m_0e = 0;
}
#include "PacketTypes/MSG_P2P_STATISTICS.inc"
    // ORIG 0x822e1d2：逐字段写入序 d,e,10,12,14,18,1c,20,24,28,2a,2c,30,34,38,3c（字段声明升序）。
    // -O0 下 ctor 先发射 init-list 成员（声明序）；全部移入 init-list 使发射序与 ORIG 升序一致。
    : m_0d(0), m_0e(0), m_10(0), m_12(0), m_14(0), m_18(0), m_1c(0),
      m_20(0), m_24(0), m_28(0), m_2a(0), m_2c(0), m_30(0), m_34(0), m_38(0), m_3c(0)
{}
#include "PacketTypes/MSG_PARTY_TELEPORT.inc"
{
    // ORIG C1：byte/byte/word/word/byte 全零
    m_field0d = 0;
    m_field0e = 0;
    m_field0f = 0;
    m_field11 = 0;
    m_field13 = 0;
}
#include "PacketTypes/MSG_PARTY_TELEPORT_CONFIRM.inc"
{
    m_confirmFlag = 0;  // ORIG C1（字段化改名）
}
#include "PacketTypes/MSG_PURIFY_ITEM.inc"
{
    // ORIG C1：四组标量清零
    m_0d = 0;
    m_0f = 0;
    m_13 = 0;
    m_15 = 0;
}
#include "PacketTypes/MSG_PVP_MISSION_HP_PERCENT.inc"
#include "PacketTypes/dtor_MSG_PVP_MISSION_HP_PERCENT.inc"
#include "PacketTypes/MSG_QUEST_SHOP.inc"
{
    // ORIG C1：+0xd/+0xe byte 清零
    m_0d = 0;
    m_0e = 0;
}
#include "PacketTypes/MSG_QUICK_CHAT.inc"
{  // ORIG C1：标量初始化列表先于 m_quick 构造（声明序）
}
#include "PacketTypes/MSG_QUICK_PARTY.inc"
{
    // ORIG C1：+0xe word=0、+0x10 byte=0xff
    m_0e = 0;
    m_10 = 0xff;
}
#include "PacketTypes/MSG_REFUND_SKILL.inc"
{
    // ORIG C1：+0xd int 与 +0x11..+0x13 bytes 清零
    m_0d = 0;
    m_11 = 0;
    m_12 = 0;
    m_13 = 0;
}
#include "PacketTypes/MSG_RENAME_CREATURE.inc"
{
    memset(m_name, 0, sizeof(m_name));  // ORIG C1：memset(this+0x14,0,0xd)（字段化改名）
}
#include "PacketTypes/MSG_RENT_ITEM.inc"
{  // ORIG C1：+0xd/+0xe byte 清零、memset(this+0xf,0,2)
    m_0d = 0;
    m_0e = 0;
    memset(m_0f, 0, sizeof(m_0f));
}
#include "PacketTypes/MSG_REQUEST_CHARAC_SKILL_INFO.inc"
{
    m_0d = 0xff;  // ORIG C1：哨兵
}
#include "PacketTypes/MSG_REQUEST_INGAME_ADVERTISEMENT.inc"
#include "PacketTypes/MSG_REQUEST_GUILD_JOIN.inc"
{  // ORIG C1：+0xd int=0；memset(+0x11)；+0x28 int=0；memset(+0x2c)
    m_0d = 0;
    m_28 = 0;
    memset(m_11, 0, sizeof(m_11));
    memset(m_2c, 0, sizeof(m_2c));
}
#include "PacketTypes/MSG_RESET_RANDOM_OPTION.inc"
    : m_equipSlot(0), m_scrollSlot(0), m_rarityChoice(0)
{
}
#include "PacketTypes/MSG_RIGHT_OF_CHANGE_GROW_TYPE.inc"
{
    m_0d = 0;  // ORIG C1：word、byte
    m_0f = 0;
}
#include "PacketTypes/MSG_SAVE_CHARACTER_OPTION.inc"
#include "PacketTypes/MSG_SAVE_GAME_OPTION_1.inc"
#include "PacketTypes/MSG_SAVE_GAME_OPTION_2.inc"
#include "PacketTypes/MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON.inc"
#include "PacketTypes/MSG_SAVE_QUEST_NOTIFY.inc"
{
    memset(m_0d, 0, sizeof(m_0d));  // ORIG C1：memset(this+0xd,0,0x28)
}
#include "PacketTypes/MSG_SECURITY_LOG_PACKET.inc"
{  // ORIG C1：memset(this,0,0x25)
    memset(this, 0, 0x25);    // ORIG C1：constants 先装(先 arg3/arg2 再 this)——去掉 (char*) 强转
}
#include "PacketTypes/MSG_SELECT_STRIKER.inc"
{
    m_0d = 0xff;  // ORIG C1：哨兵
    m_0e = 0;
}
#include "PacketTypes/MSG_SKILL_COMMAND.inc"
{  // ORIG 0x8230b?? memset(this,0,4)：仅包头前 4 字节（整体清零语义，CQuickChattingOption::clear 同型）
    memset(this, 0, 4);    // ORIG C1：constants 先装(先 arg3/arg2 再 this)——去掉 (char*) 强转(其致 this 先装,与 ORIG 常数先不符)
}
#include "PacketTypes/MSG_SKILL_INIT.inc"
{
    m_0d = 0;  // ORIG C1：+0xd byte 清零
}
#include "PacketTypes/MSG_SOLO_TELEPORT.inc"
{  // ORIG 0x0822d764 调用链实证
    memset(m_0d, 0, sizeof(m_0d));  // +0xd..+0x2a
    m_2c = 0;
    m_30 = 0;
    m_34 = -1;      // +0x34 dword = 0xffffffff
    m_tp.reset();   // stPartyTelePort::reset @ +0x38（ORIG 出线调用）
    m_44 = 0;
}
#include "PacketTypes/MSG_STACKABLE_ACTION_USE.inc"
    : m_0d(0), m_0f(0), m_10(0), m_14(0), m_15(0), m_19(0) {}  // ORIG 0x826033e
#include "PacketTypes/MSG_TEMP_STRUCT.inc"
#include "PacketTypes/MSG_UPDATE_CONTRACT_OF_CUBE_INFO.inc"
{
    m_useFlag = 0;    // ORIG C1（字段化改名，值语义不变）
    m_selectIdx = 0;
}
#include "PacketTypes/MSG_USER_HISTORY_LOG.inc"
{
    // ORIG C1：+0xd byte、+0x10 int 清零、memset(this+0x14,0,0x100)
    m_0d = 0;
    m_10 = 0;
    memset(m_14, 0, sizeof(m_14));
}
#include "PacketTypes/MSG_USE_DYE.inc"
{
    // ORIG C1：+0xe/+0x10 word 清零
    m_0e = 0;
    m_10 = 0;
}
#include "PacketTypes/MSG_USE_LIMIT_CUBE.inc"
{  // ORIG C1：memset(this,0,4)（同 MSG_SKILL_COMMAND）
    memset(this, 0, 4);    // ORIG C1：constants 先装(先 arg3/arg2 再 this)——去掉 (char*) 强转(其致 this 先装,与 ORIG 常数先不符)
}
#include "PacketTypes/MSG_USE_RENAME_CARD.inc"
{
    m_0e = 0;  // ORIG C1：+0xe word
}
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
{
    m_0d = -1;  // ORIG C1：word=0xffff
}
#include "PacketTypes/MsgTournamentRewardSelect.inc"
{
    m_0d = 0xff;  // ORIG C1：双 byte 哨兵
    m_0e = 0xff;
}

#include "PacketTypes/AbnormalUseStackable.inc"
{
    m_4 = 0;  // ORIG C1：byte
}
#include "PacketTypes/BuyItemParam.inc"
    : m_item(), m_resultItem(), m_80(0), m_84(0), m_88(0), m_8c(0),
      m_90(0), m_94(0), m_pair(), m_a0(-1), m_a4(-1), m_a8(0), m_ac(0), m_b0(0)
{  // ORIG 0x8230906：初始化列表按声明序 emit —— 两 Inven_Item C1、六连 0 写
}

#include "PacketTypes/CheckSecurityProtection.inc"
{
    m_4 = 0;  // ORIG C1：byte
}
#include "PacketTypes/InformNoticeData.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/InformNoticeData2nd.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/LimitNpcBuyItem.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/PcRoomPlayTimeRewardParam.inc"
#include "PacketTypes/ReqDebugClearQuestResult.inc"
#include "PacketTypes/ReqResult.inc"
#include "PacketTypes/STQuestShopBuyParam.inc"
{  // ORIG C1：+4 byte、+6 word、+8 int
    m_4 = 0;
    m_6 = 0;
    m_8 = 0;
}
#include "PacketTypes/STQuestShopParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/STUdateItemParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/ScanBotByDllParam.inc"
{  // ORIG C1：+4 int、+8 byte、+a word
    m_4 = 0;
    m_8 = 0;
    m_a = 0;
}
#include "PacketTypes/ScanBotByDrvParam.inc"
{  // ORIG C1：+4 int、+8 byte
    m_4 = 0;
    m_8 = 0;
}
#include "PacketTypes/ScriptHashParam.inc"
{  // ORIG C1：+4/+5 byte、+8 int 清零
    m_4 = 0;
    m_5 = 0;
    m_8 = 0;
}
#include "PacketTypes/SecuCardRpyParam.inc"
{
    m_4 = 0;  // ORIG C1：两 byte
    m_5 = 0;
}
#include "PacketTypes/SendMercenaryCompetitionResult.inc"
// [b28c-32] verdict=DIFF：N91/P216（competition_reward_result ctor nothrow 待 PacketTypes.h 代落）。
#include "PacketTypes/SendMercenaryReturnResult.inc"
{  // ORIG C1：competition_reward_result 出线构造于 +0xc（成员自动构造承担）
}

// ORIG _slot_String 成员函数（0x8239e66/0x8239f16 实证）：word@+0、string@+2。
//   copy ctor / op= 首 2 字节均走一次 16 位字拷贝（movzwl (%eax),%edx; mov %dx,(%eax)），
//   m_slot 现为单一 unsigned short（PacketTypes.h 代落），两连并行字拷贝即 byte-identical。
#include "PacketTypes/_slot_String.inc"
#include "PacketTypes/_slot_String_1.inc"
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
#include "PacketTypes/dtor__slot_String.inc"
#include "PacketTypes/TempParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/saveQuestNotifyParam.inc"
#include "PacketTypes/stAddFriendParam.inc"
#include "PacketTypes/stAvatarDisjointRandomReward.inc"
#include "PacketTypes/stBingoQuiz.inc"
#include "PacketTypes/stBingoReward.inc"
#include "PacketTypes/stBlueMarble.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stBlueMarbleConfirmInfo.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stBossDieCheckParam.inc"
{  // ORIG C1：+4 int、+8 byte、+a word、+c int
    m_4 = 0;
    m_8 = 0;
    m_a = 0;
    m_c = 0;
}
#include "PacketTypes/stBreakAwayParam.inc"
#include "PacketTypes/stCallMemberListParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stChangeAnotherSkillTree.inc"
#include "PacketTypes/stChangeAvatarOption.inc"
#include "PacketTypes/stChangeEmotionParam.inc"
{
    m_4 = 0;  // ORIG C1：word
}
#include "PacketTypes/stChangeHostWarRoom.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stChangeLetterStatParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stCharacSlotExtendEffect.inc"
{  // ORIG C1：+4 int、+8/+9 byte
    m_4 = 0;
    m_8 = 0;
    m_9 = 0;
}
#include "PacketTypes/stClientSpecStatisticParam.inc"
{
    m_spec = 0;  // ORIG C1：byte（字段化改名）
}
#include "PacketTypes/stCloseDisjointStoreParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stCollectItems.inc"
{  // ORIG C1：+4 int、+8/+a word 清零
    m_4 = 0;
    m_8 = 0;
    m_a = 0;
}
#include "PacketTypes/stComboSkill.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stComboSkillExtensionQuickSlotReset.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stCreateCharac.inc"
#include "PacketTypes/stCreateDisjointStoreParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stDeleteCharac.inc"
#include "PacketTypes/stDeleteItemParam.inc"
    : m_4(0), m_8(0), m_9(0)
{  // ORIG 0x8230b38：i=0..100 清零三个数组
    for (int i = 0; i < 100; ++i)
    {
        m_slotsA[i] = 0;
        m_slotsD2[i] = 0;
        m_counts19c[i] = 0;
    }
}
#include "PacketTypes/stDeleteToBlackListParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stDieCharacInDungeon.inc"
#include "PacketTypes/stDropItemParam.inc"
    : m_errNo(0), m_8(0)
{  // ORIG C1 另清 +0xc int（字段已补入头文件）
    m_c = 0;
}
// NOTE(ORIG 0x8230d02)：还存在 +0xc int 清零，待头文件补 int m_c 后加入初始化列表。
#include "PacketTypes/stEnchantByBeadParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stEnchantParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stEnterDisjointStoreParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stEquipslotSwitch.inc"
{
    m_result = 0;  // ORIG C1：byte（字段化改名）
}
#include "PacketTypes/stExchangeRandomItemReward.inc"
#include "PacketTypes/stExpandEquipslotFlagUpdate.inc"
{
    m_4 = 0;  // ORIG C1：byte
}
#include "PacketTypes/stExpertExtraction.inc"
{  // ORIG 0x82317fe
    m_4 = 0;
    m_8 = 0;
    m_list.clear();
}
#include "PacketTypes/stFatigueAttendance.inc"
#include "PacketTypes/stFinishLoadParam.inc"
#include "PacketTypes/stGetItemParam.inc"
#include "PacketTypes/stGoldTakeIncreasingAmount.inc"
{  // ORIG C1：+4 int、+8 byte
    m_4 = 0;
    m_8 = 0;
}
#include "PacketTypes/stGrowthCreature.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stGrowthEquipment.inc"
#include "PacketTypes/stGrowthPowerParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stGuildBoardParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stGuildCargoHistoryParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stGuildCargoMoveItemParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stGuildCargoParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stGuildCargoPopItemParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stGuildCargoPushItemParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stHatchCreature.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stImageCommunicationEquipmentUse.inc"
{
    m_4 = 0;  // ORIG C1：三连 int
    m_8 = 0;
    m_c = 0;
}
#include "PacketTypes/stItemDictionary.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stItemHyperLinkMessage.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stLeavePartyParam.inc"
#include "PacketTypes/stLogIngameAdvertisement.inc"
#include "PacketTypes/stMailboxExtractParam.inc"
{
    m_4 = 0;  // ORIG C1：双 int
    m_8 = 0;
}
#include "PacketTypes/stMailboxOpenParam.inc"
{
    m_4 = 0;  // ORIG C1：双 int
    m_8 = 0;
}
#include "PacketTypes/stMailboxSendParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stMemberEnterReplyParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stMemberSecedeParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stMonstercardBind.inc"
{  // ORIG 0x823175a：vector C1 先于标量（成员自动构造），随后拷贝链与 clear
    m_4 = 0;
    m_c = 0;
    m_a = m_c;
    m_8 = m_a;
    m_list.clear();
}
#include "PacketTypes/stMoveItemParam.inc"
#include "PacketTypes/stMoveMapParam.inc"
#include "PacketTypes/stObjectBringUp.inc"
#include "PacketTypes/stOnBuySkillParam.inc"
    : m_4(0), m_350(-1)
{  // ORIG 0x8230a04：m_infos[70] 隐式逐元素 buySkillInfoC1（GCC 循环），尾部标量与
   // memset(this+8,0,0x348)。唯一指令级差异：ORIG 将 m_4 清零置于数组构造前。
    m_354 = 0;
    m_355 = 0;
    memset(m_infos, 0, sizeof(m_infos));
}
#include "PacketTypes/stOnChangeSkillParam.inc"
#include "PacketTypes/stOnGetUserInfoParam.inc"
{  // ORIG C1：+4 word、+6 byte
    m_4 = 0;
    m_6 = 0;
}
#include "PacketTypes/stOnPvPTimeOut.inc"
#include "PacketTypes/stOnRecoverStaminaParam.inc"
#include "PacketTypes/stOnSetPvPReadyParam.inc"
#include "PacketTypes/stOnSortItemParam.inc"
#include "PacketTypes/stOnUseEquipParam.inc"
#include "PacketTypes/stOpenCeraPackage.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stPartyTeleport.inc"
#include "PacketTypes/stPartyTeleportConfirm.inc"
#include "PacketTypes/stQuickChatParam.inc"
#include "PacketTypes/stQuickPartyParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stRefundSkill.inc"
#include "PacketTypes/stRegenROIResult.inc"
#include "PacketTypes/stRegisterToBlackListParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stRemoveFriendParam.inc"
#include "PacketTypes/stRenameCreature.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stRentItemParam.inc"
#include "PacketTypes/stRepairDisjointMachineParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stRepairEquipParam.inc"
#include "PacketTypes/stRequestAssaultPrice.inc"
#include "PacketTypes/stRequestBlackListParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stRequestCharacSkillInfo.inc"
#include "PacketTypes/stRequestDisjointItemParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stRequestIngameAdvertisement.inc"
#include "PacketTypes/stRequestMemberEnterParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stRightOfChangeGrowType.inc"
#include "PacketTypes/stSaveCharacOption.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stSecretShopBuyItemParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stSecretShopOpenCloseParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stSecurityLogParam.inc"
#include "PacketTypes/stSelectCharac.inc"
// NOTE(stSelectDungeonParam ORIG 0x8230dd8：+4/+8/+c=0、+10=2、+14=0 —— 头文件字段就绪后改写)
#include "PacketTypes/stSelectDungeonParam.inc"
{  // ORIG 0x8230dd8：写入序 +4/+8/+c=0、+10=2、+14=0
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
    m_10 = 2;
    m_14 = 0;
}
#include "PacketTypes/stSelectStriker.inc"
#include "PacketTypes/stSendClearedMission.inc"
#include "PacketTypes/stSeriaRoomDecoEvent.inc"
{  // ORIG C1：+4 int、+8/+a word
    m_4 = 0;
    m_8 = 0;
    m_a = 0;
}
#include "PacketTypes/stSetAreaParam.inc"
{  // ORIG C1：写入序 +4 int、+8/+9 byte
    m_errNo = 0;
    m_8 = 0;
    m_9 = 0;
}
// NOTE(ORIG 0x8230e5e)：另有 +8/+9 两字节清零，待头文件补字段。
#include "PacketTypes/stSetPartyInfo.inc"
#include "PacketTypes/stSkillCommandParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stSkillInit.inc"
{
    // ORIG C1：+4 int、+8 byte 清零
    m_4 = 0;
    m_8 = 0;
}
#include "PacketTypes/stSoloTeleportParam.inc"
{
    m_4 = 0;  // ORIG C1：三连 int
    m_8 = 0;
    m_c = 0;
}
#include "PacketTypes/stStackableActionUse.inc"
#include "PacketTypes/stStartGameParam.inc"
{  // ORIG C1：+4=0、+8=0、+0xc=2
    m_errNo = 0;
    m_8 = 0;
    m_c = 2;
}
// NOTE(ORIG 0x8230f2a)：另清 +8 int 与 +c int=2，待头文件补字段。
#include "PacketTypes/stTeleportParam.inc"
{  // ORIG 0x8231362：写入序 +4、+8(word)、+10、+11、+14
    m_4 = 0;
    m_8 = 0;
    m_10 = 0;
    m_11 = 0;
    m_14 = 0;
}
#include "PacketTypes/stTournamentRewardSelect.inc"
#include "PacketTypes/stTournamentRewardSelectState.inc"
#include "PacketTypes/stUpdateContractOfCubeInfo.inc"
{  // ORIG C1：+4 int、+8/+9 byte
    m_4 = 0;
    m_8 = 0;
    m_9 = 0;
}
#include "PacketTypes/stUpgradeDisjointMachineParam.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stUseCoinParam.inc"
{
    m_8 = 0;  // ORIG C1：+4 int、+8 word 清零
}
#include "PacketTypes/stUseDye.inc"
  // ORIG C1：+0x04 int 清零
#include "PacketTypes/stUseLimitCube.inc"
{  // ORIG C1：+4 byte、+8/+c int、+10 word
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
}
#include "PacketTypes/stUseRenameCard.inc"
  // ORIG C1：+0x04 int 清零
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
{
    int in_len = static_cast<int>(in.size());
    int i = 0;
    int in_ = 0;
    // ORIG 0x0807ec17 `movl $0x0,-0xc(%ebp)`（第三枚函数级 int 清零）：原码遗留
    // 死变量（P171 照抄；4.4 对带初始化的未使用局部照样发 store，P232）。
    int j = 0;
    (void)j;
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
        // ORIG 0x0807ed93..0x0807ee6f：三处尾循环的计数均落在 -0x10(in_) 槽
        // （`mov -0x14,%eax; mov %eax,-0x10` = in_=i 起、`addl $1,-0x10` 自增）
        // ⇒ 原码复用函数级 in_ 作循环变量，非块内新声明。
        for (in_ = i; in_ <= 3; in_++)
            c4[in_] = 0;
        for (in_ = 0; in_ < 4; in_++)
            c4[in_] = static_cast<unsigned char>(
                base64_chars.find(static_cast<char>(c4[in_]), 0));
        c3[0] = static_cast<unsigned char>((c4[0] << 2) + ((c4[1] & 0x30) >> 4));
        c3[1] = static_cast<unsigned char>((c4[1] << 4) + ((c4[2] & 0x3c) >> 2));
        c3[2] = static_cast<unsigned char>((c4[2] << 6) + c4[3]);
        for (in_ = 0; in_ < i - 1; in_++)
            out.push_back(c3[in_]);
    }
}
