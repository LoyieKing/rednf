// GmCmds_D.cpp — game_master GM 命令批次 D（10 类）。
// 还原自 docs/class_func_reports/game_master__*/ 与 df_game_r objdump。
//
// 头文件 GameMasterCmd.h 为多批次共享占位声明，本批次禁止改动：
//   - m_str0(+0x08) 占位 std::string；对 ORIG 实际为 POD(int/short/bool) 的类，
//     本 TU 一律以 reinterpret_cast 按偏移访问（见各类注释）。占位 string 由
//     隐式默认构造占用该槽（空串无堆分配），与 ORIG"不构造"仅差一次空串
//     初始化；对象经基类指针删除且基类无虚析构，派生成员析构与 ORIG 相同
//     均不发生。
//   - CIndependentDrop 真实布局 sizeof=0x3c：
//       +0x00 vptr +0x04 m_parser +0x08 bool m_all +0x09..0x0b pad
//       +0x0c std::set<int> m_monsterSet(0x18) +0x24 std::set<int> m_aiSet(0x18)
//     （ctor 0x082a87de objdump：两次 _ZNSt3setIiSt4lessIiESaIiEEC1Ev）
//     头内占位 m_str0/m_f1..m_f11 恰好同尺寸覆盖，本 TU 以 placement new 构造。
//   - CIndependentDrop 的 4 个带参辅助与 CLevelUp::getLevel/setLevel 在头里被
//     占位成无参成员声明；真实带参实现以"文件局部函数 + __asm__(ORIG mangled
//     名)"落符号（CParty.cpp asm-label 先例同法），类内无参占位保持未定义。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameTypes.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CMonster.h"
#include "STMonsterScript.h"
#include "CParty.h"
#include "CBattle_Field.h"
#include "WarField.h"
#include "CMap.h"
#include "CDungeon.h"
#include "MonsterIndependentDrop.h"
#include "HeroMissionEvent.h"
#include "Arad_DataManager.h"
#include "ServerParameterScript.h"
#include "DisPatcherCore.h"
#include "CEventScriptMng.h"      // EventClassify::CEventScriptMng（桥清零）
#include "CEnvironment.h"
#include "DNFFunctionLib.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <new>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace game_master
{

// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（CParty.h 以 pad 占位、
// 未引入 CBattle_Field 类型；Main 裁定本组 TU 局部视图，与 CParty.cpp 口径一致）。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};

// ---------------------------------------------------------------------------
// 外部函数桥（头文件未声明的 ORIG 符号；不改共享头，按 mangled 名链接）
// ---------------------------------------------------------------------------

// ORIG W 0x084b410e CAICharacter::getIndepentDropItem() const（sret 返回 vector）
// §4.5 例外：CAICharacter 权威镜像头 MonsterIndependentDrop.h 未建模这两个 W 访问器
// （getIndepentDropItem/GetName），CAICharacter 建模属 Wide21A2 邻域（CCreature/
// CDeathTower），故按原生 extern "C" 保留（目标确无权威头声明）。
extern "C" void _ZNK12CAICharacter19getIndepentDropItemEv(
    std::vector<stMonsterIndependentDrop_t>* ret, const CAICharacter* self);
// ORIG W 0x084b4138 CAICharacter::GetName() const
extern "C" const char* _ZNK12CAICharacter7GetNameEv(const CAICharacter* self);

// ---------------------------------------------------------------------------
// 头文件占位限制：GameMasterCmd.h 将下列带参辅助声明成了无参成员
// （PrintItemDropList()/ResultToStr()/SendChatMsg()/printClient()/
//   CLevelUp::getLevel()/setLevel()）。本 TU 不能改共享头，故真实带参实现
// 以文件局部函数 + __asm__("ORIG mangled 名") 落符号；类内的无参占位声明
// 保持未定义（无调用点）。
// ---------------------------------------------------------------------------

// ORIG W 0x084b165c game_master::CIndependentDrop::ResultToStr(
//   std::vector<stMonsterIndependentDrop_t> const&, char*)
void GmD_CIndependentDrop_ResultToStr(
    CIndependentDrop* self,
    const std::vector<stMonsterIndependentDrop_t>& drops, char* out)
    __asm__("_ZN11game_master16CIndependentDrop11ResultToStrERKSt6vectorI26stMonsterIndependentDrop_tSaIS2_EEPc");
// ORIG W 0x084b1756 game_master::CIndependentDrop::PrintItemDropList(
//   stMonsterIndependentDrop_t const&, char*, bool)
void GmD_CIndependentDrop_PrintItemDropList(
    CIndependentDrop* self, const stMonsterIndependentDrop_t& drop,
    char* out, bool detail)
    __asm__("_ZN11game_master16CIndependentDrop17PrintItemDropListERK26stMonsterIndependentDrop_tPcb");
// ORIG W 0x084b14b8 game_master::CIndependentDrop::printClient(CUser*, char const*, char const*)
void GmD_CIndependentDrop_printClient(CIndependentDrop* self, CUser* user,
                                      const char* fileName,
                                      const char* contents)
    __asm__("_ZN11game_master16CIndependentDrop11printClientEP5CUserPKcS4_");
// ORIG W 0x084b15c4 game_master::CIndependentDrop::SendChatMsg(CUser*, char*)
void GmD_CIndependentDrop_SendChatMsg(CIndependentDrop* self, CUser* user,
                                      char* msg)
    __asm__("_ZN11game_master16CIndependentDrop11SendChatMsgEP5CUserPc");

// ---------------------------------------------------------------------------
// 本地 POD 镜像：头内占位槽位按 ORIG 真实类型的具名访问（生成 disp 寻址，
// 与 ORIG mov 0x8(%eax) 形态一致；不改动共享头）。
// ---------------------------------------------------------------------------
namespace {

struct GmDBase { void* vptr; void* parser; };                // CCommand +0x00/+0x04

template <typename T>
struct GmDFld : GmDBase { T f08; };                          // 单 POD @+0x08

struct GmDID : GmDBase                                       // CIndependentDrop 0x3c
{
    bool all;                                                // +0x08
    char pad09[3];                                           // +0x09..0x0b
    std::set<int> mons;                                      // +0x0c
    std::set<int> ais;                                       // +0x24
};

// CHeroMissionCommand +0x0c/+0x10 的 enum 字段视图（P254）：ORIG execute
// 0x084b3e91/0x084b3e99 为 `mov disp(%eax),%eax; mov %eax,%esi/%ebx` 两段装载
// （4.1 对 enum 字段保留 eax→callee-saved 中转），4.4 对 unsigned 字段恒折单段。
enum GmDParamEnum { GmD_PARAM_ENUM_0 };
struct GmDHeroMissionCmdView : GmDBase
{
    void* str0;                                              // +0x08 std::string(COW)
    HeroMissionCondition::MissionNo::T no;                   // +0x0c
    GmDParamEnum param;                                      // +0x10
};

} // namespace

// ---------------------------------------------------------------------------
// EUC-KR 字面量（与 df_game_r .rodata 字节一致；源码以转义保真）
// ---------------------------------------------------------------------------
namespace {

// ORIG 0x08c80298 "%s파일이 생성되었습니다.\n"
const char kFileCreatedFmt[] =
    "%s\xc6\xc4\xc0\xcf\xc0\xcc \xbb\xfd\xbc\xba\xb5\xc7\xbe\xfa\xbd\xc0"
    "\xb4\xcf\xb4\xd9.\n";
// ORIG 0x08c802b4 "출력할 지정드랍 리스트가 없습니다.\n"（memcpy 0x24 字节含 NUL）
const char kNoDropList[] =
    "\xc3\xe2\xb7\xc2\xc7\xd2 \xc1\xf6\xc1\xa4\xb5\xe5\xb6\xf8 \xb8\xae\xbd"
    "\xba\xc6\xae\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.\n";
// ORIG 0x08c802df "리스트 드랍\n"（memcpy 0xd 字节含 NUL）
const char kListDrop[] = "\xb8\xae\xbd\xba\xc6\xae \xb5\xe5\xb6\xf8\n";
// ORIG 0x08c802ec "난이도\t%d\n"
const char kDifficultyFmt[] = "\xb3\xad\xc0\xcc\xb5\xb5\t%d\n";
// ORIG 0x08c802f7 "드랍율(%%)\t%f\n"（sprintf 格式 %% → 字面 %）
const char kDropRateFmt[] = "\xb5\xe5\xb6\xf8\xc0\xb2(%%)\t%f\n";
// ORIG 0x08c80306 "시도수\t%d\t%d\t%d\t%d\n"
const char kTryCountFmt[] = "\xbd\xc3\xb5\xb5\xbc\xf6\t%d\t%d\t%d\t%d\n";
// ORIG 0x08c8031a "총 드랍율(%%)\t%f\t%f\t%f\t%f\n"
const char kTotalDropRateFmt[] =
    "\xc3\xd1 \xb5\xe5\xb6\xf8\xc0\xb2(%%)\t%f\t%f\t%f\t%f\n";
// ORIG 0x08c80335 "총 개수\t%d\n\n"
const char kTotalCountFmt[] = "\xc3\xd1 \xb0\xb3\xbc\xf6\t%d\n\n";
// ORIG 0x08c80342 "아이템 리스트\n"（memcpy 0xf 字节含 NUL）
const char kItemList[] = "\xbe\xc6\xc0\xcc\xc5\xdb \xb8\xae\xbd\xba\xc6\xae\n";

} // namespace

// ===========================================================================
// CItemInfoCmd — ORIG ctor 0x082a82ba / parse 0x084aa29e / execute 0x084aa2d2
// 字段：+0x08 std::string m_str0（物品名，真实 string 成员）。
// ===========================================================================

// ORIG ctor：基类 vptr → 自身 vptr(0x8c81a50)；SsC1Ev 构造 +0x08。

// ORIG parse 0x084aa29e：trim(parser->args)；m_str0 = parser->args。
void CItemInfoCmd::parse()
{
    trim(m_parser->m_args);
    m_str0 = m_parser->m_args;
}

// ORIG execute 0x084aa2d2：按名查物品并回发查询结果包，找到则再 GM 发放一件。
// 【b28d-solo-GmCmds_D 判词 N78/N82】p167 CLEAR（49=49）；getStackableLimit 双调用
// && 链（P48/P214①）已归位。残差 = ORIG 两分支各一次 memset 被 4.1 内联成对齐剥离
// 慢路（各 39 条）而 4.4 恒 call（N78）+ 全帧槽号错位（N82），封顶。
void CItemInfoCmd::execute()
{
    // b28d-solo-GmCmds_D 判词 N78：ORIG 两分支各一次 `memset(info,0,0x100)` 被
    // 4.1 内联成对齐剥离慢路（各 39 条，0x084aa30c/0x084aa48c）；4.4 恒发 call
    // memset（两次 call 还会令 p167 EXTRA +2）⇒ 改用 `={0}`（P214③）一次 rep
    // stos 表达同一写前清零语义，p167 CLEAR；慢路形态差记 N78 封顶。
    char info[0x100] = {0};                                  // -0x157
    CItem* item = G_CDataManager()->find_item(m_str0.c_str());
    if (item == 0)
    {
        sprintf(info, "cannot find item (%s) ", m_str0.c_str());
        PacketGuard packet;                                  // -0x50
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(static_cast<int>(strlen(info)));
        packet.put_str(info, static_cast<int>(strlen(info)));
        packet.finalize(true);
        GetUser()->Send(packet);
        return;
    }

    sprintf(info, "item_index(%d) ", item->get_index());     // ORIG get_index 0x8110c48
    PacketGuard packet;                                      // -0x58
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int(static_cast<int>(strlen(info)));
    packet.put_str(info, static_cast<int>(strlen(info)));
    packet.finalize(true);
    GetUser()->Send(packet);

    const int itemIdx = item->get_index();                   // local_2c
    CUser* user = GetUser();                                 // local_28
    int count = 100;                                         // local_20
    // ORIG 0x084aa642/0x084aa657：getStackableLimit 两次独立调用（比较+赋值，
    // P48 无 CSE）且为 && 短路链末项（setl 物化）⇒ 单条 && 链 + 两次调用。
    if (item != 0 && item->is_stackable() &&
        static_cast<CStackableItem*>(item)->getStackableLimit() < count)
    {
        count = static_cast<CStackableItem*>(item)->getStackableLimit();
    }
    ENUM_ITEMSPACE space;                                    // local_44（AddItem 出参）
    const int added = user->AddItem(itemIdx, count,
                                    static_cast<eItemAddReason>(6), space, 0);
    if (added >= 0)
    {
        const char* name = user->getCurCharacName();
        char* accStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x539, 0);
        trace("%s:'%s' GM Mode Add Item(%d).", accStr, name, added);
        user->SendUpdateItemList(CUser::eSendTarget_1, space, added);
    }
}

