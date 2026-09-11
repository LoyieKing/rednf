// ---- PushInputStream（ORIG 0x8ad0a00）----
// buffer 魔数校验（首 short == 0xd0b0；不匹配释放 buffer 并失败）；
// 非空栈先把旧栈顶 offset 保存；push 新流并置 offset_（魔数匹配时跳过
// 2 字节头）。
bool CompiledDNFLex::PushInputStream(const char* dir, const char* filename,
                                     char* buffer, unsigned int bufferSize)
{
    // [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：同 PopInputStream 族。
    // ORIG 0x8ad0a00 170 条 vs 我们 207 条，ndiff 52（b28d-solo 前 219 条/200）。
    // 已修两处（正向收益）：①`dir/filename` 的 assign 由「if(dir==0){assign("",0);}
    //   else{assign(dir,strlen(dir));}」四调用点合并为三目单调用点
    //   `assign(dir==0?"":dir, dir==0?0:strlen(dir))` ⇒ 复现 ORIG 的
    //   `test %edx,%edx; je <空串>; …; call assign` 单点形，p167 EXTRA
    //   `_ZNSs6assignEPKcj`×2 归零；②`stream.size/buffer` 赋值前置到 empty_file
    //   检查之前（ORIG +0x72..+0x77 顺序）。残余：①析构内联 vs ORIG out-of-line
    //   stream_t D1（见 PopInputStream 判词①，p167 MISSING ×1 + EXTRA
    //   `_ZNSs4_Rep10_M_destroyERKSaIcE`×2）；②empty_file=false 的 store 落点
    //   （ORIG 在 `cmpw; je` 目标内，我们在 cmpw 后直落）；③offset_ 回写用
    //   edi vs ORIG 的 edx（寄存器分配）。语义全等 ⇒ N 封顶。
    // [J10 复验 2026-09-11] p167：ORIG calls=13 我方 14 —— MISSING `stream_t::D2Ev`×1、
    // `_ZNSsC2ERKSs`×2、`_ZNSsD2Ev`×1（C1/C2、D1/D2 同址别名伪差 P93）；EXTRA
    // `_ZNSs4_Rep10_M_destroy`×2（内联展开）、`_ZNSsC1ERKSs`×2/`_ZNSsD1Ev`×1（同前别名）。
    // fresh 177 vs 211（gap −34 ≈ 1 处 stream_t 析构内联展开）⇒ 源码不可控，维持 N。
    stream_t stream;
    stream.dir.assign(dir == 0 ? "" : dir,
                      dir == 0 ? static_cast<size_t>(0) : std::strlen(dir));
    stream.filename.assign(filename == 0 ? "" : filename,
                           filename == 0 ? static_cast<size_t>(0)
                                         : std::strlen(filename));

    stream.size = bufferSize;
    stream.buffer = buffer;

    bool empty_file = false;
    if (buffer == 0 || (empty_file = true, bufferSize != 0))
    {
        if (bufferSize < 2)
            return false;
        if (*reinterpret_cast<short*>(buffer) != static_cast<short>(0xd0b0))
        {
            delete[] buffer;
            return false;
        }
        empty_file = false;
    }

    if (!streamStack_.empty())
        streamStack_.back().offset = offset_;

    streamStack_.push_back(stream);

    offset_ = buffer;
    if (!empty_file)
        offset_ = buffer + 2;
    return true;
}