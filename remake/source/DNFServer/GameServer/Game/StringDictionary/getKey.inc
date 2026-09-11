// ----------------------------------------------------------------------------
// getKey(int index) const（0x8ab2628）
//   越界（<0 或 >= size）返回 0，否则 entities_[index].key。
// ----------------------------------------------------------------------------
const char* StringDictionary::getKey(int index) const
{
    return (index >= 0 && index < static_cast<int>(entities_.size()))
               ? entities_[index].key : 0;
}