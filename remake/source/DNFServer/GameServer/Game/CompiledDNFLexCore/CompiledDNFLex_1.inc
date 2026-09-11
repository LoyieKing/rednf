// ---- CompiledDNFLex(ScriptStringManager*)（ORIG 0x8ad0580：create(false)）----
// [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：同 C1Ev（libstdc++ deque swap 守卫 +
// 局部 tmp 构造调度在成员 streamStack_ 之前 + tmp 析构位置）。ORIG 0x8ad0580 119 条
// vs 我们 112 条；成员初始化仅差 `stringManager_ = pStringManager`（0x10(%ebx) 直存
// 两形一致），create(false) 一致。语义等价 ⇒ N 封顶。
// [b28d-solo 复验] gnu++0x 下 119 vs 112，ndiff 81。与 C1Ev 同源差异，维持 N。
// [J10 复验 2026-09-11] p167：ORIG calls=9（含 3 landing：08ad0787 无清理 / 08ad078f 清
// stream_stack_ / 08ad07a4 清 tmp）我方 6（1 landing）—— 差集 `deque D2Ev`×3 +
// `_Unwind_Resume`×2，EXTRA `deque D1Ev`×2（同址别名 P93）。正常路径两侧都在 memset 前
// 析构 tmp（src 为块作用域），4.1 仅把 EH region 保守延长 ⇒ 源码不可控，维持 N。
CompiledDNFLex::CompiledDNFLex(ScriptStringManager* pStringManager)
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(pStringManager),
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
    create(false);
}