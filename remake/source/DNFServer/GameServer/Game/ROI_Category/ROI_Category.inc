ROI_Category::ROI_Category()
{
    // ORIG 0x0822ac10: 前两 movl 共享基址（8 字节整块），第三重读 this —— long long 块清 + 第三 dword
    *(long long*)this = 0;
    m_dw8 = 0;                       // ORIG movl $0,0x8(%eax)（disp 折叠）
