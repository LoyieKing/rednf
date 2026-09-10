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
