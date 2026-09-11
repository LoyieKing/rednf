 // ---------------------------------------------------------------------------
// CUser::setCharacSlotLimit(unsigned char limit) W 0x0869755c sz0x1b
// 报告缺失（nm 0x0869755c 定址后 objdump 忠实转写）：
//   mov %al,-0x4; mov 0xc(%ebp)→eax? 实参经 -0x4 截断回读 movzbl 后写
//   this+0x8d245 = m_ceraUserInfo.m_field61（与 getCharacSlotLimit 读侧对称）。
// ---------------------------------------------------------------------------
void CUser::setCharacSlotLimit(unsigned char limit)
{
    m_ceraUserInfo.m_field61 = limit;
}