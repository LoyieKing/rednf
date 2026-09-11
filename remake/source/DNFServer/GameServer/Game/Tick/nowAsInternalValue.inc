unsigned int Tick::nowAsInternalValue()
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a）→ 试=④：ORIG 0x08ade887
    //   `mov -0x10(%ebp),%eax; mov %eax,%edx; sar $0x1f,%edx; imul $0x3e8,%eax,%ebx`
    //   （截断 64 位乘的**死**符号扩展，%edx 随后被 0x431bde83 magic 覆盖）在我方恒被
    //   折叠：探针 7 形（`(long long)x*1000`、具名 ll、1000LL、ull、显式 cast 等）全部
    //   折成 32 位 imul 或升真 64 位乘；再换 -B cc1plus446bin 后端复测同样 25 条
    //   （gap +2 不变）⇒ 非后端版本差、属 4.1 前端遗留。⇒ 维持 N18a。
    // [b28d-39 判词 · N18a/跨版本] ORIG 0x08ade86a：ORIG 保留截断 64 位乘的
    // 死符号扩展 `mov %eax,%edx; sar $0x1f,%edx`（4.1 未消除），4.4 对
    // `(long long)tv_sec*1000` 的截断结果恒折 32 位 imul（少 2 条）；时钟源/
    // 毫秒换算/magic 除法逐条等价。
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // [LaneB-P345 2026-09-11 两形复核·均回滚] 目标 = ORIG 0x08ade887 死符号扩展
    //   `mov %eax,%edx; sar $0x1f,%edx`（27 条）：
    //   试形1 `(int)((long long)ts.tv_sec*1000)` 显式截断 ⇒ 26 条（gap 27→1 好）
    //     但 ndiff 2→**3**（`imul $0x3e8,%eax,%eax; mov %eax,%ebx` 仍非 ORIG
    //     `imul $0x3e8,%eax,%ebx` 单条）⇒ 按「ndiff 不劣」闸门回滚。
    //   试形5 `long long ms=...; return (unsigned)(ms + ...)` ⇒ 39 条 / ndiff 17（劣）⇒ 回滚。
    //   ⇒ 维持原形（25 条 / ndiff 2），N18a 封顶判词维持。
    return (unsigned int)((long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}