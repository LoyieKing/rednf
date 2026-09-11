// b28d-20: ORIG 0x08ab9226。修法：while(true)+`if(!ScanInt||!ScanInt||!ScanStr)break;`（P133）；
// 槽序 key,face,text —— 4.4 后声明占深槽，ORIG text@-0x14/key@-0xc/face@-0x10（P28 4.4 反向）。
// 结果 IDENTICAL_AE（76=77，尾 nop 剥离）。
void GlobalScriptTutorial::LoadDialogTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    while (true)
    {
        int key, face;
        std::string text;
        if (!ScanInt(&key) || !ScanInt(&face) || !ScanStr(&text))
            break;
        m_data[static_cast<int>(job)].addDialogData(key, face, text.c_str());
    }
}