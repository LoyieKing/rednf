bool DB_ReportMannerlessUser::dispatch(int /*userIdx*/, int /*slot*/,
                                       Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    DB_MannerlessUserView* sig =
        reinterpret_cast<DB_MannerlessUserView*>(
            stream->GetOutBuffer<SIG_MANNERLESS_USER>());
    static std::string cache_query;
    if (!cache_query.empty())
        cache_query += ",";
    cache_query += "(";
    cache_query += sig->m_reporterId;
    cache_query += ",";
    cache_query += sig->m_targetId;
    cache_query += ",";
    cache_query += sig->m_reason;
    cache_query += ",";
    // ORIG 0x08427add mov $0xff,%ebx（待清字节数 255）+ 起始槽 -0x123 非 4 对齐
    // ⇒ 0xff 字节数组（P312/P253；旧 0x100 使 4.4 折 0x40 dwords 简单 rep stos）
    char timeStr[0xff] = { '\0' };
    // ORIG 0x08427b39..0x08427b48：getCurSec 结果先落具名槽 -0x1c 再回读传参（P0）
    time_t now = GlobalData::s_systemTime_.getCurSec();
    sprintf(timeStr, "from_unixtime(%d)", now);
    cache_query += timeStr;
    cache_query += ")";
    // [b28d-TakeW8LT 块序复原] ORIG 0x08427b9f..08427bc3 是
    // `cmp $0x273,%eax; seta %al; test; je <clear 块>` + 真分支
    // `mov $0x1,%eax; jmp <ret>` ⇒ 源码形 = 正极性提前 return 1（跳过 clear）；
    // 旧形 `< 0x274` 生成 setbe 且 clear 落成直落块（P222/P258 块序反）。
    // 语义等价：!(0x6000-size > 0x273) ⇔ (0x6000-size <= 0x273) ⇔ 旧形 clear 条件。
    if (0x6000U - cache_query.size() > 0x273U)
        return 1;
    cache_query.clear();
    return 1;
