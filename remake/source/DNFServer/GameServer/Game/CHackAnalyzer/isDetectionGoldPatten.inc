bool Secu_AccountHacking::isDetectionGoldPatten()
{
    // ORIG W 0x082792a0（isDetectionGoldPatten.md）：前四标志任一置位；
    // 线性测试→共享 true 块（P45），用 || 短路链逐字同构。
    return m_flag[0] || m_flag[1] || m_flag[2] || m_flag[3];
}