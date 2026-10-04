// ============================================================================
// DB_UpdateBloodBestRecord::dispatch —— ORIG 0x08431c74（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8431c74：GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<
// SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD>（+0=charac_no/+4=best_time/
// +8=best_round/+0xc=dungeon_index）→ upDate charac_blood_best_record set
// best_round=%d, best_time=%u where charac_no=%u and dungeon_index=%d；
// exec 失败（!=1）或成功且 getAffectedRowCount()==0 → 补 inSert
// charac_blood_best_record(charac_no,dungeon_index,best_round,best_time)
// values(%u,%d,%d,%u)，返回 insert exec==1；update 成功影响行 → 返 1。
// ============================================================================
// [b28c-31] P0 清绑（原 register bool needInsert asm("eax") 已删）；残差
// N78+N18a：needInsert 汇聚 ORIG %eax vs 4.4 %ebx，且 `or %edx,%eax; test`
// 直分支 ORIG vs 4.4 setne 物化（0x08431d0a/0x08431d1a 实证）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08431c74
bool DB_UpdateBloodBestRecord::dispatch(int /*userIdx*/, int /*slot*/,
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD* sig = /*SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD*/(
        stream->GetOutBuffer<SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD>());
    db->set_query(
        "upDate charac_blood_best_record set best_round=%d, best_time=%u where charac_no=%u and dungeon_index=%d",
        sig->m_bestRound, sig->m_bestTime, sig->m_characNo,
        sig->m_dungeonIndex);
    // ORIG 0x08431c74：needInsert 无槽——4.1 以 %eax 物化双常数岛
    // `mov $1; jmp T / mov $0 / T: test al; je`（首段 xor$1;test;jne =
    // exec!=1 直落 true 路）。[b28d-solo] P226/P316 语句表达式零槽短路物化
    // （`if (({ A || B; }))`）复现该形；P0 合规（无 register 绑定、无具名局部）。
    if (({ db->exec(true) != 1 || db->getAffectedRowCount() == 0; }))
    {
        db->set_query(
            "inSert into charac_blood_best_record(charac_no,dungeon_index,best_round,best_time) values(%u,%d,%d,%u)",
            sig->m_characNo, sig->m_dungeonIndex, sig->m_bestRound,
            sig->m_bestTime);
        if (!db->exec(true))
            return 0;
        return 1;
    }
    return 1;
}