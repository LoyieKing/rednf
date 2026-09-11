// clear —— ORIG 0x8911cf6：j=0..10 双层清空 avatar 数组；stiker 数组不动。
// b28d-21: IDENTICAL（55=55）— P183 删 `vector& bucket = m_avatars[j]` 引用缓存
// （ORIG 每轮重发 j*12+0x14 全链）+ 具名 int n 槽 + 有符号比较（setl）。
void AvatarRoulette::clear()
{
    // b28d-21：ORIG 0x08911cf6 每轮都重发 `j*12+0x14` 完整寻址（P183 禁
    // `vector& bucket = m_avatars[j]` 缓存引用），且 size() 结果落具名 int 槽
    //（ORIG -0xc 每 j 轮算一次；循环尾 `cmp -0xc(%ebp),%eax; setl` 有符号比较）。
    for (int j = 0; j <= 10; ++j)
    {
        // ORIG 槽序：k=-0x10（深）、n=-0xc（浅）⇒ k 先声明/先使用。
        int k = 0;
        int n = static_cast<int>(m_avatars[j].size());
        for (; k < n; ++k)
        {
            m_avatars[j][k].clear();
        }
        m_avatars[j].clear();
    }
}