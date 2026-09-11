// ============================================================================
// DB_AvatarExtendPeriod::dispatch —— ORIG 0x083ffb22（报告 dispatch.md 全文）
// ============================================================================
// DBDispatcher::dispatch(userIdx, slot, stream) 失败返 0 → get_binary(7)
// 解包 SIG_AVATAR_RENEW 失败返 0 → ExtendPeriod(renew) 成功返 1，失败返 0。
// ============================================================================
bool DB_AvatarExtendPeriod::dispatch(int userIdx, int slot, Stream* stream)
{
    // P102：同一 bool 局部逐次保存结果（ORIG -0x9 槽：get_binary/ExtendPeriod
    // 结果落槽再 movzbl/xor/test；首检查 DBDispatcher::dispatch 直判不落槽）。
    SIG_AVATAR_RENEW renew;   // -0x10（7 字节，-0x10..-0xa）
    bool ok;                  // -0x9
    if (DBDispatcher::dispatch(userIdx, slot, stream) != 1)
        return 0;
    ok = stream->get_binary(&renew, (int)sizeof(SIG_AVATAR_RENEW));
    if (ok != 1)
        return 0;
    ok = ExtendPeriod(renew);
    if (ok != 1)
        return 0;
    return 1;
}