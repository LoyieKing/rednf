int CPacketVerifier::GetSequence()
{
    // b28d-D52 终裁（推翻旧 N18a 判词）：ORIG 0x08696e4e/0x08696e52 双段
    // `movzwl 0x14(%eax),%eax; movzwl %ax,%eax` = 字段 unsigned short 读 + 返回
    // 类型 int 的零扩展（探针 p_ushort：ushort 返回单段、int 返回双段全等）。
    return (unsigned short)m_sequence;
}