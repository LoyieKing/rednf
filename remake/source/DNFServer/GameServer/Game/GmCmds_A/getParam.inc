// ORIG getParam 0x084b235c：按空格切 token（≤16），tokens[0]=="move"/"enter" 置
// 对应标记，atoi(tokens[1]) 写步数字节。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084b235c orig/ours=183/178
//   残差：槽号平移 + std::string 局部构造/拷贝实例差。
// [b28d-solo-GmCmds_A] verdict=IDENTICAL/AE ndiff=0 [P0/P212]
// 删局部 used 改用成员 m_param(+0xc) + pos 声明先于 tokens。
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
}