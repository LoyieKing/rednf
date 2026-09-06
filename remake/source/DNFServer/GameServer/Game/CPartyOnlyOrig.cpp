// ============================================================================
// CPartyOnlyOrig.cpp —— CParty「仅 ORIG」批次（Wave5 OnlyOrig_APSEJ 分域，
// worklist /tmp/only_orig/CParty.txt 47 符号，2026-08-27）
//
// 全部函数逐份完整读取 docs/class_func_reports/CParty/<Func>.md（## 汇编 +
// ## 反编译 C）转写；无独立报告的叶子依赖按 ORIG nm/objdump 转写并标注。
//
// thunk 解析（报告汇编中被 strip 的 W 内联符号，经 df_game_r nm 核实）：
//   0x080da37c CUser::get_unique_id() const
//   0x080da27e CUserCharacInfo::getCurCharacInvenR() const
//   0x080da2b8 CUserCharacInfo::get_charac_level() const
//   0x080da2da CUserCharacInfo::IsCurCharacGhost()
//   0x08110dc2 CUser::GetPCRoomNo()
//   0x0819a8a6 CUser::getCurCharacQuestR() const
//   0x0822d0d8 CBattle_Field::get_dungeon_index()
//   0x080f981c CBattle_Field::get_dungeon_diff()
//   0x085bf3d8 CPartyResultRecvFlag::SetRecvFlag(int)
//   0x085bf406 CPartyResultRecvFlag::IsRecv(int) const
//   0x085bf6e0 CParty::reset_recv_flag()
// 依赖唯一 owner（链接期解析，本 TU 不重复定义）：
//   CBattle_Field::{kill_blood_monster,DestroyPassiveObject} —— CBattle_Field.h
//     已声明（BF 域批次落地定义）；CUser::Send(PacketGuard&)（CUser 域批次）；
//     CUser::CheckLimitMiniumAge(int,int&)（PartyUserLink 批次）。
// ============================================================================
#include "CParty.h"

#include "CDungeon.h"              // m_entryRequiredItem/get_dimension_possible/
                                   //   getLimitCoinDiff/m_87c/m_898
#include "CBattle_Field.h"         // BF 视图转发（先例：CParty.cpp checkBossRoom）
#include "CDataManager.h"          // G_CDataManager()/find_dungeon/get_dimensionInout
#include "CInventory.h"            // check_item_exist/GetInvenSlot
#include "PacketGuard.h"            // PacketGuard（InterfacePacketBuf 子类）
#include "TimerQueue.h"             // G_TimerQueue/InsertTimer/gen_timer_key 支持
#include "CGuildServerProxy.h"     // CEventBase/CEventManager::GetRepeatEvent→IsEventing
#include "Quest.h"                 // m_index/m_triggerType/m_triggerSubType
// HistoryLog::WriteUseCoin 以本 TU 最小视图接入（先例 WarRoom.cpp：权威
// HistoryLog.h 经 CGameManager.h 的 include 链与本 TU 冲突；mangled 一致）。
class HistoryLog
{
public:
    static void WriteUseCoin(_IO_FILE* file, const char* reason);
};
#include "cMyTraceNoop.h"
#include "DataManagerInitDepends.h" // TOD_UserState/TOD_Layer

namespace
{

// +0xb14 BattleData 逐成员已用硬币数组具名成员视图。
inline int* memberUsedCoin(CParty* party)
{
    return party->m_battleData.m_arr7e8;
}

}  // namespace

// ============================================================================
// P2P 连接管理区
// ============================================================================

// ORIG T 0x0859a48c _ZN6CParty10ConnectP2PEP5CUserPci（ConnectP2P.md）。
// 队伍状态必须为 1（入队/出发态），否则抛 ENUM_ERROR(0x13)；
// 目标成员 IP 缓冲（cMember+0x0e 起 4 字节）复制 min(port,4) 字节并把
// cMember+0x0c 置 1；全员连接完成后清坏 P2P 成员。ip 参数仅作 memcpy 源
// （ORIG 未读其长度，端口参数被 min 截断使用【忠实转写】）。
void CParty::ConnectP2P(CUser* user, char* ip, int port)
{
    if (m_state != 1)
    {
        throw static_cast<ENUM_ERROR>(0x13);
    }

    bool allConnected = true;
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i))
        {
            if (m_member[i].m_pUser == user)
            {
                if (m_member[i].m_fieldc == 1)
                {
                    throw static_cast<ENUM_ERROR>(0x12);
                }
                m_member[i].m_fieldc = 1;
                int copyLen = port < 4 ? port : 4;
                memcpy(m_member[i].m_fielde, ip, copyLen);
            }
            if (m_member[i].m_fieldc != 1)
            {
                allConnected = false;
            }
        }
    }
    if (allConnected)
    {
        ClearBadP2PUser();
    }
}

