// [b28d-23 N·N82] ⇒ N。ORIG 0x0854fd64
void CSwitchLog::operator()(const char* fmt, ...)
{
    if (log_write_[m_flagIndex]) {
        char buf[4096];
        sprintf(buf, "%s(%d): ", m_name, m_line);
        va_list ap;
        va_start(ap, fmt);
        int len = strlen(buf);
        // [N6S5ZeroA 落地] ORIG 0x0854fdc7 `mov -0xc(%ebp),%eax; mov $0xfff,%ecx;
        //   sub %eax,%ecx`（先装载 len 再 reg-reg 减法）vs 原写法 `0xfff - len`
        //   （int 有符号）产 `mov $0xfff,%eax; sub -0xc(%ebp),%eax; mov %eax,%edx`
        //   （内存源减法 + 一条 P231 复本，两处 block 差 8 条）。
        //   轴 = 上限表达式的**类型**：`sizeof(buf) - 1 - len` 是 size_t（无符号）
        //   运算，常量折叠后仍是 0xfff 但 lowering 走无符号臂 ⇒ 逐条等于 ORIG
        //   （blocks=0，111:111）。`size_t len` 单独改无效（更差，110:111）。
        vsnprintf(buf + len, sizeof(buf) - 1 - len, fmt, ap);
        // [L23-r4 P0/P183] ORIG 槽表无 log 具名槽（我方多 `mov %eax,-0x14(%ebp)`
        //   使 buf 基址由 -0x1014 漂到 -0x1018）⇒ instance() 结果实参位直传。
        Guard<Mutex> g((Mutex*)((char*)CLog::instance() + m_level * 0x18));
        switch (m_level) {
        case 0:
            CLog::instance()->logNotice(buf);
            break;
        case 1:
            CLog::instance()->logError(buf);
            break;
        case 2:
            CLog::instance()->logDebug(buf);
            break;
        case 3:
            CLog::instance()->logConsole(buf);
            break;
        case 4:
            CLog::instance()->logMoney(buf);
            break;
        case 5:
            CLog::instance()->logCritical(buf);
            break;
        }
    }
}