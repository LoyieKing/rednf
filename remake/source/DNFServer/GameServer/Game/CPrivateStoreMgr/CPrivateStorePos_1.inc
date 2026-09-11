// ORIG W 0x85cb7fc：写宿主 store 指针后，以中心点 (centerX, centerY) 构造
// 0xa0 x 0x64 的碰撞矩形（左上 = 中心 - 半宽/半高）。
inline CPrivateStorePos::CPrivateStorePos(CPrivateStore* store, int centerX, int centerY)
    : m_rect(), m_pStore(store)
{
    m_rect.m_pos.m_x = centerX - 0x50;
    m_rect.m_width = 0xa0;      // ORIG 先写 width 再 m_y（-O0 语句序）
    m_rect.m_pos.m_y = centerY - 0x32;
    m_rect.m_height = 0x64;
}