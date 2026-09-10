void CInGameAdvertisementManager::InsertADExposeInfo(stInGameADExposeCondition& cond)
{
    // ORIG new 无括号：trivial 类不做 value-init（无 rep stos 清零、无 ctor call）
    CAdvertisementExposeInfo* info = new CAdvertisementExposeInfo;
    info->SetAdvertisementExposeInfo(cond);
    m_exposeInfo.insert(std::make_pair(cond.m_key, info));
