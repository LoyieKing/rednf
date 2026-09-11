// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d5a5e orig/ours=90/80 ndiff-blocks=9
//   帧差 sub $130 vs $138 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %ebx | sub    $0x130,%esp
int Dispatcher_PvpMissionComboClear::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d5a5e
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x3958,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // ORIG 0x081d5aa9 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`：msg 具名引用槽；
    // 0x081d5aac / 0x081d5ad1 / 0x081d5b95 三处均 `mov -0xc(%ebp),%eax; movzbl 0xd(%eax),%eax`
    // 折 disp 重读，全文无 `mov %al,-0x9(%ebp)` 槽写入 ⇒ 原码无 comboIdx 具名局部（P0）。
    MSG_BASE& mref = msg;
    if (*(unsigned char*)((char*)&mref + 0xd) <= 8)   // ORIG cmp $0x8; jbe
    {
        // ORIG 实参序：第1参 = msg+0xe（byte→int），第2参 = msg+0xd（byte→short）
        user->Update_ComboMission(
            (int)*(unsigned char*)((char*)&mref + 0xe),
            (short)*(unsigned char*)((char*)&mref + 0xd));
        return 0;
    }

    // [Lane1Dispatchers 2026-09-10] N78/P214③ 复测：`= {0}` 在 4.4 -O0 对 256B 常量
    //   零初始化**内联** rep stos（probe /tmp/l1/probe1.cpp cSimple 逐条实证），
    //   旧「发 .rodata 复制模板/缺符号」判词作废 ⇒ 恢复 ORIG 内联形。
    char logBuf[256] = {0};
    OS_API::snprintf(logBuf, 0x100,
                     "[@missionSystem] invalid Combo Mission index:%d",
                     (unsigned int)*(unsigned char*)((char*)&mref + 0xd));
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x3963, 5);
    trace(logBuf);

    if (user->isGMUser())
    {
        user->SendNotiPacketMessage(logBuf, (ENUM_MESSAGE_TYPE)0);
        return 0;
    }
    return LineFunc(0x396a,
                    __PRETTY_FUNCTION__,
                    0, user->get_acc_id());
}