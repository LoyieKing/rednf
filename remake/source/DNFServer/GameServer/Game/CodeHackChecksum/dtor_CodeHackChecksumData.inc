CodeHackChecksumData::~CodeHackChecksumData()             // T @0x0808ac98
{
    m_checksums.clear();                                  // ORIG 先显式 vector::clear@0808ad4c，再走成员析构
}