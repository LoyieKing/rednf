unsigned char CUser::GetCharacJob(int idx) const   // CUser.h:3290 uchar（DCoreC6 caller cmp %al 证据，定义同步）
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return 0xb;
    // ORIG 0x822ff5c：单条 movzbl 0x26(%eax),%eax。4.4 对 char 字段两种写法均多一级
    // 扩展（值 cast=双 movzbl；地址重解释=add+双 movzbl），需字段 uchar 化（Main 评估中，
    // Dispatchers 0x6306 movsbl 依赖 char 语义）——暂取值 cast（ndiff 更小）。
    return (unsigned char)m_characList[idx].m_field26;
