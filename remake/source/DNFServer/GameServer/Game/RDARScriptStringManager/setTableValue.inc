void RDARScriptStringManager::setTableValue(int table, int key, const char* value)
{
    // ORIG 0x8aa5d98
    if (static_cast<unsigned>(table) < dictionaries_.size())
    {
        // ORIG：operator[] 结果落具名槽 -0xc 后解引用（P0 有槽往返）
        StringDictionary*& dict = dictionaries_[table];
        dict->setValue(key, value);
    }
}