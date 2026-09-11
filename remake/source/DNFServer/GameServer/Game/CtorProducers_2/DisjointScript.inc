DisjointScript::DisjointScript()
{
    // ORIG 0x08375d4c：成员容器自动构造（含 6×匿名 _381@+0x5c8 循环调其 C1），
    // 函数体仅调 clear()；m_skillSelect/m_fieldCC/m_creationConst 的初值由
    // clear()（0x08375bca）写入，ctor 不再重复赋值（ORIG 实证无这些 store）。
    clear();
}