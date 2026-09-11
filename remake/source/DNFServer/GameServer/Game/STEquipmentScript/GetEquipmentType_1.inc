int GetEquipmentType(const char* name)
{
    // [OurOnly33 符号名轴 2026-09-11] ORIG 0x08990f1f 循环体 `mov 0x93f4fc0(,%eax,4),%eax`
    // = 全局表 `characterEquipmentTypeTags`（D, 0x68=26 指针；定义 InitScriptProducers_2.cpp，
    // 声明 DataManagerInitDepends.h:239）⇒ 删本函数内 static kNames
    //（我方发明名 `_ZZ16GetEquipmentTypePKcE6kNames`，ORIG 全 ELF 无）。
    for (int i = 0; i < 26; ++i)
    {
        if (strcmp(name, characterEquipmentTypeTags[i]) == 0)
            return i;
    }
    return 26;
}