// ORIG T 0x0859a664（_ClearConnectP2PAbsolute.md）：除 slot 外全员中与该槽
// P2P 不通的逐个以 ENUM_PARTY_INFO_TYPE 2 离队；恒返回 1。
int CParty::_ClearConnectP2PAbsolute(int slot)   // [P9] ORIG 尾返 1
{
    for (int i = 0; i < 4; i++)
    {
        // [拆&&] ORIG 双段直分支，&& 合一 4.4 出 5 行物化岛。
        if (slot != i)
        {
            if (_IsCannotConnectP2P(slot, i))
            {
                leave_user(m_member[i].m_pUser,
                           static_cast<ENUM_PARTY_INFO_TYPE>(2));
            }
        }
    }
    return 1;
}

// ORIG T 0x0859a6dc（_ClearConnectP2PRelative.md）：仅扫描槽位 2..3，
// 与 param 的 P2P 点数相等者离队；恒返回 1。
int CParty::_ClearConnectP2PRelative(int slot)
{
    for (int i = 2; i < 4; i++)
    {
        if (_GetConnectP2PPoint(i) == slot)
        {
            leave_user(m_member[i].m_pUser,
                       static_cast<ENUM_PARTY_INFO_TYPE>(2));
        }
    }
    return 1;
}

// ORIG T 0x0859a7a2（_GetConnectP2PMaxMinPoint.md）：max=0/min=4 初值后，
// 对有效成员取 P2P 点数并收拢到 [min,max]。
void CParty::_GetConnectP2PMaxMinPoint(int& maxPoint, int& minPoint)
{
    maxPoint = 0;
    minPoint = 4;
    int point = 0;
    for (int i = 0; i < 4; i++)
    {
        // [X1·判据③有消费者] ORIG 0x085bxxxx `call;xor$1,%eax;test;jne`。
        register unsigned char v asm("eax") = (unsigned char)_checkValidUser(i);
        v ^= 1;
        if (!v)
        {
            point = _GetConnectP2PPoint(i);
            if (point < minPoint)
            {
                minPoint = point;
            }
            if (maxPoint < point)
            {
                maxPoint = point;
            }
        }
    }
}

// ORIG T 0x0859a830（ClearBadP2PUser.md）：绝对清理两个全通槽位，再迭代
// 相对点数差于最差的相对清理。
void CParty::ClearBadP2PUser()
{
    _ClearConnectP2PAbsolute(0);
    _ClearConnectP2PAbsolute(1);

    int maxPoint = 0;
    int minPoint = 0;
    for (int i = 2; i <= 3; i++)
    {
        _GetConnectP2PMaxMinPoint(maxPoint, minPoint);
        if (maxPoint == minPoint)
        {
            break;
        }
        _ClearConnectP2PRelative(minPoint);
    }
}

// ORIG T 0x0859cea2（send_party_ipinfo.md）：header(0,0xb) 广播各成员
// uniqueId/内外网 IP/端口/账号/NAT 类型/MTU 与角色耐力字节，末尾
// finalize(true) 后 send_to_party。
// P157/P48：ORIG 每个使用点都重新走 m_member[i].m_pUser 取用户链（从不
// 缓存局部），getCurCharacR 在 then/else 臂各调用一次；uid/port 低 16 位
// 截断是 caller 侧 movzwl（用 static_cast<unsigned short>，非 & 0xffff）。
void CParty::send_party_ipinfo()
{
    PacketGuard packet;
    packet.put_header(0, 0xb);
    packet.put_byte(get_member_count());
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i))
        {
            packet.put_short(static_cast<unsigned short>(
                m_member[i].m_pUser->get_unique_id()));
            packet.put_int(m_member[i].m_pUser->m_network.get_inner_ip());
            packet.put_int(m_member[i].m_pUser->m_network.get_ip());
            packet.put_short(static_cast<unsigned short>(
                m_member[i].m_pUser->m_network.get_port()));
            packet.put_int(m_member[i].m_pUser->get_acc_id());
            packet.put_byte(m_member[i].m_pUser->m_network.get_nat_type());
            packet.put_int(m_member[i].m_pUser->m_network.get_mtu());
            if (reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                    ->getCurCharacR() != 0)
            {
                packet.put_byte(
                    reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                        ->getCurCharacR()->m_stamina);
            }
            else
            {
                packet.put_byte(0);
            }
        }
    }
    packet.finalize(true);
    send_to_party(packet);
}

// ORIG T 0x0859d258（send_invite_pvp.md）/ 0x0859d388（send_invite_warroom.md）：
// header(0,7)+短地图索引+邀请类型（pvp=2/warroom=3）+发起者 uid 低 16 位。
void CParty::send_invite_pvp(int mapIdx)
{
    PacketGuard packet;
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i))     // ORIG 正向 test/jne+真路径 fall-through（技巧17/19）
        {
            packet.clear();
            packet.put_header(0, 7);
            packet.put_short(mapIdx);
            packet.put_byte(2);
            packet.put_int(m_member[i].m_pUser->get_unique_id() & 0xffff);
            packet.finalize(true);
            m_member[i].m_pUser->Send(packet);
        }
    }
}

void CParty::send_invite_warroom(int mapIdx)
{
    PacketGuard packet;
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i))     // ORIG 正向 test/jne+真路径 fall-through（技巧17/19）
        {
            packet.clear();
            packet.put_header(0, 7);
            packet.put_short(mapIdx);
            packet.put_byte(3);
            packet.put_int(m_member[i].m_pUser->get_unique_id() & 0xffff);
            packet.finalize(true);
            m_member[i].m_pUser->Send(packet);
        }
    }
}

