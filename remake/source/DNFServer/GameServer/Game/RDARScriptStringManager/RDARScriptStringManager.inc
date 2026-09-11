RDARScriptStringManager::RDARScriptStringManager()
{
    // ORIG C1 0x8aa4e98：STScriptFileList(+0) / vector<StringDictionary*> (+0x18)
    // / wstring (+0x24) 依次默认构造。头成员 fileList_/dictionaries_/tag_ 由
    // 成员初始化列表默认构造（map/vector/wstring 空构造），体为空即达 ORIG 语义。
}