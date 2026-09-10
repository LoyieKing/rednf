bool PVP_CHANNEL::is_integrated_pvp_channel()
{
    // P209 探针：unsigned 局部 + 无 cast 比较或复现 ORIG `cmpl $0x10,mem; ja`。
    unsigned int channelType = G_GameWorld()->GetChannelType();
    // ORIG 0x08197653 `cmpl $0x10; ja`——越界与未命中共享统一 false 落点
    // （N6 位折叠已由 `(1 << t) & C` 原形保留，前批注释豁免维持）。
    if (channelType <= 0x10 && (((1 << channelType) & 0x1e700U) != 0U))
        return 1;
    return 0;