// ORIG T 0x085ba5d6（send_to_alive_member.md）：存活（user state==5 且
// +0x380 liveFlag 非 0）的成员直发。
void CParty::send_to_alive_member(PacketGuard& packet)
{
    for (int i = 0; i < 4; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // [判据①] ORIG O@10 `test;je` 无 xor：==0 直判形。
        if (m_member[i].m_pUser->get_state() == 5 && m_battleData.m_arr54[i] != 0)
        {
            m_member[i].m_pUser->Send(packet);
        }
    }
}

// ============================================================================
// 成员查询 / 访问器
// ============================================================================

// ORIG T 0x085b66fa（isMyMember.md）：任一有效成员 uid 低 16 位命中即 1。
int CParty::isMyMember(unsigned int userNo)
{
    for (int i = 0; i <= 3; i++)
    {
        // [判据②岛形+拆&&] ORIG O@25 `sete%al;test;je`。
        if (_checkValidUser(i))
        {
            register bool hit asm("al") =
                (static_cast<unsigned int>(
                     m_member[i].m_pUser->get_unique_id()) & 0xffff) == userNo;
            if (hit)
            {
                return 1;
            }
        }
    }
    return 0;
}

// ORIG T 0x085b357c（GetCoinCount.md）：INT_MAX - 该成员 BattleData 已用硬币。
int CParty::GetCoinCount(CUser* user)
{
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_member[i].m_pUser == user)
        {
            // [P49] 弃 static inline helper（产真 call）：直成员访问让
            // 4.4 折叠 scale4 单段（ORIG `add $0x2c0,%edx; mov 0x14(%eax,%edx,4)`）。
            return 0x7fffffff - m_battleData.m_arr7e8[i];
        }
    }
    return 0;
}

// ORIG T 0x0859a1ae（get_total_level.md）：有效成员角色等级合计。
int CParty::get_total_level()
{
    int total = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            total += m_member[i].m_pUser->get_charac_level();
        }
    }
    return total;
}

// ORIG T 0x085b603e（getMemberMinLevel.md）：std::min 形态最小等级；
// 无有效成员时保持初值 INT_MAX。
int CParty::getMemberMinLevel()
{
    int minLevel = 0x7fffffff;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        int level = m_member[i].m_pUser->get_charac_level();
        if (level < minLevel)
        {
            minLevel = level;
        }
    }
    return minLevel;
}

// ORIG T 0x085b687c（isSamePCRoomParty.md）：首个成员 PCBang 号缓存后逐一
// 相同；任一为 0 或不一致即 false，全部一致 true。
bool CParty::isSamePCRoomParty()
{
    int firstRoomNo = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))     // ORIG 正向判定+真路径 fall-through（技巧17/19）
        {
            int roomNo = m_member[i].m_pUser->GetPCRoomNo();
            if (roomNo == 0)
            {
                return false;
            }
            if (firstRoomNo == 0)
            {
                firstRoomNo = roomNo;
            }
            else if (firstRoomNo != roomNo)
            {
                return false;
            }
        }
    }
    return true;
}

// ORIG T 0x085b8fa6（IsThereGhostInParty.md）。
bool CParty::IsThereGhostInParty()
{
    for (int i = 0; i <= 3; i++)
    {
        // [拆&&] ORIG 双段直分支，&& 合一 4.4 产物化岛 5 行。
        if (_checkValidUser(i))
        {
            if (reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                    ->IsCurCharacGhost())
            {
                return true;
            }
        }
    }
    return false;
}

// ORIG T 0x085b900c（getMemberPassParty.md）：param 为带新人入队的用户；
// 排除其师徒上端与本尊成员后取队伍最高等级。无人可排除返回 0；
// 最高级与新人 dungeon 前等级差 >6 时返回 1（不可加入），否则 0。
int CParty::getMemberPassParty(CUser* user)
{
    int maxLevel = 0;
    int level = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // P133/P45：两比较各自 je/jne 短路桥（ORIG +0x6b/+0xad），不物化 bool。
        if (reinterpret_cast<CUserCharacInfo*>(user)
                ->get_connect_upper_member() ==
            m_member[i].m_pUser->get_charac_no(-1))
        {
            continue;
        }
        if (user->get_charac_no(-1) ==
            m_member[i].m_pUser->get_charac_no(-1))
        {
            continue;
        }
        // ORIG +0xee：std::max 外部实例 call（0808e218）+ 返回指针解引用赋值。
        level = m_member[i].m_pUser->get_charac_level();
        maxLevel = std::max(maxLevel, level);
    }
    if (maxLevel == 0)
    {
        return 0;
    }
    if (maxLevel -
        reinterpret_cast<CUserCharacInfo*>(user)
            ->get_charac_level_before_dungeon() <= 6)
    {
        return 0;
    }
    return 1;
}

