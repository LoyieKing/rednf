// ORIG W 0x08ac31a0：for i<size: if (first<=ch && ch<=second) return true；
// ORIG 用 vector::operator[](i) 两次取 first/second——保持成员访问形态。
bool RestrictInputtingNameData::isInRange(wchar_t ch) const
{
    for (unsigned int i = 0; i < unicodeRangeDataList_.size(); ++i)
    {
        // [b28c-L15 P198②] ORIG `test %al,%al; je` 直测汇聚寄存器无槽往返
        //   ⇒ 谓词整体内联进 if（提具名 bool 反多 mov%al,槽 + cmpb 两步）。
        if (unicodeRangeDataList_[i].first <= (int)ch &&
            (int)ch <= unicodeRangeDataList_[i].second)
            return true;
    }
    return false;
}