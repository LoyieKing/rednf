// b28d-20: ORIG 0x08ab930a。修法：while(true)+`if(!ScanInt||!ScanInt)break;`（P133/P225，
// ORIG xor$1/test/jne + mov$1/$0 汇聚岛）。结果 IDENTICAL_AE（40=40）。
void GlobalScriptTutorial::LoadAvatarTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int type, index;
    while (true)
    {
        if (!ScanInt(&type) || !ScanInt(&index))
            break;
        m_data[static_cast<int>(job)].addAvatarIndex(type, index);
    }
}