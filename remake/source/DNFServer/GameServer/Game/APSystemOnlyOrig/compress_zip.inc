bool compress_zip(char* out, unsigned int& outLen, const char* in, unsigned int inLen)
{
    // [b28d-27] ORIG 0x086b201f 逐条：形参 out/inLen 各复制进具名局部（-0x10/-0x14，
    // P0 槽往返证据），bound 为 **int**（imul 0x66666667+sar 有符号除 5，槽 -0x1c），
    // dst4 具名指针（-0xc），compress2 返回值具名 ret（-0x18），末段 bound 先写回
    // 再赋 outLen（两步）。
    // [b28d-solo-APSystemOnlyOrig] 声明序 = ORIG 槽位映射（P309）：bound -0x1c、
    // ret -0x18、len -0x14、dst -0x10、dst4 -0xc；赋值语句序保持 ORIG 指令序。
    unsigned int bound;
    int ret;
    int len;
    char* dst;
    char* dst4;
    dst = out;
    len = (int)inLen;
    if (len <= 0)
    {
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x086b2037 立即数 = 0x8cf5ab8
        // "error; compress failed:unexpected message size"（旧写 "d%c%02d"
        // 是相邻字面量 0x8cf5ab0，日志内容错）。
        LogManager::logFormat(
            1, "Utility.cpp",
            __PRETTY_FUNCTION__, 0x1c6,
            "error; compress failed:unexpected message size");
        return false;
    }
    // [b28d-solo-APSystemOnlyOrig] bound 为 unsigned（ORIG 尾段
    // `mov -0x1c,%edx; mov 0xc,%eax; mov %edx,(%eax)` 单段直落；int 版多
    // `mov %eax,%edx` 中转）。除法仍按 int 有符号 magic 0x66666667。
    bound = static_cast<unsigned int>(len + len / 5 + 12);
    // dst4 声明 char*、调用点再转 unsigned char*：ORIG 0x086b209e 在压参前把
    // arg0 预装 %eax（call 点转换触发提前物化）；直接传 unsigned char* 时 4.4
    // 把 arg0 排到最后装载。
    dst4 = dst + 4;
    ret = compress2(reinterpret_cast<unsigned char*>(dst4),
                    reinterpret_cast<unsigned long*>(&bound),
                    reinterpret_cast<const unsigned char*>(in),
                    static_cast<unsigned long>(len), -1);
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x086b20c3：单条负向守卫
    // `ret != 0 || outLen < bound` → jne/jae 双跳同落 fail（P214① 正极性源形），
    // 两个独立 if-return 会翻成 fail 直落布局。
    if (ret != 0 || outLen < bound)
        return false;
    *reinterpret_cast<unsigned int*>(dst) =
        static_cast<unsigned int>(len) & 0x3fffffff;  // ORIG and 截断 30 位
    bound += 4;
    outLen = bound;
    return true;
