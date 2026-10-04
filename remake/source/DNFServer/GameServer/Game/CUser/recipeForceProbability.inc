void CUser::recipeForceProbability(int v)
{
    *(int*)((char*)&m_recipeProbability) = v;
}