// ORIG 返回单字节：isInPathArea 调用点 0x08891f20 段为 `call; test %al,%al; je`
// （4.4 对 int 返回 callee 恒先 `test %eax; setne %al` 物化）⇒ 返回 bool（P3/P9）。
bool isInArea(int x, int y, MapArea* area)
{
    if (x < area->m_x)
        return 0;
    if (x >= area->m_x + area->m_w)
        return 0;
    if (y < area->m_y)
        return 0;
    if (y >= area->m_y + area->m_h)
        return 0;
    return 1;
}