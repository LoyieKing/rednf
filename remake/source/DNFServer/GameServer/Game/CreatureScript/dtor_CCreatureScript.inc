// ORIG 0x833c848：释放 +0x20 数组（delete[]），析构 +0x68/+0x34/+8。
// [b28-BM7 判词] we-larger 22 = N89：ORIG 3 处容器 clear 为 4.1 发射的
// out-of-line 模板实例（0x8083dda/0x80d9f34，objdump 误标 _GLOBAL__I），
// 4.4 libstdc++ 全内联展开循环+delete call；EH landing 双段同构。语义全等。
CCreatureScript::~CCreatureScript()
{
    if (m_artifactSlotFlags != 0)
    {
        delete[] m_artifactSlotFlags;
        m_artifactSlotFlags = 0;
    }
}