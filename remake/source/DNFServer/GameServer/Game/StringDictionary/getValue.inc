const char* StringDictionary::getValue(int index) const
{
    return (index >= 0 && index < static_cast<int>(entities_.size()))
               ? entities_[index].value : 0;
