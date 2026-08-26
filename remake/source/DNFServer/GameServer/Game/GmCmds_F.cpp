// GmCmds_F.cpp — game_master GM 命令批次 F（11 类）。
// 对照 docs/class_func_reports/game_master__<Class>/：
//   CRequestDisjointCmd / CReselectDailyQuestCmd / CResetTrainingQuestCmd /
//   CSecretShopCheckLottery / CSecretShopNpc / CSetGuildFund /
//   CSetMultiboxFailCnt / CSetTutorialFlagCmd / CStartEvent(+getParam) /
//   CStartMacroCmd / CStopMacroCmd
//
// 头文件 GameMasterCmd.h 本批禁止编辑：+0x08 通用槽在头里为占位
// std::string（gcc4.4 旧 ABI sizeof=4），本批多数类的 ORIG 真实语义是
// int（atoi 解析结果），经 str_as_int 别名访问；CStartEvent 的 +0x08..0x13
// 真实布局为 [int count][int eventId][short p1][short p2]，经占位字段
// m_str0/m_f1 别名映射（总大小 0x14 一致）。
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "CSecretShop.h"
#include "CParty.h"
#include "CDungeon.h"
#include "GameTypes.h"
#include "PacketHeader.h"

// Packet_Monitor_Event_Start —— ORIG 权威定义三处同构：
//   Monitor/DNFPacketTranslater.h:439（0x44f/0x12，eventCode + p1/p2 ushort）
//   Guild/DNFApplication.h:145 / COServer/DNFApplication.h:96。
// 因 COServer 版头会拉入与 Game/CUser.h 冲突的 COServer DNFUser（class CUser
// 重定义），本 TU 按权威布局最小化复刻对接（packed，sizeof=0x12；ctor 实现
// 位于 Monitor/DNFPacketTranslater.cpp:5674）。
class Packet_Monitor_Event_Start : public PacketHeader
{
public:
    Packet_Monitor_Event_Start();
    unsigned int m_field1;    // +0xa eventType/eventCode
    unsigned short m_field2;  // +0xe param1
    unsigned short m_field3;  // +0x10 param2
} __attribute__((packed));

// ---------------------------------------------------------------------------
// 外部符号（本批禁改共享头，按 ORIG mangled 直连 / 等价自由函数声明）：
//
// ORIG T 0x086475d8 _ZN5CUser14RewardTutorialEj —— CUser::RewardTutorial(uint)，
// CUser.h 未声明（禁改）；i386 成员 ABI 即 this 首参压栈，extern "C" 等价直连。
extern "C" void _ZN5CUser14RewardTutorialEj(CUser* user, unsigned int flag);

// ORIG T 0x081f92ca _ZN23DisPatcher_DisJointItem9_disjointEP5CUserii14ENUM_CMDPACKETS1_t
// —— DisPatcher_DisJointItem 静态成员（参数表 P5Cuser i i ENUM_CMDPACKET CUser* ushort，
// 无 this）；DisPatcherCore.h 未声明（禁改），按同参数表自由函数 extern。
// （game_master 命令链路只做编译期调用约定对接，符号名以注释为准）
extern void DisPatcher_DisJointItem_disjoint(CUser* requestUser, int reason,
                                             int unk0, ENUM_CMDPACKET cmd,
                                             CUser* targetUser,
                                             unsigned short region);

