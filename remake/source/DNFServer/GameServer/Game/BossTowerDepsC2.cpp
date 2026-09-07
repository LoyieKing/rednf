// ============================================================================
// BossTowerDepsC2.cpp —— TowerOfDespair / pvp_assault 强制移动域依赖恢复
// （BossTowerDepsC2 批次，2026-08-24；独占文件，Main 统一登记 CMake）
//
// 逐份完整读取报告（## 汇编 + ## 反编译 C 全文）后转写：
//   docs/class_func_reports/TowerOfDespairMgr/CheckEnterable.md
//       ORIG T 0x086440ee _ZN17TowerOfDespairMgr14CheckEnterableEP5CUserPK8CDungeon
//   docs/class_func_reports/TOD_UserState/getEnterCount.md
//       ORIG T 0x08643872 _ZNK13TOD_UserState13getEnterCountEv
//   docs/class_func_reports/TOD_UserState/getLastClearTime.md
//       ORIG T 0x0864387e _ZNK13TOD_UserState16getLastClearTimeEv
//   docs/class_func_reports/pvp_assault__CAssaultMgr/ForcedMoveAtPowerWarPlace.md
//       ORIG T 0x082ed672 _ZN11pvp_assault11CAssaultMgr25ForcedMoveAtPowerWarPlaceEP5CUser
//   docs/class_func_reports/pvp_assault__CAssaultPlace/ForcedMoveAtPowerWarPlace.md
//       ORIG T 0x082ed6a6 _ZN11pvp_assault13CAssaultPlace25ForcedMoveAtPowerWarPlaceEP5CUser
//   docs/class_func_reports/pvp_assault__CForcedMoveCharacter/{README,ForcedMove}.md
//       ORIG T 0x082e567c _ZN11pvp_assault20CForcedMoveCharacter10ForcedMoveEv
//   （TowerOfDespairMgr::SendAPCInfo ORIG T 0x08644338 已由 TowerOfDespairMgr.cpp
//     按 SendAPCInfo.md 恢复——本 TU 不重复定义。）
//
// 调用方（此前 blocked）：
//   CPartyDepsA2.cpp GetDungeonIndex_isTOD —— tod->CheckEnterable(manager, target)
//   CParty.cpp:2736 walkout_member         —— mgr->ForcedMoveAtPowerWarPlace(target)
//   DisPatcherCore.cpp DisPatcher_LeaveParty::process（原 BLOCKED 注释即本链）
// ============================================================================

#include "CDataManager.h"            // DataManagerInitDepends.h 前置要求
#include "DataManagerInitDepends.h"  // TowerOfDespairMgr/TOD_Layer/TOD_UserState 权威布局

#include "CAssaultMgr.h"             // pvp_assault::CAssaultMgr 声明
#include "CGameManager.h"            // pvp_assault::CAssaultPlace 完整类型
#include "CUser.h"                   // CUser 方法集（含 CUserCharacInfo 继承面）
#include "CDungeon.h"                // CDungeon::m_entryRequiredItem（RequiredItem）
#include "CInventory.h"              // check_item_exist/GetInvenSlot/Inven_Item
#include "CParty.h"                  // CParty::get_member_count
#include "CPowerManager.h"           // CPowerManager::IsPowerWarEventOn
#include "GlobalData.h"              // s_systemTime_/s_power_manager
#include "CEnvironment.h"            // G_CEnvironment()->m_serverEnvir.m_fatigueTime
#include "GameWorld.h"               // move_area/send_to_area/G_GameWorld
#include "PacketGuard.h"

// ORIG _Z22CheckDailyScheduleTimeill T 0x0846c0a8；定义 GameUtils.cpp:88。
// 与 CBoosterGage.cpp:122 / CGameManager.cpp:114 同款局部声明口径（无权威头）。
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime);

