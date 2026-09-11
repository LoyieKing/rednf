// ORIG W 0x084ed216 (size 0xb)：return this+0xc 的 int（现头具名字段
// m_mailLoadCount，原 m_pad0c 区）。
int WongWork::CMailBox::getMailLoadCount() const
{
    return m_mailLoadCount;
}