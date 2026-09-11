// StartAction @ 0x080c9bb8：遍历 CGameManager+0x22c（accId→CUser* map），
// 对每个在线用户发 bingo 数据包。
// [b28d-D9] 修复/N89：BOOST_FOREACH 复现（102→5）。原「残 = getUserByAccIdMap
// inline getter out-of-line call」已消除：ORIG 无该 getter 符号（直读 +0x22c），
// 调用点改 friend 直访 m_userByAccId。
void BingoEvent::StartAction()
