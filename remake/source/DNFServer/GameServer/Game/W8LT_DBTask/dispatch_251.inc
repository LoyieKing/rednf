// [b28d-solo-3] AE：P231 裸 register 缓存 characNo（ORIG `mov (%eax),%ebx` 跨
// blob_to_str call 保活）+ blob_to_str 结果内联进 set_query 实参（P0 无 blob 槽）。
// ORIG 08435e1e。
bool DB_UpdateNPCRelationship::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    UpdateNpcRelationView* sig = reinterpret_cast<UpdateNpcRelationView*>(
        stream->GetOutBuffer<SIG_UPDATE_NPC_RELATIONSHIP>());
    // 残留（N 类）：ORIG 0x08435e1e `mov (%eax),%ebx` npcCnt 入 callee-saved
    // %ebx 跨 call 保活（push %ebx/sub $0x34 帧）；4.4 具名槽外推 4B、
    // register asm("ebx") 反产 spill 槽+双段 mov（_SaveExpandEquipslot 同症）。
    register int characNo = sig->m_characNo;
    db->set_query(
        "upDate charac_npc set npc_cnt=%d,npc_data='%s' where charac_no=%d",
        sig->m_npcCnt,
        db->blob_to_str(0, sig->m_npcData, sig->m_npcCnt * 12),
        characNo);
    db->exec(true);
    return 1;
}