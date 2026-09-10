// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyOnlyOrig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CParty.h"

#include "CDungeon.h"              // m_entryRequiredItem/get_dimension_possible/
                                   //   getLimitCoinDiff/m_87c/m_898
#include "CBattle_Field.h"         // BF 视图转发（先例：CParty.cpp checkBossRoom）
#include "CDataManager.h"          // G_CDataManager()/find_dungeon/get_dimensionInout
#include "CInventory.h"            // check_item_exist/GetInvenSlot
#include "SECRET_SHOP_DATA.h"      // SECRET_SHOP_DATA::m_info（GetUserSecretShop 具名视图）
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

// +0x1dc CPartyResultRecvFlag 接收标志区视图。ORIG 三处 receiver 取址
// （0x085b3f58 / 0x085b3f85 / 0x085a464）均为 `mov this,%eax;
// lea 0x1dc(%eax),%edx`；4.4 对「视图 cast + 具名成员」产同形 lea，对
// `reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)` 取址恒产 add（N107）
// ⇒ 以视图具名成员表达 receiver（探针 /tmp/b28d_s187/p2.cpp m1..m3）。
struct CPartyRecvFlagView
{
    unsigned char m_pad1dc[0x1dc];
    CPartyResultRecvFlag m_recvFlag;   // +0x1dc
};

}  // namespace

// ============================================================================
// P2P 连接管理区
// ============================================================================

// ORIG T 0x0859a48c _ZN6CParty10ConnectP2PEP5CUserPci（ConnectP2P.md）。
// 队伍状态必须为 1（入队/出发态），否则抛 ENUM_ERROR(0x13)；
// 目标成员 IP 缓冲（cMember+0x0e 起 4 字节）复制 min(port,4) 字节并把
// cMember+0x0c 置 1；全员连接完成后清坏 P2P 成员。ip 参数仅作 memcpy 源
// （ORIG 未读其长度，端口参数被 min 截断使用【忠实转写】）。
#include "CPartyOnlyOrig/ConnectP2P.inc"
{
    if (get_state() != 1)
    {
        throw static_cast<ENUM_ERROR>(0x13);
    }

    bool allConnected = true;
    for (int i = 0; i < 4; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_member[i].m_pUser == user)
        {
            if (m_member[i].m_fieldc == 1)
            {
                throw static_cast<ENUM_ERROR>(0x12);
            }
            m_member[i].m_fieldc = 1;
            memcpy(m_member[i].m_fielde, ip, std::min(port, 4));
        }
        if (m_member[i].m_fieldc != 1)
        {
            allConnected = false;
        }
    }
    if (allConnected)
    {
        ClearBadP2PUser();
    }
}

// ORIG T 0x0859a664（_ClearConnectP2PAbsolute.md）：除 slot 外全员中与该槽
// P2P 不通的逐个以 ENUM_PARTY_INFO_TYPE 2 离队；恒返回 1。
#include "CPartyOnlyOrig/_ClearConnectP2PAbsolute.inc"
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
#include "CPartyOnlyOrig/_ClearConnectP2PRelative.inc"
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
#include "CPartyOnlyOrig/_GetConnectP2PMaxMinPoint.inc"
{
    maxPoint = 0;
    minPoint = 4;
    int point = 0;                                    // -0x10
    for (int i = 0; i <= 3; i++)                      // -0xc（P2/P51：i<=3 → cmpl $0x3;setle 尾检）
    {
        // P168a①：bool callee 裸形直产 call; xor$1; test; jne→nop 岛
        if (!_checkValidUser(i))
            continue;
        point = _GetConnectP2PPoint(i);
        if (minPoint > point)                           // ORIG cmp (*min),point; jle
            minPoint = point;
        if (maxPoint < point)                           // ORIG cmp point,(*max); jge
            maxPoint = point;
    }
}

// ORIG T 0x0859a830（ClearBadP2PUser.md）：绝对清理两个全通槽位，再迭代
// 相对点数差于最差的相对清理。
#include "CPartyOnlyOrig/ClearBadP2PUser.inc"
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
#include "CPartyOnlyOrig/send_party_ipinfo.inc"
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
#include "CPartyOnlyOrig/send_invite_pvp.inc"
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

#include "CPartyOnlyOrig/send_invite_warroom.inc"
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
#include "CPartyOnlyOrig/send_to_alive_member.inc"
{
    for (int i = 0; i < 4; i++)
    {
        // ORIG O@0x24 `test%al;je` 无 xor（bool 直测跳过 ⇒ 嵌套 if 守卫）。
        if (_checkValidUser(i))
        {
            if (m_member[i].m_pUser->get_state() == 5 &&
                m_battleData.m_arr54[i] != 0)
            {
                m_member[i].m_pUser->Send(packet);
            }
        }
    }
}

// ============================================================================
// 成员查询 / 访问器
// ============================================================================

