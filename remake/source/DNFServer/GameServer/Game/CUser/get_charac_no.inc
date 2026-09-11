int CUser::get_charac_no(int param)
{
    // ORIG 0x08677a?? prologue cmp $-1 的 fall-through 块 = param==-1 路径
    //（技巧 17：等号形让 -1 块前置，!= 形被 4.4 后置）。
    if (param == -1)
    {
        if (getCurCharacR() == 0)
            return 0;
        return *((unsigned int*)getCurCharacR());
    }
    if (param < 0 || (int)m_characList.size() <= param)
        return 0;
    return *((unsigned int*)&m_characList[param]);
}