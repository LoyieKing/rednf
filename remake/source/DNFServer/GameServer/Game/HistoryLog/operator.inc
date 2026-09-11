// ============================================================================
// cHistoryTrace::operator()(char const*, ...) —— ORIG 0x0854f990
//   CLog::instance() 后对 +0xa8（m_mutexes[7]，Mutex 0x18）构造 Guard<Mutex>，
//   再调 CLog::logHistory(fmt, va_list)，Guard 析构解锁。
// ============================================================================
void cHistoryTrace::operator()(char const* fmt, ...)
{
    // ORIG 0x0854f998/0x0854f9ba：Guard 与 logHistory 各调一次 CLog::instance()，
    // 两次调用不缓存（logHistory 的 CLog* 直载 %eax 寄存器）。
    Guard<Mutex> guard(&CLog::instance()->m_mutexes[7]);

    va_list ap;
    va_start(ap, fmt);
    CLog::instance()->logHistory(fmt, reinterpret_cast<char*>(ap));  // ORIG logHistory(fmt, &stack)

    va_end(ap);
}