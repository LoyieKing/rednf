CTimeInterval* CTimeInterval::Create()
{
    // [b28d-51] 判词 N（ORIG 0x0856f974，N77 族）：三 movl $0 前置清零 = 4.1 对
    // `new T()` value-init 老规则（CWG434 后 4.4 对 user-ctor 类不 emit；手写
    // memset/placement-new 均改布局）。语义/构造集齐，byte 层封顶。
    // [b28c-27] N77 复核：ORIG 三 movl $0 前置清零 = 4.1 对 `new T()` value-init
    // 老规则（CWG434 后 4.4 对 user-ctor 类不 emit；手写 memset/cast 清零形均改
    // 调用布局）。语义等价（ctor 已写 vptr+基类成员），ndiff 3 封顶。
    // ORIG @0x0856f974：operator new(0xc) → 三槽 u32 清零 →
    // 构造派生 CUnixTimeInterval（ORIG call _ZN15message_counter17CUnixTimeIntervalC1Ev，
    // 置派生 vtable；基类构造为空实现）→ 返回。
    // [b28-L11 判 N·N77] 三 movl $0 前置清零 = 4.1 对 `new T()` value-init 的
    // 老规则（CWG434 后 4.4 对 user-ctor 类不再 emit memset/直存段；手写
    // memset/cast 清零形均改调用布局，N77 判词在册）。语义等价（ctor 已写
    // vptr+基类成员），ndiff 3 封顶。
    return new CUnixTimeInterval();   // ORIG：_Znwj(0xc) → C1（置 vptr）
