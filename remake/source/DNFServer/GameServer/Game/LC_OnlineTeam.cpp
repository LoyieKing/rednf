// df_game_r 还原 —— LinkClosureC 批次：online_preliminary::COnlinePreliminaryTeamMgr 两方法。
// ORIG 依据：
//   FindTeam           T 0x0858875e（报告 online_preliminary__COnlinePreliminaryTeamMgr/FindTeam.md）
//   ResetDailyMidnight T 0x08588c5c（同目录 ResetDailyMidnight.md）
#include "OnlinePreliminary.h"

// ORIG 0x0858875e：m_teamMap(+0x04) find(teamId)；命中返回 second，否则 NULL。
online_preliminary::COnlinePreliminaryTeam*
online_preliminary::COnlinePreliminaryTeamMgr::FindTeam(int teamId)
{
    std::map<int, COnlinePreliminaryTeam*>::iterator it = m_teamMap.find(teamId);
    // ORIG je→zero-island、成功臂 fall-through ⇒ 正极性 if（P47 镜像律试形）
    if (it != m_teamMap.end())
        return it->second;
    return 0;
}

// ORIG 0x08588c5c：遍历 m_teamMap，逐 team 调 COnlinePreliminaryTeam::ResetDailyMidnight。
void online_preliminary::COnlinePreliminaryTeamMgr::ResetDailyMidnight()
{
    for (std::map<int, COnlinePreliminaryTeam*>::iterator it = m_teamMap.begin();
         it != m_teamMap.end(); ++it)
    {
        it->second->ResetDailyMidnight();
    }
}

// ============================================================================
// 头依赖：包构造（PacketGuard/InterfacePacketBuf）、CUser/CUserCharacInfo、
// 流/消息队列（CStreamGuard/StreamPool/MsgQueueMgr/GlobalData）、
// 定时器（TimerDispatcher.h）与 GameWorld 广播。
// ============================================================================
#include <string.h>

#include "CUser.h"
#include "CUserCharacInfo.h"   // SetCurCharacOnlinePreliminaryTeam /
                               // GetCurCharacOnlinePreliminaryTeamId
#include "PacketGuard.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "TimerDispatcher.h"   // Timer_UpdateOnlinePreliminaryTeam::RegistNextTimer
#include "GameWorld.h"         // G_GameWorld / send_all

// ORIG 0x085887c8 T
// _ZN18online_preliminary25COnlinePreliminaryTeamMgr11OnEnterUserEP5CUserPc
//（报告 online_preliminary__COnlinePreliminaryTeamMgr/OnEnterUser.md）。
// 键语义：teamData 头部 int teamId —— find/make_pair 的 const int& 参数在
// ABI 上直接复用 teamData 指针解引用（ORIG 0x01a 传 edx=teamData 作引用）。
// [b28d-54 N] ORIG 0x085887c8：帧/槽整体平移（N82）+ map 迭代器临时槽（N86）；
//   调用序列全等。
bool online_preliminary::COnlinePreliminaryTeamMgr::OnEnterUser(CUser* user, char* teamData)
{
    int teamId = *reinterpret_cast<int*>(teamData);
    std::map<int, COnlinePreliminaryTeam*>::iterator it = m_teamMap.find(teamId);
    // [L6 round3 · p167 + Ghidra 证据] ORIG 0x0858881a 用
    // `_Rb_tree_iterator::ne`（!=）+ `test %al,%al; je <+0x93>`（Ghidra：
    // `cVar2 = operator!=; if (cVar2 == '\0') { Acquire 新建分支 }`）⇒ 原码是
    // **`if (it != end) { 已存在分支 } else { 新建分支 }`**（块序 = 已存在在前）。
    // 旧 `if (it == end) { 新建 } else { 已存在 }` 产 `eq` + 反向块序。
    if (it != m_teamMap.end())
    {
        user->SetCurCharacOnlinePreliminaryTeam(it->second);   // 0x073
        it->second->IncreaseCurrentUserCount();                // 0x08588d96
    }
    else
    {
        COnlinePreliminaryTeam* team = m_factory->Acquire();   // 0x08588dd8（this+0）
        if (team == 0)
            return false;                                      // ORIG ebx=0 返回
        team->Reset();                                         // 0x0858838a
        user->SetCurCharacOnlinePreliminaryTeam(team);         // 0x08588dba（this=user）
        team->SetCurrentUserCount(1);                          // 0x08588d88
        team->loadDataPreliminaryTeam(teamData);               // 0x085882b8
        m_teamMap.insert(std::make_pair(teamId, team));        // 0x10a..0x13b

        // 日志流：StreamPool::Acquire("OnlinePreliminaryTeam.cpp",0x106)，
        // 流序 << 0x1ec << -1 << teamId → MsgQueueMgr::put(DB_Q, guard)。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("OnlinePreliminaryTeam.cpp", 0x106), true);
        // [L6 round3 · p167 MISSING `_ZN12CStreamGuarddeEv`×3] ORIG 三个 << 各先调
        // `CStreamGuard::operator*()`（P132：`(*guard) << x` 代理链），成员直调 `guard<<x`
        // 不发 de() 调用点 ⇒ 显式 operator*()->operator<< 形。
        guard.operator*()->operator<<(0x1ec);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<(teamId);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        Timer_UpdateOnlinePreliminaryTeam::RegistNextTimer(teamId);  // 0x0863977e
    }

    // 全服广播：header(0,0x118) | short(uniqueId&0xffff) | int(teamId)
    //          | int(strlen(teamData+4)) | str(队名) | finalize(true)。
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x118);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_int(teamId);
    packet.put_int(static_cast<int>(strlen(teamData + 4)));
    packet.put_str(teamData + 4, static_cast<int>(strlen(teamData + 4)));
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return true;
}

// ORIG 0x08588ae8 T
// _ZN18online_preliminary25COnlinePreliminaryTeamMgr11OnLeaveUserEP5CUser
//（报告 OnLeaveUser.md）。
// [b28d-54 N] ORIG 0x08588ae8：槽整体平移 4（N82/N95）+ 迭代器/临时槽（N86）；
//   调用序列全等。
void online_preliminary::COnlinePreliminaryTeamMgr::OnLeaveUser(CUser* user)
{
    int teamId = user->GetCurCharacOnlinePreliminaryTeamId();  // 0x08645de2
    std::map<int, COnlinePreliminaryTeam*>::iterator it = m_teamMap.find(teamId);
    // [L6 round3 · p167 实测] ORIG 0x08588b40-0x08588b47 用
    // `_Rb_tree_iterator::ne`（!=）+ `test %al,%al; je <尾>` ⇒ **正极性包裹**
    // `if (it != end) { 体 }`（旧 `if (it == end) return;` 产 eq + jne 反向）。
    // 且 ORIG 逐点重算 `it->second`（`ptEv`×4：GetCurrentUserCount/saveData/Free/
    // DecreaseCurrentUserCount 各一次；我方 1 次 + MISSING 3 次）⇒ 删 `team` 具名缓存
    // （P183/P167，且其 -0xc 槽与 ORIG 的 end 临时槽 -0xc 冲突）。
    if (it != m_teamMap.end())
    {
        user->SetCurCharacOnlinePreliminaryTeam(0);            // 0x08588dba(team=0)
        if (it->second->GetCurrentUserCount() == 1)            // 0x08588d7e
        {
            it->second->saveData();                            // 0x08588d60
            m_factory->Free(it->second);                       // 0x08588dec（this+0）
            m_teamMap.erase(it);                               // 0x08589836
        }
        else
        {
            it->second->DecreaseCurrentUserCount();            // 0x08588da8
        }
    }
}
