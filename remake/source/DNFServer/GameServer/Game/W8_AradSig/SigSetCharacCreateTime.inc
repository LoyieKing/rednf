// ---------------------------------------------------------------------------
// SigSetCharacCreateTime —— ORIG W：
//   C1/C2 0x0822ee3c -> init() 0x0822ee50 -> set(0,0) 0x0822ee74；
//   set(a,b)：+0=m_characNo=a、+4=m_createTime=b（两个 dword 直写）
// ---------------------------------------------------------------------------
SigSetCharacCreateTime::SigSetCharacCreateTime()
{
    init();
}