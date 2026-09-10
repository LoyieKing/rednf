void CHackAnalyzer::_analyzeSpeedHack(unsigned int ms)
{
    // ORIG 0x080f7cca（_analyzeSpeedHack.md）：ratio=(int)(100.0f*ms/elapsed)；
    // ratio<=0x45 或 >0x82 时记 0x3c1。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_SPEED))
        return;

    if (m_field24 == 0)
    {
        _reportIncurrectReport();
        return;
    }

    long long numMs;                                     // ORIG 声明在前（槽最深），赋值在后

    int elapsed = m_field24 - m_field20;                 // ORIG fildl（先物化 elapsed）
    numMs = ms;                                          // ORIG fildll（有符号直转，无 2^64 修正段）
    // ORIG flds 在 fdivrp 之后 ⇒ 左操作数为内存 float 变量（乘法左序）。
    int ratio = (int)(((double)numMs / (double)elapsed) *
                      _S_HACK_RATIO_COEF_100F);

    if (ratio <= 0x45 || ratio > 0x82)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_SPEED, (ENUM_HACKTYPE)0x3c1, ratio);
    }
