// ORIG 0x082f03bc：idx∈[1,200]，取 m_vec588(+0x588)[idx-1]。
int ServerParameterScript::getAssaultPrice(int idx)
{
    if (idx > 200 || idx <= 0)
    {
        return 0;
    }
    return m_vec588[idx - 1];
}