// ORIG T 0x085b66fa（isMyMember.md）：任一有效成员 uid 低 16 位命中即 1。
#include "CPartyOnlyOrig/isMyMember.inc"
{
    for (int i = 0; i <= 3; i++)
    {
        // [判据②岛形+拆&&] ORIG O@25 `sete%al;test;je`。
        if (_checkValidUser(i))
        {
            // b28d-45 P0/N96 清绑：`register bool hit asm("al")` 删除，裸形
            // （P158 bool 局部形实测反劣 46→45 AE，已回退；ORIG O@25
            // sete%al;test;je 岛形在 4.4 无绑定下不可复现，按 N96/N18a 封顶）。
            if ((static_cast<unsigned int>(
                     m_member[i].m_pUser->get_unique_id()) & 0xffff) == userNo)
            {
                return 1;
            }
        }
    }
    return 0;
}

// ORIG T 0x085b357c（GetCoinCount.md）：INT_MAX - 该成员 BattleData 已用硬币。
#include "CPartyOnlyOrig/GetCoinCount.inc"
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
#include "CPartyOnlyOrig/get_total_level.inc"
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
#include "CPartyOnlyOrig/getMemberMinLevel.inc"
{
    int level;                       // P112 槽序：ORIG level@-0x10 浅于 minLevel@-0x14
    int minLevel = 0x7fffffff;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        level = m_member[i].m_pUser->get_charac_level();
        minLevel = std::min(minLevel, level);
    }
    return minLevel;
}

// ORIG T 0x085b687c（isSamePCRoomParty.md）：首个成员 PCBang 号缓存后逐一
// 相同；任一为 0 或不一致即 false，全部一致 true。
#include "CPartyOnlyOrig/isSamePCRoomParty.inc"
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
#include "CPartyOnlyOrig/IsThereGhostInParty.inc"
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
// b28d-solo-CPartyOnlyOrig 判词：**AE** @0x085b900c（clean 口径 ndiff=1，余为
// R_386_PC32 重定位显示差）。修法：level/maxLevel 声明序翻转（4.4 按首次使用
// 序，N93 标量例外）+ 删 `level = 0`（ORIG 只有 -0x14/-0xc 两条清零）+ 两比较
// 合并为单个 `||` 短路（复现 ORIG `mov $1/jmp/mov $0/test` 常量岛）+ 尾段按
// Ghidra C 的 `&&` 物化 + 重复零判形。
#include "CPartyOnlyOrig/getMemberPassParty.inc"
{
    // b28d-solo-CPartyOnlyOrig：ORIG 槽 maxLevel=-0x14 / level=-0x10（Ghidra C
    // 同序）——4.4 按首次使用序分配，声明序翻转为 level 在前才得 ORIG 槽序
    //（N93 标量例外）。level 无清零（ORIG 只有 -0x14/-0xc 两条 movl $0）。
    int level;
    int maxLevel = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // b28d-solo-CPartyOnlyOrig：ORIG 0x6b/0xad 两比较汇入
        // `mov $1; jmp; mov $0; test %al; jne continue` 常量岛物化 ⇒ 源码是
        // 单个 `||` 短路条件（P133 反例：两个独立 if 生 sete/test 而非常量岛）。
        if (reinterpret_cast<CUserCharacInfo*>(user)
                    ->get_connect_upper_member() ==
                m_member[i].m_pUser->get_charac_no(-1) ||
            user->get_charac_no(-1) ==
                m_member[i].m_pUser->get_charac_no(-1))
        {
            continue;
        }
        // ORIG +0xee：std::max 外部实例 call（0808e218）+ 返回指针解引用赋值。
        level = m_member[i].m_pUser->get_charac_level();
        maxLevel = std::max(maxLevel, level);
    }
    // b28d-solo-CPartyOnlyOrig：Ghidra C 尾段 = bVar1 常量岛物化
    //（`mov $1; jmp; mov $0; test %al; je`，ORIG +0x131..+0x13f），随后
    // **再次**判 maxLevel==0（+0x148），否则返回 1 ⇒ 单个 `&&` 条件 +
    // 重复零判，而非两个独立 return（后者 4.4 产 setle/test 物化）。
    if (maxLevel != 0 &&
        maxLevel -
            reinterpret_cast<CUserCharacInfo*>(user)
                ->get_charac_level_before_dungeon() <= 6)
    {
        return 0;
    }
    if (maxLevel == 0)
    {
        return 0;
    }
    return 1;
}

