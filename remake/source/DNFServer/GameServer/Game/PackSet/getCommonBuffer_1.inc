unsigned char* IPackFileProcess::getCommonBuffer(unsigned int needSize)  // @0x08ac773e
{
    if (!(needSize <= s_common_buffer_size_))   // ORIG cmp %eax,mem; jbe 方向
    {
        if (s_common_buffer_ != 0 && s_common_buffer_ != 0)
        {
            operator delete[]((void*)s_common_buffer_);
        }
        s_common_buffer_ = new unsigned char[needSize];
        s_common_buffer_size_ = (s_common_buffer_ != 0) ? needSize : 0;   // ORIG 三元 je/两臂形
    }
    return s_common_buffer_;
}