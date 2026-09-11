// dailyresetData @ 0x080c9d82：按日重置棋盘/quiz；随后遍历在线用户，清
// BingoData、重发数据包并刷新装饰物计数。
// [b28d-D9] 修复/N89：BOOST_FOREACH 复现（143→5）。原「残 = getUserByAccIdMap
// inline getter out-of-line call」已消除（同上：friend 直访 m_userByAccId）。
void BingoEvent::dailyresetData()