// ORIG T 0x085b9e80（GetUserSecretShop.md）：+0x2a8 + slot*0x1c 起的逐成员
// secret 商店数据视图；slot 出界返回空。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE。修法：CParty.h +0x2a8 落
// POD 具名数组成员 `m_secretShopInfo[4][0x1c]`（R2 已广播，sizeof/偏移不变）
// ⇒ `&this->m_secretShopInfo[slot]` 复现 ORIG 的 28x 强度归约链与
// `lea 0x2a0(%edx),%eax; add this; add $8` 常量拆分；cast 取址形恒多
// `mov this,%ecx` 中转（探针 /tmp/b28d_s187/p11/p14/p18）。
#include "CPartyOnlyOrig/GetUserSecretShop.inc"
{
    int slot = GetUserPosition(user);
    // [Ghidra 0x085b9e80 双证] 命中区间写正向 `0<=slot<=3`（ORIG js/jg 双
    // 跳共享 0 岛）；ORIG 总偏移 = this+0x2a0+8+slot*0x1c = **0x2a8**+slot*0x1c
    //（CParty.h 中 SECRET_SHOP_DATA 头 m_padShop@+0x2a4 + 数组
    // m_secretShopInfo@+0x2a8；前人误将区域基址 0x2a0 当子对象基址写 8，实产
    // 0x2ac 差 4B）。
    if (slot >= 0 && slot <= 3)
    {
        // ORIG 地址 this+0x2a8+slot*0x1c = SECRET_SHOP_DATA(+0x2a4).m_info
        // （+0x04，SECRET_SHOP_INFO 0x1c）。
        // [b28d-solo-CParty2] CParty.h 字段化：+0x2a4 区为真实成员
        // m_secretShop（SECRET_SHOP_DATA），m_info 数组 @+0x04 ⇒ 本式偏移
        // 仍 = this+0x2a8+slot*0x1c（与原 m_secretShopInfo 同址）。
        return reinterpret_cast<secretshop::SECRET_SHOP_INFO*>(
            &m_secretShop.m_info[slot]);
    }
    return 0;
}

// ============================================================================
// 队伍状态 / 组队生命周期
// ============================================================================

// ORIG T 0x085ba680（switchPostion.md）：整个 cMember 槽（0x18 字节含
// m_pUser）对调。
#include "CPartyOnlyOrig/switchPostion.inc"
{
    cMember tmp = m_member[a];
    m_member[a] = m_member[b];
    m_member[b] = tmp;
    return 1;
}

// ORIG T 0x0859b2b6（join_user.md）：空指针日志 -1；满员且 state==2 特判
// -2；找到空槽写入成员/userUid、回写 PartyIndex、清连击胜场并返回 1。
#include "CPartyOnlyOrig/join_user.inc"
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
#include "CPartyOnlyOrig/change_manager_for_quick_party.inc"
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
#include "CPartyOnlyOrig/delete_party_for_quick_party.inc"
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
#include "CPartyOnlyOrig/CheckMemberArea.inc"
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
#include "CPartyOnlyOrig/setMonsterMoveHack.inc"
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
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE。修法：+0x1dc 视图具名成员
// receiver（`CPartyRecvFlagView::m_recvFlag`）复现 ORIG `lea 0x1dc(%eax),%edx`
// —— 4.4 对取址 cast 恒 add（N107），对视图具名成员产 lea。
#include "CPartyOnlyOrig/set_recv_party_packet_flag.inc"
{
    int slot = GetMemberSlotNo(const_cast<const CUser*>(user));
    // b28d-45 P223 变体：ORIG 0x085b3f70 体尾 `jmp +0x3c; nop` 岛（4.4 对
    // 正向 if 恒 jump-thread）⇒ 改裸早返形复现。
    if (slot < 0)
    {
        return;
    }
    reinterpret_cast<CPartyRecvFlagView*>(this)->m_recvFlag.SetRecvFlag(slot);
}

// ORIG T 0x085b3f72（recv_party_packet_all.md）：全员均已回包才返回 true。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE（+0x1dc 视图具名 receiver，
// 同 set_recv_party_packet_flag 修法）。
// b28d-45 recv_party_packet_all 0x085b3f72：已修 P248 档①（CPartyResultRecvFlag::
// IsRecv uchar→bool，0x085b3fc0 xor 段对齐）。
#include "CPartyOnlyOrig/recv_party_packet_all.inc"
{
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // N96 清绑：ORIG call; xor$1; test; jne 岛——4.4 对 uchar callee 恒
        // setXX 物化（P168a④ 在册），裸形取最小差。
        if (!reinterpret_cast<CPartyRecvFlagView*>(this)
                 ->m_recvFlag.IsRecv(i))
            return false;
    }
    return true;
}

