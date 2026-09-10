short CUser::GetIncreID() const
{
    // 字段 short（Main 回滚 :2248，混合号性 N 类）——ORIG 0x82a6842 `movzwl` 需
    // (unsigned short) 中间 cast 形（short 直读反产 movswl/符号形）。
    return (short)(unsigned short)m_field704a6;
