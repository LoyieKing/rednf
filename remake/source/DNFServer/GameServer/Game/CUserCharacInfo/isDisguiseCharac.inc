// ORIG W 0x0822f8f6：m_selected 非空时返回其 +0xee9 字节，否则 0。
// 返回 bool：调用点 0x0821b4d8 `xor $0x1,%eax;test %al,%al`（uchar/int 声明不出 xor，探针实证）。
bool CUserCharacInfo::isDisguiseCharac()
{
    if (m_selected)
    {
        return CUR->m_isDisguiseCharac;
    }
    return false;
}