SigServerState::tagData::tagData() {}

// ---------------------------------------------------------------------------
// SigAvatarRoulette —— ORIG W C1/C2 0x081a092e -> init() 0x081a0942：
//   movb $0x0,(%eax); movb $0x0,0x1(%eax); movb $0x0,0x2(%eax)
// （清 m_type / m_slotIdx / m_field02 三字节，含消费侧未读的第 3 字节）
// ---------------------------------------------------------------------------
