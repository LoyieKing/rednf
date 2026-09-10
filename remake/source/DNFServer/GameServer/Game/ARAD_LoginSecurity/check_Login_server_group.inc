int PVP_CHANNEL::check_Login_server_group(SIG_LOGIN_DATA* data)
{
    // [b28d-46 P0 清绑 + P248 档①] 删除历史 `register uchar allPvp asm("eax")`
    // 绑定（P0/N96 全域禁）；callee bool 化后裸 `!call()` 直产 ORIG 的
    // `call; xor $1,%eax; test %al,%al; je`。
    int channelType = G_GameWorld()->GetChannelType();
    if (!is_all_pvp_channel())
    {
        data->m_channelRestrictField = 0;
    }
    return 1;
