int Dispatcher_CeraPackageOpen::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* const M = reinterpret_cast<char*>(&msg);
    // +0x10 数量字节落在冻结头 m_pad10 区（MSG_OPEN_CERAPACKAGE 无具名字段，
    // 已 hub 报 Main 补 m_itemCount；此处按具名成员（内联视图，零新增槽）访问）
