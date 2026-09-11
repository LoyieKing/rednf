// ORIG T _ZN27CInGameAdvertisementManager33CheckIngameAdvertisementConditionEP5CUserR20stNotifyIngameADInfoi
// @0x080fcfa8（报告 CheckIngameAdvertisementCondition.md）：multimap 按 short
// 键 [lower_bound, upper_bound) 遍历 → checkCondition 命中者 memcpy 0x63c 入
// vector → RandNum 随机取一条 rep movsl 0x63c 拷出。
// [L6S6 2026-09-11 语义轴修正] 父泳道 p167 联网实证真缺口：ORIG 调
// `_ZNSt6vectorI20stNotifyIngameADInfoSaIS0_EE2atEj`（vector::at，越界抛
// std::out_of_range），我方误写 `operator[]`（EXTRA …ixEj）⇒ 已改回 `.at()`。
// 修复后 p167 该缺口消失（仅余 vector ctor/dtor 的 C1/C2 别名名差，同址 weak）。
// 旧记：b28d-solo AE 闭合（无源码改动）114/114。
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
    // [L6S6 2026-09-11 语义轴 P175 修正] p167 联网实证：ORIG 调
    // `_ZNSt6vectorI20stNotifyIngameADInfoSaIS0_EE2atEj`（vector::at，带边界检查，
    // 越界抛 std::out_of_range），我方误写 `operator[]`（EXTRA `…ixEj`）。
    // ORIG 0x080fd0d0 `call <vector at>` 后 `mov $0x18f,%eax; rep movsl`（399×4 =
    // 0x63c 内联聚合赋值）。改回 at() 恢复异常语义；返回值即元素指针，直接整体赋值。
    info = vec.at(static_cast<unsigned int>(RandNum(n)));
    return true;
}