// ============================================================================
// DB_UpdateOntimeLastRecvIdx::dispatch —— ORIG 0x0844280a（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_D,0) → 解包 mId(uint)/lastNo(int) → tableNo = mId % 10
// → set_query("upDate event_1112_ontime_user_%u set last_no=%u where m_id=%s",
// tableNo, lastNo, NumberToString(mId,0)) → exec(true)；失败或 affectedRowCount
// ==0 → cMyTrace(__PRETTY_FUNCTION__, 0xa407, 5)("... failed at m_id(%s)",
// tableNo, lastNo, mIdStr) 返 0；成功返 1。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 0844280a
// [L4-r1] cMyTrace 改 P227 全表达式临时链 ⇒ ndiff 54→8（余 4 条 mov 中转 =
//   N18a 单段直落族封顶）。
// [r3 负结论] `unsigned int tableNo = mId % 10;` 拆成声明+赋值两行实测**零变化**
//   （our 98/ORIG 94 未动、ndiff 8、四数 141/1750/12/57 逐值相同）⇒ 4.4 对
//   `%` 的商中间值恒走槽（ORIG 0844280c `shr $0x3,%edx` 用寄存器保活）⇒ N18a 封顶。
bool DB_UpdateOntimeLastRecvIdx::dispatch(int /*userIdx*/, int /*slot*/,
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;   // -0x24
    int lastNo = 0;         // -0x28
    *stream >> mId;
    *stream >> lastNo;
    unsigned int tableNo = mId % 10;   // -0xc（ORIG 0xcccccccd mul 序列）
    db->set_query("upDate event_1112_ontime_user_%u set last_no=%u where m_id=%s",
                  tableNo, lastNo, NumberToString(mId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa407, 5)(
            "upDate event_1112_ontime_user_%u set last_no=%u failed at m_id(%s)",
            tableNo, lastNo, NumberToString(mId, 0));
        return 0;
    }
    return 1;
}