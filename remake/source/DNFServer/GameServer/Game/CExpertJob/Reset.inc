// ORIG 0x849f252：先清 +0x0c/+0x0e（short），再清 +0x00/+0x04/+0x08。
void ExpertJobMaterial::Reset()
{
    m_slot = 0;
    m_field0e = 0;
    m_user = 0;
    m_item = 0;
    m_itemIndex = 0;
}