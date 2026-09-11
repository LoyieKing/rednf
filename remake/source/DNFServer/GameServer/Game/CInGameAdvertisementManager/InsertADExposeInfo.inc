// ORIG T _ZN27CInGameAdvertisementManager18InsertADExposeInfoER25stInGameADExposeCondition
// @0x080fd8e4（报告 InsertADExposeInfo.md）：new(0x850) → SetAdvertisementExposeInfo
// → multimap<short,CAdvertisementExposeInfo*>::insert(make_pair(cond+0x204, info))。
// 注：CAdvertisementExposeInfo 无用户 ctor（ORIG nm 无 C1/C2），new 不生成 ctor call。
void CInGameAdvertisementManager::InsertADExposeInfo(stInGameADExposeCondition& cond)
{
    // ORIG new 无括号：trivial 类不做 value-init（无 rep stos 清零、无 ctor call）
    CAdvertisementExposeInfo* info = new CAdvertisementExposeInfo;
    info->SetAdvertisementExposeInfo(cond);
    m_exposeInfo.insert(std::make_pair(cond.m_key, info));
}