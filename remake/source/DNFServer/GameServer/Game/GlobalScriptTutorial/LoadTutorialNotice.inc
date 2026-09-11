// b28d-20: ORIG 0x08ab95ac。修法：while(true)+`if(!ScanInt×3||!ScanStr)break;`（P133）；
// 槽序 key,field8,fieldc,path（4.4 后声明占深槽，ORIG path@-0x18/key@-0xc）。
// 结果 IDENTICAL_AE（84=84）。
void GlobalScriptTutorial::LoadTutorialNotice(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    while (true)
    {
        int key, field8, fieldc;
        std::string path;
        if (!ScanInt(&key) || !ScanInt(&field8) || !ScanInt(&fieldc) || !ScanStr(&path))
            break;
        m_data[static_cast<int>(job)].addTutorialNotice(key, field8, fieldc, path.c_str());
    }
}