// ORIG T 0x085b3fd8（CheckRecvFromAllMember.md）：登记本人标志并轮询全员。
#include "CPartyOnlyOrig/CheckRecvFromAllMember.inc"
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
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE（+0x1dc 视图具名 receiver）。
// b28d-45 TimeoutCompleteAfterAssault 0x0859a34e：IsRecv bool 化（P248 档①）+
// 正向 if 包裹（P214①）后 85:85。
#include "CPartyOnlyOrig/TimeoutCompleteAfterAssault.inc"
{
    if (m_recvResultFlag != 0)
    {
        m_recvResultFlag = 0;
        return 0;
    }
    if (!recv_party_packet_all())
    {
        for (int i = 0; i <= 3; i++)
        {
            if (!_checkValidUser(i))
            {
                continue;
            }
            // b28d-45 P248 档①：IsRecv 已 bool 化，ORIG 0x085a467 为
            // `call; xor $0x1; test %al,%al; je`（正向 if 包裹），非 continue 形。
            if (!reinterpret_cast<CPartyRecvFlagView*>(this)
                     ->m_recvFlag.IsRecv(i))
            {
                // P227 全表达式临时链：R2L 求值 name(→%esi)/partyIdx(→%ebx)
                // 跨 ctor 保活（ORIG +0x0a6/+0x0b3）
                cMyTrace(__PRETTY_FUNCTION__, 0x1fa, 0)(
                    "CParty::TimeoutCompleteAfterAssault(index:%d, name:%s)",
                    GetPartyIndex(),
                    reinterpret_cast<CUserCharacInfo*>(get_user(i))
                        ->getCurCharacName());
                leave_user(get_user(i),
                           static_cast<ENUM_PARTY_INFO_TYPE>(2));
            }
        }
    }
    return 1;
}

// ORIG T 0x085a82d2（IsClearedQuest.md）：任一有效成员未清任务即 false。
// b28d-45 IsClearedQuest 0x085a82d2：FIX→AE —— Main 已落 QuestClear bool 链
// （STClearedQuest::m_cleared / CQuestClear::isClearedQuest / CUser.h:1270），
// 调用点改裸取反 `if(!isClearQuest(...))` 后 40:40 槽位集全等、xor 段逐条对齐；
// 余为 R_386_32 重定位显示差（链接后即 ORIG 绝对地址）。
#include "CPartyOnlyOrig/IsClearedQuest.inc"
{
    for (int i = 0; i <= 3; i++)
    {
        // ORIG O@0x0f `test%al;je` 无 xor（bool 直测跳过 body ⇒ 嵌套 if 守卫）；
        // 内部 isClearQuest `!=1` 用 xor（P194 聚合布尔物化）。
        if (_checkValidUser(i))
        {
            // b28d-45 P248 档①：Main 已落 QuestClear/isClearQuest bool 链
            // （ORIG 0x085a8320 `call; xor $0x1; test %al,%al; je`）⇒ 裸取反形。
            if (!m_member[i].m_pUser->getCurCharacQuestR()
                     ->isClearQuest(questIdx))
            {
                return false;
            }
        }
    }
    return true;
}

