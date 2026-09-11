// ORIG 0x08234cd4：type∈[0,1] → *(int*)(amp + 0x9c + type*4)；越界返回 0。
int CItemAmplifier::getPurifyMaterialItemIdx(int type) const
{
    if (type >= 0 && type <= 1)
    {
        return m_script.m_purifyMatItem[type];
    }
    return 0;
}