bool RandomOptionField::empty() const
{
    return m_field0 == 0;  // W 0x811ed22：movzbl (%eax); test; sete