// ORIG T 0x085b9e80（GetUserSecretShop.md）：+0x2a8 + slot*0x1c 起的逐成员
// secret 商店数据视图；slot 出界返回空。
void* CParty::GetUserSecretShop(CUser* user)
{
    int slot = GetUserPosition(user);
    // [Ghidra 0x085b9e80 双证] 命中区间写正向 `0<=slot<=3`（ORIG js/jg 双
    // 跳共享 0 岛）；偏移为 **8**+slot*0x1c（this+0x2a8，原 4 系差 4B bug）。
    if (slot >= 0 && slot <= 3)
    {
        return &m_padShop[8 + slot * 0x1c];
    }
    return 0;
}

// ============================================================================
// 队伍状态 / 组队生命周期
// ============================================================================

// ORIG T 0x085ba680（switchPostion.md）：整个 cMember 槽（0x18 字节含
// m_pUser）对调。
int CParty::switchPostion(char a, char b)
{
    cMember tmp = m_member[a];
    m_member[a] = m_member[b];
    m_member[b] = tmp;
    return 1;
}

// ORIG T 0x0859b2b6（join_user.md）：空指针日志 -1；满员且 state==2 特判
// -2；找到空槽写入成员/userUid、回写 PartyIndex、清连击胜场并返回 1。
int CParty::join_user(CUser* user)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "party.cpp",
                              __PRETTY_FUNCTION__, 0x419,
                              "Null user");
        return -1;
    }
    if (m_state == 2)
    {
        return -2;
    }
    // [Ghidra 0x0859b2b6+setge 朝向] 满员判定写 count>=max 正向（ORIG
    // `cmp %edx,%eax; setge`）；耗尽返 0、满员/空参返 -1。
    if (get_member_count() >= static_cast<int>(m_userMax))
    {
        return -1;
    }
    for (int i = 0; i <= 3; i++)
    {
        if (m_member[i].m_pUser == 0)
        {
            m_member[i].m_pUser = user;
            m_member[i].m_field8 = static_cast<int>(user->GetUID());
            user->SetPartyIndex(static_cast<short>(m_partyIndex));
            reinterpret_cast<CUserCharacInfo*>(user)
                ->resetStraightVictories();
            return 1;
        }
    }
    return 0;
}

// ORIG T 0x0859b404（change_manager_for_quick_party.md）：manager 本人以
// PARTY_INFO_TYPE 无关路径摘出（SetPartyIndex(-1) 并清槽），再选首个有效
// 成员为新 host/manager；找不到则记录 no manager 日志返回 0。
int CParty::change_manager_for_quick_party()
{
    for (int i = 0; i <= 3; i++)
    {
        if (m_member[i].m_pUser == m_manager)
        {
            m_manager->SetPartyIndex(-1);
            m_member[i].m_pUser = 0;
            m_member[i].m_field8 = 0;
            break;
        }
    }
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            m_manager = m_member[i].m_pUser;
            set_host();
            send_host_info();
            return 1;
        }
    }
    LogManager::logFormat(
        1, "party.cpp", __PRETTY_FUNCTION__, 0x45d,
        "there is no manager(party empty)");
    return 0;
}

// ORIG T 0x0859b52c（delete_party_for_quick_party.md）：末人解散入池销毁；
// manager 引换手；普通成员直接清槽；查无此人返回 1。
int CParty::delete_party_for_quick_party(CUser* user,
                                         std::list<CUser*>& list)
{
    if (user == 0)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x465, "QUICK_PARTY_LOG : NO EXIST USER!!");
        return 1;
    }
    if (get_member_count() == 1)
    {
        // P87/P52：ORIG 以 %edi/%esi/%ebx 跨 NumberToString/cMyTrace ctor
        // 缓存三实参（+0x27/+0x31/+0x38），4.4 普通局部恒 spill；register
        // 形态贴近 callee-saved 驻留。
        register int partyIdx = static_cast<short>(user->GetPartyIndex());
        register int userState = user->get_state();
        register char* idStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x46c, 0);
        trace("<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
              idStr, userState, static_cast<int>(partyIdx));
        user->SetPartyIndex(-1);
        list.push_back(user);
        destroy();
        return 0;
    }
    if (m_manager == user)
    {
        change_manager_for_quick_party();
        list.push_back(user);
        return 0;
    }
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_member[i].m_pUser == user)
        {
            m_member[i].m_pUser = 0;
            m_member[i].m_field8 = 0;
            user->SetPartyIndex(-1);
            list.push_back(user);
            return 0;
        }
    }
    return 1;
}

// ORIG T 0x0859d656（CheckMemberArea.md）：以首个有效成员区域为基准逐一比较。
int CParty::CheckMemberArea()
{
    int area = -1;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (area == -1)
        {
            area = m_member[i].m_pUser->get_area(false);
        }
        else if (area != m_member[i].m_pUser->get_area(false))
        {
            return 0;
        }
    }
    return 1;
}

// ORIG T 0x085ba762（setMonsterMoveHack.md）：对全体成员记 0xd3 服务端反外挂计数。
void CParty::setMonsterMoveHack()
{
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            m_member[i].m_pUser->getHackAnalyzer()->addServerHackCnt(
                m_member[i].m_pUser, static_cast<WongWork::ENUM_HACKTYPE>(0xd3),
                1, 0, 0);
        }
    }
}

