void STAICharacterMapArrangeData::Clear()
{
    m_type = 0;
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 0;
    m_camp = 0;
    m_roleType = 0;
    m_posX = 0;                             // +0x18 movb $0
    m_posY = 0;                             // +0x19 movb $0
    m_championFeatures.clear();             // vector<int>@0x1c
    m_npcIdx = -1;                          // +0x28 movl $-1
    m_collisionGroup.clear();               // vector<uchar>@0x2c
