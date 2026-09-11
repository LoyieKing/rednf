// ---- cMyTrace（GameTypes.h 声明，ORIG 0x854f718/0x854f746）----
// ORIG 3 参 0x854f718：m_field_c=1、m_field_d=0、m_name/m_line/m_flag 直存。
cMyTrace::cMyTrace(const char* name, int line, int flag) throw()   // [Main 全局 A/B 实验] 见 GameTypes.h 注释
{
    m_field_c = 1;
    m_field_d = 0;
    m_name = name;
    m_line = line;
    m_flag = flag;
}