namespace pvp_assault
{

// ============================================================================
// pvp_assault::CForcedMoveCharacter —— 强制移动请求值对象（仅本 TU 使用，
// 类定义置于 .cpp 内避免跨 TU ODR 风险；后续 CExtraMatrix/CDynamicDisposition/
// CStaticDisposition::MoveUser(CForcedMoveCharacter*) 批次可迁至权威头）。
//
// 布局依据 ForcedMove.md 汇编读取偏移（this+0x08..+0x1c）；ORIG 唯一构造点
// （CAssaultPlace::ForcedMoveAtPowerWarPlace 栈上组装）不调用任何构造函数、
// 无 vtable 写入证据，故 +0x00..+0x07 未建模。
// ============================================================================
// pvp_assault::CForcedMoveCharacter 权威完整定义见 CAssaultMgr.h（本 TU 本地镜像已删，
// 避免与全域权威头 ODR 冲突）。以下 ForcedMove ORIG T 0x082e567c 报告全文转写。
void CForcedMoveCharacter::ForcedMove()
{
    if (m_user == 0)
    {
        return;
    }

    // ORIG：%al 标志直传（无内存 bool 中转）。先比村庄（movsbl 后有符号
    // 比较），不等即走 move_area；相等再比 get_area(false)，均等才原地
    // set_position。
    int needMove = 0;
    if (static_cast<int>(static_cast<signed char>(m_user->getCurCharacVill())) !=
        m_destVill)
    {
        needMove = 1;
    }
    else if (m_user->get_area(false) != m_destArea)
    {
        needMove = 1;
    }

    if (needMove)
    {
        G_GameWorld()->move_area(m_user, m_destVill, m_destArea, m_x, m_y,
                                 m_dir, false, 0, 0, 0);
    }
    else
    {
        m_user->set_position(static_cast<unsigned short>(m_x),
                             static_cast<unsigned short>(m_y),
                             static_cast<char>(m_dir));
    }

    PacketGuard packet;
    packet.put_header(0, 0x8a);
    packet.put_short(
        static_cast<int>(static_cast<unsigned short>(m_user->get_unique_id())));
    packet.put_short(m_x);
    packet.put_short(m_y);
    packet.put_byte(m_dir);
    packet.finalize(true);

    if (m_destVill != 0 || m_destArea != 0)
    {
        G_GameWorld()->send_to_area(m_destVill, m_destArea, packet);
    }
}

// ============================================================================
// CAssaultPlace::ForcedMoveAtPowerWarPlace —— ORIG T 0x082ed6a6（静态成员，
// asm 无 this 槽）。报告全文转写：IsPowerWarEventOn() 且村庄==7 才处理；
// getPowerSide 两次独立调用（ORIG 指令流如此）；非 1/2 时其余字段不写
// （保持未初始化栈值，与 ORIG 一致）。
// ============================================================================
void CAssaultPlace::ForcedMoveAtPowerWarPlace(CUser* user)
{
    int enter = 0;
    if (GlobalData::s_power_manager->IsPowerWarEventOn())
    {
        if (user->getCurCharacVill() == 7)
        {
            enter = 1;
        }
    }
    if (!enter)
    {
        return;
    }

    CForcedMoveCharacter request;   // ORIG 栈对象，无构造调用
    request.m_user = user;
    if (user->getPowerSide() == 1)
    {
        request.m_destVill = 7;
        request.m_destArea = 0;
        request.m_x = 0x255;
        request.m_y = 300;
    }
    else if (user->getPowerSide() == 2)
    {
        request.m_destVill = 7;
        request.m_destArea = 4;
        request.m_x = 0x139;
        request.m_y = 0x13a;
    }
    request.ForcedMove();
}

// ============================================================================
// CAssaultMgr::ForcedMoveAtPowerWarPlace —— ORIG T 0x082ed672（报告全文转写）：
//   try { CAassaultPlace::ForcedMoveAtPowerWarPlace(user); } catch(...) {}——
//   landing pad 取异常指针入局部后 __cxa_end_catch 吞掉。
// ============================================================================
void CAssaultMgr::ForcedMoveAtPowerWarPlace(CUser* user)
{
    try
    {
        CAssaultPlace::ForcedMoveAtPowerWarPlace(user);
    }
    catch (void* caughtException)
    {
        // ORIG landing pad：__cxa_begin_catch 取异常对象内容（指针值）入栈
        // 局部后 __cxa_end_catch 吞掉——按 void* 值捕获同型。
        (void)caughtException;
    }
}

}  // namespace pvp_assault

// ============================================================================
// TOD_UserState::{getEnterCount,getLastClearTime} —— ORIG T 0x08643872/0x0864387e
// （两报告均为 2 条指令字段读；声明/字段见 DataManagerInitDepends.h）。
// ============================================================================
int TOD_UserState::getEnterCount() const
{
    return m_data.m_enterCount;      // ORIG：*(int*)(this+0x10)
}

int TOD_UserState::getLastClearTime() const
{
    return m_data.m_lastClearTime;   // ORIG：*(int*)(this+0x1c)
}

// ============================================================================
// TowerOfDespairMgr::CheckEnterable —— ORIG T 0x086440ee（报告全文转写）：
// [b23S3C] N：帧重排+跨 call 寄存器缓存（N82/N18a 族），语义等价已核（详见 b22_s3_verdicts.tsv）
//   user 空 → 0x15；level < m_minLevel(+0x34e) → 0xe；
//   TOD_UserState(getCharacExpandData 0xd).getEnterCount()>0 时：
//     非 GM 且 !CheckDailyScheduleTime(env.m_fatigueTime(+0x37c),
//     lastClearTime, curSec) → 0xf5；
//     check_item_exist(requiredItem1.m_0)==-1 → 0xf5；
//     GetInvenSlot(1,slot).get_add_info()(+0x07) < requiredItem1.m_4 → 0x11；
//   party 空且未走上方分支 → 0x15；member_count>1 → 0xab；否则 0。
//   （env+0x37c = Server_Envir::m_fatigueTime，CEnvironment.h 权威布局。）
// ============================================================================
int TowerOfDespairMgr::CheckEnterable(CUser* user, const CDungeon* dungeon)
{
    if (user == 0)
    {
        return 0x15;
    }

    if (user->get_charac_level() < static_cast<int>(m_minLevel))
    {
        return 0xe;
    }

    TOD_UserState* state = reinterpret_cast<TOD_UserState*>(
        user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
    if (state->getEnterCount() > 0)
    {
        if (!user->isGMUser())
        {
            long curSec = GlobalData::s_systemTime_.getCurSec();
            long lastClearTime = state->getLastClearTime();
            if (!CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                                        lastClearTime, curSec))
            {
                return 0xf5;
            }
        }

        const CInventory* inven = user->getCurCharacInvenR();
        int slot = inven->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx);
        if (slot == -1)
        {
            return 0xf5;
        }

        Inven_Item slotItem = inven->GetInvenSlot(1, slot);
        // ORIG：cmp %eax,%edx 后 jge 直跳（无 setl 链）——用 !(a>=b) 形态。
        if (!(slotItem.get_add_info() >= dungeon->m_entryRequiredItem.m_itemCount))
        {
            return 0x11;
        }
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return 0x15;
    }
    if (party->get_member_count() > 1)
    {
        return 0xab;
    }
    return 0;
}
