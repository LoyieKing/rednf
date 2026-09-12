// [R25scanLD 2026-09-13 · 类型轴 ④] 返回类型随 CUserCharacInfo.h:784 改 bool：
//   ORIG 定义体 13 条（0x085bfcf6）无 setne 归一化 = bool 字段 + bool 返回；
//   消费点 0x085adc8a `call; test %al,%al; je` 无 setne（P292）双向闭合。
bool CUserCharacInfo::IsExpItemAffected()
{
    return m_selected ? CUR->m_isExpItemAffected : 0;
}