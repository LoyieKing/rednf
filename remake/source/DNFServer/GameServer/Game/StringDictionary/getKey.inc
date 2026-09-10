const char* StringDictionary::getKey(int index) const
{
    return (index >= 0 && index < static_cast<int>(entities_.size()))
               ? entities_[index].key : 0;
