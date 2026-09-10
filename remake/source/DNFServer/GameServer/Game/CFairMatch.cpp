// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFairMatch/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <string.h>

#include "CFairMatch.h"

#include "CDataManager.h"    // G_CDataManager / find_item / m_serverParameter
#include "CUser.h"           // CUser::GetRating / Send / CUserCharacInfo getters
#include "CEquipItem.h"      // CEquipItem::GetUsableEquipmentType
#include "Inven_Item.h"

namespace fair_pvp
{

// ---------------------------------------------------------------------------
// 静态数据成员
// ---------------------------------------------------------------------------

/* ORIG B 0x943e148 */
CMultiMatchRoomList* CFairMatch::pWaitingRoomList_ = 0;
/* ORIG B 0x943e14c */
int CFairMatch::nWaitingRoomListRefCount_ = 0;
/* ORIG D 0x93abb76（.data 初值 01） */
bool CFairMatch::bMatchable_ = true;

// ---------------------------------------------------------------------------
// fair_pvp::PlayerInfoThird
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// fair_pvp::TeamInfoThird
// ---------------------------------------------------------------------------

// ORIG C1/C2 0x85668ff0（W）：仅对 4 个 PlayerInfoThird 子对象做默认构造
// （esi=3 递减循环 call PlayerInfoThird::C1）；不动 m_field00/m_field04
// （由 reset 显式置值）。编译器自动为 m_players[4] 逐个调用 PlayerInfoThird()。
#include "CFairMatch/TeamInfoThird.inc"
{
}

// ORIG reset 0x8562582：清玩家数组后 m_field00=0、m_field04=2。
// reset 置于 PlayerInfoThird::reset 定义前：callee 未知时 4.4 outgoing 保守
// 分配恰为 ORIG 帧 0x28（定义在后则继承 callee 小帧 0x14，槽号连带漂移）。
#include "CFairMatch/reset.inc"
{
    for (int i = 0; i < 4; ++i)
        m_players[i].reset();
    m_field00 = 0;
    m_field04 = 2;
}

// ORIG 仅清 +0x00/+0x04(1B)/+0x08/+0x0c；+0x05 的 CharacterKind m_kind 不初始化。
#include "CFairMatch/reset_1.inc"
{
    m_user = 0;
    m_flag = 0;
    m_pvpGrade = 0;
    m_rating = 0.0f;
}

// ORIG add 0x85625c6：首个空位（m_flag==0）写入；满返 0，成功返 1。
// [b28d-solo-CFairMatch] P248 档②（bool 字段 + 裸 if(!field) ⇒ `movzbl; xor $1;
// test; je`）+ P157/P183（禁 slot 引用缓存：ORIG 每字段重算 i*0x10 全链）。
// 残余 N18a（1 条）：ORIG 0x08562638 段 getCurCharacGrowType 结果先 `mov %eax,%edx`
// 再 `mov %dl,0xe(%ecx,%eax,1)`（this 装 %eax）；4.4 折单段（this 装 %edx、值留
// %al）。源侧 12 形穷举全折叠（无 cast / (unsigned char) / (unsigned char)(unsigned
// int) / (unsigned char)(int) / (int) RHS / (unsigned int) RHS / (unsigned short) /
// &0xff / *1u / +0 / 裸语句表达式 / 具名语句表达式[劣化75]），callee 侧旋钮需改
// CUserCharacInfo 返回类型（跨 .cpp，单文件铁律外）⇒ 已上报 Main。
#include "CFairMatch/add.inc"
{
    for (int i = 0; i < 4; ++i)
    {
        if (!m_players[i].m_flag)
        {
            m_players[i].m_flag = true;
            m_players[i].m_user = user;
            m_players[i].m_kind.m_job = (unsigned char)user->get_charac_job();
            m_players[i].m_kind.m_grow = user->getCurCharacGrowType();
            m_players[i].m_pvpGrade = user->get_pvp_grade();
            m_players[i].m_rating = user->GetRating();
            return 1;
        }
    }
    return 0;
}

// ORIG del 0x85626aa：命中占用且指针相等者清 flag，返 1；否则返 0。
#include "CFairMatch/del.inc"
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0 && m_players[i].m_user == user)
        {
            m_players[i].m_flag = 0;
            return 1;
        }
    }
    return 0;
}

// ORIG refresh 0x85624de：重算每人 rating（未占用/空指针者清零）；
// 之后与 reset 相同置 m_field00=0、m_field04=2。
#include "CFairMatch/refresh.inc"
{
    int deadSlot = 0;  // ORIG -0x10 死槽（0x85624e5 movl $0x0 初始化后未读）
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0 && m_players[i].m_user != 0)
            m_players[i].m_rating = m_players[i].m_user->GetRating();
        else
            m_players[i].m_rating = 0.0f;
    }
    m_field00 = 0;
    m_field04 = 2;
}

// ORIG size 0x856270c（const）：统计占用槽位数。
#include "CFairMatch/size.inc"
{
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0)
            ++count;
    }
    return count;
}

