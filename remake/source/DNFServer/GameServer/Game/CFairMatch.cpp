// ============================================================================
// df_game_r 还原 —— fair_pvp::CFairMatch 生命周期 + 轻量组 &
//            fair_pvp::TeamInfoThird 全方法
// 权威头：CFairMatch.h（勿在此重复类定义）。
// 报告依据 docs/class_func_reports/fair_pvp__CFairMatch/*.md 与
//          docs/class_func_reports/fair_pvp__TeamInfoThird/*.md。
// 静态数据成员（ORIG nm）：
//   pWaitingRoomList_          B 0x943e148
//   nWaitingRoomListRefCount_  B 0x943e14c
//   bMatchable_                D 0x93abb76（初值 1，objdump -s 实证）
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

// ORIG 仅清 +0x00/+0x04(1B)/+0x08/+0x0c；+0x05 m_job、+0x06 m_growType 不初始化。
void PlayerInfoThird::reset()
{
    m_user = 0;
    m_flag = 0;
    m_pvpGrade = 0;
    m_rating = 0.0f;
}

// ---------------------------------------------------------------------------
// fair_pvp::TeamInfoThird
// ---------------------------------------------------------------------------

// ORIG C1/C2 0x85668ff0（W）：仅对 4 个 PlayerInfoThird 子对象做默认构造
// （esi=3 递减循环 call PlayerInfoThird::C1）；不动 m_field00/m_field04
// （由 reset 显式置值）。编译器自动为 m_players[4] 逐个调用 PlayerInfoThird()。
TeamInfoThird::TeamInfoThird()
{
}

// ORIG reset 0x8562582：清玩家数组后 m_field00=0、m_field04=2。
void TeamInfoThird::reset()
{
    for (int i = 0; i < 4; ++i)
        m_players[i].reset();
    m_field00 = 0;
    m_field04 = 2;
}

// ORIG add 0x85625c6：首个空位（m_flag==0）写入；满返 0，成功返 1。
int TeamInfoThird::add(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0)
            continue;
        PlayerInfoThird& slot = m_players[i];
        slot.m_flag = 1;
        slot.m_user = user;
        slot.m_job = (unsigned char)user->get_charac_job();
        slot.m_growType = user->getCurCharacGrowType();
        slot.m_pvpGrade = user->get_pvp_grade();
        slot.m_rating = user->GetRating();
        return 1;
    }
    return 0;
}

// ORIG del 0x85626aa：命中占用且指针相等者清 flag，返 1；否则返 0。
int TeamInfoThird::del(CUser* user)
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
void TeamInfoThird::refresh()
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag == 0 || m_players[i].m_user == 0)
            m_players[i].m_rating = 0.0f;
        else
            m_players[i].m_rating = m_players[i].m_user->GetRating();
    }
    m_field00 = 0;
    m_field04 = 2;
}

// ORIG size 0x856270c（const）：统计占用槽位数。
int TeamInfoThird::size() const
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
void TeamInfoThird::SendToTeam(PacketGuard& pkt)
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0 && m_players[i].m_user != 0)
            m_players[i].m_user->Send(pkt);
    }
}

// ORIG GetTeamRating 0x85627b2：非零 rating 求平均，全零返 0。
float TeamInfoThird::GetTeamRating()
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
CFairMatch::CFairMatch()
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
CFairMatch::~CFairMatch()
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

// ORIG GetUserList 0x8565184：memcpy 第一支队伍（m_teams[0]，0x48 字节）到出参。
void CFairMatch::GetUserList(void* out)
{
    memcpy(out, &m_teams[0], sizeof(TeamInfoThird));
}

// ORIG RequestMap 0x8562bae：mapNo=0；mode!=6 时取 CDataManager::GetPvPMode。
void CFairMatch::RequestMap(short& mapNo, PVP_BATTLE_MODE& mode)
{
    mapNo = 0;
    if ((int)mode != 6)
    {
        mode = (PVP_BATTLE_MODE)G_CDataManager()->GetPvPMode();
    }
}

// ORIG OnStartFight 0x8564952：状态机 +4 置 5（对战中）。
void CFairMatch::OnStartFight()
{
    m_field04 = 5;
}

// ORIG OnEndPvP 0x8565c4e：纯 ret。
void CFairMatch::OnEndPvP(PvP_Room*)
{
}

// ORIG CheckSeat 0x8562b78。
int CFairMatch::CheckSeat(int a)
{
    if (a == 2)
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// 非 IMatch worklist 方法（本批符号）
// ---------------------------------------------------------------------------

// ORIG SetMatchAble 0x8562b96。
void CFairMatch::SetMatchAble(bool able)
{
    bMatchable_ = able;
}

// ORIG SetPlayCountPerOneTeam 0x8562b90：空体 ret。
void CFairMatch::SetPlayCountPerOneTeam(unsigned int)
{
}

// ORIG CheckUnusableEquipment 0x8562bdc：
//   空 item→0；item->m_addInfo==0→1；find_item 失败→0；
//   可堆叠→0；equipment type==0→1；
//   CheckUnusableEquipmentInfo(type, usableLevel)==1 时再以
//   vtable+0xc（GetItemType）与 get_index 查 CheckUnusableItem 判定；
//   其余返回 0。
int CFairMatch::CheckUnusableEquipment(const Inven_Item* item)
{
    if (item == 0)
        return 0;

    CDataManager& dm = *G_CDataManager();

    // ORIG *(int*)(param_1 + 2)：+0x02 m_addInfo（item idx）
    int itemIdx = item->m_addInfo;
    if (itemIdx == 0)
        return 1;

    CItem* found = dm.find_item(itemIdx);
    if (found == 0)
        return 0;

    if (!found->is_stackable())
    {
        CEquipItem* equip = static_cast<CEquipItem*>(found);
        int equipType = equip->GetUsableEquipmentType();
        if (equipType == 0)
            return 1;

        if (dm.m_serverParameter.CheckUnusableEquipmentInfo(
                (ENUM_UNUSABLE_EQUIPMENT_TYPE)equipType, found->getUsableLevel()))
        {
            // ORIG vtable 槽 +0xc：CItem::GetItemType()
            int itemType = const_cast<const CItem*>(found)->GetItemType();
            return dm.m_serverParameter.CheckUnusableItem(
                       (ENUM_EQUIPMENTTYPE)itemType,
                       (unsigned long)found->get_index()) ? 1 : 0;
        }
        return 0;
    }
    return 0;
}

// ORIG _GetOppositeTeam 0x8564f8c：1↔2，其余原样返回。
int CFairMatch::_GetOppositeTeam(int team)
{
    if (team == 1)
        return 2;
    if (team == 2)
        return 1;
    return team;
}

// ORIG _GetOtherTeam 0x8566476：0↔1，否则 2。
ENUM_SCHOOL_MATCH_TEAM CFairMatch::_GetOtherTeam(ENUM_SCHOOL_MATCH_TEAM team)
{
    switch (team)
    {
    case 0:
        return (ENUM_SCHOOL_MATCH_TEAM)1;
    case 1:
        return (ENUM_SCHOOL_MATCH_TEAM)0;
    default:
        return (ENUM_SCHOOL_MATCH_TEAM)2;
    }
}

// ORIG W 0x08234f64：movzbl 0x5b 直返（m_giveItem 字节）。
unsigned char CFairPvPScore::GetGiveItem() const
{
    return m_giveItem;
}

}  // namespace fair_pvp
