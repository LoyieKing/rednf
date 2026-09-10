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
