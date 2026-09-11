// ORIG 0x0815b97c：GetOutBuffer<Taiwan::SigUpdateExitTime> null → throw "handle
// null"；updateSSOExpireTime(m_timeA, m_timeB) != 1 → throw "exec"；catch
// (const char*) 记 logFormat(1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp",
// 0x1d4, __PRETTY_FUNCTION__, "[Taiwan, Auth] update member_login last_play_time.
// (error:%s)", e) 返 0；成功返 1。
bool DBUpdateExitTime::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        Taiwan::SigUpdateExitTime* sig =
            stream->GetOutBuffer<Taiwan::SigUpdateExitTime>();
        if (sig == 0)
            throw "handle null";
        if (GlobalData::s_psimpleSSO->updateSSOExpireTime(
                reinterpret_cast<DBUpdateExitTimeView*>(sig)->m_timeA,
                reinterpret_cast<DBUpdateExitTimeView*>(sig)->m_timeB) != 1)
            throw "exec";
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x1d4,
            "[Taiwan, Auth] update member_login last_play_time. (error:%s)", e);
        return 0;
    }
}
