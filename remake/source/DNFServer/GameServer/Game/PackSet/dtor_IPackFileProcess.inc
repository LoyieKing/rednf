IPackFileProcess::~IPackFileProcess()       // D1 @0x08ac76a8（D2 别名）
{
    s_ref_count_ -= 1;
    if (s_ref_count_ < 1)
    {
        if (s_common_buffer_ != 0 && s_common_buffer_ != 0)
        {
            operator delete[]((void*)s_common_buffer_);   // 4.4 delete[] 自带判空，显式调用保 ORIG 双 test 形
        }
        s_common_buffer_ = 0;
        s_common_buffer_size_ = 0;
    }
}