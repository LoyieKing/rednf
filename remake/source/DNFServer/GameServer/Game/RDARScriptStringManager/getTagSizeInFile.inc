int RDARScriptStringManager::getTagSizeInFile(int table)
{
    // ORIG 0x8aa5df2
    if (static_cast<unsigned>(table) < dictionaries_.size())
        return dictionaries_[table]->getCount();
    return -1;
