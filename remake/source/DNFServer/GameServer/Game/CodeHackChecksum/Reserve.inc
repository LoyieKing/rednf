void CodeHackChecksumData::Reserve(long count)            // T @0x0808a62e = vector::reserve
{
    m_checksums.reserve(static_cast<unsigned int>(count));
}