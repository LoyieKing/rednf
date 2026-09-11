  // ORIG C1：+0x04 int 清零
stMonstercardBind::stMonstercardBind()
{  // ORIG 0x823175a：vector C1 先于标量（成员自动构造），随后拷贝链与 clear
    m_4 = 0;
    m_c = 0;
    m_a = m_c;
    m_8 = m_a;
    m_list.clear();
}