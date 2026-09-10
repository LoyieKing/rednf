bool CInGameAdvertisementManager::CheckIngameAdvertisementCondition(
    CUser* user, stNotifyIngameADInfo& info, int flag)
{
    stNotifyIngameADInfo buf;
    // ORIG：vec 先构造、it 后初始化（调用顺序）；upper_bound 在循环条件处
    std::vector<stNotifyIngameADInfo> vec;
    std::multimap<short, CAdvertisementExposeInfo*>::iterator it =
        m_exposeInfo.lower_bound(static_cast<short>(flag));
    while (it != m_exposeInfo.upper_bound(static_cast<short>(flag)))
    {
        CAdvertisementExposeInfo* ad = it->second;
        if (checkCondition(user, ad))
        {
            std::memcpy(&buf,
                &ad->getExposeCondition()->m_sentinel,   // +0x210（连续块起点）
                0x63c);
            vec.push_back(buf);
        }
        ++it;
    }
    int n = static_cast<int>(vec.size());
    if (n == 0)
    {
        return false;
    }
    // ORIG 尾部 rep movsl 0x18f×4：operator[] 内联后对齐已知，聚合赋值内联；
    // at() 边界检查路径会展开成通用逐段拷贝。
    info = vec[static_cast<unsigned int>(RandNum(n))];
    return true;
