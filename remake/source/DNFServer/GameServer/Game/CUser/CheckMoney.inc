bool CUser::CheckMoney(int money)
{
    // ORIG 0x866af2f/0x866af3c：accName→%esi、level→%ebx 跨 call 缓存（push esi/ebx
    // 帧 0x20）；limit@-0x10、curMoney@-0xc；尾 `cmp -0x10,%eax; jg F; mov $1; jmp;
    // F: mov $0` 直分支岛。4.4 goto-RET0 形在 label 前恒插 1 字节 nop 填充（探针
    // nl11_probe1/2 实证），if/else return 形无 nop（仅 cmp 操作数序 4.4 恒折反向）。
    register const char* accName = get_acc_name();   // b28d-solo-CUser：裸 register（非绑定，P0 合规）跨 call 保活 %esi
    register int level = get_charac_level();   // b28d-solo-CUser：裸 register 跨 call 保活 %ebx（P231/P248）
    int limit = G_CDataManager()->GetMoneyLimitPerLevel(level, accName);
    int curMoney = getCurCharacMoney();
    if (curMoney + money <= limit)
        return true;
    else
        return false;
