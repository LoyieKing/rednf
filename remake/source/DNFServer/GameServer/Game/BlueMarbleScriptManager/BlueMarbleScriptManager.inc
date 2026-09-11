// BlueMarbleInfoScript（+0x30）、m_tiles/m_maps 两 map（成员默认构造即等价）；
// 全部成员构造后调用 clear()（0x88d622c，清两文件列表与两 map）。
BlueMarbleScriptManager::BlueMarbleScriptManager()
    : m_mapFileList(), m_tileFileList(), m_info(), m_tiles(), m_maps()
{
    clear();
}