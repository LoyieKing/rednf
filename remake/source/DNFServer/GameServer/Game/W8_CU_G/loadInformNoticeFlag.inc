void CUser::loadInformNoticeFlag(char* src)
{
    resetInformNoticeFlag();
    if (src != 0)
    {
        for (int i = 0; i < 0x10; ++i)
        {
            // b28d-W8: ORIG 0x0868f929 单段 `movzbl (%edx),%ecx`；4.4 恒
            // `movzbl (%edx),%edx; mov %edx,%ecx` 两段（+1 条，N18a 反向）；
            // P266 uchar 左值读旋钮 / 隐式转换两形实测同为 29 ⇒ 封 N。
            // b28d-W8b 复测：显式 (unsigned char) cast 无效（同 29）⇒ P266 左值读旋钮。
            m_informNoticeFlags[i] = *reinterpret_cast<unsigned char*>(src + i);
        }
        sendInformNoticeFlag();
    }
    else
    {
        return;
    }
