// ============================================================================
// ORIG T 0x0864d90a：从 m_list711ec（+0x711ec list<int>）删除 state。
// ============================================================================
void CUser::RemoveRestrictedGoods(int state)
{
    // ORIG 0x0864d90a：b/end/it 三迭代器各落槽 -0x10/-0x14/-0x18——end() 仅
    // call 一次（具名 e 局部缓存，it != e 复用）；find 结果槽 sret -0x18。
    std::list<int>::iterator b = m_list711ec.begin();
    std::list<int>::iterator e = m_list711ec.end();
    std::list<int>::iterator it = std::find(b, e, state);
    if (it != e)
        m_list711ec.erase(it);
}