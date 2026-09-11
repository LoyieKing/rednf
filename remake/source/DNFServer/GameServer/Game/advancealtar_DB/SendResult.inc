// [report SendResult.md / objdump 0x813ea46]
// [L2Free4to15.S6 本波归零] 原判 N122 撤销：ORIG 0x0813ea54 `cmpb $0x0,-0x1c(%ebp);
// je +0xe6` 是**正面条件包裹**形（`if (ok) { 体 } return true;`），非就地取反。
// 探针实证（4.4.7 -m32 -O0）：`bool f(bool ok){ if(ok){g();} return true; }` 生
// `mov 0x8(%ebp),%eax; mov %al,-0xc(%ebp); cmpb $0x0,-0xc(%ebp); je …` —— 与 ORIG
// 逐条同形（-0x1c 即 bool 形参 spill 槽）；而 `if (!ok) return true;` 生
// `movzbl; xor $0x1; test` 且 return 物化位置漂移（N93附）。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
bool DB_Load::SendResult(int uid, int uniqueId, _CharacAdvanceAltarDbData& data, bool ok)
{
    if (ok)
    {
        // ORIG 无 stream 具名槽：Acquire 结果经 eax 直传（P0/P89）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "localglobal/global_AdvanceAltar_Dispatcher.cpp", 0x1bf),
                           true);
        guard.operator*()->operator<<(uid);
        guard.operator*()->operator<<(uniqueId);
        guard.operator->()->put_binary(&data, 0x51c);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);   // ORIG QUEUE_IDX=1
    }
    return true;
}