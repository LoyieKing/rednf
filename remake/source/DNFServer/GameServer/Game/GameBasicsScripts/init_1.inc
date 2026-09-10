void stAvatarExpansionInfo_t::init()
{
    // ORIG 0x083422d：for(i=0;i<=1;++i) 循环写 movw $0,(%eax,%edx,2)（P25 setle 形）。
    for (int i = 0; i <= 1; ++i)
        m_color[i] = 0;
