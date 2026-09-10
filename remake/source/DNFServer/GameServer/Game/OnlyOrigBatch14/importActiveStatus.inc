void importActiveStatus(STActiveStatus& st, bool flag)
{
    // 16 处 push_back 均为 prvalue 直传（见函数头判词①）；ScanInt 结果直落
    // 各自匿名物化临时槽 -0x58..-0x1c，实参取该槽址，与 ORIG 逐处同形。
    // bool 形参不另建具名副本（P244：ORIG -0x6c 是 4.1 的一次性截断 spill）。
    int type;
    int i9;
    int iD;
    int i4;
    std::string s;
    // [gap 复核] 删除原 `bool f = flag;`：ORIG 0x088c1e96 `mov 0xc(%ebp),%eax;
    // mov %al,-0x6c(%ebp)` 是 4.1 对 **bool 形参**的一次性截断 spill（P244
    // 配套），不是第二个具名局部；写 f 会让 4.4 多发 `movzbl -0xac; mov %al,-0x9`
    // 两条（形参 spill + f 初始化）并把帧推大 0x40。

    ScanStr(&s);
    type = getActiveStatusType(s);
    st.m_type = type;
    st.m_value4 = ScanFloat((bool*)0);
    st.m_value8 = ScanInt((bool*)0);
    if (flag)
    {
        st.m_flagC = ScanInt((bool*)0) != 0;
    }
    st.m_value10 = ScanInt((bool*)0);
    if (type == 5)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 0)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 0xc)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 7)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 0xf)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 0x11)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 0xa)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 2)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 6)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 0xb)
    {
        st.m_list14.push_back(ScanInt((bool*)0));
    }
    else if (type == 9)
    {
        for (i9 = 0; i9 <= 3; ++i9)
        {
            st.m_list14.push_back(ScanInt((bool*)0));
        }
    }
    else if (type == 0xd)
    {
        for (iD = 0; iD <= 3; ++iD)
        {
            st.m_list14.push_back(ScanInt((bool*)0));
        }
    }
    else if (type == 4)
    {
        for (i4 = 0; i4 <= 3; ++i4)
        {
            st.m_list14.push_back(ScanInt((bool*)0));
        }
    }
