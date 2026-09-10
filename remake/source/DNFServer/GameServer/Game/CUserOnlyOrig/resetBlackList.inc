void CUser::resetBlackList()
{
    // ORIG 0x0867f1ba：`test %al,%al; jne` 直落形——空 then + else + 尾 return
    // （probe16 r4 实证：if(b){}else{X;}return; → call;test;jne skip;call X;jmp;nop;，
    //   规避技巧1 的 xor 物化；ORIG 0x0867f1d0 nop + 0x0867f1ce jmp 尾岛同步复现）
    if (m_vec8cef4.empty())
    {
    }
    else
    {
        m_vec8cef4.clear();
    }
    return;
