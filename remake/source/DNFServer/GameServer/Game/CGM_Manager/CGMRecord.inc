// ORIG C1/C2 0x082990b4：movb $0,(+0)；movl $0,(+4)（函数体赋值形态，无 ecx 缓存）。
CGMRecord::CGMRecord()
{
    m_mode = 0;
    m_level = 0;
}