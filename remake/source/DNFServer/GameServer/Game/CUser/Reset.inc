// ORIG W 0x08486818：PlayInfo::Reset() 后清 +0x3c。
void Character::Reset()
{
    PlayInfo::Reset();
    m_f3c = 0;
}