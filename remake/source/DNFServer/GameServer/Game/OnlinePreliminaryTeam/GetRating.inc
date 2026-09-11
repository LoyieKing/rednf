// ORIG W 0x08568eee GetRating() const【推断：objdump】：flds *(float*)(this+8)。
float COnlinePreliminaryTeam::GetRating() const
{
    return m_rating08;   // ORIG flds *(float*)(this+8)
}