// ===========================================================================
// CInvalidGold — ORIG ctor 0x082a83a8 / execute 0x084ab16e（sizeof=0x8 无字段）
// 行为：金钱短暂改为函数局部静态 invalidMoney(-10000 递减) 触发列表更新后
// 立即恢复原值。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c819a8)。

// ORIG execute 0x084ab16e。
// 【b28d-solo-GmCmds_D 判词 AE】旧「N-P75 参数物化时刻」判词作废：末段
// set_money(invenW, oldMoney) 的 ORIG `mov -0xc(%ebp),%ebx`（receiver call 前
// 装载 callee-saved）经裸 register 中转局部复现（P231/P248④：值跨 call 保活、
// 4.4 不自升、ORIG 目标 %ebx；非 asm 绑定，P0 合规）。探针 /tmp/b28d_gmd/p2.cpp
// f4 与 fA/fC/f5/f6 对照：取址形（*(&old)/volatile）反把 old 挤到深槽。
void CInvalidGold::execute()
{
    CUser* user = GetUser();
    const unsigned int oldMoney = user->getCurCharacMoney();
    // ORIG .data 0x093abb40 初值 ffffd8f0（-10000）；每次执行再减 10000。
    static int invalidMoney = (int)0xffffd8f0u;
    invalidMoney -= 10000;
    user->getCurCharacInvenW()->set_money(invalidMoney);
    user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, 0);
    // b28d-331：ORIG 末段 `mov -0xc(%ebp),%ebx` 在 receiver call 前装载
    // （4.1 实参先求值）⇒ 经裸 register 中转局部（P231/P248 合规，非 asm 绑定）
    // 令 4.4 提前物化入 %ebx 并复用 oldMoney 槽（探针 /tmp/b28d_gmd/p2.cpp f4 同形）。
    register unsigned int restoreMoney = oldMoney;
    user->getCurCharacInvenW()->set_money(restoreMoney);
}

