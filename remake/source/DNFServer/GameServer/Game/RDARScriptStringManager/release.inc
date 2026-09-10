void RDARScriptStringManager::release()
{
    // ORIG 0x8aa5014：begin/end 迭代器循环逐元素 delete，随后 clear vector。
    std::vector<StringDictionary*>::iterator it = dictionaries_.begin();
    std::vector<StringDictionary*>::iterator e = dictionaries_.end();
    for (; it != e; it++)
    {
        delete *it;
    }
    dictionaries_.clear();
