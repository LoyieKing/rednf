unsigned int Tick::nowAsInternalValue()
{
    // [b28d-39 判词 · N18a/跨版本] ORIG 0x08ade86a：ORIG 保留截断 64 位乘的
    // 死符号扩展 `mov %eax,%edx; sar $0x1f,%edx`（4.1 未消除），4.4 对
    // `(long long)tv_sec*1000` 的截断结果恒折 32 位 imul（少 2 条）；时钟源/
    // 毫秒换算/magic 除法逐条等价。
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned int)((long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
