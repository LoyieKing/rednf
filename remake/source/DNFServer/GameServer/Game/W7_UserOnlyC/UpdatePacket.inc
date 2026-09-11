// 返回错误码：0=正常；1=同包洪水；3=timechecker 节流；4=拍卖包洪水。
// b28d-solo-W7_UserOnlyC 判词：P225 while+break 复现 setg 物化 + m_checksum 经
// HeaderChecksumIntView int 视图同型直存（P211）⇒ 120=120，TU 层 AE。
// 本件无 class_func_reports 报告（报告缺失，按 nm 切片 + objdump 兜底）。
int CPacketVerifier::UpdatePacket(const _header& hdr)
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    if (!m_checker.check())
        return 3;

    // 拍卖类包（id 0xb9..0xc2）独立计数窗口。
    // P0：ORIG 无 id 槽，movzwl 后直接 sub/cmp。
    if ((unsigned int)(hdr.m_id - 0xb9) <= 8)
    {
        ++m_auctionCount;
        // P225：ORIG 0x8696ead `setg %al; test %al,%al; je` 为 4.1 的 bool 物化条件；
        // 4.4 对裸 if 恒折 `jle` ⇒ 用 while+break 单趟形复现（b28 BL5 实证）。
        while (m_auctionCount > m_nAuctionVaridCount)
        {
            long now = time(0);
            if (now - m_auctionTime < m_nAuctionVaridSecond)
                return 4;
            m_auctionCount = 0;
            m_auctionTime = now;
            break;
        }
    }

    // ORIG 比较按 int 直读 class/id/checksum。
    if (static_cast<int>(hdr.m_class) == m_lastClass &&
        static_cast<int>(hdr.m_id) == m_lastId &&
        hdr.m_checksum == m_lastChecksum)
    {
        ++m_sameCount;
        if (m_sameCount > m_nVaridCount)
        {
            long now = time(0);
            if (now - m_sameTime < m_nVaridSecond)
                return 1;
            m_sameCount = 0;
            m_sameTime = now;
        }
    }
    else
    {
        m_lastClass = hdr.m_class;
        m_lastId = hdr.m_id;
        // P211：int 具名视图令 RHS 与 int 字段同型 ⇒ 复现 ORIG `mov 0x7(%eax),%edx`。
        m_lastChecksum =
            reinterpret_cast<const HeaderChecksumIntView*>(&hdr)->m_checksum;
        m_sameCount = 0;
        m_sameTime = time(0);
    }
    return 0;
}