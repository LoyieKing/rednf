// [b28d-51] 判词 N（ORIG 0x082fef12，N82/N86 族）：vector 迭代器/临时槽
// 阶梯方向差（ORIG 帧 0x28 vs 4.4 0x24+ebx）；语义/调用集齐，byte 层封顶。
void CClearCondition::SetDungeonClearConditionVector(
    std::vector<DungeonClearCondition>& vec)
{
    // ORIG 0x82fef12：Clear 后若 vec 非空，+0x14 存 vec 地址，并对每个
    // 条目：m_flag(+0x00) += 条目 m_8 字段，同时向 m_vec push 一个 0。
    // [L6S2b · P13 end 缓存] ORIG 在循环外各求值一次 begin()/end()
    //（0x082fef42..f54 → it@-0x14 / 0x082fef57..f69 → end@-0x18），循环条件只调
    // `__gnu_cxx::operator!=`（0x082fefc2）；旧译 `it != vec.end()` 每次迭代重调
    // end()（+3 条/次、临时槽 -0x10 挤占）。语义不变，仅求值次数对齐。
    Clear();
    if (!vec.empty())
    {
        m_pCondVec = &vec;
        std::vector<DungeonClearCondition>::iterator it = vec.begin();
        std::vector<DungeonClearCondition>::iterator itEnd = vec.end();
        while (it != itEnd)
        {
            // [L6S2b · P167 调用点判据] p167 在基线 .o 上给出 MISSING
            // `_ZNK9__gnu_cxx17__normal_iteratorIP21DungeonClearCondition...EdeEv`(−1) +
            // EXTRA `...ptEv`(+1) ⇒ ORIG 用 `*it`（deEv），旧译 `it->m_8` 走 ptEv。
            // [M4S2 · P183 具名指针物化] ORIG 0x082fef6e..f79 先 `call deEv` 并把
            //   结果落槽 -0xc，随后 `add %eax,%edx` 双语句形（op0=this->m_flag 先
            //   加载进 edx、op1=p->m_8 后加载）；旧译单语句 `m_flag += (*it).m_8`
            //   使 op1 内含 call ⇒ 编译器把 m_flag 提升进 callee-saved %ebx 跨 call
            //   保活（多 push/pop、帧 0x24+ebx、`lea (%ebx,%eax,1)`）⇒ 具名指针
            //   两语句形消去该跨 call 保活。
            DungeonClearCondition* pCond = &*it;
            m_flag += pCond->m_8;
            m_vec.push_back(0);
            ++it;
        }
    }
}