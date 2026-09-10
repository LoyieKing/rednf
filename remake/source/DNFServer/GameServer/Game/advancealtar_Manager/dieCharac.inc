void Manager::dieCharac(CUser* user)
{
    // ORIG 精确控制流（逐指令对齐，报告 dieCharac.md 全文已读）：
    //   +0x06 `cmpl $0,0x8(%ebp); je +0x100`(user==0 → nop+jmp leave 岛)
    //   +0x1e `cmpl $0,-0xc(%ebp); je +0x104`(control==0 → 直落统一 leave，无中继岛)
    //   +0x3b `xor $0x1,%eax; test; jne +0x103`(isRewardState!=1 → nop;leave 岛)
    //   两 success 支 `jmp +0x104`。
    // P0 槽-局部双向对应律：control 为具名局部（-0xc 槽往返）；ORIG 无 ProcStage
    //   具名局部——每个 ProcStage 调用点现取现用 `control->getProcStage()->…`（结果
    //   寄存器直传不落槽，缓存成命名局部会合并 getProcStage 少发 call）。isReward 用
    //   `if(!x) return` 触发 bool 取反 xor 形（P22）对齐 ORIG。
    // [b28d-D33] 尾块布局 N 封顶：早返形（本形）69 条 vs ORIG 68 条，唯一残差为
    // 多一个 `jmp <epilogue>;nop` 岛（我们 3 岛 vs ORIG 2 岛，ORIG control==0
    // 直跳 leave）。P223 负条件 goto 尾标签形与 P63 && 合并形实测均劣（69/72），
    // 属 4.1 vs 4.4 基本块落点布局差，语义等价（call 序列逐条一致）。
    // [b28d-solo-advancealtar_Manager] ORIG 出口块序 user(-0x100 岛)/control(直落)/
    // isReward(直落) ⇒ 前两守卫为嵌套正向 if（P257/P59），仅 isReward 保留早返。
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control != 0)
    {
        if (!control->getProcStage()->isRewardState())
        {
            return;
        }
        if (control->getProcStage()->getStageType() == 1)
        {
            control->getProcStage()->clearStageAndReward();
            control->getProcStage()->setState(ProcStage::State::S_N5);
            Timer_StageControl::RegistTimerClearReward(control->getIndex(),
                                                       StageEndType::END_N1);
        }
        else
        {
            control->getProcStage()->setState(ProcStage::State::S_N6);
            Timer_StageControl::RegistTimerEPLP(control->getIndex(),
                                                StageEndType::END_N1);
            HistoryLog::dungeonClearInfo(*user, *control, false);
        }
    }
