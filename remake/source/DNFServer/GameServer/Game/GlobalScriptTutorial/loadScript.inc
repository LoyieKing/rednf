// b28d-20: ORIG 0x08ab8e4a。修法：外层/内层扫描循环改 while(true)+`if(!ScanType/ScanStr)break;`
// （P133/P225 负极性短路：ORIG 每条件 xor$1/test + mov$1/$0 汇聚岛 + ebx 循环标志，4.4 对
// `while(ScanType(token,true))` 折成正极性直跳）；token/s 移入循环体（ORIG 每轮构造/析构）；
// 尾部迭代器遍历改索引 size()/operator[] 形（P175）。结果 IDENTICAL_AE（132=133，尾 nop 剥离）。
bool GlobalScriptTutorial::loadScript()
{
    if (!loadRDARScriptFile("Etc/GlobalTutorialInfo/", "CharacterList.lst"))
        return false;

    std::vector<std::string> files;
    while (true)
    {
        std::string token;
        if (!ScanType(token, true))
            break;
        if (token == "[list]")
        {
            while (true)
            {
                std::string s;
                if (!ScanStr(&s))
                    break;
                files.push_back(s);
            }
        }
    }
    unloadRDARScriptFile();

    for (unsigned int i = 0; i < files.size(); ++i)
    {
        if (!loadScript("Etc/GlobalTutorialInfo/", files[i].c_str()))
            return false;
    }
    return true;
}