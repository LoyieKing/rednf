// =======================================================================
// CGameMasterCharacter 族（全局命名空间，ORIG W 弱符号族）。
// 基类仅设 vptr；sendServerState 基版为纯 ret。派生版把文本以
// put_header(0,0xc)/byte/short/byte/int(strlen)/str/finalize(true)
// 打包后 user->Send（ORIG 0x084b4330，0xf6 字节逐指令转写）。
// =======================================================================
// P210 实证（b28）：本 ctor 定义前置到基类 CGameMasterCharacter() 之前，令
// 4.4 编译本函数体时基类 ctor 定义尚不可见 → 保守 outgoing 帧 sub $0x18，
// 与 ORIG 逐条全等（推翻 b23「6 形探针穷举不可达」判词——缺的正是定义顺序维度）。
CGameMasterCharacterItemDropRate::CGameMasterCharacterItemDropRate() throw()  // 0x084b4434
{
}