// ORIG T 0x085b65a2（HasNoQuestGoWithApcInParty.md）：状态 5/10 的成员若未在
// 做与 APC 同行任务（triggerType==3 且 triggerSubType==0 判定配对）则 false。
// b28d-solo-CPartyOnlyOrig 判词：DIFF（N 封顶）@0x085b65a2。修法已落：删
// userQuest 局部槽（内联 getCurCharacQuestR()->IsDoingQuest）对齐 ORIG 直接
// 压参形；外层条件改正向 `==5 || ==10`。残余 N 封顶：4.4 对 `||` 第二条件后
// 恒物化 `mov $1/$0; test %al,%al` 岛，4.1 为 `je body / jne continue` 直跳
//（探针 f1..f6 + 本轮 p19 f1..f5：正/负极性、嵌套 if、goto、switch 五形全产
// 物化岛）。语义等价。
#include "CPartyOnlyOrig/HasNoQuestGoWithApcInParty.inc"
{
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            // C05③ 试形：ORIG 0x73/0xa8/0xb3/0xc0 四条失败 jcc 汇同一 false
            // 出口（0x666b `mov $0,%eax`）= 4.1 单条短路链的 lowering
            // （0x41 `je body` 为 `||` 首条件直跳）。原两层 if 写法令 4.4 逐层
            // 各物化一次（多 `mov $1; jmp; mov $0; test; je` 5 条）。
            if ((m_member[i].m_pUser->get_state() == 5 ||
                 m_member[i].m_pUser->get_state() == 10) &&
                m_member[i].m_pUser->getCurCharacQuestR()
                    ->IsDoingQuest(quest->m_index) &&
                quest->m_triggerType == 3 && quest->m_triggerSubType == 0)
            {
                return false;
            }
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
// b28d-solo-CPartyOnlyOrig 判词：DIFF（部分修复，残余 N 封顶）@0x0859e7fe。
// 本轮修法：①`while (slots[i] == -1) { return 0; }` 复现 ORIG 0x0859e97c
// `cmp $-1; sete %al; test %al,%al; je`（4.4 的 `if` 直形恒折 `jne`，探针
// p22 g6）；②`int i = 0;` + `for (i = 0; ...)` 双初始化复现 ORIG 双
// `movl $0,-0xc`（P191）。残余 N 封顶：ORIG 第二处 `slots[i]` 读复用 %ebx
// （i 缓存）vs 4.4 恒重读 `-0xc(%ebp)`（+1 条，N7/N18a 寄存器分配族；register
// 提示反致槽漂移 -0x1c→-0x28，已回退）。**报告撞名警示：
// docs/class_func_reports/CParty/CheckEnterAncientDungeon.md 是 4 参重载
// 0x0859e528 的内容，本件 ORIG 基线取自二进制 nm 切片 0x0859e7fe..0x0859eac2。**
// 修法已落：`!= 1` 取反形复现 ORIG `xor $1; test; je`（P194；CheckEnterEventDungeon
// 返回 bool）；Main 代落 RequiredItem::m_enableFlag char→bool 后
// `!m_enableFlag` 段与 ORIG xor 形对齐。
#include "CPartyOnlyOrig/CheckEnterAncientDungeon.inc"
{
    Inven_Item items[4];
    int slots[4];
    unsigned char flag;    // ORIG -0x1d：items 顶与 slots 底之间声明于函数作用域
    memset(items, 0, sizeof(items));
    memset(slots, 0, sizeof(slots));

    if (const_cast<CDungeon*>(dungeon)->m_entryRestrictFlag)
    {
        flag = 0;    // ORIG +0x79 分支内清零
        // b28d-solo-CPartyOnlyOrig：ORIG +0x0a7 `xor $0x1,%eax; test %al,%al;
        // je <mov $1>`（P194 `!=1` 形；CheckEnterEventDungeon 返回 bool）。
        // 旧 `==1?1:0` 产 `test %al;je` 直跳（少 xor）。
        if (CheckEnterEventDungeon(dungeon, flag, items, slots) != 1)
        {
            return 0;
        }
        return 1;
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
    // b28d-solo-CPartyOnlyOrig：ORIG 0x0859e8f8/0x0859e8ff 双 `movl $0,-0xc`
    // （4.1 声明初始化 + for 初始化各一条，P191 配套）。
    int i = 0;
    for (i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // [b23S3C] P48 去缓存：ORIG 两用点各自 call getCurCharacInvenR
        // （+0x163 / +0x1b4 两次独立 call，结果 eax 直落实参槽无 inv 槽）。
        slots[i] =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                ->getCurCharacInvenR()
                ->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx);
        // b28d-solo-CPartyOnlyOrig：ORIG 0x0859e97c 为 `cmp $-1; sete %al;
        // test %al,%al; je`（4.1 槽比物化）；4.4 的 `if` 直形恒折 `jne`，
        // `while (cond) { return X; }` 形逐条复现（P225 扩展，探针 p22 g6）。
        while (slots[i] == -1)
        {
            return 0;
        }
        items[i] =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                ->getCurCharacInvenR()
                ->GetInvenSlot(1, slots[i]);
        if (items[i].m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
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
// b28d-solo-CPartyOnlyOrig 判词：**AE** @0x085be56c（clean 口径 ndiff=0，102:102）。
// 修法：P225 拆嵌套 if 复现 4.1 每条件 `setne/setge %al; test %al,%al; je`
// 物化（合并 `&&` 在 4.4 恒折 jl/jne 直跳，探针 p1..p4）+ Main 代落
// CDungeon::getLimitCoinDiff int→bool（ORIG `test %al`）+ 本 TU
// CheckHellCoinLimint int→bool（ORIG `call; xor $1; test %al`）。
#include "CPartyOnlyOrig/checkLimitUsedCoinCondition.inc"
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
    // b28d-solo-CPartyOnlyOrig：P225 拆 && —— ORIG 每条件 `setne/setge %al;
    // test %al,%al; je` 物化；4.4 对合并 `&&` 恒折 jl/jne 直跳（探针 p1/p4），
    // 拆嵌套 if 后逐条同形（探针 p2/p1v）。
    if (GetPartyMemberCoinLimit() != 0)
    {
        if (GetUsedCoinCount() >= GetPartyMemberCoinLimit())
        {
            return 0x16;
        }
    }

    int limitCoin = 0;
    if (dungeon->getLimitCoinDiff(
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->get_dungeon_diff(),
            limitCoin))
    {
        if (limitCoin != -1)
        {
            if (GetUsedCoinCount() >= limitCoin)
            {
                return 0x16;
            }
        }
    }
    if (dungeon->m_87c != -1)
    {
        if (GetUsedCoinCount() >= dungeon->m_87c)
        {
            return 0x16;
        }
    }
    if (dungeon->m_898 != -1)
    {
        if (CheckHellCoinLimint(dungeon->m_898) != 1)
        {
            return 0x16;
        }
    }
    return 0;
}

// ORIG T 0x085be4ae（checkUnLimitUsedCoinCondition.md）：type 0x5e 重复事件
// 处于进行中且副本开启维度次数限制时豁免硬币上限。
#include "CPartyOnlyOrig/checkUnLimitUsedCoinCondition.inc"
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
    // ORIG O@0x76 `test%al; jle`：dim 按有符号 char 判 >0（值域≥0x80 分叉）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x5e)->IsEventing(0)
        && static_cast<signed char>(
               const_cast<CDungeon*>(dungeon)->get_dimension_possible()) > 0)
    {
        return true;
    }
    return false;
}

// ORIG T 0x085a834a（CheckHellCoinLimint.md）：hell 场景且 +0xcd4 开关打开、
// 未清完全部 hell 组时，对照已用硬币数判断是否越过本副本 hell 限额。
// b28d-45 CheckHellCoinLimint 0x085a834a：FIX+N —— Main 已落
// IsKilledAllHellGruoups bool 链（调用点裸取反后 xor 段对齐）；IsEnableHellDungeon
// 按 Main 口径维持 uchar（其体依赖被指字段 uchar，5 个 ORIG 调用点无判别力）⇒
// 本件 4.4 恒多 `setne` 归一段，N 封顶（29:31，语义等价）。
#include "CPartyOnlyOrig/CheckHellCoinLimint.inc"
{
    // P133 拆嵌套守卫；b28d-45：ORIG 0x085a8350 为 `call; test %al,%al; je`
    // 正向直判 ⇒ 改裸 `if(f())`（`> 0` 形 4.4 反产 setne 归一段）。
    if (CBattle_Field::IsEnableHellDungeon())
    {
        if (m_battleField.m_hellPartyMode == 1)
        {
            // b28d-45 P248 档①：Main 已落 IsKilledAllHellGruoups bool 链
            // （ORIG 0x085a8377 `call; xor $0x1; test %al,%al; je`）；
            // IsEnableHellDungeon 按 Main 口径维持 uchar（N 封顶）。
            if (!reinterpret_cast<CBattle_Field*>(&m_battleField)
                     ->IsKilledAllHellGruoups())
            {
                if (GetUsedCoinCount() >= param)
                    return 0;
            }
        }
    }
    return 1;
}

// ORIG U 0x0859a210（is_limit_minimum_age.md）：任一成员不满足年龄下限即 0
// （param 输出违规成员索引由 CUser::CheckLimitMiniumAge 内部填写）。
// [b28c-L7][SIGFIX:CParty::is_limit_minimum_age] int→bool（判词见 CParty.h:671）
#include "CPartyOnlyOrig/is_limit_minimum_age.inc"
{
    for (int i = 0; i <= 3; i++)
    {
        // [判据②直分支形+拆&&] ORIG O@26-31：`call;xor$1;test;je` +
        // `mov$0;jmp` 直返，无 && 物化岛——两条件拆嵌套。
        if (_checkValidUser(i))
        {
            if (m_member[i].m_pUser->CheckLimitMiniumAge(age, param) == 0)
            {
                return false;
            }
        }
    }
    return true;
}

// ORIG U 0x0859a286（is_limit_minimum_level.md）：等级不足者以 uid 回填 param。
// [b28c-L7][SIGFIX:CParty::is_limit_minimum_level] int→bool（同族，ORIG 出口
// `mov $0x0/$0x1,%eax`；mangled 不变）
#include "CPartyOnlyOrig/is_limit_minimum_level.inc"
{
    for (int i = 0; i <= 3; i++)
    {
        // [判据②岛形+拆&&] ORIG O@22-26：`call;cmp;setl%al;test;je`。
        if (_checkValidUser(i))
        {
            // [b28c-L7] P0 清绑：原 `register bool low asm("al")` 与「-O0 局部
            // 恒占栈槽、寄存器仅承载表达式值」相悖（N96 登记口径）；改比较直用
            // 形复测，残差按 P168a④/N18a 判词。
            if (m_member[i].m_pUser->get_charac_level() < level)
            {
                param = m_member[i].m_pUser->get_unique_id() & 0xffff;
                return false;
            }
        }
    }
    return true;
}

// ORIG T 0x085bcab6（CheckMemberLastTryAssaultTime.md）：任一成员 600 秒内
// 有混沌击杀时间记录即 1（禁止立即组队袭击）。
#include "CPartyOnlyOrig/CheckMemberLastTryAssaultTime.inc"
{
    for (int i = 0; i <= 3; i++)
    {
        if (!checkValidUser(i))
        {
            continue;
        }
        // ORIG 0x85bcb2e 域：info 槽(-0x14) 物化 → getCurSec(-0x10) → killTime；
        // 4.4 R→L/寄存器分配异构（单表达式形实测 48 insn 更差，见Lane6探针轮），
        // 残差为槽序/求值序 N82 族，维持双语句最优 43 基线。
        // b28d-45 试形：ORIG 先物化 info 到 -0x14（具名局部）再 getCurSec。
        CUserCharacInfo* info =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser);
        int curSec = GlobalData::s_systemTime_.getCurSec();
        int lastKillTime = info->GetCurCharacChaosKillTime();
        if (curSec < lastKillTime + 600)
        {
            return 1;
        }
    }
    return 0;
}

