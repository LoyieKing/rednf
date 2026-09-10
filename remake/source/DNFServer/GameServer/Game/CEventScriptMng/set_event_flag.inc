void CEventEntity::set_event_flag(bool flag)
{
    // ORIG 0x810b022：movzbl 参数 + mov %dl,0x4(%eax)（bool→uchar 直赋）。
    m_isEventing = flag;
