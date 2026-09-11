// ORIG 0x081c0e4a T _ZN19DisPatcher_UseEquip7processEP5CUserR8MSG_BASER9ParamBase
// err=check_error；>0：SendCmdErrorPacket(0x33, err&0xff) 返回 -1；==0：
// m_errNo=getCurCharacInvenW()->use_equip((int)(sbyte)msg.m_0d)，m_8=msg.m_0d，
// 返回 err。
int DisPatcher_UseEquip::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x33, (unsigned char)(errNo & 0xff));
        return -1;
    }
    // b28 清违规：register asm 绑定 P0 全域禁用（N96），恢复纯 C++ 单语句
    // 链形（P52：slot 表达式先物化跨 receiver 调用存活；P200：拓宽直传实参）。
    // 残差 `movsbl→eax 中转再落 ebx` = N18a 单段直落不可造，封顶。
    stOnUseEquipParam& out = static_cast<stOnUseEquipParam&>(param);
    MSG_DECREASE_DURABILITY& req = *reinterpret_cast<MSG_DECREASE_DURABILITY*>(&msg);
    out.m_errNo = user->getCurCharacInvenW()->use_equip((int)(signed char)req.m_0d);
    out.m_8 = req.m_0d;   // ORIG +0x85 `movzbl 0xd(%eax),%edx` 单步 QI 直存（同型）
    return errNo;
}
