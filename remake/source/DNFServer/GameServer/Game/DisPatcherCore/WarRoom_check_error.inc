int DisPatcher_UseCoin::WarRoom_check_error(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // ORIG +0x06：msg 先落远槽 -0x14（req 先声明，技21/13），room 后落 -0x10。
    MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
    (void)param;   // ORIG 第 3 参未消费（coin 读自 msg+0xd）
    WarRoom* room = static_cast<WarRoom*>(user->GetWarRoom());
    if (room == 0)
    {
        return -2;
    }
    // [b28d-solo-DisPatcherCore] P0：ORIG +0x3a `test %al,%al; je`（无槽无保活）
    // ⇒ 原码是内联表达式，无具名 bool 局部（register 形会引 push %ebx 反劣，实测）。
    if (user->getCurCharacInvenR()->IsCoinUsable())
    {
        int ret = room->ReviveUserByCoin(user, req.m_0d);   // ORIG -0xc(%ebp)
        // ORIG +0x61：`cmpl $0x0,-0xc; jle 0-岛; mov -0xc,%eax`（技6/7 尾置 if 岛）。
        if (ret > 0)
        {
            return ret;
        }
        return 0;
    }
    return 0x11;
}
