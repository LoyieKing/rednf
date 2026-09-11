// ORIG 0x08234cfa：type∈[0,1] → *(int*)(amp + 0xa4 + type*4)；越界返回 0。
int CItemAmplifier::getPurifyMaterialItemCount(int type) const
{
    if (type >= 0 && type <= 1)
    {
        return m_script.m_purifyMatCount[type];
    }
    return 0;
}