// ============================================================================
// 结果接收 / 任务 / 副本入场券
// ============================================================================

// ORIG T 0x085b3f34（set_recv_party_packet_flag.md）：+0x1dc 结果接收标志区。
void CParty::set_recv_party_packet_flag(CUser* user)
{
    int slot = GetMemberSlotNo(const_cast<const CUser*>(user));
    if (slot >= 0)
    {
        reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)->SetRecvFlag(slot);
    }
}

// ORIG T 0x085b3f72（recv_party_packet_all.md）：全员均已回包才返回 true。
bool CParty::recv_party_packet_all()
{
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // [X1·判据③有消费者] ORIG `call; xor$1,%eax; test; je`。
        register unsigned char r asm("eax") =
            (unsigned char)reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)
                ->IsRecv(i);
        r ^= 1;
        if (r)
        {
            return false;
        }
    }
    return true;
}

// ORIG T 0x085b3fd8（CheckRecvFromAllMember.md）：登记本人标志并轮询全员。
bool CParty::CheckRecvFromAllMember(CUser* user)
{
    set_recv_party_packet_flag(user);
    if (!recv_party_packet_all())
    {
        return false;
    }
    reset_recv_flag();
    return true;
}

// ORIG T 0x0859a34e（TimeoutCompleteAfterAssault.md）：+0x118 守卫非 0 时
// 复位并返回 0；为 0 时按未回包成员逐个踢出（trace 后 leave_user type2），
// 返回 1（ORIG 主路径不写 +0x118；调用方 CAssaultMgr 两处均 test %al 消费返回值）。
int CParty::TimeoutCompleteAfterAssault()
{
    if (m_recvResultFlag != 0)
    {
        m_recvResultFlag = 0;
        return 0;
    }
    if (recv_party_packet_all() == 0)
    {
        for (int i = 0; i <= 3; i++)
        {
            if (!_checkValidUser(i))
            {
                continue;
            }
            // [X1·判据③] ORIG `call; xor$1,%eax; test; je`。
            register unsigned char r asm("eax") =
                (unsigned char)reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)
                    ->IsRecv(i);
            r ^= 1;
            if (r)
            {
                // P87/P36：两中间结果跨 cMyTrace ctor 调用以 callee-saved
                // %esi/%ebx 缓存（ORIG +0x0a6/+0x0b3）。
                register const char* name =
                    reinterpret_cast<CUserCharacInfo*>(get_user(i))
                        ->getCurCharacName();
                register int partyIdx = GetPartyIndex();
                // P37：cMyTrace 临时链式（args 在 ctor 后设置）。
                cMyTrace(__PRETTY_FUNCTION__, 0x1fa, 0)(
                    "CParty::TimeoutCompleteAfterAssault(index:%d, name:%s)",
                    partyIdx, name);
                leave_user(get_user(i),
                           static_cast<ENUM_PARTY_INFO_TYPE>(2));
            }
        }
    }
    return 1;
}

// ORIG T 0x085a82d2（IsClearedQuest.md）：任一有效成员未清任务即 false。
bool CParty::IsClearedQuest(int questIdx)
{
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        UserQuest* quest = m_member[i].m_pUser->getCurCharacQuestR();
        if (!quest->isClearQuest(questIdx))
        {
            return false;
        }
    }
    return true;
}

// ORIG T 0x085b65a2（HasNoQuestGoWithApcInParty.md）：状态 5/10 的成员若未在
// 做与 APC 同行任务（triggerType==3 且 triggerSubType==0 判定配对）则 false。
bool CParty::HasNoQuestGoWithApcInParty(Quest const* quest)
{
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            // P48：两比较各调一次 get_state（ORIG +0x1a/+0x2d 双 call）。
            if (m_member[i].m_pUser->get_state() != 5 &&
                m_member[i].m_pUser->get_state() != 10)
            {
                continue;
            }
            register int qIdx = quest->m_index;
            UserQuest* userQuest = m_member[i].m_pUser->getCurCharacQuestR();
            if (userQuest->IsDoingQuest(qIdx) &&
                quest->m_triggerType == 3 && quest->m_triggerSubType == 0)
            {
                continue;
            }
            return false;
        }
    }
    return true;
}

// ORIG T 0x0859e7fe（CheckEnterAncientDungeon_0859e7fe.md，单参别名版）：
// dungeon 未开组队入口时做 ancient 入场券检查；否则按 4 参事件版判定。
// 残差归因（N 类）：ORIG 将 itemIdx 读入 %esi 跨 getCurCharacInvenR 调用
// 缓存（4.4 恒为 call 后现场重读，P75 outgoing-arg 装载时刻族）、i 槽 -0xc
// 与 items 基 -0x111（4.1 帧内 4B 交错，P112）、循环头双 movl $0 初始化
// 复制（4.1 tail-duplicator 产物）均无法由合法源码复现。
int CParty::CheckEnterAncientDungeon(CDungeon const* dungeon)
{
    Inven_Item items[4];
    int slots[4];
    memset(items, 0, sizeof(items));
    memset(slots, 0, sizeof(slots));

    if (const_cast<CDungeon*>(dungeon)->m_entryRestrictFlag)
    {
        unsigned char flag = 0;    // ORIG -0x1d：items 顶与 slots 底之间，+0x79
                                  // 分支内清零
        return CheckEnterEventDungeon(dungeon, flag, items, slots) ? 1 : 0;
    }

    if (dungeon->m_entryRequiredItem.m_itemIdx == -1)
    {
        return 1;
    }
    if (!dungeon->m_entryRequiredItem.m_enableFlag)
    {
        return 1;
    }
    if (m_battleField.m_bloodState == 1)
    {
        return 1;
    }
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        CInventory const* inv =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                ->getCurCharacInvenR();
        slots[i] = inv->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx);
        if (slots[i] == -1)
        {
            return 0;
        }
        items[i] = inv->GetInvenSlot(1, slots[i]);
        if (items[i].get_add_info() <
            dungeon->m_entryRequiredItem.m_itemCount)
        {
            return 0;
        }
    }
    return 1;
}

