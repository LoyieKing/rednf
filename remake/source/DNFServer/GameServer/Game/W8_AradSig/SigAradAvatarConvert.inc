// ---------------------------------------------------------------------------
// SigAradAvatarConvert —— ORIG W C1/C2 0x081a0a6a -> init() 0x081a0a7e：
//   movw $0x0,(%eax)[m_selectNo]; movw $0x0,0x2(%eax)[m_slot];
//   movl $0x0,0x4(%eax)[m_itemId]; movl $0x0,0x8(%eax)[m_resultItemId];
//   movw $0x0,0xc(%eax)[m_resultCount]
// ---------------------------------------------------------------------------
SigAradAvatarConvert::SigAradAvatarConvert()
{
    init();
}