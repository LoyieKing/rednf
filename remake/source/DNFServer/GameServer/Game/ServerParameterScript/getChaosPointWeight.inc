// ORIG 0x082f0410：idx<=0 → 0；idx>size 时 clamp 到 size；返回 v[idx-1]。
float ServerParameterScript::getChaosPointWeight(int idx)
{
    if (idx <= 0)
    {
        return 0.0f;
    }
    int size = m_float5a4.size();
    if (idx <= size)
    {
    }
    else
    {
        idx = size;
    }
    return m_float5a4[idx - 1];
}