// [b22判词·AE] CCeraAuctionServerProxy D1Ev（ORIG 0x82aa9c8，orig10/our10）：空体正确，
// 编译器自动发 vptr=&CCeraAuctionServerProxy vtable（ORIG `movl $0x8c1ebf8,(%eax)`，未
// relink .o 显占位 `$0x8`）+ call _ZN15BaseServerProxyD1Ev（P167 实测 1=1）。逐条同 ORIG，
// 残余唯 vtable 址 + call 目标两处链接期重定位占位（§6.3 假阳性），relink 后转 AE。
CCeraAuctionServerProxy::~CCeraAuctionServerProxy()
{
}