// ===========================================================================
// CLinkSystem — ORIG ctor 0x082a84ea / execute 0x084abdd0（sizeof=0x8 无字段；
// 仅 execute 无 parse）。副本 0x2aff 开关切换。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c81888)。

// ORIG execute 0x084abdd0。
void CLinkSystem::execute()
{
    std::string msg("gm Link Charac : ");                    // local_14（构造后未使用，照抄）
    trim(m_parser->m_args);
    const char flag =
        static_cast<char>(atoi(m_parser->m_args.c_str()));   // movsbl 截断
    // b28d-17/48：ORIG 0x084abdd0 为 switch 决策树（movsbl;test;je;cmp $1;je）
    // 且不缓存 ServerParameterScript&（p167：G_CDataManager 调用 3 次，每分支重取）。
    switch (flag)
    {
    case 0:
        G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, false);
        G_GameWorld()->send_all_dungeon_inout_message(0x2aff, false);
        break;
    case 1:
        if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff) != true)
        {
            G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, true);
            G_GameWorld()->send_all_dungeon_inout_message(0x2aff, true);
        }
        break;
    default:
        break;
    }
}

// ===========================================================================
// CGetCashInven — ORIG ctor 0x082a85ca / parse 0x084ac80a / execute 0x084ac846
// 字段：+0x08 short（ORIG POD，头占位为 string；按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c817e0)，+0x08 不初始化。

