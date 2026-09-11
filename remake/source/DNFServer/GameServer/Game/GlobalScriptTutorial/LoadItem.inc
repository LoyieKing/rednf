// b28d-20: ORIG 0x08ab96a8。修法：while(true)+`if(!ScanInt||!ScanInt)break;`（P133/P225）。
// 结果 IDENTICAL_AE（40=40）。
void GlobalScriptTutorial::LoadItem(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int item, count;
    while (true)
    {
        if (!ScanInt(&item) || !ScanInt(&count))
            break;
        m_data[static_cast<int>(job)].addItem(item, count);
    }
}