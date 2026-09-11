// ============================================================================
// DB_UpdateGiftCount::dispatch ORIG T @ 0x08427112 [dispatch.md]
//   read u32 cnt + i32 id → MySQL(6, group==0)：
//   "upDate member_login set last_gift_time=unix_timestamp(now()),
//    gift_cnt=gift_cnt+%d where m_id=%s" % (id, NumberToString(cnt,0))
//   → exec(true)；返回 1。
// ============================================================================
bool DB_UpdateGiftCount::dispatch(int userIdx, int slot, Stream* stream)
{
    (void)userIdx;
    (void)slot;
    unsigned int mId = 0;      // local_14：Stream >> (uint&)
    int giftCnt = 0;           // local_18：Stream >> (int&)

    stream->operator>>(mId);
    stream->operator>>(giftCnt);

    MySQL* handle = GlobalData::s_db_mgr->GetDBHandle(
        static_cast<ENUM_DB_HANDLE_IDX>(6), static_cast<ENUM_SERVER_GROUP>(0));

    char midBuf[20];                       // local_2c[20]
    memset(midBuf, 0, sizeof(midBuf));
    memcpy(midBuf, "member_login", 13);    // 汇编 memcpy(local_2c,"member_login",0xd)
    // [b28d-53 P134] ORIG 无 cntStr 具名局部：NumberToString 结果 %eax 直落
    // set_query 的 0xc(%esp)，且先物化 giftCnt 入 %edx 跨 call 保活。
    handle->set_query(
        "upDate member_login set last_gift_time=unix_timestamp(now()),gift_cnt=gift_cnt+%d where m_id=%s",
        giftCnt, NumberToString(mId, 0));
    handle->exec(true);
    return true;
}