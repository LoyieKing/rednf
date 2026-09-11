// ----------------------------------------------------------------------------
// ORIG T 0x08142cde _ZN8WongWork10CBossTower15_onTimerProcessE13TIMER_MESSAGEj：
// msg 0xa3 → onClearBossTower()；0xa4 → onReturnVillage()；0xa5 →
// onReadyTimerCheck()；其余忽略。第二参数未使用。
// [batch-19 N 封顶 ndiff=3] 残余 = 末级比较极性：ORIG `cmp $0xa3,%eax;`
// `jne exit` + 体 fall-through，4.4 恒 `je body; jmp exit`（switch 尾 case
// 与空 default 汇入同一出口时 4.4 不 reverse 最后比较）。实测否决三形：
// ① 去 default → 3（同）；② if-else 链 → 11（4.4 折 `cmpl $imm,mem` 丢
// `mov 0xc(%ebp),%eax` 三级 + jne 正向）；③ `register` 形参 + if-else →
// 20（cmp 用寄存器但 body 全 fall-through，块序反）。switch+default 形最优。
// ----------------------------------------------------------------------------
// [b28d-54 N 复核] ORIG 0x08142cde：末级比较极性（ORIG `cmp $0xa3; jne exit` +
//   体 fall-through vs 4.4 switch 恒 `je body; jmp exit`）——4.4 switch 布局固有差；
//   本波复核 switch/default 形仍最优（ndiff 3），语义等价。
// [b28d-solo-CBossTower] N：23:24。4.4 switch 末级比较恒 `je body;jmp exit`，
//   4.1 为 `jne exit`+body fall-through（P285/P46 布局固有）；去 default/if-else/
//   register 三形历史实测均劣，维持 switch。
void CBossTower::_onTimerProcess(TIMER_MESSAGE msg, unsigned int a)
{
    (void)a;   // ORIG 忽略第二参数
    // [N 封顶 ndiff=3] ORIG 0x08142cde 末级比较 `cmp $0xa3; jne exit`+Ready 体
    // fall-through；4.4 对 switch 恒 `je body;jmp end` 不反转末比较。
    // batch-19 实测否决三形（去 default / if-else 链 / register 形参）；
    // 本轮补测 goto 链两形（register 局部落 %ebx+push/pop 反劣；普通形参
    // 4.4 折 `cmpl $imm,mem` 丢 `mov 0xc(%ebp),%eax`）同不通，维持 switch。
    switch (msg)
    {
    // ORIG 0x08142ce4..+0x1e：比较序 0xa4→0xa5→0xa3（4.1 决策树），
    // 叶块物理序 0xa3(+0x1e)/0xa4(+0x2b)/0xa5(+0x38) = case 书写序（P46/P324）。
    case (TIMER_MESSAGE)0xa3:
        onClearBossTower();
        break;
    case (TIMER_MESSAGE)0xa4:
        onReturnVillage();
        break;
    case (TIMER_MESSAGE)0xa5:
        onReadyTimerCheck();
        break;
    default:
        break;
    }
}