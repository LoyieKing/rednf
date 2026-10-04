void CUser::setCharacAntibotSerialNum(int v)
{
    *(int*)((char*)&m_antibotSerialNum) = v;
}