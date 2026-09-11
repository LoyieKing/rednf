AreaData::AreaData()
{
    // ORIG C1 only constructs m_slotImage, m_slots, and m_vec20; it does not
    // call clear().  Their C++ member initializers perform those operations.
}