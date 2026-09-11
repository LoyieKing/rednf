// ORIG clear 0x084b40b0（报告缺失，objdump 直转）：+0x08=-1、+0x0c=0。
void CBoosterTest::clear()
{
    m_mode = -1;
    m_count = 0;
}