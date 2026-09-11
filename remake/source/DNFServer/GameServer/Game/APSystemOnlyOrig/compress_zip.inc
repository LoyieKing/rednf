// ----------------------------------------------------------------------------
//   bound = len + len/5 + 12（ORIG imul $0x66666667+sar 序列 = gcc 除 5 magic；
//            即 zlib compressBound 的 1.2 倍形式，非 1.1 倍）；
//   zerr = compress2(out+4,&bound,in,len,-1)；失败返回 false；
//   成功但 *outLen（入参容量）< bound → 返回 false（ORIG 防御性容量检查）；
//   成功：out 头 dword = len & 0x3fffffff（ORIG and 截断 30 位），outLen = bound+4。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82（已按 P0 修）：ORIG 0x086b201f；补回 out/inLen/bound/dst4/ret
// 五个具名局部（ORIG 有槽往返）后 ndiff 35→14，残差为跨版本槽号分配（N82），
// [b28d-solo-APSystemOnlyOrig] 判词：**AE**（check_tu routed；66=66；残差仅 <A>/<T> 归一）。
// p167 CLEAR（2/2），语义闭合。
bool compress_zip(char* out, size_t& outLen, const char* in, size_t inLen)
{
    // [b28d-27] ORIG 0x086b201f 逐条：形参 out/inLen 各复制进具名局部（-0x10/-0x14，
    // P0 槽往返证据），bound 为 **int**（imul 0x66666667+sar 有符号除 5，槽 -0x1c），
    // dst4 具名指针（-0xc），compress2 返回值具名 ret（-0x18），末段 bound 先写回
    // 再赋 outLen（两步）。
    // [b28d-solo-APSystemOnlyOrig] 声明序 = ORIG 槽位映射（P309）：bound -0x1c、
    // ret -0x18、len -0x14、dst -0x10、dst4 -0xc；赋值语句序保持 ORIG 指令序。
    // [S5-2026-09-11 §6.5] prett 串：ORIG .rodata = `bool compress_zip(char*, size_t&,
    //   const char*, size_t)`（grep 自 ORIG 全字节，0x8cf… 区）；我方原为 `unsigned int`
    //   展开拼写 ⇒ 签名行改 `size_t`（同型别名，mangled/指令零变化），串逐字节命中。
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
}