// ORIG 0x081c3eae T _ZN25DisPatcher_RecoverStamina7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=9, err&0xff) 后返回 -1；err<0：LineFunc(0xf9d,pretty,0,acc_id)
// 并返回其值；err==0：param+0x4 = user->serviceOfShusia()，返回 err。
int DisPatcher_RecoverStamina::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)9, (unsigned char)(errNo & 0xff));
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0xf9d,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    // ORIG 0x081c3eae：`mov 0x14(%ebp),%eax; mov %eax,-0xc(%ebp)` = out 引用槽物化
    //（技13 声明位置=落槽时机；技21 errNo 先声明占远槽 -0x10、out 近槽 -0xc）；
    // 写形 `mov -0xc(%ebp),%edx; mov %eax,0x4(%edx)`（slot 载入非 %ebx）。
    stOnRecoverStaminaParam& out = static_cast<stOnRecoverStaminaParam&>(param);
    out.m_errNo = user->serviceOfShusia();
    return errNo;
}
