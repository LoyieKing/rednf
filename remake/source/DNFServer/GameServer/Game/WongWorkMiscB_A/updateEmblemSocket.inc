// _ZN8WongWork11Avatar_Item18updateEmblemSocketEjj W 0x082fa2a6：
//   以 grade 索引 m_emblemInfo.stEmblemSocket[grade]（pack(1) 下元素 6 字节，
//   起始 +0x21）；socketType(u16)==0 → return 0；否则写入 itemIdx 并 return 1。
int Avatar_Item::updateEmblemSocket(unsigned int grade, unsigned int itemId)
{
    // [b28d-solo-WongWorkMiscB_A] P248 档③ 同型化：ORIG 写段为 LHS-first
    // `lea 0x20(%eax),%edx; mov 0x10(%ebp),%eax; mov %eax,0x3(%edx)`；
    // 字段 int + 形参 unsigned 在 4.4 下被判异型 ⇒ RHS-first + `push %ebx`
    // （探针 fA/fJ）。改用 GamePODTypes.h 新增的 unsigned 同址视角
    // `emblemItemIdxU_`（探针 fU 逐条复现，含 `leal 0x20(%eax),%edx`）。
    if (m_emblemInfo.stEmblemSocket[grade].emblemSocketType_ == 0)
        return 0;
    m_emblemInfo.stEmblemSocket[grade].emblemItemIdxU_ = itemId;
    return 1;
}