// ---- ORIG T 0x08130bb8（advancealtar__Manager/dieMob.md 全文 + 汇编逐块核对）----
// [b28d-solo-advancealtar_Manager AE] 本次修：control 守卫正向包裹（ORIG +0x38
// `je <共享尾 return 1>`）+ endType 去初值（ORIG 无 `movl $1,-0x14` 写点）+
// 具名 bool isEnd（ORIG +0x174 `mov %al,-0x9; cmpb $0,-0x9`）+ endType 派发改
// switch（ORIG 决策树 `test;je END_N0` + N3 直落；P46 叶块序 N3 在前）⇒
// 199=199 逐条全等。p167 差集 CLEAR。
int Manager::dieMob(CUser* user, unsigned short mobSeq, unsigned short param,
                    const MSG_MONSTER_DIE& msg)
{
    if (user == 0)
    {
        return 0;
    }
    StageControl* control = user->getAdvanceAltar();
    // [b28d-solo-advancealtar_Manager] ORIG +0x38 `je +0x2ed`（共享尾 return 1 块）
    // ⇒ 正向包裹（P222/P257），非 early-return。
    if (control != 0)
    {
    // ORIG 8130c04..8130e5c：getProcStage 每用点重取（P48 去缓存，共 9 处）
    bool killed = control->getProcStage()->dieMob(mobSeq, param, msg);
    if (killed)
    {
        PacketGuard packet;
        packet.put_header(0, 0x26);
        packet.put_short(mobSeq);
        packet.put_byte(0);           // ×5 个 put_byte(0)（ORIG +0x66..+0x108）
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.finalize(true);
        user->Send(packet);
    }

    // [b28d-solo-advancealtar_Manager] ORIG +0x165 `lea -0x14(%ebp),%edx` 前
    // 无初值写入 ⇒ 原码 endType 未初始化声明（isEndStage 引用输出；P171 照抄）。
    StageEndType::T endType;
    // ORIG +0x174 `mov %al,-0x9(%ebp); cmpb $0,-0x9(%ebp)` ⇒ 具名 bool 槽（P0）。
    const bool isEnd = control->getProcStage()->isEndStage(endType);
    if (isEnd && control->getProcStage()->isRewardState())
    {
        // ORIG +0x1a8 决策树：`test; je END_N0 叶块`（N0 叶块在后）、
        // `cmp $3; jne end` 直落 N3 叶块 ⇒ switch（P46 叶块序=case 书写序）。
        switch (endType)
        {
        case StageEndType::END_N3:
        {   // ORIG local_18==3：全阶段完成
            if (control->getProcStage()->endProcAllKillMonster())
            {
                control->getProcStage()->setState(ProcStage::State::S_N5);
                Timer_StageControl::RegistTimerClearReward(control->getIndex(), endType);
                HistoryLog::dungeonClearInfo(*user, *control, true);
            }
            else
            {
                control->sendMapInfo();
            }
            break;
        }
        case StageEndType::END_N0:
        {   // ORIG local_18==0：全清死亡分支
            if (control->getProcStage()->getStageType() == 1)
            {
                control->getProcStage()->setState(ProcStage::State::S_N5);
                control->getProcStage()->clearStageAndReward();   // [跨片] clearStage 链定义归 AltarProc 批
                Timer_StageControl::RegistTimerClearReward(control->getIndex(), endType);
                // HistoryLog::dungeonClearInfo(true) 分支 ORIG 无（该段属 endType==3）
            }
            else
            {
                control->getProcStage()->setState(ProcStage::State::S_N6);
                Timer_StageControl::RegistTimerEPLP(control->getIndex(), endType);
                HistoryLog::dungeonClearInfo(*user, *control, false);
            }
            break;
        }
        }
    }
    }
    return 1;
}