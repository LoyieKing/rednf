// ---- CompiledDNFLex()（ORIG 0x8ad07c0，默认构造：create(true) 自建串表）----
// [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：libstdc++ deque 实现 + 构造调度差。
// 本 TU 路由 4.4.6 -O2；ORIG 0x8ad07c0 118 条 vs 我们 111 条，差异三处：
// ①ORIG 把局部 tmp deque 的构造（清零+_M_initialize_map）**排在成员 streamStack_
//   构造之前**（4.1 -O2 独立调用重排；C++ 语义要求成员先构造，4.4 照语义发射）；
// ②ORIG 的 `streamStack_.swap(tmp)` 带 `test tmp.map; je` 守卫（4.1.2 deque::swap
//   内联形），4.4.6 无守卫直换；
// ③tmp 析构位置：ORIG 在 swap 后立即（源码为块作用域），我们当前在函数尾。
// ①②属跨版本/库实现差异不可翻；③可改块作用域但不改变 DIFF 判定（①仍存）。
// 指令集合与成员偏移（vptr/offset_/lastToken_/stringManagerDelete_/stringBuffer_/
// tokenBuffer_ 0x2000 rep stos/flag2040_/lexerError_/loadStream_/create(true)）全等，
// 语义等价 ⇒ N 封顶。
// [b28d-solo 复验] gnu++0x 下 118 vs 111，ndiff 81（与 gnu++98 逐条相同）。
// 帧 0x6c vs 0x5c（差 0x10，随 tmp 构造先后互换）。①②为 GCC 4.1/4.4 的调用重排与
// libstdc++ deque::swap 内联形差异，源码不可控（成员先构造是 C++ 语义）⇒ 维持 N。
// [J10 复验 2026-09-11] 注意 game_func_report 对 `_ZN14CompiledDNFLexC1Ev` 的 ORIG
// 切片会取到 0x8ad0580（C1E）；权威地址以 nm 为准：C1Ev/C2Ev @**0x8ad07c0**
// size 0x237。p167：ORIG calls=9 我方 6 —— MISSING `deque::D2Ev`×3 + `_Unwind_Resume`×2
// （ORIG 3 个 EH landing 08ad0a??/08ad0b??/08ad0c?? 清理 tmp/stream_stack_），
// EXTRA `deque::D1Ev`×2（D1/D2 同址别名伪差，P93）。属 4.1 保守 EH region 差异 ⇒ 维持 N。
CompiledDNFLex::CompiledDNFLex()
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(0),
      stringBuffer_(0)
{
    // ORIG 08ad073e/08ad0741（`mov %esi,(%esp)` + `call <_ZNSt5dequeI…stream_t…D1Ev>`）
    // 位于 08ad0746（tokenBuffer_ memset 准备）之前 ⇒ tmp 的来源作用域在 swap 后即结束
    // （析构提前到 memset/create 之前）。块作用域复现该提前析构位置。
    {
        std::deque<stream_t> tmp;
        streamStack_.swap(tmp);
    }
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    flag2040_ = 1;
    lexerError_ = 0;
    loadStream_ = 0;
    create(true);
}