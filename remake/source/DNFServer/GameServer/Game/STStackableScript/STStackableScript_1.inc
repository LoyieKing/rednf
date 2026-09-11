// [b28d-solo-STStackableScript] NEAR（orig=643 our=643，ndiff 44 全为块内
// 迭代器槽号分配；p167 仅 STItemScript C2/D2 vs D1 同址别名伪差（P93）。
// 本轮修正：for 布局 + src_pair 逐成员赋值（p.first/p.second）+ insert 传值对。
STStackableScript::STStackableScript(STStackableScript& other)
{
    operator=(other);
    if (!other.m_module4bc.empty())
    {
        // ORIG 0x0898fbf7..0x0898fd68：4.1 for 布局（begin 后 jmp 尾条件、每次
        // 迭代重调 other.end()）；循环体内 dst_end=end() → it->first(ptEv) →
        // find → ne(found,dst_end)；body 为 src_pair 逐成员赋值
        // （src_pair.first=it->first；src_pair.second=new ...；*second=*it->second）。
        // [b28d-solo-STStackableScript] N93/P212 封顶：ORIG 槽序（4.1 块内先声明
        // 深）it(-0x38) > dst_end(-0x30) > found(-0x2c)；4.4 块内先声明浅
        // （dst_end -0x34 / found -0x38，方向相反）。三种改写实测：
        //   ①三迭代器提函数头：+3 C1Ev call、帧 +0x10（P50 反例）；
        //   ②found/dst_end 提 for 前：+2 C1Ev call、帧 +0x10（ndiff 24 但引入
        //   EXTRA 调用）；③循环体内声明（本形）：orig=643 our=643、无额外 call、
        //   残差仅槽号，ndiff 44 NEAR ⇒ 取 ③，p167 无缺口。
        // [P2S3STSkillStack 2026-09-11] NEAR→AE 落地：③ 的残差 12 块全是块内
        // 4 槽轮换（ORIG -0x38/-0x34/-0x30/-0x2c ↔ 我方 -0x30/-0x2c/-0x38/-0x34，
        // 即同样的槽集、不同的「变量↔槽」映射）。合并 dst_end/found 两个具名
        // 迭代器为单表达式 `find(...) != end()`（求值序仍是 end→find，语义等价）
        // ⇒ 槽集与映射同时归一：verdict=AE 643/643、fdiff blocks=0。
        // 负结论（勿再试）：仅把 `it` 声明提到 for 之前（同一块内）读数零变化
        // （n1_v 仍 NEAR 644，槽序不变）—— 说明驱动槽向的是 dst_end/found 这对
        // 内联临时，而非 for-init 里的 it。
        for (std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator it =
                 other.m_module4bc.begin();
             it != other.m_module4bc.end(); ++it)
        {
            if (m_module4bc.find(it->first) != m_module4bc.end())
            {
                std::pair<ENUM_MODULE_TYPE, EquipmentParameterInfo*> src_pair;
                src_pair.first = it->first;
                src_pair.second = new EquipmentParameterInfo();
                *src_pair.second = *it->second;
                m_module4bc.insert(src_pair);
            }
        }
    }
}