namespace game_master
{

// 占位 std::string 槽（+0x08）按 ORIG 实际 int/short 语义别名访问。
// 用宏而非 inline 函数：-O0 下函数调用会引入 ORIG 没有的额外 call 形态。
#define GM_STR0_INT(obj) (*(reinterpret_cast<int*>(&(obj))))

// CStartEvent 真实布局（头文件占位 m_str0/m_f1 别名映射，总大小 0x14 一致）：
//   +0x08 int 参数计数 / +0x0c int eventId / +0x10 short p1 / +0x12 short p2
#define GM_EV_COUNT(c)  (*(reinterpret_cast<int*>(&(c)->m_str0)))
#define GM_EV_ID(c)     ((reinterpret_cast<int*>(&(c)->m_str0))[1])
#define GM_EV_P1(c)     (*(reinterpret_cast<unsigned short*>(&(c)->m_f1)))
#define GM_EV_P2(c)     ((reinterpret_cast<unsigned short*>(&(c)->m_f1))[1])

// ===========================================================================
// CRequestDisjointCmd（id 注册序见 Mgr ctor；sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a8124：基类 ctor → 自身 vptr(0x8c81ba0)；string@+0x08 默认构造。
CRequestDisjointCmd::CRequestDisjointCmd()
    : m_str0()
{
}

// ORIG parse 0x084a97c0：trim(parser->args)；m_str0 = parser->args（目标角色名）。
void CRequestDisjointCmd::parse()
{
    trim(m_parser->m_args);
    m_str0 = m_parser->m_args;
}

// ORIG execute 0x084a97f4：按角色名查目标用户，存在则以 GM 发起者名义请求
// 拆解（DisPatcher_DisJointItem::_disjoint(user, 9, 0, 0x1c, target, 0xffff)；
// 9=请求来源 GM（推断），0x1c=CMD 包类型（对应 dispatchers[28]），
// 0xffff=目标区域通配（推断））。
void CRequestDisjointCmd::execute()
{
    char const* name = m_str0.c_str();   // ORIG 先取名字符串再取 GameWorld
    CUser* target = G_GameWorld()->find_user_by_charac_name(name);
    if (target != 0)
    {
        CUser* self = GetUser();
        DisPatcher_DisJointItem_disjoint(self, 9, 0,
                                         static_cast<ENUM_CMDPACKET>(0x1c),
                                         target, 0xffff);
    }
}

// ===========================================================================
// CReselectDailyQuestCmd（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a8370：基类 ctor → 自身 vptr，无数据成员。
CReselectDailyQuestCmd::CReselectDailyQuestCmd()
{
}

// ORIG execute 0x084ab4f2：G_GameWorld()->ReselectDailyQuest()（返回值丢弃）。
void CReselectDailyQuestCmd::execute()
{
    G_GameWorld()->ReselectDailyQuest();
}

// ===========================================================================
// CResetTrainingQuestCmd（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a838c：基类 ctor → 自身 vptr，无数据成员。
CResetTrainingQuestCmd::CResetTrainingQuestCmd()
{
}

// ORIG execute 0x084ab508：GetUser()->ResetTrainingQuest()。
void CResetTrainingQuestCmd::execute()
{
    GetUser()->ResetTrainingQuest();
}

// ===========================================================================
// CSecretShopCheckLottery（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a847a：基类 ctor → 自身 vptr，无数据成员。
CSecretShopCheckLottery::CSecretShopCheckLottery()
{
}

// ORIG execute 0x084ab9c4：GlobalData::s_secret_shop->CheckLottery()。
void CSecretShopCheckLottery::execute()
{
    GlobalData::s_secret_shop->CheckLottery();
}

// ===========================================================================
// CSecretShopNpc（sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a8496：基类 ctor → 自身 vptr；string@+0x08 默认构造。
CSecretShopNpc::CSecretShopNpc()
    : m_str0()
{
}

// ORIG parse 0x084ab9da：trim(args)；+0x08(int 占位槽) = atoi(args)（npc id）。
void CSecretShopNpc::parse()
{
    trim(m_parser->m_args);
    GM_STR0_INT(m_str0) = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084aba12：GM 用户处于 state 5（组队等待态，推断）时，把本人
// 秘密商店数据的 npc id 改为参数值（限 1000/0x3ea/0x3eb/0x3ec 四个合法值），
// 条件：无队伍，或队伍不在血色地下城中（party->m_dungeon == 0 或
// dungeon->m_bloodDungeonType == 0）。随后聊天通告 + 广播 0x114 npc 替换包。
void CSecretShopNpc::execute()
{
    CUser* user = GetUser();
    if (user->get_state() != 5)
    {
        return;
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    // ORIG CUser::GetSecretShopData 返回秘密商店数据块指针，首 int 为 npc id。
    int* shopData = static_cast<int*>(user->GetSecretShopData());
    if (shopData == 0)
    {
        return;
    }

    int npcId = GM_STR0_INT(m_str0);
    if (!(npcId == 1000 || npcId == 0x3ea || npcId == 0x3eb || npcId == 0x3ec))
    {
        return;
    }
    if (party != 0 && party->m_dungeon != 0 &&
        party->m_dungeon->m_bloodDungeonType > 0)
    {
        return;
    }

    *shopData = npcId;

    // ORIG：256 字节栈缓冲清零后 sprintf 韩文通告。
    char buf[0x100];
    for (int i = 0; i < 0x40; ++i)
    {
        reinterpret_cast<unsigned int*>(buf)[i] = 0;
    }
    sprintf(buf, "비밀상점 npc가 %d로 교체되었습니다", *shopData);

    int newNpcId = *shopData;
    const char* name = user->getCurCharacName();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x73f, 0);
    trace("[SECRETSHOP] change npc(%s, %d)", name, newNpcId);

    PacketGuard chatPacket;
    G_GameWorld()->make_packet_chat_msg(chatPacket, user, buf,
                                        static_cast<int>(strlen(buf)), 2,
                                        static_cast<ENUM_NOTIPACKET>(0xc));

    PacketGuard packet;
    packet.clear();                          // ORIG InterfacePacketBuf::clear
    packet.put_header(0, 0x114);
    packet.put_int(*shopData);
    packet.finalize(true);

    if (party == 0)
    {
        user->Send(chatPacket);
        user->Send(packet);
    }
    else
    {
        party->send_to_party(chatPacket);
        party->send_to_party(packet);
    }
}

// ===========================================================================
// CSetGuildFund（sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a8576：基类 ctor → 自身 vptr；string@+0x08 默认构造。
CSetGuildFund::CSetGuildFund()
    : m_str0()
{
}

// ORIG parse 0x084ac668：trim(args)；+0x08(int 占位槽) = atoi(args)（金额）。
void CSetGuildFund::parse()
{
    trim(m_parser->m_args);
    GM_STR0_INT(m_str0) = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084ac6a0：向发起者所在公会代理发送加 fund 请求
// SendAddGuildFund(characNo, guildKey, amount, eFundType_GM)。
void CSetGuildFund::execute()
{
    CUser* user = GetUser();
    unsigned int amount = static_cast<unsigned int>(GM_STR0_INT(m_str0));
    unsigned int guildKey =
        static_cast<unsigned int>(user->get_charac_guildkey());
    unsigned int characNo = static_cast<unsigned int>(user->get_charac_no(-1));
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendAddGuildFund(characNo, guildKey, amount,
                            Packet_Add_Guild_Fund::eFundType_GM);
}

// ===========================================================================
// CSetMultiboxFailCnt（sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a891e：基类 ctor → 自身 vptr；string@+0x08 默认构造。
CSetMultiboxFailCnt::CSetMultiboxFailCnt()
    : m_str0()
{
}

// ORIG parse 0x084b34a4：+0x08(int 占位槽) = 0；trim(args)；
// atoi(parser->m_extra[1])（第二个分词）。
void CSetMultiboxFailCnt::parse()
{
    GM_STR0_INT(m_str0) = 0;
    trim(m_parser->m_args);
    std::vector<std::string>& extra = m_parser->m_extra;
    GM_STR0_INT(m_str0) = atoi(extra[1].c_str());   // ORIG 直接 operator[](1)，无边界检查
}

// ORIG execute 0x084b34f8：set_multiboxLotteryItemFailCnt(v)；v > 2 时追加
// 下发 0x1c6 空包头通知客户端（多重盒子抽取失败次数刷新）。
void CSetMultiboxFailCnt::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    int cnt = GM_STR0_INT(m_str0);
    user->set_multiboxLotteryItemFailCnt(cnt);
    if (cnt > 2)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x1c6);
        packet.finalize(true);
        user->Send(packet);
    }
}

