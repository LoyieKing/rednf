// 字段化后保持占位版语义：+0xd 包头后全量清零（ORIG 无出线 ctor，此为本仓库构造约定）。
MSG_BUY_SKILL::MSG_BUY_SKILL()
    : m_treeKind(0), m_count(0)
{
    memset(m_entries, 0, 4);  // ORIG 0x8230956 仅清 +0xf 起 4 字节
}