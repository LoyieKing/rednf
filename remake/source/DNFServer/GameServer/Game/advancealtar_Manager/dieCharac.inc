// ----------------------------------------------------------------------------
// ORIG T 0x08130ab2 _ZN12advancealtar7Manager9dieCharacEP5CUser
// user 判空 / getAdvanceAltar() 判空直接返回；仅 isRewardState() 时处理：
//   stageType==1 → clearStageAndReward() + setState(5) +
//                  RegistTimerClearReward(getIndex(), END_N1)
//   其它       → setState(6) + RegistTimerEPLP(getIndex(), END_N1) +
//                  HistoryLog::dungeonClearInfo(user, control, false)
// clearStageAndReward(0x0812e3f6) 现按 ORIG 直调（声明已在 advancealtar_ProcStage.h；
//   定义归 ProcStage 批次，链接期解析）。
//
// [b22判词·N族(P16/P63/P112 返回落点岛)] 函数体逐条与 ORIG 一致（含 5 处
//   getProcStage 现取现用、isRewardState 的 `xor $0x1; test; jne` bool 取反、
//   setState/RegistTimer*/dungeonClearInfo 全序）；-O0 下 ORIG 把 control==0 的
//   统一 return 直落 `leave`、user==0/isReward 各占一 nop+jmp 落点岛，本工具链
//   4.4.7 三处 early-return 各生成独立 nop+jmp 岛（多 1 对 nop/jmp，+2 insn）。
//   此为跨 GCC 版本基本块落点布局差异，非可执行语义（call 目标/数据地址经 relink
//   消解，见 §6.3），语义等价，byte 层 N 封顶。
// ----------------------------------------------------------------------------
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
}