// ORIG 0x0864a830：lea -0x20(%ebp) 调 CommonTime::SetCurTime（0x082a4224），
// 之后只读 -0x20..-0x1c 五个日期字节（年/月/日/时/分），与 m_delReqTime[+0x39]
// 比较。SetCurTime 还会把 time() 写到对象 +0x08/+0x0c（0x082a42a9、0x082a42b2）；
// 原版帧 0x34 里这两条落在 -0x18/-0x14 的死槽，函数不再读。
// 8 字节 char 缓冲装不下这两条，g++ 13 上打金丝雀。char[0x10] 只是写入宽度，
// 不是类型。这里要的就是当前 CommonTime。
bool CUser::CheckDeleteCharacTime(int slot)
{
#if !remake_verify
    // 创建时 CreateCharac 把 SetCurTime 写入 +0x39。同一天删角这里返回 false，
    // check_error 回 0x16，send 发错误包。remake 频道关掉这天限制。
    (void)slot;
    return true;
#else
    CommonTime now;
    now.SetCurTime();
    if ((unsigned char)m_characList[slot].m_delReqTime[0] == (unsigned char)now.m_year
        && (unsigned char)m_characList[slot].m_delReqTime[1] == (unsigned char)now.m_month
        && (unsigned char)m_characList[slot].m_delReqTime[2] == (unsigned char)now.m_day)
    {
        int req = (int)m_characList[slot].m_delReqTime[3] * 60
                  + (int)m_characList[slot].m_delReqTime[4];
        int cur = (int)now.m_hour * 60 + (int)now.m_min;
        if (cur - req > 0x59f)
            goto RET1;
        return false;
    }
    RET1:
    return true;
#endif
}