// ORIG @0x08ad5302：m_pRes->get(m_id, out)，返回 bool（test %al）。
bool StringResourceDelayedItem::get(CharString const** out) const
{
    return m_pRes->get(m_id, out);
}