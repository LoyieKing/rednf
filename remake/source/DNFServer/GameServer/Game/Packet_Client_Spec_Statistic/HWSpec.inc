// ============================================================================
// LinkFinal2 核销 —— HWSpec::HWSpec()（ORIG W @0x0822b890，0x1f）。
// objdump 逐指令转写：+0x00 byte=0xff；+0x04/+0x08 dword=0xffffffff。
// ============================================================================
HWSpec::HWSpec()
{
    m_category1 = static_cast<char>(0xff);   // ORIG movb $0xff,(%eax)
    m_category2 = 0xffffffffu;               // ORIG movl $0xffffffff,+0x04
    m_category3 = 0xffffffffu;               // ORIG movl $0xffffffff,+0x08
}