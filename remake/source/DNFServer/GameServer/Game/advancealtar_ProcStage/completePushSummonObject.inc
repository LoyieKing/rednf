// ----------------------------------------------------------------------------
// ORIG T 0x0812d5d0（completePushSummonObject.md 全文）：
//   firstIndex>0 且 size()>firstIndex 时对 [firstIndex, end) 按 compare 升序 sort。
// ----------------------------------------------------------------------------
void ProcStage::ProcSummonObjectMs::completePushSummonObject(int firstIndex)
{
    // b28d-solo-ProcStage：ORIG 0x0812d5e0 用 size()+operator[] 取地址形
    //   （`call size; lea -0x1(%eax),%edx; …call operator[]`，无 begin()/end()
    //   迭代器构造）；且第二实参是 `&m_summons[size()-1]`（P171 照抄 ORIG 的
    //   末元素不参与排序行为，勿按语义"修正"成 end()）。
    // ORIG 0x0812d5d7 物化形 = `(fi<=0) || (size()<=fi)` 短路（`cmpl $0,mem; jle <mov$1>`
    //   + `cmp fi,%eax; jg <mov$0>` + fall `mov $1`）⇒ 条件写德摩根取反形
    //   （4.4 对 `A&&B` 恒双失败跳，与 ORIG 极性不符）。
    if (({ firstIndex <= 0 ||
          static_cast<int>(m_summons.size()) <= firstIndex; }))
    {
        return;
    }
    std::sort(&m_summons[firstIndex], &m_summons[m_summons.size() - 1],
              ProcStage::ProcSummonObjectMs::compare);
}