// ORIG T 0x084414e0（dispatch.md 全文）：GetOutBuffer<SIG_LOAD_KILL_MONSTER_INFO>
// 为 null 返 0；LoadMonsterInfo 失败 cMyTrace 记录 charac_no 返 0；
// 成功 → SendResult(userIdx, slot, sig) → 返 1。
bool DB_LoadKillMonsterInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    // ORIG 0x084414e0：帧头 `movb $0,-0xd` 预置 bool（SendResult 结果落槽
    // 但不参与返回，P31 dead-slot 对齐）；失败分支 `mov 0xafc8(%eax),%ebx`
    // 前置缓存 charac_no 跨 ctor（P87）。
    bool r = false;
    SIG_LOAD_KILL_MONSTER_INFO* sig =
        stream->GetOutBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
    if (sig == 0)
        return 0;

    if (!LoadMonsterInfo(sig))
    {
        register int cn = sig->m_characNo;
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa04b, 5);
        trace("DB_LoadKillMonsterInfo::LoadMonsterInfo ERROR charac_no=%u",
              cn);
        return 0;
    }
    r = SendResult(userIdx, slot, sig);
    return 1;
}