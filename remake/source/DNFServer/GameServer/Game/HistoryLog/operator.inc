void cHistoryTrace::operator()(char const* fmt, ...)
{
    // ORIG 0x0854f998/0x0854f9ba：Guard 与 logHistory 各调一次 CLog::instance()，
    // 两次调用不缓存（logHistory 的 CLog* 直载 %eax 寄存器）。
    Guard<Mutex> guard(&CLog::instance()->m_mutexes[7]);

    va_list ap;
    va_start(ap, fmt);
    CLog::instance()->logHistory(fmt, reinterpret_cast<char*>(ap));  // ORIG logHistory(fmt, &stack)

    va_end(ap);