// ORIG parse 0x084ac80a：trim(args)；*(short*)(this+8) = atoi(c_str(args))。
void CGetCashInven::parse()
{
    trim(m_parser->m_args);
    m_cashType = static_cast<short>(atoi(m_parser->m_args.c_str()));
}

// ORIG execute 0x084ac846：空函数（push ebp; mov ebp,esp; pop ebp; ret）。
void CGetCashInven::execute()
{
}

// ===========================================================================
// CItemDropInDungeon — ORIG ctor 0x082a84b2 / parse 0x084abcb0 /
// execute 0x084abce8。字段：+0x08 int 物品 idx（POD，按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c818b8)。

// ORIG parse 0x084abcb0：*(int*)(this+8) = atoi(c_str(trim(args)))。
void CItemDropInDungeon::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084abce8：可堆叠或 avatar/creature 类型(idx 0x16/0x17) 物品跳过，
// 其余交队伍 drop_item_gm。
// 【b28d-solo-GmCmds_D 判词 AE】p167 CLEAR；P0 表达式直读（无 reqIdx/skip 槽）、
// party==0 正向包裹消尾岛（P285/P223 变体）均已按 ORIG 复现。
void CItemDropInDungeon::execute()
{
    // b28d-solo-GmCmds_D：ORIG 无 reqIdx 槽（P0 表达式直读；末段 drop_item_gm
    // 处重读 +0x08 实证），skip 亦无 bool 槽（`test %al,%al` 直测）。
    CItem* item = G_CDataManager()->find_item(((const GmDFld<int>*)this)->f08);
    if (item == 0)
    {
        return;
    }
    // ORIG 虚表第 4 槽(vtable+0xc)=GetItemType。
    if (item->is_stackable() || item->GetItemType() == 0x16 ||
        item->GetItemType() == 0x17)
    {
        return;
    }
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    // ORIG party==0 的 `je` 直落 end（无 nop 岛，+0xc5）⇒ 正向包裹而非早返。
    if (party != 0)
    {
        party->drop_item_gm(
            user, static_cast<unsigned int>(((const GmDFld<int>*)this)->f08));
    }
}

// ===========================================================================
// CInGameEventSetDay — ORIG ctor 0x082a87a6 / parse 0x084b09d0 /
// execute 0x084b0a08。字段：+0x08 int day（POD，按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c81630)。

