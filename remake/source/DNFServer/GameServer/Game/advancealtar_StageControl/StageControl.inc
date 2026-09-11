// ----------------------------------------------------------------------------
// ORIG T 0x0812f9a2（StageControl.md 全文）：m_index=-1 →
//   ProcStage::ProcStage(this+4, characData@this+0x98)（T 0x812dc18）+
//   CharacData::CharacData(this+0x98)（T 0x812d024）。
// 子对象以字节槽持有（advancealtar.h 布局注释），此处 placement 构造。
// [b28d-solo-StageControl] C1：ORIG 34 / ours 62。ORIG 为真成员直构（`lea 0x98(%eax),
//   %edx` + `add $0x4,%eax` + call ProcStage::C1(CharacData&) + call CharacData::C1，
//   仅 esi/ebx + sub 0x10 帧）。我方字节槽 + placement new ⇒ N83：4.4-O0 恒发
//   `movl $0x94,(%esp); call _ZnwjPv`（size 装载行）+ test/je + 每子对象一段 EH
//   cleanup（~28 条）。修法唯一路径 = StageControl 成员改真实类型，但
//   advancealtar.h → advancealtar_ProcStage.h → NOTIPacketStruct.h → advancealtar.h
//   为双向 include 环（NOTIPacketStruct.h 需 advancealtar::_Stage），成员化须拆环
//   （波及全仓 include CGameManager.h/advancealtar.h 的 TU）。语义等价（m_index=-1 +
//   ProcStage(*characData)@+0x04 + CharacData@+0x98，槽宽恰 0x94/0x28）。N 封顶（N83/P67）。
// [b28d-sweep 2026-09-09 复核] 解环路径已定位（供 Main 决策，本泳道未落）：①把
//   advancealtar::_Stage（advancealtar.h:71-73 + 断言 491）抽到叶子头
//   advancealtar_Stage.h；②NOTIPacketStruct.h 改 include 该叶子头（去 advancealtar.h
//   依赖）；③advancealtar.h 便可 include advancealtar_ProcStage.h，StageControl 成员改
//   真实类型 ProcStage m_procStage / CharacData m_characData。波及面：include
//   advancealtar.h 的 17 个 .cpp（含经 CGameManager.h/CUser.h 的传递消费）须复编；
//   4.4 生成的 EH landing 布局是否与 ORIG 4.1 逐条一致亦需实测（N91 族）。风险与
//   波及超出单 TU 泳道 ⇒ 维持 N83/P67 封顶，路径交 Main 定夺。
// [S4AltarStageMisc 2026-09-11 复核] p167 差集 = C2/D2 同址别名 + placement-new 机制
//   （ORIG 0x0812f9c9 `call ProcStageC2(RCharacData&)` / 0x0812f9e0 EH 段
//   `call ProcStageD2` + `call _Unwind_Resume`；我方 ProcStageC1/CharacDataC1 +
//   _ZnwjPv/_ZdlPvS_×2）⇒ **子对象构造/析构调用齐全，非"漏调用"**。显式
//   try/catch 补 ORIG EH 段实测：our_insns 62→74 且新引入 __cxa_begin_catch /
//   __cxa_rethrow / __cxa_end_catch（ORIG 无此三符号）⇒ 回滚。异常路径
//   （CharacData ctor 抛）ProcStage 不被析构属真缺口，但修法同为成员化（跨泳道）。
StageControl::StageControl()
{
    m_index = -1;
    CharacData* characData = reinterpret_cast<CharacData*>(
        reinterpret_cast<char*>(this) + 0x98);
    new (reinterpret_cast<void*>(m_procStage)) ProcStage(*characData);
    new (characData) CharacData;
}