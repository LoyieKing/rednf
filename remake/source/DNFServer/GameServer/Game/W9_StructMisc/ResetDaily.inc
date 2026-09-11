// ORIG T 0x08643624：enterCount=0 + CData::alter()
void TOD_UserState::ResetDaily()
{
    m_data.m_enterCount = 0;
    alter();
}