// ===========================================================================
// CSetTutorialFlagCmd（sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a80b4：基类 ctor → 自身 vptr；string@+0x08 默认构造。
CSetTutorialFlagCmd::CSetTutorialFlagCmd()
    : m_str0()
{
}

// ORIG parse 0x084a921a：trim(args)；+0x08(int 占位槽) = atoi(args)（教程标记）。
void CSetTutorialFlagCmd::parse()
{
    trim(m_parser->m_args);
    GM_STR0_INT(m_str0) = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084a9252：GetUser()->RewardTutorial(flag)
//（ORIG T 0x086475d8，经 extern "C" mangled 直连）。
void CSetTutorialFlagCmd::execute()
{
    CUser* user = GetUser();
    unsigned int flag = static_cast<unsigned int>(GM_STR0_INT(m_str0));
    _ZN5CUser14RewardTutorialEj(user, flag);
}

// ===========================================================================
// CStartEvent（sizeof=0x14）
//   ORIG 真实布局：+0x08 int 参数计数 / +0x0c int eventId /
//                 +0x10 short param1 / +0x12 short param2。
//   头文件占位 m_str0(string)@+0x08、m_f1(uint)@+0x10 总大小一致，此处别名映射。
// ===========================================================================

namespace
{
inline int& ev_count(CStartEvent* c)   // +0x08
{
    return *reinterpret_cast<int*>(&c->m_str0);
}
inline int& ev_event_id(CStartEvent* c) // +0x0c
{
    return reinterpret_cast<int*>(&c->m_str0)[1];
}
inline unsigned short& ev_param1(CStartEvent* c) // +0x10
{
    return *reinterpret_cast<unsigned short*>(&c->m_f1);
}
inline unsigned short& ev_param2(CStartEvent* c) // +0x12
{
    return reinterpret_cast<unsigned short*>(&c->m_f1)[1];
}
} // namespace

// ORIG ctor 0x082a8442：仅基类 ctor → 自身 vptr(0x8c81918)，不清数据字段
//（清零动作在 parse 开头）。
CStartEvent::CStartEvent()
{
}

// ORIG parse 0x084ab5b6：清零四字段；trim(args)；构造 args 副本交 getParam 切分。
void CStartEvent::parse()
{
    GM_EV_ID(this) = 0;
    GM_EV_P1(this) = 0;
    GM_EV_P2(this) = 0;
    GM_EV_COUNT(this) = 0;

    trim(m_parser->m_args);
    std::string args = m_parser->m_args;
    getParam();   // ORIG 语义：getParam(args)（见 getParam 注释）
}

// ORIG getParam 0x084ab64e _ZN11game_master11CStartEvent8getParamESs
// —— 真实签名为 void getParam(std::string)（按值副本）；因本批禁止编辑
// GameMasterCmd.h，头内占位声明为无参。此处以无参成员承载同一逻辑：内部
// 显式复制 parser->m_args 副本后再切分，保证 ORIG「按值副本」语义
//（substr 回写不影响 parser->m_args）。
// 逻辑：按 ' ' 循环切分入 16 个栈上 string 槽（ORIG 无上限防护，原样保留）；
// 余串入末槽；随后 atoi：param[0]→eventId(int)，param[1]→param1(short)，
// param[2]→param2(short)。
void CStartEvent::getParam()
{
    std::string param = m_parser->m_args;   // ORIG 按值形参副本
    int count = 0;
    std::string params[16];

    while (true)
    {
        size_t pos = param.find(' ', 0);
        if (pos == std::string::npos)
        {
            break;
        }
        params[count] = param.substr(0, pos);
        param = param.substr(pos + 1);
        count = count + 1;
    }
    params[count] = param;

    GM_EV_ID(this) = atoi(params[0].c_str());
    GM_EV_P1(this) = static_cast<unsigned short>(atoi(params[1].c_str()));
    GM_EV_P2(this) = static_cast<unsigned short>(atoi(params[2].c_str()));
}

// ORIG execute 0x084ab886：仅 GM 用户生效；组 Packet_Monitor_Event_Start
//（0x44f/0x12：eventType=eventCode，p1/p2 两 ushort），经监控代理按
// 发起者 server group 下发 TCP 包。
void CStartEvent::execute()
{
    CUser* user = GetUser();
    if (!user->isGMUser())
    {
        return;
    }

    Packet_Monitor_Event_Start packet;
    packet.m_field1 = static_cast<unsigned int>(GM_EV_ID(this));
    packet.m_field2 = GM_EV_P1(this);
    packet.m_field3 = GM_EV_P2(this);

    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x12);
}

// ===========================================================================
// CStartMacroCmd（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a81a2：基类 ctor → 自身 vptr，无数据成员。
CStartMacroCmd::CStartMacroCmd()
{
}

// ORIG execute 0x084a9bfa：函数体仅 push %ebp; mov %esp,%ebp; pop %ebp; ret
// —— 原始二进制即为空实现（宏录制的启停在 CMacro/包处理链路，非此命令）。
// 如实还原为空。
void CStartMacroCmd::execute()
{
}

// ===========================================================================
// CStopMacroCmd（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a81be：基类 ctor → 自身 vptr，无数据成员。
CStopMacroCmd::CStopMacroCmd()
{
}

// ORIG execute 0x084a9c00：同 CStartMacroCmd，原始二进制即为空实现。
void CStopMacroCmd::execute()
{
}

} // namespace game_master
