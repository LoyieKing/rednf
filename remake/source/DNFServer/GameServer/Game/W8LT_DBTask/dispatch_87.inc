// ============================================================================
// DB_UpdateDetailChannel::dispatch —— ORIG 0x0841bcee
// ============================================================================
// ORIG 0x841bcee：GetOutBuffer<SIG_UPDATE_DETAIL_CHANNEL>（4×int 载荷）→
// getCurSec() % 0x708，余数 < 900 则回退余数、否则补 0x708-余数（对齐 15 分钟
// 槽）→ GetDBHandle(DB_HANDLE_4,0) → inSert concurrent_user_status 5 占位
// （from_unixtime(occ_time),player_status,dungeon_index,channel_no,occ_count）；
// exec 失败返 0；成功后 cMyTrace(__PRETTY_FUNCTION__,0x4f28,0) 日志同 4 值。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0841bcee
// [L4-r1] **已修 AE**（ndiff 72→0）：P227 全表达式临时链 cMyTrace(...)(...)
//   ⇒ 复现 ORIG 的 3 push（edi/esi/ebx）+ 帧 0x5c + C1 前 varargs 物化。
bool DB_UpdateDetailChannel::dispatch(int userIdx, int slot, Stream* stream)
{
    DB_UpdateDetailChannelView* sig =
        reinterpret_cast<DB_UpdateDetailChannelView*>(
            stream->GetOutBuffer<SIG_UPDATE_DETAIL_CHANNEL>());
    int t = GlobalData::s_systemTime_.getCurSec();
    int mod = t % 0x708;
    // [SweepIntervalSet] ORIG 0x0841bd?? 是两条独立分支：`sub %eax,-0x24`（t -= mod）
    // 与 `mov $0x708,%eax; sub -0x20,%eax; add %eax,-0x24`（t += 0x708-mod）；
    // 三目 `t += (mod<900)?-mod:(0x708-mod)` 被 4.4 折成 `neg %eax` 单链。
    if (mod < 900)
        t -= mod;
    else
        t += 0x708 - mod;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into concurrent_user_status(occ_time,player_status,dungeon_index,channel_no,occ_count) values(from_unixtime(%d),%d,%d,%d,%d)",
        t, sig->m_playerStatus, sig->m_dungeonIndex, sig->m_channelNo,
        sig->m_occCount);
    if (db->exec(true) != 1)
        return 0;
    // [L4-r1] P227 全表达式临时链：ORIG 0x0841bde0-0x0841be43 在 cMyTrace C1
    // 之前把 4 个 sig 字段装入 %edi/%esi/%ebx/-0x3c 并跨 C1 保活（3 push +
    // 帧 0x5c），两语句形 4.4 只在 ctor 后求实参（仅 push ebx + 帧 0x44）。
    cMyTrace(__PRETTY_FUNCTION__, 0x4f28, 0)(
        "content concurrent %d,%d,%d,%d\n",
        sig->m_playerStatus, sig->m_dungeonIndex, sig->m_channelNo,
        sig->m_occCount);
    return 1;
}