// ORIG SendToTeam 0x856274c：向全员 CUser::Send(PacketGuard&)。
#include "CFairMatch/SendToTeam.inc"
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0 && m_players[i].m_user != 0)
            m_players[i].m_user->Send(pkt);
    }
}

// ORIG GetTeamRating 0x85627b2：非零 rating 求平均，全零返 0。
// [b28 判词·N85] AE-ndiff≈4：唯一真差 = `rating != 0.0f` 的 x87 比较 4.1 发
// `fucompp; fnstsw %ax; sahf`（3 条）vs 4.4 恒 `fucomip; fstp`（2 条），负例 N85
// 「x87 比较两向跨版本」定案（GetResult/BuyAuctionItem 族）；源形不可择。
// 其余 43/43 逐条同（含 mov $0 整型零初始化 sum、fildl/fdivp 平均段、-0x14 中转槽）。
// [b28d-solo-CFairMatch] N85→AE | x87 比较 lowering：ORIG `fucompp; fnstsw %ax; sahf`
// vs 4.4.7 `fucomip; fstp`（0x085627fa 段），其余 43/43 逐条同。本机实测
// -march=i486 -mtune=generic 使该件归零（GCC 4.4 无 x86 函数级 target 旋钮，
// `#pragma GCC target("i486")` 实测报 attribute 未知）⇒ 已上报 Main；Main 已将
// *Game/CFairMatch.cpp 加入 i486 路由组（dnf_game_cxx.sh L355，P230 补先例），
// check_tu_routed 复验该件 AE（TU ae 24→25 / diff 3→2）。
#include "CFairMatch/GetTeamRating.inc"
{
    float sum = 0.0f;
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        sum += m_players[i].m_rating;
        if (m_players[i].m_rating != 0.0f)
            ++count;
    }
    if (count != 0)
        sum = sum / (float)count;
    return sum;
}

// ---------------------------------------------------------------------------
// fair_pvp::CFairMatch 生命周期
// ---------------------------------------------------------------------------

// ORIG C1/C2 0x856284a：IMatch 基类、PVP_ROOM_INFO@+8 与 TeamInfoThird[2]@+0x1c
// 子对象由默认初始化完成（vptr 同步）；随后延迟单例化 CMultiMatchRoomList 并
// 引用计数++；最后 m_fieldC0=0。
#include "CFairMatch/CFairMatch.inc"
{
    if (pWaitingRoomList_ == 0)
    {
        pWaitingRoomList_ = new CMultiMatchRoomList();
    }
    ++nWaitingRoomListRefCount_;
    m_fieldC0 = 0;
}

// ORIG D1/D2 0x856292e / D0 0x85629ca。
// 体：引用计数--并在归零且单例仍存活时 delete；m_fieldC0=0。
// （ORIG 尾部 operator delete(this) 仅出现在 D0，由编译器按虚析构语义生成。）
#include "CFairMatch/dtor_CFairMatch.inc"
{
    --nWaitingRoomListRefCount_;
    if (pWaitingRoomList_ != 0 && nWaitingRoomListRefCount_ == 0)
    {
        ::operator delete(pWaitingRoomList_);
    }
    m_fieldC0 = 0;
}

// ---------------------------------------------------------------------------
// IMatch 覆盖（本批符号）
// ---------------------------------------------------------------------------

// ORIG GetUserList 0x8565184：内联 rep movsl（18 dword）拷贝第一支队伍
// （m_teams[0]，0x48 字节）到出参。
// [b28 P83] 4.1 把常量 memcpy 内联成 rep movsl 而 4.4 恒 call memcpy（N84）；
// 改结构整体赋值 `*(TeamInfoThird*)out = m_teams[0]` 触发 4.4 发射同形
// rep movsl 三件套缓存（探针 /tmp/l2_p5.cpp z2：dst→edi/src→esi/count→ecx，
// 与 ORIG `mov $0x12,%eax; lea 0x1c(%eax),%ebx` 结构一致）。语义不变。
#include "CFairMatch/GetUserList.inc"
{
    *reinterpret_cast<TeamInfoThird*>(out) = m_teams[0];
}

// ORIG RequestMap 0x8562bae：mapNo=0；mode!=6 时取 CDataManager::GetPvPMode。
#include "CFairMatch/RequestMap.inc"
{
    mapNo = 0;
    if ((int)mode != 6)
    {
        mode = (PVP_BATTLE_MODE)G_CDataManager()->GetPvPMode();
    }
}

// ORIG OnStartFight 0x8564952：状态机 +4 置 5（对战中）。
#include "CFairMatch/OnStartFight.inc"
{
    m_field04 = 5;
}

// ORIG OnEndPvP 0x8565c4e：纯 ret。
#include "CFairMatch/OnEndPvP.inc"
{
}