// ORIG T 0x0859bfe2（increase_dimension_inout_count.md）：副本允许维度进出时，
// 各维度 inout 槽 (dm 上限, 成员现值) 同加 delta 并截断到 dm 上限。
#include "CPartyOnlyOrig/increase_dimension_inout_count.inc"
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
#include "CPartyOnlyOrig/GetExpPenaltyPercent.inc"
{
    if (a < 20)
    {
        unsigned int diff = a - b > 0 ? a - b : b - a;
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
        unsigned int diff = a - b;
        if (5 < diff && diff < 10)
        {
            return (10 - diff) * 20;
        }
        if (9 < (a - b))
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
#include "CPartyOnlyOrig/RestartDungeon.inc"
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
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE。修法：map_item::m_count uchar→bool
// （WarField.h，R2 已广播）+ 裸取反 `!item.m_count` ⇒ 复现 ORIG
// `movzbl -0x60; xor $1,%eax; test %al,%al; je`（P248 档②；uchar 字段下
// `(m_count^1)!=0`/`!=1` 七写法探针全折 `cmpb $1`）。
#include "CPartyOnlyOrig/isMonsterDrop_item.inc"
{
    map_item item;
    // ORIG 0x085a3b44 物化形：found 落 char 槽 -0x9 后 `cmpb $0;je` 直判
    // 需 unsigned char 声明（bool `!`/`==0` 出 movzbl;xor;test，4.1 vs 4.4
    // 指令选择差）；二段 m_count 真值取反 `movzbl;xor$1;test;je`。
    unsigned char found = reinterpret_cast<CBattle_Field*>(&m_battleField)
                              ->get_field_item(monsterIdx, item);
    // ORIG O@0x33..0x4b：found!=0 先 cmpb 直判（跳过 body → 返 1 岛），
    // 体内 m_count 真值取反 movzbl;xor$1;test（两 je 同达返 1 岛）。
    // b28d-solo-CPartyOnlyOrig：P0 判读 —— ORIG 第二判定全程寄存器直传
    // （无槽往返）⇒ 原码无 cnt 局部，取反表达式内联进短路条件。
    if (found != 0 && !item.m_count)
    {
        return false;
    }
    return true;
}

// [N-类·帧布局] CParty::VoteLackUser（本 TU 声明对应定义在 CPartyDepsD2）
// 剩 31 全为槽文本差（4.4 帧底保留区致 i/temp/iterator 槽整体深 4B）+
// ebx/esi 角色互换（P78 禁追），语义已双证正确，勿再投预算。

// ORIG T 0x085a3b6a（kill_blood_monster.md）：血轮击杀转发战场层。
#include "CPartyOnlyOrig/kill_blood_monster.inc"
                                unsigned short* param)
{
    reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->kill_blood_monster(user, monsterIdx, param);
}

// ORIG T 0x085b6540（DestroyPassiveObject.md）：摧毁被动物件并在触发清除条件
// 时结算整场。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→修复 @0x085b6540（gap 1→0）。4.4 对普通
// bool 局部条件恒折 `cmpb $0,-0x9(%ebp); je`，ORIG 4.1 为 `movzbl -0x9(%ebp),%eax;
// test %al,%al; je`（P322② volatile 强占形）。探针 /tmp/probe_hnq/dp3.cpp v9
// （`volatile bool destroyed` + `if (destroyed)` + `return (unsigned char)destroyed`）
// 逐条复现 ORIG 29 条（含尾部 movzbl 返回）；普通 bool（v11）仍产 cmpb。
// 旧判词「uchar/bool/int 局部 × 五种条件形全产 cmpb」（探针 d1..d5/p23 f1..f5）
// 对 volatile 形不成立，已作废。
#include "CPartyOnlyOrig/DestroyPassiveObject.inc"
{
    // [ORIG 0x085a3b6a 物化形] 声明初始化 0 + call 后赋值两段（movb$0 再
    // movzbl reload）+拆&&嵌套。b28d-solo-CPartyOnlyOrig：ORIG O@0x25 为
    // `movzbl -0x9,%eax; test %al,%al; je`（零扩展回读 + 直测，P292 字段/局部
    // bool 形）⇒ 局部 bool + volatile（volatile 令 4.4 每个使用点重载并保留
    // movzbl/test 直测形，探针 dp3 v9）。
    volatile bool destroyed = false;
    destroyed = static_cast<bool>(
        reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->DestroyPassiveObject(param));
    if (destroyed)   // ORIG O@0x25 movzbl reload+test
    {
        if (reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->ClearCondition(
                    static_cast<ENUM_DUNGEON_CLEAR_CONDITION_TYPE>(0), param))
        {
            ClearDungeon();
        }
    }
    return static_cast<unsigned char>(destroyed);
}

// ORIG T 0x085b9f28（SetRidableObjectState.md）：坐骑状态转发战场层（返回值穿透）。
#include "CPartyOnlyOrig/SetRidableObjectState.inc"
{
    return reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->SetRidableObjectState(state, type);
}

// ============================================================================
// TOD / 血轮复活
// ============================================================================

// ORIG T 0x085ba906（OnTODRewardTimer.md）：0x2c 报文发送 TOD 结算奖励包，
// EPLP 置 2、注册 0x15 层推进定时器后回到 state1，并注册 8 秒后的 0x2c 重投递。
#include "CPartyOnlyOrig/OnTODRewardTimer.inc"
                                                             // P85：register 形参恢复 ORIG mov;cmp 物化
{
    if (msg == 0x2c)
    {
        _Send_TOD_ClearReward();
        SetEPLPState(2);
        // b28d-45 P0/N96 清绑：`register int key15 asm("esi")` 删除，改 P200
        // 内联实参形（ORIG O@19 `mov%eax,%esi` 由自然升格承载）。
        G_TimerQueue()->InsertTimer(
            static_cast<TimerEntry::OBJ_TYPE>(1), GetPartyIndex(),
            static_cast<TIMER_MESSAGE>(0x15), 0x3c,
            gen_timer_key(static_cast<TIMER_MESSAGE>(0x15)), 0);
        set_state(1);
        G_TimerQueue()->InsertTimer(
            static_cast<TimerEntry::OBJ_TYPE>(1), GetPartyIndex(),
            static_cast<TIMER_MESSAGE>(0x2c), 8,
            gen_timer_key(static_cast<TIMER_MESSAGE>(0x24)), 0x24);
    }
    return 1;
}

// ORIG T 0x085a22a8（SelectEPLPCommand.md）：EPLP 选令：置状态 3 并按指定秒数
// 注册 0x14 定时器。
#include "CPartyOnlyOrig/SelectEPLPCommand.inc"
{
    SetEPLPState(3);
    // b28d-45 P0/N96 清绑：改 P200 内联实参形。
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(1),
                                GetPartyIndex(), static_cast<TIMER_MESSAGE>(0x14),
                                cmd,
                                gen_timer_key(static_cast<TIMER_MESSAGE>(0x14)), 0);
}

