// is_all_pvp_channel @ 0x081976c7
// [b28d-46 P248 档①] 返回类型 int→bool（mangled 不含返回类型）：调用点 ORIG
// `call; xor $1; test %al; je` 的唯一复现解是 bool callee + 裸 `!call()`
// （P168a 定论：int callee 一切调用点写法恒产 setne）。本头仅本 TU 消费。
bool PVP_CHANNEL::is_all_pvp_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    if (is_integrated_pvp_channel() || channelType == 7)
        return true;
    return false;
}