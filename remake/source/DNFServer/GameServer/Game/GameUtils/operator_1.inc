void cMyTrace::operator()(const char* fmt, ...)
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0854f788 无 `cmpl $0,0xc(%ebp)` 前置，
    // 我们多 1 条且帧 0x1020→0x1030（P0 表达式直用族）；语义等价。
    // [L13-round3 P171 修正] 该防御分支 ORIG 完全不存在（0x0854f788 首条即
    // `movl $0x1000,0x8(%esp)`）⇒ 按 P171「ORIG 不写即删」删除 `if(!fmt)return;`：
    // 消 `cmpl $0,0xc(%ebp); je` 2 条 + 帧 0x1030→0x1020 + buf 槽 -0x101c→-0x1014。
    // 注：原码对 fmt==NULL 不做检查（照抄 ORIG 越界/空指针行为）。
    // ORIG 0x854f788（0x207 字节）：0x1000 栈缓冲清零；level(m_flag) 非 8/9 时
    // sprintf("%s(%d): ", m_name, m_line) 前缀；vsnprintf 追加变参（上限 0xfff）；
    // Guard<Mutex>(&CLog::instance()->m_mutexes[m_flag*0x18]) 加锁后按 level 分发：
    // 0=logNotice 1=logError 2=logDebug 3=logConsole 4=logMoney 5=logCritical
    // 8=logSnapShot+logInit(m_field_c,m_field_d)；6/7/9 无操作。
    char buf[0x1000];
    std::memset(buf, 0, sizeof(buf));
    if (m_flag != 8 && m_flag != 9)
        std::sprintf(buf, "%s(%d): ", m_name, m_line);
    size_t used = std::strlen(buf);
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf + used, 0xfff - used, fmt, ap);
    va_end(ap);

    // [L13-round3 P0/P183] ORIG 浅槽仅 ap@-0x10 / used@-0xc / guard@-0x14，
    // **无 log/mutex 具名槽**（`CLog::instance()` 与 mutex 取址在每个使用点内联）；
    // 我方两具名局部占 -0x18/-0x1c 并整体下移 8 字节。改为 guard 实参内联。
    {
        Guard<Mutex> guard(reinterpret_cast<Mutex*>(
            &CLog::instance()->m_mutexes[m_flag * 0x18]));
        switch (m_flag)
        {
        case 0: CLog::instance()->logNotice(buf); break;
        case 1: CLog::instance()->logError(buf); break;
        case 2: CLog::instance()->logDebug(buf); break;
        case 3: CLog::instance()->logConsole(buf); break;
        case 4: CLog::instance()->logMoney(buf); break;
        case 5: CLog::instance()->logCritical(buf); break;
        case 8:
            // ORIG case8 → logSnapShot 后无 break，落到 case9 共用唯一 logInit call site。
            CLog::instance()->logSnapShot(buf);
        case 9:
            CLog::instance()->logInit(m_field_d != 0, m_field_c != 0, buf);
            break;
        default:  // 6/7：ORIG 跳表指向收尾 0x854f95f，无日志输出
            break;
        }
    }
