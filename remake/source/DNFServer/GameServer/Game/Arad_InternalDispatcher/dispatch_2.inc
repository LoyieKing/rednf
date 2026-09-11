// ORIG 0x08185574：select event_id, charac_no ... limit 10 →
//   SigLoadRewardEventItem(count + eventId[10] + characNo[10]) →
//   make_internal_stream_jpn(0x26, uid) + put_binary(0x56) →
//   MsgQueueMgr::put(DISPATCH_Q)。返回值 = put 的返回值。
// b28d-solo-Arad_InternalDispatcher: AE（clean 口径）——ORIG 0x08186f7c，204/204 条
//   助记符逐条全等，残差仅 .o 未解析重定位伪差；p167 调用点差集 CLEAR；
//   P0 已清寄存器绑定（N96；保留纯 register 存储类提示，P231 合规）。
bool Arad_DB_kLoadRewardEventItem::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08186f7c：GetOutBuffer<arad::SigLoadRewardEventItem> 模板实例。
    //   %ebx 缓存返回（4.1 EH 形）：put 结果 0x0818583e `mov %eax,%ebx`，
    //   catch 0x081858b0 `mov$0,%ebx`，merge 0x081858d1 `mov%ebx,%eax`。
    register bool ret;
    arad::SigLoadRewardEventItem* sig = stream->GetOutBuffer<arad::SigLoadRewardEventItem>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x796, "SigRewardEventItem is null.");
        ret = false;
        goto done;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";

        int curSec = GlobalData::s_systemTime_.getCurSec();

        if (!db->set_query(
                " seLect event_id, charac_no from event_reward_item_arad "
                " where (m_id = %u and end_date > from_unixtime(%u))  "
                " order by occ_date desc limit 10 ",
                sig->m_field0, curSec))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";

        // 行循环：col0=event_id(int) → m_eventCodes[i]；
        //         col1=charac_no(uint) → m_characNos[i]；每行 m_count++。
        // ORIG 0x081856e0/0x08185738：get_int/get_uint 目标 = lea out+4*i(+0x28)+6/0xe
        //   直填字段（技巧37 无具名中间槽）；m_count++ 0x08185794（movzwl+add+mov%ax）。
        int i = 0;
        arad::SigLoadRewardEventItem out;
        while (db->fetch())
        {
            if (!db->get_int(0, *(int*)&(out.m_eventCodes[i])))
                throw "EventCode";
            // ORIG 0x08185746：i++ 发生在 get_uint 地址算好后、call 前
            //   ⇒ 源形为 get_uint(1, m_characNos[i++])（技巧4 后缀 ++）。
            if (!db->get_uint(1, *(unsigned int*)&(out.m_characNos[i++])))
                throw "Character";
            ++out.m_count;
        }

        // ORIG 0x081857ca：Acquire 结果直落 ctor 实参（无 acquired 槽）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_DatabaseDispatcher.cpp", 0x7b8), true);
        make_internal_stream_jpn(guard, (ENUM_INTERNALPACKET_JPN)0x26, uid);
        // ORIG 0x08185806/0x0818581d：`call CStreamGuard::operator->` 后再
        //   `call CStreamGuard::put_binary`（p167 差集：ORIG 无 operator*）⇒
        //   源形为 `guard->put_binary(...)`（P167 判据）。
        guard->put_binary(&out, 0x56);

        ret = GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7c0,
                              "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
        ret = false;
    }
done:
    return ret;
}