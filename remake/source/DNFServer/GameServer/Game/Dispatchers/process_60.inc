// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d1b5e orig/ours=108/108 ndiff-blocks=5
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZTI10ENUM_ERROR,0x4(%esp) | movl   $&_ZTI10ENUM_ERROR,0x4(%esp) | mov    -0x10(%ebp),%eax
int Dispatcher_ChangeEmotion::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);                     // ORIG 0x081d1cec（二参版）
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }

    // 声明顺序决定落槽：ORIG 0x081d1cex 先物化 msg→-0x14、后物化 param→-0x10
    // （技巧21 先声明占远槽），我方原顺序相反导致两槽互换 + 后续 `mov -0x10/%-0x14` 全错。
    MSG_CHANGE_EMOTION& req = reinterpret_cast<MSG_CHANGE_EMOTION&>(msg);
    stChangeEmotionParam& out = static_cast<stChangeEmotionParam&>(param);
    out.m_4 = req.m_emotionIdx;                                 // msg+0x0d short → param+0x04

    try
    {
        if (user->get_state() <= 2)                             // cmp $0x2,setle
        {
            throw static_cast<ENUM_ERROR>(3);
        }
        // 同上配方：ORIG 0x081d1ebe 为 `xor $1,%eax; test %al,%al` 单段。
        // 0x081d1c0d `call isVaildEmotionValue` → 0x081d1c12 `xor $0x1,%eax` →
        // 0x081d1c15 `test %al,%al`（无 `mov %al,-0xd` 槽往返）⇒ 删具名 emBad。
        // [P248 档①] isVaildEmotionValue 已 bool 化（Dispatchers.h:807 + inc）
        // ⇒ 裸 `!call()` 直产 ORIG 0x081d1c12 `xor $0x1,%eax; test %al,%al; je`。
        if (!isVaildEmotionValue(static_cast<short>(out.m_4)))
        {
            throw static_cast<ENUM_ERROR>(1);
        }
    }
    catch (ENUM_ERROR e)                                        // catch handler @0x081d1c56
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x102, (unsigned char)e);
        return -1;
    }
    // [G4-60 2026-09-11 · P16/P59] ORIG try 只包两个 throw 检查：handler @0x081d1c49 紧接
    //   第二 throw，catch 体 0x081d1c56..0x081d1c8a + 清理 LP 0x081d1c8c..0x081d1c9c；
    //   0x081d1ca1 起的 SetCurCharacEmotion 参数装载无 LP 覆盖，body 尾 0x081d1cdd
    //   `mov $0x0,%ebx` 直落 epilogue（无 jmp）⇒ 该段在 try 之外。
    user->SetCurCharacEmotion((short)out.m_4);              // CUserCharacInfo::SetCurCharacEmotion
    APSystem::CUserProc::ClearActionAndSendtoUser(
        user, (APSystem::_ActionGroupIndex)3, 0, (ENUM_PACKETCLASS)0);  // ORIG 0x08122390
    return 0;
}