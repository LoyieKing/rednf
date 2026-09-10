bool Arad_DB_kSaveServerState::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08185abe：flag==0 → deLete by category/code；否则 inSert(msg blob→str)。
    //   %ebx 缓存返回（4.1 EH 形）。
    register bool ret;
    arad::SigServerState* sig = stream->GetOutBuffer<arad::SigServerState>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7ef, "SigServerState is null.");
        ret = false;
        goto done;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG 0x08185b65 `movzbl (%eax),%eax; test; je →delete 岛`：flag 真值测试、
        //   fall-through = inSert 分支在前（技巧17）；blob_to_str/endSec 内联实参
        //   （技巧37，ORIG 0x08185b73 endSec→%ebx 先载、0x08185b96 结果 %eax 直存
        //   0x10(%esp)，无具名槽）。
        if (sig->m_flag)
        {
            if (!db->set_query(
                    " inSert into server_state_info values (%d, %d, '%s', now(), from_unixtime(%u)) ",
                    sig->m_data[0].m_category, sig->m_data[0].m_code,
                    db->blob_to_str(0, &sig->m_data[0].m_msg, 0xc),
                    sig->m_data[0].m_msg.m_endSec))
                throw "set_query";
        }
        else
        {
            if (!db->set_query(" deLete from server_state_info where category=%d and code=%d ",
                               sig->m_data[0].m_category, sig->m_data[0].m_code))
                throw "set_query";
        }
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x812, "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