// ORIG parse 0x084b09d0：*(int*)(this+8) = atoi(c_str(trim(args)))。
void CInGameEventSetDay::parse()
{
    trim(m_parser->m_args);
    m_day = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084b0a08：event_script_mng->test_process_change_day(day)。
void CInGameEventSetDay::execute()
{
    // ORIG：this+8 物化 %ebx 跨 2 call；mng 结果 eax 直通 receiver，无具名槽。
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->test_process_change_day(((const GmDFld<int>*)this)->f08);
}

// ===========================================================================
// CIndependentDrop — ORIG ctor 0x082a87de(sizeof=0x3c) / parse 0x084b0b08 /
// execute 0x084b0b4e / printClient 0x084b14b8 / SendChatMsg 0x084b15c4 /
// ResultToStr 0x084b165c / PrintItemDropList 0x084b1756。
// 真实布局见文件头注释。
// ===========================================================================

namespace {

// b28d-solo-GmCmds_D：P49 —— -O0 下 inline 访问器仍发真实 call 符号
// （p167 EXTRA：indMonsterSet×5/indAiSet×5/indAllFlag×2，ORIG 无）⇒ 改宏，
// 零 call 且寻址形与 ORIG 的 disp 访问一致。
#define GMD_IND_ALL(p)  (((GmDID*)(p))->all)
#define GMD_IND_MONS(p) (((GmDID*)(p))->mons)
#define GMD_IND_AIS(p)  (((GmDID*)(p))->ais)

} // namespace

// ORIG ctor 0x082a87de：基类 vptr(0x8c81600) 后对 +0x0c/+0x24 各构造一次
// set<int>（_ZNSt3setIiSt4lessIiESaIiEEC1Ev）；+0x08 bool 不初始化。

// ORIG parse 0x084b0b08：trim(args)；args=="all" → +0x08=1 否则 0。
void CIndependentDrop::parse()
{
    trim(m_parser->m_args);
    if (m_parser->m_args == "all")
        ((GmDID*)this)->all = 1;
    else
        ((GmDID*)this)->all = 0;
}

// ORIG execute 0x084b0b4e。
// 【b28d-solo-GmCmds_D 判词 N82/N78/P215】p167：MISSING 0 / EXTRA 1
// （vector<stMonsterIndependentDrop_t>::C1Ev，P215：ORIG 4.1 对空默认 ctor 展开为
// 内联三指针清零，4.4 恒发 call）；gap=-5（616 vs 621）。语义闭合：4 个缓冲清零
// 按 ORIG 逐 buffer 形复现（lineBuf/chatMsg 简单 rep stos、content call memset、
// fileName 慢路）；hash_map/set/list/vector 迭代器 cv+后置增量、P183 去
// `it->second`/`getMonsterScript` 缓存、P49 宏化访问器均已归位。残余为 4.4 槽复用
// vs 4.1 独立槽的全帧槽号错位（N82）与 N78/P215 形差，封顶。
void CIndependentDrop::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return;
    }
    const unsigned short dungIdx = party->getDungIndex();    // ORIG 0x82a5a14 movzwl
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungIdx);
    if (dungeon == 0)
    {
        return;
    }

    // ORIG 逐 buffer 形不同（0x084b0bb6/0x084b0be6/0x084b0bf1/0x084b0c40）：
    // lineBuf/chatMsg 对齐 ⇒ 简单 rep stos（`={0}`）；content 95000B ⇒ call
    // memset；fileName[0x1e] 非对齐 ⇒ 内联慢路（P312）。
    char lineBuf[0x100] = {0};                               // -0x508 sprintf 行缓冲
    char content[95000];                                     // auStack_17c24 输出累计缓冲
    memset(content, 0, sizeof(content));
    char fileName[0x1e] = {0};                               // -0xc6
    char chatMsg[0x100] = {0};                               // -0x908

    GMD_IND_MONS(this).clear();
    GMD_IND_AIS(this).clear();

    if (!GMD_IND_ALL(this))
    {
        // 当前副本当前地图：roleType<4 → 普通怪集，4..8 → AI 集合。
        CBattle_Field* field =
            &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField;  // ORIG 内嵌 CBattle_Field
        MapInfo* curMap = field->GetCurrentMapInfo();        // ORIG 0x822cbcc
        for (std::map<int, map_monster>::iterator it = curMap->m_monsterMap.begin();
             it != curMap->m_monsterMap.end(); it++)
        {
            if (it->second.m_roleType < 4)
            {
                GMD_IND_MONS(this).insert(it->second.m_mobIndex);
            }
            else if (it->second.m_roleType < 9)
            {
                GMD_IND_AIS(this).insert(it->second.m_mobIndex);
            }
        }
    }
    else
    {
        // 全地图收集属于本副本的所有普通怪/AI idx。
        // ORIG 调 hash_map::begin() const / end() const ⇒ 原码经 const 指针访问。
        const CMapList* mapList =
            G_CDataManager()->m_mapList->GetMapList();       // ORIG this=datamgr+4
        const int di = dungeon->get_index();                 // ORIG CDungeon::get_index
        for (__gnu_cxx::hash_map<int, CMap*>::const_iterator it =
                 mapList->m_mainMap.begin();
             it != mapList->m_mainMap.end(); it++)
        {
            // ORIG 循环体 3 次独立 `it->second`（ptEv @0x084b0deb/0x084b0ea3/
            // 0x084b0f63，P183 禁指针缓存）⇒ 逐处表达式直读，无 map 局部。
            // ORIG list<int>::begin()/end() const + find<_List_const_iterator> ⇒
            // dungs 经 const 指针访问；守卫用正极性 `!=`（P220，消 operator==）。
            const std::list<int>* dungs = it->second->getDungeonLists();
            if (std::find(dungs->begin(), dungs->end(), di) != dungs->end())
            {
                std::list<_mapMonster> monsters =
                    it->second->get_monster_list();          // 按值返回（ORIG sret）
                for (std::list<_mapMonster>::iterator mit = monsters.begin();
                     mit != monsters.end(); mit++)
                {
                    GMD_IND_MONS(this).insert(mit->m_mobId); // insert 取首 int
                }
                const std::vector<STAICharacterMapArrangeData>* ais =
                    it->second->getAICharacters();
                for (std::vector<STAICharacterMapArrangeData>::const_iterator ait =
                         ais->begin();
                     ait != ais->end(); ait++)
                {
                    GMD_IND_AIS(this).insert(ait->m_type);   // insert 取首 int
                }
            }
        }
    }

    // ---- 普通怪独立掉落输出（script+0x3ac 表拷贝逐条 ResultToStr）----
    int count = 0;                                           // -0x34
    for (std::set<int>::const_iterator sit = GMD_IND_MONS(this).begin();
         sit != GMD_IND_MONS(this).end(); sit++)
    {
        CMonster* monster =
            static_cast<CMonster*>(G_CDataManager()->find_monster(*sit));
        if (monster == 0)
        {
            continue;
        }
        // ORIG 两次独立 `monster->getMonsterScript()`（0x084b10eb/0x084b1126，
        // P0/P183 无 script 局部缓存）。
        std::vector<stMonsterIndependentDrop_t> drops(
            static_cast<STMonsterScript*>(monster->getMonsterScript())
                ->m_independentDrops);
        if (!drops.empty())
        {
            sprintf(lineBuf, "%d\t%d\t%s\n", *sit, 0,
                    static_cast<STMonsterScript*>(monster->getMonsterScript())
                        ->m_name.c_str());
            strcat(content, lineBuf);
            GmD_CIndependentDrop_ResultToStr(this, drops, content);
            ++count;
        }
    }

    // ---- AI 角色独立掉落输出 ----
    for (std::set<int>::const_iterator sit = GMD_IND_AIS(this).begin();
         sit != GMD_IND_AIS(this).end(); sit++)
    {
        CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(// ORIG datamgr+0x869c
            static_cast<unsigned int>(*sit));
        if (ai == 0)
        {
            continue;
        }
        std::vector<stMonsterIndependentDrop_t> drops;
        _ZNK12CAICharacter19getIndepentDropItemEv(&drops, ai);
        if (!drops.empty())
        {
            sprintf(lineBuf, "%d\t%d\t%s\n", *sit, 1,
                    _ZNK12CAICharacter7GetNameEv(ai));
            strcat(content, lineBuf);
            GmD_CIndependentDrop_ResultToStr(this, drops, content);
            ++count;
        }
    }

    if (GMD_IND_ALL(this))
    {
        sprintf(fileName, "item_indepedent_drop_all_%s.txt",
                dungeon->GetDungeonName());
    }
    else
    {
        sprintf(fileName, "item_indepedent_drop_%s.txt", dungeon->GetDungeonName());
    }

    if (count == 0)
    {
        memcpy(chatMsg, kNoDropList, 0x24);
    }
    else
    {
        GmD_CIndependentDrop_printClient(this, GetUser(), fileName, content);
        sprintf(chatMsg, kFileCreatedFmt, fileName);
    }
    GmD_CIndependentDrop_SendChatMsg(this, GetUser(), chatMsg);
}