// ORIG T 0x085be56c（checkLimitUsedCoinCondition.md）：party/dungeon 双层
// 硬币限额校验；超限返回 0x16、找不到副本返回 0x13、通过返回 0。
// 形参直写回 0xc(%ebp)（ORIG +0x34 `mov %eax,0xc(%ebp)`：find_dungeon 结果
// 直接覆写形参槽，无本地别名副本）；diff 以内联实参入 getLimitCoinDiff。
int CParty::checkLimitUsedCoinCondition(CDungeon const* dungeon)
{
    if (dungeon == 0)
    {
        int dungeonIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                             ->get_dungeon_index();
        dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
        if (dungeon == 0)
        {
            return 0x13;
        }
    }
    if (GetPartyMemberCoinLimit() != 0 &&
        GetUsedCoinCount() >= GetPartyMemberCoinLimit())
    {
        return 0x16;
    }

    int limitCoin = 0;
    if (dungeon->getLimitCoinDiff(
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->get_dungeon_diff(),
            limitCoin) &&
        limitCoin != -1 && GetUsedCoinCount() >= limitCoin)
    {
        return 0x16;
    }
    if (dungeon->m_87c != -1 && GetUsedCoinCount() >= dungeon->m_87c)
    {
        return 0x16;
    }
    if (dungeon->m_898 != -1 && CheckHellCoinLimint(dungeon->m_898) != 1)
    {
        return 0x16;
    }
    return 0;
}

// ORIG T 0x085be4ae（checkUnLimitUsedCoinCondition.md）：type 0x5e 重复事件
// 处于进行中且副本开启维度次数限制时豁免硬币上限。
bool CParty::checkUnLimitUsedCoinCondition(CDungeon const* dungeon)
{
    if (dungeon == 0)
    {
        int dungeonIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                             ->get_dungeon_index();
        dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
        if (dungeon == 0)
        {
            return false;
        }
    }
    // ORIG 无 ev 判空（虚调用直发，+0x024..0x031）；尾条件为有符号 >0
    // （+0x03b.. test/jle），忠实转写。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x5e)->IsEventing(0))
    {
        // [判据②直分支形] ORIG O@36-42 `jle;mov$1;jmp;mov$0`，勿写表达式
        // 返回（4.4 出 setne 物化）。
        if (const_cast<CDungeon*>(dungeon)->get_dimension_possible() > 0)
        {
            return true;
        }
        return false;
    }
    return false;
}

// ORIG T 0x085a834a（CheckHellCoinLimint.md）：hell 场景且 +0xcd4 开关打开、
// 未清完全部 hell 组时，对照已用硬币数判断是否越过本副本 hell 限额。
int CParty::CheckHellCoinLimint(int param)
{
    if (CBattle_Field::IsEnableHellDungeon() && m_battleField.m_hellPartyMode == 1 &&
        reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->IsKilledAllHellGruoups() != 1 &&
        GetUsedCoinCount() >= param)
    {
        return 0;
    }
    return 1;
}

