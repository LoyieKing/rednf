// ---------------------------------------------------------------------------
// GetPredictResult（报告 GetPredictResult.md，ORIG T 0x085d1f68 sz 0x3e2）。
// 函数级静态结果向量（guard 变量 b 0x09472f80 / 向量 b 0x09472f88）：
//   [0] 机主本人且 rarity>1 → script+0x34（selfService[1]，奖励 A 物品）
//   [1] map 命中项的 stDisjointResult.m_field0（cube 索引）
//   [2..] 附加/大奖物品表中未收录过的 m_itemIndex（std::find 去重）
// ---------------------------------------------------------------------------
// [L2Free4to15.S3 复核] p167 CLEAR（仅 C1/C2·D1/D2 别名）。prologue/帧全等、
// ORIG-only/OURS-only 空、gap=0；残差 = 全帧槽号整体深 8
// （ORIG -0x4b/-0x50/-0x54/-0x58 ↔ 我方 -0x53/-0x58/-0x5c/-0x60）
// ⇒ N82/N95 族封顶。
std::vector<int>* CDisjointer::GetPredictResult(CUser* user, CItem* item,
                                                Inven_Item* inventItem)
{
    static std::vector<int> result;   // ORIG GetPredictResult(...)::result
    result.clear();

    bool unidentified = false;
    // [b28d-solo-ExpertJobLink4 P198①/P194]：ORIG 0x85d1fd1 `movb $0,-0x1d`
    // 预清 + 条件 mov$1/mov$0 物化后 `mov %al,-0x1d`（赋值表达式，非 if 置位）。
    unidentified =
        (inventItem != 0 && !inventItem->m_amp.isIdentified());

    // [b28d-solo-ExpertJobLink4 P225 推广]：ORIG 0x85d2049 `cmp $0x1; setg %al;
    // test; je` 双条件各自物化 ⇒ 拆嵌套 if（`&&` 合写在 4.4 恒折 jle 直跳）。
    if (isMine(user))
    {
        if (item->get_rarity() > 1)
        {
            // ORIG 0x85d2053 `mov 0x34(%eax),%eax; mov %eax,-0x54; lea -0x54`
            // ⇒ push_back 实参先物化到临时槽（4.4 直传成员地址）。
            int selfReward = GetScript()->m_selfDisjoin.m_field4;
            result.push_back(selfReward);
        }
    }

    DisjointMachineGrade key((unsigned char)item->get_rarity(),
                             (unsigned char)get_disjoint_machine_grade(),
                             unidentified);
    // ORIG find/end 各独立 `call GetScript`（无 script 具名槽）。
    std::map<DisjointMachineGrade, stDisjointResult>::iterator it =
        GetScript()->m_disjointResultMap.find(key);
    if (it != GetScript()->m_disjointResultMap.end())
    {
        stDisjointResult& res = it->second;
        // ORIG 0x85d2117 `mov (%eax),%eax; mov %eax,-0x44` 值临时 ⇒ 右值重载
        // push_back(int&&)（p167 差集 MISSING `push_backEOi`）。
        result.push_back((int)res.m_field0);

        // [b28d-solo-ExpertJobLink4 P96/P270/P315]：p167 差集实证 ORIG 用
        // 非 const iterator（deEv/ppEv/ne 非 const 实例）；且 ORIG 0x85d2135
        // 先 `call iterator::C1Ev`（P50 复活）再 `rate = begin()`，两个循环
        // 复用同一 rate 槽 -0x58（第二个循环无 C1Ev）。
        std::vector<stItemSelectRate>::iterator rate;
        for (rate = res.m_additionalItems.begin();
             rate != res.m_additionalItems.end(); ++rate)
        {
            // [b28d-solo-ExpertJobLink4 P167]：ORIG 0x85d20xx 用
            // `call operator*; mov %eax,%ebx`（m_itemIndex@+0 ⇒ &(*rate)）
            // 而非 `operator->` ⇒ 源写 `(*rate).m_itemIndex`。
            if (std::find(result.begin(), result.end(), (*rate).m_itemIndex) ==
                result.end())
                result.push_back((*rate).m_itemIndex);
        }
        for (rate = res.m_bigwinItems.begin();
             rate != res.m_bigwinItems.end(); ++rate)
        {
            if (std::find(result.begin(), result.end(), (*rate).m_itemIndex) ==
                result.end())
                result.push_back((*rate).m_itemIndex);
        }
    }
    return &result;
}