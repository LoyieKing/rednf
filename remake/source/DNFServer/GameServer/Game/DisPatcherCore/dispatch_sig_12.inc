int DisPatcher_Lottery::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() < 3)
    {
        return 0;
    }
    if (user->CheckInTrade())
    {
        PacketGuard guard;
        guard.put_header(1, 0x1d);
        guard.put_byte(0);
        guard.put_byte(0xda);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    // [G5-DisPatcherCore] 负结论（已回滚）：ORIG 0x081e1e0x 对 idx 取址为两步
    // `lea -0x1f(%ebp),%eax; add $0xd,%eax`（⇒ 地址 -0x12，即原码是「0xd 字节头 + 2 字节
    // 字段」的局部对象）。试形 `struct { char m_hdr[0xd]; char m_idx[2]; } view;
    // + *(short*)view.m_idx`（非 packed，避开 packed 成员引用限制）⇒ 读数 166→169
    // worse（多 3 条：对象落位与预期不符，且第二处读取未折叠回单条 movzwl）⇒ 回滚，
    // 保持独立 `short idx`（槽位 -0x12 与 ORIG 一致，仅差取址步数）。
    short idx;                                 // ORIG msg 视图未定（-0x1f 基址，见上）
    if (!packet.get_short(idx))
    {
        return LineFunc(0x861c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int result = user->lottery_item((int)idx);
    // [DPC-SigC 2026-09-11] P324/P59 块序：ORIG 0x081e1e6x `cmpl $0x0,-0x10(%ebp);
    // jle <拒绝块=+0x??>`（LCS O82）⇒ fall-through 是**成功**块（O83..O122 的
    // PacketGuard/put_header(1,0x1d)/Send 序列），拒绝块在其后；我方原写
    // `if (result < 1) {拒绝;return}` 使 fall-through 为拒绝块（N82..N118），
    // 成功块被 `jg` 抛出（LCS insert OUR[119:160] 41 条）。改写为
    // `if (0 < result) {成功;return}` + 拒绝路径落尾；对 int 语义等价。
    // 预期 41+41 两条大块归零（仅剩 idx 两步取址 1 块），指令数不变 166。
    if (0 < result)
    {
        PacketGuard guard;
        guard.put_header(1, 0x1d);
        guard.put_byte(0);
        guard.put_byte(result);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    {
        if (user->getLastLotteryTime() != 0)
        {
            // [G5-DisPatcherCore] P0/P66：ORIG 0x081e... 段 `call getCurSec`→%ebx 先求（左操作数），
            // `call getLastLotteryTime`→%eax 后求，`sub %eax,%edx` 直算；无具名 last 槽往返
            // （我方原 `int last = ...` 反序 + 多占 4B 槽）。
            unsigned int diff =
                GlobalData::s_systemTime_.getCurSec() - user->getLastLotteryTime();
            if (diff <= 2)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x334, 1, 0, 0);
            }
        }
        user->setLastLotteryTime(GlobalData::s_systemTime_.getCurSec());
        return 0;
    }
}