// ===========================================================================
// CIndependentDropByForce — ORIG ctor 0x082a8876 / parse 0x084b1f8c /
// execute 0x084b1fda。字段：+0x08 bool 强制掉落开关（POD，按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c815b8)。

// ORIG parse 0x084b1f8c：先清 0；trim(args)；args=="on" → 1。
void CIndependentDropByForce::parse()
{
    ((GmDFld<bool>*)this)->f08 = 0;
    trim(m_parser->m_args);
    if (m_parser->m_args == "on")
        ((GmDFld<bool>*)this)->f08 = 1;
    else
        ((GmDFld<bool>*)this)->f08 = 0;
}

// ORIG execute 0x084b1fda：回显开关状态并写入发起者 setForceDropFlag。
// 【b28d-solo-GmCmds_D 判词 AE】p167 CLEAR；字段 bool 视图 + 无 flag 槽表达式直读
// （P0/P73）、fall-through=on 块序（P47）已按 ORIG 复现。
void CIndependentDropByForce::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    // b28d-solo-GmCmds_D：ORIG 无 flag 槽（`movzbl 0x8(%eax),%eax; test %al,%al;
    // je <off>`）⇒ 表达式直读（P0）；块序 fall-through=on（P47/P191）。
    std::string msg;                                          // local_14
    if (((const GmDFld<bool>*)this)->f08)
    {
        msg = "independent drop item 100% on";
    }
    else
    {
        msg = "independent drop item 100% off";
    }
    PacketGuard packet;                                       // -0x1c
    G_GameWorld()->make_packet_chat_msg(packet, user,
                                        const_cast<char*>(msg.c_str()),
                                        static_cast<int>(msg.length()), 2,
                                        static_cast<ENUM_NOTIPACKET>(0xc));
    user->Send(packet);
    user->setForceDropFlag(((const GmDFld<bool>*)this)->f08);
}

// ===========================================================================
// CLevelUp — ORIG ctor 0x082a8956 / parse 0x084b388c / execute 0x084b38e2 /
// setLevel 0x084b395c / getLevel 0x084b398e。字段：+0x08 int 等级（POD）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c814f8)。

// ORIG parse 0x084b388c：parser 非空时 setLevel(StringToNumber(extra[1]))。
void CLevelUp::parse()
{
    if (m_parser != 0)
    {
        int level = static_cast<int>(                       // ORIG 中间变量 -0xc(%ebp)
            StringToNumber(m_parser->m_extra[1].c_str()));
        setLevel(level);
        return;                                             // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
}

// ORIG execute 0x084b38e2：栈构 DisPatcher_DebugCommand 后委托设级。
void CLevelUp::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    DisPatcher_DebugCommand dispatcher;                       // local_14
    dispatcher._debugCommandSetLevel(user, getLevel());
}

// ===========================================================================
// CHeroMissionCommand — ORIG ctor 0x082a89fe / parse 0x084b3db2 /
// execute 0x084b3e5e。字段：+0x08 string 子命令("set"/"reset")；
// +0x0c unsigned int missionNo；+0x10 unsigned int param（两个 POD 头未具名，
// 按偏移访问）。
// ===========================================================================

// ORIG ctor：基类 vptr(0x8c81480) + SsC1Ev 构造 +0x08。

