WongWork::CGMAccounts::stGMInfo_t WongWork::CGMAccounts::getGMInfo(unsigned int mid) const
{
    // ORIG T 0x0810940a (size 0xdd)：std::find(m_gmList.begin(), end(), key)
    // key={mid,3}；命中取该元素，未命中返回 {0,3}（局部 -0x1c/-0x18 初值）。
    // [b28c-L7] P212 全头部声明 + P0 槽序：ORIG 槽阶梯为 fallback(-0x1c/-0x18,
    // 浅) → key(-0x24/-0x20) → find 结果(-0x28, 最深)；4.4 块内"先声明浅"，
    // 故声明序必须 fallback → key → it（原形 key 先 ⇒ 整组阶梯反向）。
    // ORIG key 段四写 `0,0 → type=3 → mid=mid` ⇒ key 具 value-init 预清。
    stGMInfo_t fallback;
    fallback.mid = 0;
    fallback.type = 3;
    stGMInfo_t key = stGMInfo_t();
    key.type = 3;
    key.mid = mid;
    std::list<stGMInfo_t>::const_iterator it =
        std::find(m_gmList.begin(), m_gmList.end(), key);
    if (it != m_gmList.end())
    {
        return *it;
    }
    return fallback;
