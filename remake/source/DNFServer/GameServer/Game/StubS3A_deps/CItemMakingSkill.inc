CItemMakingSkill::CItemMakingSkill()
{
    // ORIG 0x08545125：time(0) → operator new(0x9c8) → CMTRand(seed)
    // b28c-L5 代落判词（lane14 件，DELEGATED→本 TU 属主；根因由 lane14 探针定）：
    //   本件 48/48 指令数全等，唯一残差在 EH/预取段——ORIG 0x08545134 在
    //   `call time@plt` 之后**立刻** `lea -0x1c(%ebp),%esi` 预取 &seed（4.1 初始化
    //   前置，N88 族），4.4 则把该 lea 延到 `call CMTRand::C1ERKm` 前一条且落
    //   %edx（惰性求值 + callee-saved 选择差）。new-expression 的 landing-pad
    //   抑制已由 P210/BM11 覆盖（caller 前置于 callee ctor 定义）——本 TU 看不到
    //   CMTRand ctor 体（定义在 CUserCharacInfo 链外 TU），R3 跨 TU 迁定义不可行
    //   ⇒ 判 N88+N18a，语义零差，勿再开轮。
    // [b28d-62] 试临时实参形：ORIG 0x08545131..0x0854514b 为
    //   `call time; mov %eax,-0x1c(%ebp); lea -0x1c(%ebp),%esi; new; CMTRand(esi)`
    // ⇒ 原码把 time(0) 作**实参内联表达式**（临时槽 -0x1c 供 const& 绑定），
    //   4.1 在 new 之前预取地址入 %esi；具名 seed 形 4.4 后置 lea（历史判词）。
    m_rand = new CMTRand(time(0));
