// ============================================================================
// stGenerateRefData_t —— ORIG ctor 0x0814a706：
//   +0x00=100；+0x04=0；+0x08 vector<bool>::clear + push_back(false)×2；
//   +0x1c=1.0f；+0x20=0；+0x24=0；+0x25=0；+0x28=11；+0x2c=0；+0x30=0
// dtor 0x0814aaf0：仅析构 +0x08 vector<bool>（成员析构自动完成）。
// ============================================================================
stGenerateRefData_t::stGenerateRefData_t()
{
    m_dropRate = 100;
    // b28d-54 翻正（P238）：ORIG 0x0814a9f8 `mov $0x0,%edx; mov %edx,0x4(%eax)`
    //   常数经 edx 中转；int 字段恒折单条 movl。经 D53 的匿名 union float 视图
    //   `m_field04f = 0.0f` 复现中转（sizeof/偏移/mangled 不变）。
    m_field04f = 0.0f;
    m_forceTag.clear();
    m_forceTag.push_back(false);
    m_forceTag.push_back(false);
    m_field1c = 1.0f;
    m_premiumDropRate = 0;
    m_field24 = 0;
    m_field25 = 0;
    m_field28 = (ENUM_CHARACTERJOB)11;   // P254 enum 字段（ctor 常数写入，同宽零码）
    m_field2c = 0;
    m_field30 = 0;
}