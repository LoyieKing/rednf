bool CUser::isUseCraneStart() const   // [P354/P292 判据] ORIG 调用点 xor $1 形 ⇒ bool（原 unsigned char 为误标）
{
    return m_field8eb98;
}