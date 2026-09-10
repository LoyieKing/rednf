int PVP_CHANNEL::is_common_channel()
{
    // is_integrated 已 bool 化（P22）：`!boolcall()` 在 4.4 天然产
    // `xor $0x1,%eax; test %al,%al`（技巧 1），X1 约束形撤销。
    // P22/技巧 1：bool callee 的 `!call()` 天然产 `xor $0x1,%eax; test %al,%al`
    // （ORIG 0x0819775x je 消费者形），负形比正向翻转少一跳。
    int channelType = G_GameWorld()->GetChannelType();
    if (!is_integrated_pvp_channel() && channelType != 7)
        return 1;
    return 0;
