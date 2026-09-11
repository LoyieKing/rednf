// ORIG 0x081c5dec T _ZN21DisPatcher_PvPTimeout7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_PvPTimeout/process.md 全文转写）
// err=check_error（ORIG 0x081c5ebe：get_state!=7 → 0x13）；>0 返回 -1；<0
// LineFunc(0x14b7,pretty,0,acc_id)；==0：room=user->GetPVPRoom()（空则
// LineFunc(0x14bc,pretty,0,acc_id)），否则 ret=room->time_out((int*)(msg+0xd))
// —— msg+0xd 即 read() 读入的 8 个 seat int 数组首址 —— param+0x4=ret，返回 err。
int DisPatcher_PvPTimeout::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    const int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0x14b7, __PRETTY_FUNCTION__, 0, user->get_acc_id());
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());  // thunk 0x0865518e
    if (room == 0)
    {
        return LineFunc(0x14bc, __PRETTY_FUNCTION__, 0, user->get_acc_id());
    }
    // MSG_PVP_TIMEOUT 载荷自包头 +0xd 起（read 读 8 个 int），ORIG 直接把该地址
    // 作为 int* 传给 time_out；结果码写入 param+0x4（stOnPvPTimeOut::m_timeOutResult），
    // 由本类 send() 消费（>0 时 put_byte 发出）。局部引用声明次序镜像 ORIG 槽位
    // （param 先于 msg），保证逐指令一致。
    stOnPvPTimeOut& out = static_cast<stOnPvPTimeOut&>(param);
    MSG_PVP_TIMEOUT& rMsg = *reinterpret_cast<MSG_PVP_TIMEOUT*>(&msg);
    out.m_timeOutResult =
        room->time_out(reinterpret_cast<int*>(rMsg.m_pad));   // +0x0d 席位数组首元素
    return errNo;
}
