void STDialog::clear()                      // ORIG 0x089e8a5e
{
    m_name.clear();
    for (int i = 0; i < 3; ++i)             // ORIG movb $0,0x4(%edx,%eax,1) 循环
        m_flag[i] = false;
}