// ORIG CheckSeat 0x8562b78。
#include "CFairMatch/CheckSeat.inc"
{
    if (a == 2)
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// 非 IMatch worklist 方法（本批符号）
// ---------------------------------------------------------------------------

// ORIG SetMatchAble 0x8562b96。
#include "CFairMatch/SetMatchAble.inc"
{
    bMatchable_ = able;
}

// ORIG SetPlayCountPerOneTeam 0x8562b90：空体 ret。
#include "CFairMatch/SetPlayCountPerOneTeam.inc"
{
}

// ORIG CheckUnusableEquipment 0x8562bdc：
//   空 item→0；item->m_addInfo==0→1；find_item 失败→0；
//   可堆叠→0；equipment type==0→1；
//   CheckUnusableEquipmentInfo(type, usableLevel)==1 时再以
//   vtable+0xc（GetItemType）与 get_index 查 CheckUnusableItem 判定；
//   其余返回 0。
// [b28d-solo-CFairMatch] N7 | 参数槽已修（static，P245）；残余两处纯寄存器角色：
// ①CheckUnusableEquipmentInfo 的 arg3→%esi / arg2→%ebx（ORIG）vs arg3→%ebx /
// arg2→%esi（4.4）；②间接虚调目标 ORIG 用 %edx vs 4.4 用 %eax（0x08562cc8 段）。
// 本机双后端实测均不可治：4.4.7 与 4.4.6（-B cc1plus446bin，含 4.4.6 头集）读数
// 完全相同（identical=37 ae=24 near=1 diff=3）。虚槽 +0xc 已按 P193/P282 核对
// _ZTV10CEquipItem vptr+0xc = CEquipItem::GetItemType 0x08514d26（语义正确）。
#include "CFairMatch/CheckUnusableEquipment.inc"
{
    if (item == 0)
        return 0;

    // ORIG *(int*)(param_1 + 2)：+0x02 m_addInfo（item idx）
    // P0/P48：ORIG 无 itemIdx 槽往返——条件与 find_item 实参各独立读一次
    //（0x08562bxx 两处 `mov 0x8(%ebp),%eax; mov 0x2(%eax),%eax`）。
    if (item->m_addInfo == 0)
        return 1;

    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（find_item /
    // CheckUnusableEquipmentInfo / CheckUnusableItem 共 3 次）；equipType 也
    // 不在比较与实参间缓存（GetUsableEquipmentType 共 2 次）。
    CItem* found = G_CDataManager()->find_item(item->m_addInfo);
    if (found == 0)
        return 0;

    // P59/P222 正极性（ORIG 0x8562c1x `call is_stackable; test %al; je` 直落
    // 主流程；负极性多 xor $1）；P0：无 equip 具名局部，found 表达式直用。
    if (found->is_stackable())
        return 0;

    if (static_cast<CEquipItem*>(found)->GetUsableEquipmentType() == 0)
        return 1;

    // P168a 裸形（ORIG 0x08562c9d/0x08562ce4 `xor $1; test %al; je`）：
    // bool 返回 callee 用 `if (!call()) return 0;`。
    if (!G_CDataManager()->m_serverParameter.CheckUnusableEquipmentInfo(
            (ENUM_UNUSABLE_EQUIPMENT_TYPE)static_cast<CEquipItem*>(found)->GetUsableEquipmentType(),
            found->getUsableLevel()))
        return 0;
    // ORIG vtable 槽 +0xc：CItem::GetItemType()
    if (!G_CDataManager()->m_serverParameter.CheckUnusableItem(
            (ENUM_EQUIPMENTTYPE)const_cast<const CItem*>(found)->GetItemType(),
            (unsigned long)found->get_index()))
        return 0;
    return 1;
}

// ORIG _GetOppositeTeam 0x8564f8c：1↔2，其余原样返回。
#include "CFairMatch/_GetOppositeTeam.inc"
{
    switch (team)
    {
    case 1:
        return 2;
    case 2:
        return 1;
    default:
        return team;
    }
}

// ORIG _GetOtherTeam 0x8566476：0↔1，否则 2。
#include "CFairMatch/_GetOtherTeam.inc"
{
    if (team == (ENUM_SCHOOL_MATCH_TEAM)0)
        return (ENUM_SCHOOL_MATCH_TEAM)1;
    if (team == (ENUM_SCHOOL_MATCH_TEAM)1)
        return (ENUM_SCHOOL_MATCH_TEAM)0;
    return (ENUM_SCHOOL_MATCH_TEAM)2;
}

// ORIG W 0x08234f64：movzbl 0x5b 直返（m_giveItem 字节）。
// [B24D判词 N] 语义正确：return m_giveItem（movzbl 0x5b 直返）。diff=ORIG GCC4.1
//   冗余 `movzbl %al,%eax` 双零扩展 vs 我方单 movzbl（4.4 已优化），7/6 纯 lowering
//   表示差异，语义等价。报告 NO 由 nm/objdump diff 兜底。
#include "CFairMatch/GetGiveItem.inc"
{
    return m_giveItem;
}

}  // namespace fair_pvp
