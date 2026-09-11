// getConvertResult —— ORIG T 0x88b1e48：先默认构造迭代器再赋值 find 结果；
// 命中返回指针，未命中 0。
AvatarConvert::ConvertResult* AvatarConvert::getConvertResult(int grade)
{
    std::map<int, ConvertResult*>::iterator it;
    it = m_convertMap.find(grade);
    if (it != m_convertMap.end())
        return it->second;
    return 0;
}