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
    // [K6-DpcSigSmallCheckErr] 改前形 = 独立 `short idx`（-0x12，取址单条 disp）
    // → 改后形 = `#pragma pack(push,1)` 视图 + 「基址 + 0xd」取址。依据 ORIG
    // 0x081e1e0x `lea -0x1f(%ebp),%eax; add $0xd,%eax`（两步取址 ⇒ 原码是
    // 「0xd 字节头 + 2 字节字段」的 pack(1) 局部对象，base -0x1f、成员 @+0xd = -0x12）
    // 且第二处读取 0x081e1e2x 为单条 `movzwl -0x12(%ebp),%eax`。旧试形（非 packed
    // struct + char[2]）实测 166→169 劣化（落位偏 + 第二读未折回），本形按 SetPVPMapIndex
    // 已验证成功的 pragma 形重试。⚠不可用 `__attribute__((packed))`（4.4 拒绝绑定
    // 紧实字段到 short&）。
#pragma pack(push, 1)
    struct LotteryMsgView
    {
        char m_head[0xd];
        short m_idx;
    };
#pragma pack(pop)
    LotteryMsgView sig;
    if (!packet.get_short(*reinterpret_cast<short*>(reinterpret_cast<char*>(&sig) + 0xd)))
    {
        return LineFunc(0x861c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int result = user->lottery_item((int)sig.m_idx);
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
