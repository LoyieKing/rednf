// ---------------------------------------------------------------------------
// take_fish：按权重随机取鱼（类型 2；权重表 m_lottery.m_items，兜底 defaultIdx）
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 9=9）。修法：`cur = std::pair<int,int>(*it)`
// 选 ORIG 的 `aSEOS0_` 右值赋值（P135）；it 声明在 cur 之前（构造序照 ORIG 086c2214/1f）；
// **while+break 真循环形**（取代 goto 尾标签）复现 ORIG 的匿名 pair 临时/ppEi/end 槽方向。
int Area::take_fish()
{
    // [b28d-48] P59 正向包裹：ORIG 0x086c2xxx 为 `jne <尾部 return -1>`、
    // 主流程直落（areaType==2 时走主流程），原 guard 写法 4.4 产 `je <主流程>`
    // + fail 块前置（反向布局）。
    if (m_areaType == 2)
    {
        int result = 0;
        int randVal = get_rand_int(100000);
        int sum = 0;
        // [b28d-solo-Area] P50 先声明后赋值 + P283 goto 尾条件 + P28 后置 it++；
        // ORIG 构造序 = it C1Ev(-0x28) 先于 cur C1Ev(-0x30) ⇒ 声明序 it 在 cur 前
        // （4.4 先声明者占浅槽，与 ORIG 同向）。
        std::vector<std::pair<int, int> >::iterator it;
        std::pair<int, int> cur;
        it = m_lottery.m_items.begin();
        while (it != m_lottery.m_items.end())
        {
            // [b28d-solo-Area] ORIG 086c226d 调 `_ZNSt4pairIiiEaSEOS0_`（右值赋值，
            // P135：显式构造临时 pair 再赋值 ⇒ 选 move operator=，与 ORIG 同符号）。
            cur = std::pair<int, int>(*it);
            sum += cur.second;
            if (sum > randVal)
            {
                result = cur.first;
                break;
            }
            it++;
        }
        if (result == 0)
            result = m_lottery.m_defaultIdx;
        return result;
    }
    return -1;
}