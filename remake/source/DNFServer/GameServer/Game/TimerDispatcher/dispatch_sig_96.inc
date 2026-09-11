bool TimerGlobalEffectItemTimeout::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08637ec2：getCurSec 结果直落 %ecx 跨后续求值（无名变量槽）。
    // 且第一实参为 0x10(%ebp)=p2（Ghidra 标 param_3 不准，asm 实证）。
    GlobalData::s_globalEffectManager_->checkTimeout(
        p2, GlobalData::s_systemTime_.getCurSec());
    return 1;
}