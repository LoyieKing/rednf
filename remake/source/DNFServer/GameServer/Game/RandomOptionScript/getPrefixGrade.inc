// ORIG 0x08a73a80（非 const，_ZN mangled；循环前缓存 vector 指针，
// 命中存 result 后 break 汇合返回——对齐 ORIG -O0 布局）。
int RandomOptionScript::getPrefixGrade(int level)
{
    int result = 0;
    std::vector<ChoosePrefix>& vec = m_choosePrefix1d8;
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        if (level >= vec[i].m_min && level < vec[i].m_max)
        {
            result = vec[i].m_grade - 1;
            break;
        }
    }
    return result;
}