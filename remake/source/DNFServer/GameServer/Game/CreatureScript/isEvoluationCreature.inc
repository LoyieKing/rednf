bool CCreatureScript::isEvoluationCreature(int creatureId)
{
    // 修正：原实现误拷贝 m_vec8 到局部（ORIG 无 copy ctor），现按 ORIG 直访
    // 且 size() 缓存进 n（P0 具名局部），与 ORIG +0x0b/+0x1c 的缓存判据一致。
    int n = m_vec8.size();
    for (int i = 0; i < n; ++i)
    {
        if (m_vec8[i] == creatureId)
        {
            return true;
        }
    }
    return false;
