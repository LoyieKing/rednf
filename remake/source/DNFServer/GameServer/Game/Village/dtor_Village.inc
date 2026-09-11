Village::~Village()
{
    destroy();
    // m_users（map）由编译器隐式析构（ORIG：destroy() 后 map D1）
}