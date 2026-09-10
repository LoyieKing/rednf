void CBattle_Field::on_not_visited_map(int x, int y, bool& flag)
{
    flag = true;
    GetCurrentMapInfo()->m_visited = 1;              // movb $0x1,0x1(%eax)
    swap_curpos_xy(x, y);
    // ORIG：先求 GetCurrentMapInfo()->m_mapIndex（驻留 ebx），再取数据管理器，
    // 最后调 find_map —— 以单条嵌套表达式复现该求值顺序。
    CMap* map = static_cast<CMap*>(
        G_CDataManager()->find_map(GetCurrentMapInfo()->m_mapIndex));
    if (map != NULL)
        m_pMap = map;                                // this+0x108
