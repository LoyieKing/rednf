// ---- _Calculate_RegenCost —— T 0x085f9094（同名报告全文转写）--------------
int RegenerationROI::_Calculate_RegenCost(int partIdx, int optType, int lv0,
                                          int lv1)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 246/236；ORIG 0x085f9094）：残余槽序/
    // 物化次数（N82/P183 族）。旧判词：
    // 语义等价。
    int cost1 = 0;
    int cost2 = 0;

    if (partIdx != 0 && partIdx <= 99)
    {
        std::map<int, std::map<int, _str_cost> >::iterator outer =
            m_regenerator.m_map18.find(partIdx);
        // [SweepRandomRental 修正] ORIG 0x085f9103 `call ne; test %al,%al;
        // je <return -1>` ⇒ 原码为 `!=` 正极性 + else 早返（旧 `==` 形使 4.4 选
        // eq：p167 MISSING `_Rb_tree_iterator::ne` ×2 / EXTRA `eq` ×2）。
        if (m_regenerator.m_map18.end() != outer)
        {
            std::map<int, _str_cost>& inner = outer->second;

            std::map<int, _str_cost>::iterator a = inner.find(lv0);
            if (inner.end() != a)
            {
                if (a->second.m_field0 == 0)
                {
                    cost1 = a->second.m_field4;
                }
            }
            else
            {
                return -1;
            }

            // [SweepRandomRental 修正] ORIG 0x085f91b8 `mov -0x4c(%ebp),%eax;
            // mov %eax,-0x38(%ebp)` ⇒ 原码对 lv1 再次 find 后**赋值回同一迭代器 a**
            // （复用槽），非新声明 b；0x085f920e `add %eax,-0x18(%ebp)` 后读回做除法
            // ⇒ 累加与除法为两句式。
            a = inner.find(lv1);
            if (inner.end() != a)
            {
                if (a->second.m_field0 == 0)
                {
                    cost1 += a->second.m_field4;
                    cost1 /= 2;
                }
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }

    if (optType != 0 && optType > 100)
    {
        std::map<int, std::map<int, _str_cost> >::iterator outer =
            m_regenerator.m_map18.find(optType);
        // [SweepRandomRental 修正] 同第一块：ORIG 0x085f92ee 走 `ne` 正极性。
        if (m_regenerator.m_map18.end() != outer)
        {
            std::map<int, _str_cost>& inner = outer->second;

            std::map<int, _str_cost>::iterator a = inner.find(lv0);
            if (inner.end() != a)
            {
                if (a->second.m_field0 == 0)
                {
                    cost2 = a->second.m_field4;
                }
            }
            else
            {
                return -1;
            }

            // [SweepRandomRental 修正] ORIG 0x085f9347 `mov -0x4c(%ebp),%eax;
            // mov %eax,-0x40(%ebp)` ⇒ 同第一块：复用迭代器 a + 两句式除法。
            a = inner.find(lv1);
            if (inner.end() != a)
            {
                if (a->second.m_field0 == 0)
                {
                    cost2 += a->second.m_field4;
                    cost2 /= 2;
                }
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }

    return cost1 + cost2;
}