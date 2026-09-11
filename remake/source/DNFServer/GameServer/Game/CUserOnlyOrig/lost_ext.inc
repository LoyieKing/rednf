// ----------------------------------------------------------------------------
// ORIG 0x0866ad92 CUser::lost_ext(int percent, int& actualLost, bool hardMode)
// 返回 unsigned char（movzbl 局部 ret，无 setne 规范化，P9）
// 注：hardMode 两分支中扣减调用（ORIG call 0x86967f0）传参按 ORIG 汇编
// （ae4d 载 -0x18=lvExpA）；Ghidra 反编译标注为 subtractCurCharacExp(loss)，
// 以 ORIG 常量为准——落地时两分支语义为「exp 高于本级下限全额扣除 / 否则
// 置回下限」的等价表达，与报告 Ghidra 文本一致。
unsigned char CUser::lost_ext(int percent, int& actualLost, bool hardMode)
{
    // ORIG 0x0866ad9f：入口 byte 槽 -0x19 先清 0（返回物化）
    unsigned char downgraded = 0;
    int lvExpA = G_CDataManager()->get_level_exp(get_charac_level());
    int lvExpB = G_CDataManager()->get_level_exp(get_charac_level() + 1);
    int loss = ((lvExpB - lvExpA) * percent) / 100;

    // ORIG 0x0866ae12-0x0866ae1b：`xor $0x1; test; je 0xeb(hard=true 岛)`——
    // hardMode==0 为 fall-through body（技巧17），且降级检查**只在 hard=true**
    if (!hardMode)
    {
        // ORIG 0x0866ae1d：exp 免槽（技巧37）；`sub -0x10,%eax; cmp -0x18,%eax;
        // setle; test; je` = (exp-loss)>lvExpA 走全扣，否则封顶置回下限
        // ORIG 0x0866ae1d-0x0866ae45：setle 为真（<=）的 cap body 在前（fall-through，
        // 技巧17）；二次 get_charac_exp `mov %eax,%edx; sub -0x18,%edx` 直存
        if (static_cast<bool>(get_charac_exp() - loss <= lvExpA)) {
            actualLost = get_charac_exp() - lvExpA;
            setCurCharacExp(lvExpA);                    // ORIG 0x819a87c 实参 -0x18=lvExpA
        } else {
            actualLost = loss;                          // ORIG 0x0866ae61 岛
            subtractCurCharacExp(loss);
        }
    }
    else
    {
        // ORIG 0x0866ae7d 岛（hard=true）：全扣 + 降级检查（Ghidra 亦此序）
        actualLost = loss;
        subtractCurCharacExp(loss);
        int newFloor = G_CDataManager()->get_level_exp(get_charac_level());  // -0xc
        if (static_cast<bool>(get_charac_exp() < newFloor)) {   // ORIG 0xae28 cmp;setl;test
            decCurCharacLevel();
            _update_charac_stat(false);
            downgraded = 1;                             // ORIG 0xaeeb movb $1,-0x19
        }
    }
    SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
    return downgraded;
}