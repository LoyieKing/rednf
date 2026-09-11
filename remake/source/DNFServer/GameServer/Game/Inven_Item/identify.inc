// ORIG W 0x0822b3d8：返回 bool = !isIdentified()（已识别→false 不改动）。
bool stAmplifyOption_t::identify()
{
    if (isIdentified())
    {
        return false;
    }
    m_abilityType &= 0x7f;
    return true;
}