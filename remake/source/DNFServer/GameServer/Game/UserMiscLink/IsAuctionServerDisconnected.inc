// ===========================================================================
// 12) IsAuctionServerDisconnected(unsigned char) —— ORIG T 0x082138f2
//     （报告缺失，objdump 转写）：payType==0 检查 GlobalData::s_auction_proxy、
//     payType==1 检查 GlobalData::s_cera_auction_proxy；对应代理未运行 → true。
// ===========================================================================
bool IsAuctionServerDisconnected(unsigned char payType)
{
    // [b28d-solo-UserMiscLink] ORIG 0x082138f2：真块(+0x40)居中、假块(+0x47) fall-through
    // leave；共享真出口由「return 短路 or 表达式」唯一复现（if+or 多一层 bool 物化 31 条、
    // 平铺双 return 28 条、goto 尾标签 29 条）。本形 26=26 与 ORIG 逐条同形。
    return (payType == 0 && !GlobalData::s_auction_proxy->IsRunning()) ||
           (payType == 1 && !GlobalData::s_cera_auction_proxy->IsRunning());
}