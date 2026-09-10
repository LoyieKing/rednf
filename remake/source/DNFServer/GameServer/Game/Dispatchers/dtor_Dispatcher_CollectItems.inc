Dispatcher_CollectItems::~Dispatcher_CollectItems() {}
// 删收集物（INVEN_TYPE 1 / reason 3），失败 LineFunc(0x5aa7)；成功把
// slot/count 回填 param，并向监控服代理上报 Packet_CollectItems(0x39 字节)。
// [b28c-31] P0 清绑（原 register int ec asm("edx") + register dst asm("eax") 已删）；
// 残差 N96+N18a：ec 中转 ORIG %edx vs 4.4 %ebx、m_count 窄化 ORIG 单段 movzwl→%edx
// vs 4.4 两段（0x081e34xx 实证）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e343c orig/ours=122/122 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,%edx | mov    %edx,0x4(%eax) | movl   $&_ZZN23Dispatcher_CollectItems7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp)