// ORIG parse 0x084b3db2：parser 非空时 extra[1]→字符串，extra[2]/extra[3] 经
// StringToNumber 写 +0x0c/+0x10。
void CHeroMissionCommand::parse()
{
    if (m_parser != 0)
    {
        m_str0 = m_parser->m_extra[1];
        m_missionNo = static_cast<unsigned int>(
            StringToNumber(m_parser->m_extra[2].c_str()));
        m_param = static_cast<unsigned int>(
            StringToNumber(m_parser->m_extra[3].c_str()));
        return;                                             // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
}

// ORIG execute 0x084b3e5e："set"→processMission，"reset"→setEventData("")，
// 最后统一 sendMissionData 回发任务数据。
// 【b28d-solo-GmCmds_D 判词 AE】旧 N18a 判词作废：ORIG 两处
// `mov 0x10/0xc(%eax),%eax; mov %eax,%esi/%ebx` 两段装载经「enum 字段视图 +
// 裸 register 局部」复现（P254 档③：4.1 对 enum 字段保留 eax→callee-saved 中转；
// 4.4 对 unsigned 字段恒折单段）。探针 /tmp/b28d_gmd/p6.cpp g10 全等。
void CHeroMissionCommand::execute()
{
    CUser* user = GetUser();
    // ORIG P48：O0 无 CSE，三个调用表达式各含一次 Singleton::Get()（共 3 call），
    // 禁函数顶缓存 mission。set 分支先按右→左读 m_param(+0x10)/m_missionNo(+0xc)
    // 入 callee-saved 跨 Get() 保活（P87 形），再 call Get 作 receiver。
    if (m_str0 == "set")
    {
        // b28d-solo-GmCmds_D：ORIG 两处 `mov 0x10/0xc(%eax),%eax; mov %eax,%esi/%ebx`
        // 两段装载（4.1 对 enum 字段保留 eax→callee-saved 中转）⇒ 经 enum 字段
        // 视图 + 裸 register 局部复现（P254/P231；探针 /tmp/b28d_gmd/p6.cpp g10 全等）。
        register unsigned int param = static_cast<unsigned int>(
            ((const GmDHeroMissionCmdView*)this)->param);
        register unsigned int no = static_cast<unsigned int>(
            ((const GmDHeroMissionCmdView*)this)->no);
        ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
            user, static_cast<HeroMissionCondition::MissionNo::T>(no), param);
    }
    else if (m_str0 == "reset")
    {
        ARAD::Singleton<HeroMissionEvent>::Get()->setEventData(user, "");
    }
    ARAD::Singleton<HeroMissionEvent>::Get()->sendMissionData(user);
}

// （namespace game_master 于文件尾闭合——asm-label 实现体同属该命名空间）

// ---------------------------------------------------------------------------
// 本 TU 提供的 ORIG mangled 符号实体（asm-label 实现体；供 compare_tu 对比）。
// ---------------------------------------------------------------------------

// ORIG ResultToStr 0x084b165c：逐条拷贝 stMonsterIndependentDrop_t，
// 按 copy+0x2c（m_rate[10] 主掉率）非 0 决定 PrintItemDropList 详细模式。
// 【b28d-solo-GmCmds_D 判词 AE】p167 CLEAR（11=11）：循环前缓存 begin/end（P13）、
// `else if (copy.m_rate[10] != 0)` 二次 test/je（P184 死块照抄）均按 ORIG 复现。
void GmD_CIndependentDrop_ResultToStr(
    CIndependentDrop* self,
    const std::vector<stMonsterIndependentDrop_t>& drops, char* out)
{
    // b28d-solo-GmCmds_D：ORIG 在循环前一次性求值 begin(-0xc)/end(-0x10)（P13
    // end 缓存），4.4 对 `it != drops.end()` 恒每次循环重调 end() ⇒ 手动缓存 end。
    std::vector<stMonsterIndependentDrop_t>::const_iterator it = drops.begin();
    std::vector<stMonsterIndependentDrop_t>::const_iterator last = drops.end();
    for (; it != last; ++it)
    {
        stMonsterIndependentDrop_t copy(*it);                // ORIG copy ctor 0x8376bb2
        if (copy.m_rate[10] == 0)
        {
            GmD_CIndependentDrop_PrintItemDropList(self, copy, out, false);
        }
        else if (copy.m_rate[10] != 0)                      // ORIG 二次 test/je（P184）
        {
            GmD_CIndependentDrop_PrintItemDropList(self, copy, out, true);
        }
    }
}

