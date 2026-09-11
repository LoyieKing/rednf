// ORIG _ZN13user_creature8CStomach14IncrementValueEf T 0x08336e66：
// m_value += amount；超过 100.0f 钳回 100.0f（0x42c80000）；返回新值。
// [b28-BM7 判词] 残余 = N85：ORIG 比较形 `fucompp; fnstsw %ax; sahf`（4.1）vs
// 4.4 `fucomip %st(1),%st; fstp %st(0)`（x87 比较两向跨版本固有差，patterns N85）；
// 常量地址 0x8c2f450 引用为 AE 归一项。语义（>100 钳位+返回）全等。
// [b28d-23 N·N85] x87 比较 lowering（fucompp/fnstsw/sahf vs fucomip/fstp）⇒ N。ORIG 0x08336e66
// [b28d-solo-CCreatureMgr AE] TU 路由 -march=i486 消 x87 比较 lowering（fucompp/fnstsw/sahf）；ORIG 0x8336e66
float CStomach::IncrementValue(float amount)
{
    m_value += amount;
    if (100.0f < m_value)
        m_value = 100.0f;
    return m_value;
}
