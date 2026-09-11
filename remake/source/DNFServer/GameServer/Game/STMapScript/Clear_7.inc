void STQuestAnimation::Clear()              // ORIG 0x089e83ae
{
    STAssignAnimation::Clear();
    m_field14 = 0;                          // +0x14
    m_flag18 = true;                        // +0x18 movb $1
}