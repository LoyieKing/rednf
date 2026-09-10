bool CUser::isSecurityServiceUsed()
{
    // ORIG 0x8688689/0x86886a0：|| 短路链双 `test; jne mov$1` 共享尾岛 + 末项
    // movzbl flag 的 `je mov$0 / fall mov$1` + bool 结果二次物化岛。
    if (getPad()->isActivate() || getSecurityCard()->isActivate() ||
        m_hades.m_secuGrade)
        return true;
    return false;
