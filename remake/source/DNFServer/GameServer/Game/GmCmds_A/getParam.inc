void BlueMarbleSet::getParam(std::string param)
{
    int pos = 0;
    // [b28d-solo-GmCmds_A] ORIG +0x009 `movl $0,-0x1c` 在 tokens 构造循环之前
    // ⇒ pos 声明在 tokens 之前。
    std::string tokens[16];
    // [b28d-solo-GmCmds_A] ORIG 用成员 m_param(+0xc) 作 tokens 下标与计数
    // （`mov 0x8(%ebp),%eax; mov 0xc(%eax),%edx`，parse 已清零），非局部 used
    // ⇒ 删 `int used = 0;`（ORIG 函数头无该清零）。
    while ((pos = static_cast<int>(param.find(' ', 0))) != -1)
    {
        tokens[m_param] = param.substr(0, pos);
        param = param.substr(pos + 1, std::string::npos);
        ++m_param;
    }
    tokens[m_param] = param;

    if (tokens[0].compare("move") == 0)
    {
        m_flag1 = 1;
    }
    else if (tokens[0].compare("enter") == 0)
    {
        m_flag2 = 1;
    }
    m_flag0 = static_cast<unsigned char>(atoi(tokens[1].c_str()));
