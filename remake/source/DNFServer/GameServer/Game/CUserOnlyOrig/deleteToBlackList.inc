// ---------------------------------------------------------------------------
// 19. CUser::deleteToBlackList(unsigned int) T 0x0867f040
bool CUser::deleteToBlackList(unsigned int id)
{
    // ORIG 0x0867f056：`test %al,%al; je +0x12` ⇒ 正向 empty() 早退岛：
    //   867f05a: mov $0x0,%eax; 867f05f: jmp 867f102（技巧6 早退尾岛）
    if (m_vec8cef4.empty())
        return 0;
    // ORIG 0x0867f040：begin() 拷贝初始化 iterator，无默认构造（无 C1 符号）
    {
        std::vector<unsigned int>::iterator it = m_vec8cef4.begin();
        for (; it != m_vec8cef4.end(); ++it) {
            if (*it == id) {
                m_vec8cef4.erase(it);
                return 1;
            }
        }
    }
    return 0;
}