int WideStringData::getLength() const
{
    return static_cast<int>(static_cast<unsigned int>(m_size) >> 2) - 1;
