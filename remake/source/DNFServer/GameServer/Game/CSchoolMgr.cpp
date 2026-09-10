// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSchoolMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CSchoolMgr（学院 PvP 频道用户/学院分区管理器，G2-4 批次）。
// 逐函数对照 docs/class_func_reports/CSchoolMgr.md 与 ORIG 反汇编实现。
// 布局（ORIG ctor 0x856f206 / dtor 0x856f0ba 推导）：
//   CSchoolMgr ：+0x00 std::map<uint,uint> m_schoolUserCount
//                +0x18 CSchoolArea m_schoolArea（sizeof 0x28）
//   CSchoolArea：+0x00 std::map<uint,int> m_areaMap
//                +0x18 std::stack<int> m_areaStack
//   CSchoolMember：CBelong(+0 vptr) + CUser*（sizeof 8）；IsRight 覆写纯虚。
// 外部依赖（RDARScriptStringManager / GameWorld::send_group / CUser 访问器）
// 以 asm-label extern 或本地视图声明对齐 ORIG 符号。
#include <stdio.h>
#include <string.h>

#include <map>
#include <stack>
#include <deque>

#include "InterfacePacketBuf.h"
#include "CEnvironment.h"
#include "GameWorld.h"
#include "CSchoolMgr.h"
#include "RDARScriptStringManager.h"
#include "PacketGuard.h"

// ---------------------------------------------------------------------------
// 外部符号声明（实现属其它 TU）
// ---------------------------------------------------------------------------
class CUser;

// ---------------------------------------------------------------------------
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char）
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CSchoolArea / CSchoolMgr 类声明见 CSchoolMgr.h（唯一声明点）

// ============================================================================
// CSchoolArea 实现
// ============================================================================

#include "CSchoolMgr/CSchoolArea.inc"
    : m_areaMap(), m_areaStack()
{
}

#include "CSchoolMgr/dtor_CSchoolArea.inc"
{
}

#include "CSchoolMgr/GetAreaIndex.inc"
{
    if (m_areaStack.empty())
    {
        return -1;
    }
    int areaIdx = m_areaStack.top();
    m_areaStack.pop();
    return areaIdx;
}

#include "CSchoolMgr/SetAreaMapping.inc"
{
    m_areaMap.insert(std::make_pair(schoolNo, areaIdx));
}

#include "CSchoolMgr/AddSchoolArea.inc"
{
    int areaIdx = GetAreaIndex();
    if (areaIdx >= 0)
    {
        SetAreaMapping(schoolNo, areaIdx);
    }
}

#include "CSchoolMgr/DelSchoolArea.inc"
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(schoolNo);
    if (it != m_areaMap.end())
    {
        m_areaStack.push(it->second);
        m_areaMap.erase(it);
    }
}

#include "CSchoolMgr/SetSchoolArea.inc"
{
    for (int area = start; area < end; ++area)
    {
        m_areaStack.push(area);
    }
}

#include "CSchoolMgr/GetAreaIndex_1.inc"
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(schoolNo);
    if (it != m_areaMap.end())
    {
        return it->second;
    }
    return -1;
}

// ============================================================================
// CBelong / CSchoolMember 实现
// ============================================================================

#include "CSchoolMgr/CSchoolMember.inc"
    : CBelong(), m_user(user)
{
}

#include "CSchoolMgr/IsRight.inc"
{
    // N96 清绑：原 `register bool right asm("eax")` 已删。ORIG 无 right 槽
    // （&& 短路直落 mov$1/mov$0 岛）⇒ 还原为单 return 短路形。清绑前后 25=25、
    // 与 ORIG（objdump 0x085690dc，报告缺失）逐条全等（仅 .o 重定位地址差）
    // ⇒ 残差 0，无需 N96+N18a 封顶。
    return m_user->GetSchoolNo() == user->GetSchoolNo() && m_user != user;
}

// ============================================================================
// CSchoolMgr 实现
// ============================================================================

#include "CSchoolMgr/CSchoolMgr.inc"
    : m_schoolUserCount(), m_schoolArea()
{
}

#include "CSchoolMgr/dtor_CSchoolMgr.inc"
{
}

#include "CSchoolMgr/AddUser.inc"
{
    if (!G_GameWorld()->IsSchoolPvPChannel())
    {
        return;
    }
    SendBroadcastUserIn(user);
    unsigned int schoolNo = user->GetSchoolNo();
    std::map<unsigned int, unsigned int>::iterator it =
        m_schoolUserCount.find(schoolNo);
    if (it != m_schoolUserCount.end())
    {
        it->second += 1;
    }
    else
    {
        m_schoolUserCount.insert(std::make_pair(user->GetSchoolNo(), 1));
        m_schoolArea.AddSchoolArea(user->GetSchoolNo());
    }
}

#include "CSchoolMgr/DelUser.inc"
{
    if (!G_GameWorld()->IsSchoolPvPChannel())
    {
        return;
    }
    SendBroadcastUserOut(user);
    unsigned int schoolNo = user->GetSchoolNo();
    std::map<unsigned int, unsigned int>::iterator it =
        m_schoolUserCount.find(schoolNo);
    if (it != m_schoolUserCount.end())
    {
        it->second -= 1;
        if (it->second == 0)
        {
            m_schoolUserCount.erase(it);
            m_schoolArea.DelSchoolArea(user->GetSchoolNo());
        }
    }
}

#include "CSchoolMgr/SendBroadcastUserIn.inc"
{
    const char* channelMsg = g_scriptStringManager_.findString(
        4, "channel_connect", 0);
    if (channelMsg != 0)
    {
        char buf[255] = {0};
        sprintf(buf, channelMsg, user->getCurCharacName(),
                G_CEnvironment()->get_channel_no());
        PacketGuard guard;
        guard.put_header(0, 0xc);
        guard.put_byte(0);
        guard.put_short(0);
        guard.put_byte(0);
        guard.put_int(strlen(buf));
        guard.put_str(buf, strlen(buf));
        guard.finalize(true);
        CSchoolMember member(user);
        G_GameWorld()->send_group(guard, &member);
    }
}

#include "CSchoolMgr/SendBroadcastUserOut.inc"
{
    const char* channelMsg = g_scriptStringManager_.findString(
        4, "channel_disconnect", 0);
    if (channelMsg != 0)
    {
        char buf[255] = {0};
        sprintf(buf, channelMsg, user->getCurCharacName());
        PacketGuard guard;
        guard.put_header(0, 0xc);
        guard.put_byte(0);
        guard.put_short(0);
        guard.put_byte(0);
        guard.put_int(strlen(buf));
        guard.put_str(buf, strlen(buf));
        guard.finalize(true);
        CSchoolMember member(user);
        G_GameWorld()->send_group(guard, &member);
    }
}

#include "CSchoolMgr/GetUserCount.inc"
{
    std::map<unsigned int, unsigned int>::iterator it =
        m_schoolUserCount.find(schoolNo);
    if (it != m_schoolUserCount.end())
    {
        return it->second;
    }
    return 0;
}

#include "CSchoolMgr/SetSchoolArea_1.inc"
{
    m_schoolArea.SetSchoolArea(start, end);
}

#include "CSchoolMgr/GetSchoolArea.inc"
{
    return m_schoolArea.GetAreaIndex(schoolNo);
}
