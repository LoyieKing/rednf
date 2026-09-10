char* WongWork::CSecurityCardCenter::generateCertKey(char* out)
{
    // ORIG T 0x085fe17e (size 0x3b)：rand∈[10000,99999] 格式化为 5 位数字串。
    // P0/P183：ORIG 无 `value` 栈槽（call 结果经 %eax 直传 snprintf 出参槽），
    // 具名局部导致 spill/reload 2 条 + 帧 +0x10 → 内联为表达式右值。
    OS_API::snprintf(out, 6, "%d", get_rand_int(89999) + 0x2710);
    return out;
