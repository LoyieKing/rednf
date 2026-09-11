// ---------------------------------------------------------------------------
// SigAradEventItemLog —— ORIG W C1/C2 0x8694f14 -> init() 0x8694f28：
//   movl $0x0,(%eax)[m_itemId]; movl $0x0,0x4(%eax)[m_itemCount]
// 仅清前两 dword（m_id/m_characNo 不初始化，与 ORIG 一致）。
// ---------------------------------------------------------------------------
SigAradEventItemLog::SigAradEventItemLog()
{
    init();
}