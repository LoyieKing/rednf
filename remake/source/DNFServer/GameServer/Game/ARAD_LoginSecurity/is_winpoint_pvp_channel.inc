// is_winpoint_pvp_channel @ 0x0819770a：channelType ∈ {4, 8}
// [b28d-46 判词·AE] 17/17 指令数全等、调用点差集零。槽变量 + 双 case switch
// （P251/P286 应用）复现 ORIG `mov -0xc,%eax; cmp $K,%eax` 寄存器比较链；
// if 短路链在 4.4 恒折 `cmpl $K,mem`（旧 b28 P209/N18b 判词作废）。
int PVP_CHANNEL::is_winpoint_pvp_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    switch (channelType)
    {
    case 4:
    case 8:
        return 1;
    default:
        return 0;
    }
}