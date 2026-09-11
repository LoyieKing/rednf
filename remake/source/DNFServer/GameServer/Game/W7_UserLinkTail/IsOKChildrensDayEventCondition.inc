// ---------------------------------------------------------------------------
// CUser::IsOKChildrensDayEventCondition() const 语义（ORIG 无 const，按头）
// W 0x08328220：state 字节 == 0xff ⇒ true。
// ---------------------------------------------------------------------------
bool CUser::IsOKChildrensDayEventCondition()
{
    // ORIG 0x08328220：movzbl 0x8e098(%eax)（具名 m_childrensDayEventState）；==0xff → true。
    if (m_childrensDayEventState == 0xff)
        return true;
    return false;
}
