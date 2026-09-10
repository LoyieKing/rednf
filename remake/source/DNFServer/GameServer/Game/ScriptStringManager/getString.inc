bool ScriptStringManager::getString(int index, std::string* outputString) const
{
    outputString->clear();  // ORIG: _M_mutate(0, length, 0)
    // [b23S3C] ORIG 0x8acf160：正索引无界检查直 assign(start[index])；
    // 负索引仅 (int)size()<index 死检查（js 分离形）。勿加 unsigned 界保护改行为。
    if (index < 0)
    {
        if (static_cast<int>(strings_.size()) < index)
            return false;
    }
    outputString->assign(strings_[index]);  // [b23S3C] begin() 形实测反劣化（+2 out-of-line），保 operator[]
    return true;
