// ORIG T 0x088dcee2 _ZNK16channel_script_t23getBonusRateEventServerEhj
// 服务器频道加成版本：命中频道信息即固定 0.02f，否则 0.0f。
float channel_script_t::getBonusRateEventServer(unsigned char channel, unsigned int idx) const
{
    // ORIG 0x088dcee2：结果指针落槽(-0xc)后 `cmpl $0; jne 真值支`，双支
    // bits 写回统一槽 flds 返回 = 具名指针三元形（!=0 直判产 setne 岛异形）。
    const void* info = getChannelInfo(channel, idx);
    return info == 0 ? 0.0f : 0.02f;
}