// ============================================================================
// advancealtar_Manager.cpp —— advancealtar::Manager::{selectStage,
// procErrorSelectStage} 真实实现（Prod_GlobalAdv 2026-08-24）。
//
// 证据：docs/class_func_reports/advancealtar__Manager/{selectStage,
// procErrorSelectStage}.md（ORIG 汇编+反编译 C 全文已读）。
// 阻塞符号（声明就位、定义待批次）：StageControl::selectStage(0x0812fa7a)。
// ============================================================================
#include "advancealtar.h"     // advancealtar::Manager / StageDifficulty / ENUM_ERROR
#include "CGameManager.h"     // G_CGameManager / getAdvanceAltar / StageControl 完整声明
#include "PacketGuard.h"      // PacketGuard : InterfacePacketBuf（put_header/put_byte/put_int/finalize）
#include "GameTypes.h"        // cMyTrace
#include "CUser.h"            // CUser 完整类型（m_field8d012 字段化写）
#include "advancealtar_ProcStage.h"  // ProcStage / Timer_StageControl / HistoryLog（本批新增叶子头）

namespace advancealtar
{

// ----------------------------------------------------------------------------
// ORIG T 0x0813090e _ZN12advancealtar7Manager11selectStageEP5CUseriNS_15StageDifficulty1TE
// user 判空 → 3；难度越界(<0||>2) → 8；getAdvanceAltar() 判空 → cMyTrace +
// "StaticPool Defence DungeonNULL" → 3；
//   StageControl::selectStage != 0 → control->destroy() 后透传错误码；
//   == 0 → setAdvanceAltarIndex(control->getIndex())（ORIG 经 CUser::setAdvanceAltarIndex
//   W 0x0813490e 写 m_field8d012，此处字段化等价写）→ 返回 0。
// ----------------------------------------------------------------------------
int Manager::selectStage(CUser* user, int stageId, StageDifficulty::T diff)
{
    if (user == 0)
    {
        return 3;
    }
    if (diff < 0 || diff > 2)
    {
        return 8;
    }

    StageControl* control = G_CGameManager()->getAdvanceAltar();
    if (control == 0)
    {
        cMyTrace trace(
            "static ENUM_ERROR advancealtar::Manager::selectStage(CUser*, int, advancealtar::StageDifficulty::T)",
            0x6ab, 5);
        trace("StaticPool Defence DungeonNULL");
        return 3;
    }

    const int err = control->selectStage(user, stageId, diff);
    if (err != 0)
    {
        control->destroy();
        return err;
    }
    user->m_field8d012 = static_cast<short>(control->getIndex());  // ORIG setAdvanceAltarIndex(+0x8d012)
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081309f0 _ZN12advancealtar7Manager20procErrorSelectStageEP5CUser10ENUM_ERROR
// user 判空或 err==0 直接返回；否则 PacketGuard: put_header(1,0x10) /
// put_byte(0) / put_int(err) / finalize(true) → CUser::Send。
// ----------------------------------------------------------------------------
void Manager::procErrorSelectStage(CUser* user, ENUM_ERROR err)
{
    if (user == 0 || err == 0)
    {
        return;
    }

    PacketGuard guard;
    guard.put_header(1, 0x10);
    guard.put_byte(0);
    guard.put_int(static_cast<int>(err));
    guard.finalize(true);
    user->Send(guard);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130ab2 _ZN12advancealtar7Manager9dieCharacEP5CUser
// user 判空 / getAdvanceAltar() 判空直接返回；仅 isRewardState() 时处理：
//   stageType==1 → clearStageAndReward() + setState(5) +
//                  RegistTimerClearReward(getIndex(), END_N1)
//   其它       → setState(6) + RegistTimerEPLP(getIndex(), END_N1) +
//                  HistoryLog::dungeonClearInfo(user, control, false)
// [未建模批次] ProcStage::clearStageAndReward(0x0812e3f6) 依赖
//   CharacAdvanceAltarManager::clearStage/addAchievementReward/rewardClearStage/
//   sendClearStage/starAdd 等约 2.2k 行报告子树，本批跳过该调用（同
//   StageControl::leaveUser 对 HistoryLog::leaveDungeon 的既有处理）。
// ----------------------------------------------------------------------------
void Manager::dieCharac(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }

    ProcStage* procStage = control->getProcStage();
    if (!procStage->isRewardState())
    {
        return;
    }

    if (procStage->getStageType() == 1)
    {
        // ORIG procStage->clearStageAndReward();
        procStage->setState(ProcStage::State::S_N5);
        Timer_StageControl::RegistTimerClearReward(control->getIndex(),
                                                   StageEndType::END_N1);
    }
    else
    {
        procStage->setState(ProcStage::State::S_N6);
        Timer_StageControl::RegistTimerEPLP(control->getIndex(),
                                            StageEndType::END_N1);
        HistoryLog::dungeonClearInfo(*user, *control, false);
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130eb2 _ZN12advancealtar7Manager10finishLoadEP5CUser
// getAdvanceAltar() 判空 → 0；isAvailableTciket() 为假 → 1（原样返回）；
// 否则 setState(1) 并回包 put_header(0,0x1e)/put_int(0)/finalize(true) → Send；
// 恒返回 1。
// ----------------------------------------------------------------------------
int Manager::finishLoad(CUser* user)
{
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return 0;
    }
    if (!user->m_advanceAltar.isAvailableTciket())
    {
        return 1;
    }

    control->getProcStage()->setState(ProcStage::State::S_N1);

    PacketGuard guard;
    guard.put_header(0, 0x1e);
    guard.put_int(0);
    guard.finalize(true);
    user->Send(guard);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130fa8 _ZN12advancealtar7Manager10giveUpGameEP5CUser
// user/getAdvanceAltar() 判空直接返回；setState(6) +
// RegistTimerEPLP(getIndex(), END_N2) + dungeonClearInfo(user, control, false)。
// ----------------------------------------------------------------------------
void Manager::giveUpGame(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }

    control->getProcStage()->setState(ProcStage::State::S_N6);
    Timer_StageControl::RegistTimerEPLP(control->getIndex(),
                                        StageEndType::END_N2);
    HistoryLog::dungeonClearInfo(*user, *control, false);
}

}  // namespace advancealtar
