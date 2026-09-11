// [b22判词·AE] CAuctionServerProxy D1Ev（ORIG 0x82aa9xx，orig10/our10）：空体正确，
// 编译器自动发 vptr=&CAuctionServerProxy vtable（ORIG `movl $0x8c1ec10,(%eax)`，未 relink
// .o 显占位 `$0x8`）+ call _ZN15BaseServerProxyD1Ev（P167 实测 1=1）。指令/助记符逐条同
// ORIG，残余唯 vtable 绝对址 + call 目标两处链接期重定位占位（§6.3 假阳性），relink 后转 AE。
CAuctionServerProxy::~CAuctionServerProxy()
{
}