// ORIG PrintItemDropList 0x084b1756：单条独立掉落统计文本追加到 out。
// 【b28d-solo-GmCmds_D 判词 N78/N82/P167】`={0}` rep stos 替代 ORIG 的内联
// memset 慢路（N78）；`if(!detail)` 反形（P168a②）、rate 第一次内联/第二次具名
// float（P252）、内层 begin/end 缓存（P13）、segRate 内联均已按 ORIG 复现。
// p167：EXTRA operator-> ×2 / MISSING operator* ×1（ORIG 单次 `*pit` + 内联 pair
// 拷贝；改用 pair 局部虽 p167 CLEAR 但会丢 ORIG 存在的 operator-> 弱符号
// 0x08193912 使 TU identical −1，故取当前形）。残差另含全帧槽号错位（N82）。
void GmD_CIndependentDrop_PrintItemDropList(
    CIndependentDrop* self, const stMonsterIndependentDrop_t& drop,
    char* out, bool detail)
{
    (void)self;
    // b28d-solo-GmCmds_D：ORIG 用内联 rep stos 清零（P214③/P253）⇒ `={0}` 形。
    char lineBuf[0x400] = {0};                               // -0x42c

    // ORIG `movzbl -0x43c,%eax; xor $1; test %al,%al; je <memcpy 块>`（P168a②）⇒
    // 源码为 `if (!detail) { find } else { memcpy }`。
    if (!detail)
    {
        CItem* item = G_CDataManager()->find_item(drop.m_itemIdx);
        if (item != 0)
        {
            sprintf(lineBuf, "%d\t%s\n", drop.m_itemIdx, item->GetItemName());
            strcat(out, lineBuf);
        }
    }
    else
    {
        memcpy(lineBuf, kListDrop, 0xd);
        strcat(out, lineBuf);
    }

    for (int i = 0; i < 5; ++i)
    {
        if (drop.m_rate[i] == 0)
        {
            continue;
        }
        sprintf(lineBuf, kDifficultyFmt, i);
        strcat(out, lineBuf);

        // ORIG 第一次 rate 无 float 槽（fildl/flds/fdivrp/fstpl 直落实参）⇒ 内联
        // 表达式；第二次 `fstps -0x14` 具名 float 局部供 4 次乘法（P252）。
        sprintf(lineBuf, kDropRateFmt,
                static_cast<double>(
                    static_cast<float>(drop.m_rate[i]) / 10000.0f));
        strcat(out, lineBuf);

        sprintf(lineBuf, kTryCountFmt, drop.m_rate[5], drop.m_rate[6],
                drop.m_rate[7], drop.m_rate[8]);
        strcat(out, lineBuf);

        const float rate =
            static_cast<float>(drop.m_rate[i]) / 10000.0f;   // ORIG fstps -0x14
        sprintf(lineBuf, kTotalDropRateFmt,
                static_cast<double>(static_cast<float>(drop.m_rate[5]) * rate),
                static_cast<double>(static_cast<float>(drop.m_rate[6]) * rate),
                static_cast<double>(static_cast<float>(drop.m_rate[7]) * rate),
                static_cast<double>(static_cast<float>(drop.m_rate[8]) * rate));
        strcat(out, lineBuf);

        sprintf(lineBuf, kTotalCountFmt, drop.m_rate[9]);
        strcat(out, lineBuf);

        if (detail)
        {
            memcpy(lineBuf, kItemList, 0xf);
            strcat(out, lineBuf);

            int prevAcc = 0;                                 // -0x10
            // ORIG 循环前一次性求值 begin(-0x24)/end(-0x20)（P13）。
            std::vector<std::pair<int, int> >::const_iterator pit =
                drop.m_pairList.begin();
            std::vector<std::pair<int, int> >::const_iterator pend =
                drop.m_pairList.end();
            for (; pit != pend; ++pit)
            {
                const int itemId = pit->first;
                const int acc = pit->second;
                CItem* item = G_CDataManager()->find_item(itemId);
                if (item != 0)
                {
                    // ORIG segRate 无槽（fildl/flds/fdivrp 直落 sprintf 实参）。
                    sprintf(lineBuf, "%d\t%f\t%s\n", itemId,
                            static_cast<double>(
                                static_cast<float>(acc - prevAcc) / 10000.0f),
                            item->GetItemName());
                    strcat(out, lineBuf);
                    prevAcc = acc;
                }
            }
            prevAcc = 0;
        }
    }
}

// ORIG printClient 0x084b14b8：put_header(0,0x13e)+byte(1)+两段 int/str 包发送。
void GmD_CIndependentDrop_printClient(CIndependentDrop* self, CUser* user,
                                      const char* fileName,
                                      const char* contents)
{
    (void)self;
    PacketGuard packet;
    packet.put_header(0, 0x13e);
    packet.put_byte(1);
    packet.put_int(static_cast<int>(strlen(fileName)));
    packet.put_str(fileName, static_cast<int>(strlen(fileName)));
    packet.put_int(static_cast<int>(strlen(contents)));
    packet.put_str(contents, static_cast<int>(strlen(contents)));
    packet.finalize(true);
    user->Send(packet);
}

// ORIG SendChatMsg 0x084b15c4：聊天包(type=2, notify=0xc) 发起者私聊回显。
void GmD_CIndependentDrop_SendChatMsg(CIndependentDrop* self, CUser* user,
                                      char* msg)
{
    (void)self;
    PacketGuard packet;
    G_GameWorld()->make_packet_chat_msg(packet, user, msg,
                                        static_cast<int>(strlen(msg)), 2,
                                        static_cast<ENUM_NOTIPACKET>(0xc));
    user->Send(packet);
}

// ORIG setLevel 0x084b395c：>70 截 70；<=0 归 1；其余原值。
void CLevelUp::setLevel(int level)
{
    if (!(level < 71))                                       // cmpl $0x46; jle
    {
        m_level = 70;
    }
    else if (level < 1)
    {
        m_level = 1;
    }
    else
    {
        m_level = level;
    }
}

// ORIG getLevel 0x084b398e。
int CLevelUp::getLevel()
{
    return m_level;
}

} // namespace game_master