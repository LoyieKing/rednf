// ----------------------------------------------------------------------------
// getValue(int index) const（0x8ab2668）
//   越界返回 0，否则 entities_[index].value。
// ----------------------------------------------------------------------------
const char* StringDictionary::getValue(int index) const
{
    return (index >= 0 && index < static_cast<int>(entities_.size()))
               ? entities_[index].value : 0;
}