// ORIG U 0x0859a210（is_limit_minimum_age.md）：任一成员不满足年龄下限即 0
// （param 输出违规成员索引由 CUser::CheckLimitMiniumAge 内部填写）。
int CParty::is_limit_minimum_age(int age, int& param)
{
    for (int i = 0; i <= 3; i++)
    {
        // [判据②直分支形+拆&&] ORIG O@26-31：`call;xor$1;test;je` +
        // `mov$0;jmp` 直返，无 && 物化岛——两条件拆嵌套。
        if (_checkValidUser(i))
        {
            if (m_member[i].m_pUser->CheckLimitMiniumAge(age, param) == 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

// ORIG U 0x0859a286（is_limit_minimum_level.md）：等级不足者以 uid 回填 param。
int CParty::is_limit_minimum_level(int level, int& param)
{
    for (int i = 0; i <= 3; i++)
    {
        // [判据②岛形+拆&&] ORIG O@22-26：`call;cmp;setl%al;test;je`。
        if (_checkValidUser(i))
        {
            register bool low asm("al") =
                m_member[i].m_pUser->get_charac_level() < level;
            if (low)
            {
                param = m_member[i].m_pUser->get_unique_id() & 0xffff;
                return 0;
            }
        }
    }
    return 1;
}

// ORIG T 0x085bcab6（CheckMemberLastTryAssaultTime.md）：任一成员 600 秒内
// 有混沌击杀时间记录即 1（禁止立即组队袭击）。
int CParty::CheckMemberLastTryAssaultTime()
{
    for (int i = 0; i <= 3; i++)
    {
        if (!checkValidUser(i))
        {
            continue;
        }
        int curSec = GlobalData::s_systemTime_.getCurSec();
        int lastKillTime =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                ->GetCurCharacChaosKillTime();
        if (curSec < lastKillTime + 600)
        {
            return 1;
        }
    }
    return 0;
}

// ORIG T 0x0859bfe2（increase_dimension_inout_count.md）：副本允许维度进出时，
// 各维度 inout 槽 (dm 上限, 成员现值) 同加 delta 并截断到 dm 上限。
void CParty::increase_dimension_inout_count(int param)
{
    if (m_battleField.m_pDungeon != 0)
    {
        if (m_battleField.m_pDungeon->get_dimension_possible() == 0)
        {
            return;
        }
        {
            for (int i = 0; i <= 3; i++)
            {
                if (!checkValidUser(i))
                {
                    continue;
                }
                {
                    for (int j = 0; j <= 5; j++)
                    {
                        int dmNext = static_cast<char>(
                            G_CDataManager()->get_dimensionInout(j)) +
                            param;                      // -0x10 单份
                        int curNext = static_cast<char>(
                            reinterpret_cast<CUserCharacInfo*>(
                                m_member[i].m_pUser)
                                ->getDemensionInoutValue(
                                    static_cast<char>(j))) +
                            param;                      // -0xc 单份
                        if (curNext > dmNext)
                            curNext = dmNext;
                        reinterpret_cast<CUserCharacInfo*>(
                            m_member[i].m_pUser)
                            ->setDemensionInoutValue(
                                static_cast<char>(j),
                                static_cast<char>(curNext));
                    }
                }
            }
        }
    }
    return;
}

// ORIG T 0x085a2320（GetExpPenaltyPercent.md）：等级差惩罚百分比表。
int CParty::GetExpPenaltyPercent(int a, int b) const
{
    if (a < 20)
    {
        int diff = a - b > 0 ? a - b : b - a;
        if (5 < diff && diff < 10)
        {
            return (10 - diff) * 20;
        }
        if (diff > 9)
        {
            return 5;
        }
    }
    else
    {
        if (a < b)
        {
            return (a * 100) / b;
        }
        int diff = a - b;
        if (5 < diff && diff < 10)
        {
            return (10 - diff) * 20;
        }
        if (diff > 9)
        {
            return 5;
        }
    }
    return 100;
}

// ============================================================================
// 副本流程 / 结算
// ============================================================================

// ORIG T 0x085a2246（RestartDungeon.md）：复位战场后以原 idx/diff 重开局。
void CParty::RestartDungeon()
{
    int dungeonIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                         ->get_dungeon_index();
    // ORIG：mov %eax,-0xc（int 槽），dungeon_start 前 movsbl %al 扩符号。
    int diff = reinterpret_cast<CBattle_Field*>(&m_battleField)
                   ->get_dungeon_diff();
    reinterpret_cast<CBattle_Field*>(&m_battleField)->reset_field();
    dungeon_start(dungeonIdx, (char)diff, static_cast<ENUM_DUNGEON_TYPE>(0));
}

// ORIG T 0x085a7786（isMonsterDrop_item.md）：场上存在该掉落且数量非 0 才算 0。
bool CParty::isMonsterDrop_item(int monsterIdx)
{
    map_item item;
    bool found = reinterpret_cast<CBattle_Field*>(&m_battleField)
                     ->get_field_item(monsterIdx, item);
    if (!found || item.m_count == 0)
    {
        return true;
    }
    return false;
}

// ORIG T 0x085a3b6a（kill_blood_monster.md）：血轮击杀转发战场层。
void CParty::kill_blood_monster(CUser* user, int monsterIdx,
                                unsigned short* param)
{
    reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->kill_blood_monster(user, monsterIdx, param);
}

// ORIG T 0x085b6540（DestroyPassiveObject.md）：摧毁被动物件并在触发清除条件
// 时结算整场。
unsigned char CParty::DestroyPassiveObject(int param)
{
    unsigned char destroyed = static_cast<unsigned char>(
        reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->DestroyPassiveObject(param));
    if (destroyed &&
        reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->ClearCondition(static_cast<ENUM_DUNGEON_CLEAR_CONDITION_TYPE>(0),
                             param))
    {
        ClearDungeon();
    }
    return destroyed;
}

// ORIG T 0x085b9f28（SetRidableObjectState.md）：坐骑状态转发战场层（返回值穿透）。
bool CParty::SetRidableObjectState(int state, eRidableObjectState type)
{
    return reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->SetRidableObjectState(state, type);
}

// ============================================================================
// TOD / 血轮复活
// ============================================================================

// ORIG T 0x085ba906（OnTODRewardTimer.md）：0x2c 报文发送 TOD 结算奖励包，
// EPLP 置 2、注册 0x15 层推进定时器后回到 state1，并注册 8 秒后的 0x2c 重投递。
bool CParty::OnTODRewardTimer(TIMER_MESSAGE msg)   // CParty.h:592 bool（PSM 落，定义同步）
{
    if (msg == 0x2c)
    {
        _Send_TOD_ClearReward();
        SetEPLPState(2);
        int key15 = gen_timer_key(static_cast<TIMER_MESSAGE>(0x15));
        G_TimerQueue()->InsertTimer(
            static_cast<TimerEntry::OBJ_TYPE>(1), GetPartyIndex(),
            static_cast<TIMER_MESSAGE>(0x15), 0x3c, key15, 0);
        set_state(1);
        int key24 = gen_timer_key(static_cast<TIMER_MESSAGE>(0x24));
        G_TimerQueue()->InsertTimer(
            static_cast<TimerEntry::OBJ_TYPE>(1), GetPartyIndex(),
            static_cast<TIMER_MESSAGE>(0x2c), 8, key24, 0x24);
    }
    return 1;
}

// ORIG T 0x085a22a8（SelectEPLPCommand.md）：EPLP 选令：置状态 3 并按指定秒数
// 注册 0x14 定时器。
void CParty::SelectEPLPCommand(int cmd)
{
    SetEPLPState(3);
    // [判据⑤] ORIG O@13 `mov%eax,%esi`：key 跨 call 钉 %esi（免槽 spill）。
    register int key asm("esi") = gen_timer_key(static_cast<TIMER_MESSAGE>(0x14));
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(1),
                                GetPartyIndex(), static_cast<TIMER_MESSAGE>(0x14),
                                cmd, key, 0);
}

// ORIG T 0x085baa06（_Send_TOD_ClearReward.md）：0x15b 包——管理者的 layer
// 扩展数据最后进入层 + 最多 9 组奖励清单（不足补 -1/0）+ 特殊奖励索引/数量。
int CParty::_Send_TOD_ClearReward()
{
    PacketGuard packet;
    packet.put_header(0, 0x15b);
    packet.put_int(
        *reinterpret_cast<const int*>(&m_battleData));   // ORIG *(int*)(this+0x32c)
    TOD_Layer lastLayer =
        reinterpret_cast<TOD_UserState*>(
            getManager()->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)))
            ->getLastEnterLayer();                    // ORIG 无条件（无 null 检查）
    packet.put_short(static_cast<unsigned short>(lastLayer.GetLayer()));
    packet.put_byte(10);   // ORIG +0x09c 固定 byte 10
    // P48/P157：size()/operator[] 每使用点独立求值（ORIG 循环条件每轮
    // reload this + call size，无缓存）。
    for (unsigned int i = 0;
         i < reinterpret_cast<CBattle_Field*>(&m_battleField)
                 ->m_todReward.m_todMap.size();
         ++i)
    {
        packet.put_int(
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->m_todReward.m_todMap[i].first);
        packet.put_int(
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->m_todReward.m_todMap[i].second);
    }
    for (unsigned int i = 0;
         i < 9U -
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->m_todReward.m_todMap.size();
         ++i)
    {
        packet.put_int(-1);
        packet.put_int(0);
    }
    packet.put_int(reinterpret_cast<CBattle_Field*>(&m_battleField)
                       ->m_todReward.m_todPos.first);
    packet.put_int(reinterpret_cast<CBattle_Field*>(&m_battleField)
                       ->m_todReward.m_todPos.second);
    packet.finalize(true);
    send_to_party(packet);
    return 1;
}

// ORIG T 0x085b27c8（ReviveUser.md）：state2/EPLP 下按 uid 找死亡成员；
// 免费复活条件不满足时扣操作者 BattleData 硬币（写使用日志），随后原地复活。
int CParty::ReviveUser(CUser* actor, unsigned short targetUid)
{
    if (get_state() != 2)
    {
        if (GetEPLPState() == 0)
        {
            return 0x13;
        }
    }

    CUser* dead = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_member[i].m_pUser->get_unique_id() == targetUid)
        {
            if (m_battleData.m_arr54[i] != 0)
            {
                return 0x12;
            }
            dead = m_member[i].m_pUser;
            break;
        }
    }
    if (dead == 0)
    {
        return 0x15;
    }

    bool freeRevival = checkFreeRevivalCondition(0);   // -0x11 物化槽
    for (int j = 0; j <= 3; j++)
    {
        if (!_checkValidUser(j))
        {
            continue;
        }
        if (m_member[j].m_pUser == actor)
        {
            if (freeRevival != 0)
            {
                goto coin_skip;
            }
            if (m_battleData.m_arr7e8[j] == 0)
            {
                return 1;
            }
            HistoryLog::WriteUseCoin(
                reinterpret_cast<_IO_FILE*>(actor->m_field796f8),
                reinterpret_cast<CUserCharacInfo*>(dead)->getCurCharacName());
            --m_battleData.m_arr7e8[j];
        coin_skip:;
        }
    }
    set_charac_live(dead, static_cast<ENUM_USER_DUNGEON_STATE>(1));
    Reset_party_overlapped_drop_ratio();
    return 0;
}
