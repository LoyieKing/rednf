void cMyTrace::operator()(const char* fmt, ...)
{
    // [T2CryptoUtils 复核] 原 [b28d-34] 的「多 1 条 + 帧 0x1020→0x1030」残差已消
    // （va_start 前置于 strlen + `int used` ⇒ 137/137，gap 0）；残余 diff_lines=16
    // 仅尾部 `0xfff-used` 的寄存器选择/`sub mem` 折叠（N4/N7 分配族，封顶）。
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
    // [T2CryptoUtils codegen+P339] ORIG 0x0854f7f0 的 va_start（`lea 0x10(%ebp),%eax;
    // mov %eax,-0x10(%ebp)`）落在 strlen **之前** ⇒ 源码须 va_start 先于 used 计算；
    // 且 used 须为 int：size_t 版使 4.4 对 `0xfff - used` 发
    // `mov $0xfff,%eax; sub -0xc(%ebp),%eax; mov %eax,%edx`（常量物化+mem 直用，
    // 136 条），int 版复现 ORIG `mov -0xc(%ebp),%eax; mov $0xfff,%ecx; sub %eax,%ecx`
    // 形态（137/137，槽 -0xc 一致；strlen ≤ 0x1000 故转换无溢出）。
    va_list ap;
    va_start(ap, fmt);
    int used = (int)std::strlen(buf);
    std::vsnprintf(buf + used, 0xfff - used, fmt, ap);
    va_end(ap);

    // [L13-round3 P0/P183] ORIG 浅槽仅 ap@-0x10 / used@-0xc / guard@-0x14，
    // **无 log/mutex 具名槽**（`CLog::instance()` 与 mutex 取址在每个使用点内联）；
    // 我方两具名局部占 -0x18/-0x1c 并整体下移 8 字节。改为 guard 实参内联。
    {
        Guard<Mutex> guard(reinterpret_cast<Mutex*>(
            // [T2CryptoUtils 语义轴③] 下标必须是 m_flag：ORIG 0x0854f8a0 段仅
            // `mov 0x8(%eax),%edx; mov %edx,%eax; add %eax,%eax; add %edx,%eax;
            //  shl $0x3,%eax`（=m_flag*24，即 m_mutexes[m_flag] 的 sizeof 换算）；
            // 旧码 `m_flag*0x18` 被再乘一次 sizeof(Mutex)=24 ⇒ 地址 m_flag*576，
            // 越界写坏 CLog 之后的堆内存（m_flag=9 时偏 0x1440）。同 TU 对照：
            // HistoryLog.cpp:334 与 LwServerWriter.cpp:38 均直用下标记法。
            &CLog::instance()->m_mutexes[m_flag]));
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
}