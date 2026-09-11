// ORIG W 0x08695f28。
bool LimitItemUsage::useIt()
{
    if (!isUsable())
        return false;
    m_field19 += 1;
    reinterpret_cast<CommonTime*>(this)->SetCurTime();
    return true;
}