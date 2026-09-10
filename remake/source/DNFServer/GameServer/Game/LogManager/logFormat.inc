void LogManager::logFormat(int level, const char* file, const char* func,
                           int line, const char* fmt, ...)
{
    // ORIG 权威二进制运行时行为 = no-op：入口字节 0x8ad3c0a 被热补为 0xc3(ret)
    // （原 push %ebp 0x55 覆盖，nm size 0x197 含 patch 前死代码体；Ghidra C 同判
    // `{ return; }`，objdump -s 实证）。重建为空体对齐 ORIG 可观察行为。
    // [b28d-24] N封顶（运行时热补）：静态比较 117 vs 5 属运行时 patch 伪差，
    // 语义=no-op 已对齐；不可也不应复现 patch 前死体。ORIG 0x08ad3c0a
    (void)level; (void)file; (void)func; (void)line; (void)fmt;
