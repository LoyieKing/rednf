// ----------------------------------------------------------------------------
// ORIG T 0x086914cc _ZN5CUser15updateBingoDataEv：
// m_needSave 置位时 DB_UpdateBingoEvent::makeRequest(GetUID(), acc_id,
// m_eventId, m_isWin)（ORIG 签名 Eijib），随后 m_needSave 清 0。
// getBingoData() 每次独立调用（P48 无 CSE）。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N18a 反向：ORIG 0x086914cc；ORIG `mov (%eax),%esi` 单段直落
// callee-saved，4.4 恒经 %eax 两段；旧探针族（具名/内联/register）均不翻 ⇒ 封顶。
// p167 CLEAR（7/7），语义闭合。
// [b28d-solo-CUserMissingA2] 复验 44/43：P226 裸语句表达式、P231 纯 register、
// 删 `static_cast<int>` 三形实测均不翻（register 反使 isWin 装载序劣化），
// ORIG `mov (%eax),%esi` 单段直落 callee-saved 不可造 ⇒ N18a 反向封顶；
// p167 CLEAR 7/7。
void CUser::updateBingoData()
{
    // 语义闭合（ndiff 43/44 残 1 条定性）：ORIG `mov (%eax),%esi` 为 m_needSave
    // 内存加载直落 callee-saved %esi；4.4 恒经 %eax 中转两步（N18a 反向，
    // 与前序 getCurCharacInvenCheckSum `mov 0x58(%eax),%ebx` 同型，探针族全败）。
    if (static_cast<BingoData*>(getBingoData())->m_needSave)
    {
        DB_UpdateBingoEvent::makeRequest(
            GetUID(), get_acc_id(),
            // [Lane14C] ORIG 0x0869145a `mov (%eax),%esi` 单段直落 callee-saved：
            // 字段 m_eventId 为 unsigned int 而 makeRequest 第 3 形参为 int
            // （mangled Eijib）⇒ 4.4 对异型实参插转换中转（`mov (%eax),%eax;
            // mov %eax,%esi`）。探针 p2(异型→两段)/p6(同型 int 视图→单段) 实证
            // ⇒ 以 int 左值视图读取（P266 同族左值视图旋钮；sizeof/偏移/符号零变化）。
            *reinterpret_cast<int*>(
                &static_cast<BingoData*>(getBingoData())->m_eventId),
            static_cast<BingoData*>(getBingoData())->m_isWin);
        static_cast<BingoData*>(getBingoData())->m_needSave = 0;
    }
}