// ORIG T 0x085baa06（_Send_TOD_ClearReward.md）：0x15b 包——管理者的 layer
// 扩展数据最后进入层 + 最多 9 组奖励清单（不足补 -1/0）+ 特殊奖励索引/数量。
// b28d-solo-CPartyOnlyOrig 判词：DIFF（N 封顶）@0x085baa06。修法已落：具名
// userState 指针落槽（P0 槽往返）+ 匿名 sret 全表达式链（P278）+
// `m_battleData.m_field0` 具名成员访问 ⇒ packet 槽 -0x24 与 ORIG 对齐。
// 残余 N 封顶：4.1 `mov 0x330(%eax),%edx` 冗余装载（N18a）与首循环前
// `call size; cmp $9` 死比较（4.1 tail-duplicator 产物）。语义等价。
#include "CPartyOnlyOrig/_Send_TOD_ClearReward.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x15b);
    // b28d-solo-CPartyOnlyOrig：具名成员（ORIG +0x32c disp 直读；裸
    // `*(int*)&m_battleData` 产 `add $0x32c; mov (%eax)` 两条）。ORIG 另有
    // `mov 0x330(%eax),%edx` 冗余装载（4.1 产物，N18a 封顶）。
    packet.put_int(m_battleData.m_field0);   // ORIG *(int*)(this+0x32c)
    // b28d-solo-CPartyOnlyOrig：ORIG +0x67 `mov %eax,-0x14` 指针落槽（P0 槽
    // 往返 ⇒ 原码有具名指针局部，此前留 edx 无槽）⇒ 补具名 userState；
    // +0x77 getLastEnterLayer 用 sret 匿名临时 -0x16 后直接 GetLayer
    //（P278 匿名 sret 全表达式链）。ORIG 无条件（无 null 检查）。
    TOD_UserState* userState = reinterpret_cast<TOD_UserState*>(
        getManager()->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
    packet.put_short(static_cast<unsigned short>(
        userState->getLastEnterLayer().GetLayer()));
    packet.put_byte(10);   // ORIG +0x09c 固定 byte 10
    // [P171/P184 死块照抄] ORIG +0x0af..+0x0bf：一次 size() 调用 + `cmp $0x9,%eax`
    // （无分支消费者）。探针 /tmp/probe_hnq/t.cpp h8 实证：空 if 体被 4.4 前端删除
    // 但比较与调用保留，逐条同形（`> 9` 无符号比较）。
    if (reinterpret_cast<CBattle_Field*>(&m_battleData)
            ->m_todReward.m_todMap.size() > 9)
    {
    }
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
#include "CPartyOnlyOrig/ReviveUser.inc"
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

// RidableObject::clear 定义置于本 TU（与 CDungeon.cpp 中的 C1 调用者分离）：
// 4.4 unit-at-a-time 对 TU 内可见 callee 塌缩 outgoing 帧（C1 sub $0x4），
// ORIG 4.1 恒发 sub $0x18（探针 /tmp/nl4p/p6.cpp vs p7.cpp 同族实证）。
#include "CPartyOnlyOrig/clear.inc"
{
    // ORIG 0x830e47c：前 5 个字段写 -1，+0x14（m_field18）写 0xc8（200）。
    m_x = -1;
    m_y = -1;
    m_field0c = -1;
    m_field10 = -1;
    m_field14 = -1;
    m_field18 = 0xc8;
}
