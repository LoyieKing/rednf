// ORIG 0x081cbe84（报告 Dispatcher_UpgradeItemSeparate/check_error.md）：
// state==3 且 curCharac!=0 否则 7；Secu Check(user,0x27) 非 0 原值返回；
// CheckItemLock(1, msg+0x1b word) → 0xd5；否则 0。
int Dispatcher_UpgradeItemSeparate::check_error(CUser* user, MSG_BASE& msg)
