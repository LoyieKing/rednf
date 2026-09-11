// _SIG_LOAD_ACTION_POINT —— W 0x0848f5d8 C1/C2 同体（0x51 字节）：
//   esi 循环 [300..1] 逐元素调 _ActionPointC1Ev，随后 memset(this,0,0xcf1)。
//   数组成员 _ActionPoint m_actionPoints[300] 由编译器自动逐元素构造
//   （与 ORIG 同形）；旧实现额外显式 D1+placement new 造成双重构造，已删。
_SIG_LOAD_ACTION_POINT::_SIG_LOAD_ACTION_POINT()
{
    memset(this, 0, sizeof(*this));                // 终态与 ORIG 全零一致
}