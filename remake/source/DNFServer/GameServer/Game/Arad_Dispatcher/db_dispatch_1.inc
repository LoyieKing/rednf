bool Arad_Dispatcher_Manager::db_dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08190080：查 db 组（+0x354），虚槽2 调用；ErrLine!=0 记日志；找到即返回 1。
    Arad_Dispatcher* d = m_dbDispatchers[code];
    if (d == 0)
        return false;
    const int errLine = d->db_dispatch(code, uid, stream);
    if (errLine != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatch_Manager.cpp",
            __PRETTY_FUNCTION__,
            0xab, "[ERROR] DB Dispatch Result dipCode[%d] uid[%d], ErrLine(%d)",
            code, uid, errLine);
    }
    return true;
}