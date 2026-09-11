// _checkUserState —— ORIG 0x0821f72a：CSecu Check(0x39)→IsBusyPrivateStore→CheckInTrade→
//   getCurSec()-lastBuyTime<0x1e→9。返回 0=通过，否则错误码。
// [N96 清绑 2026-09-09] 删除历史 `register bool isInInterval asm("al")`（P0 限定② /
// N96 全域禁用；P248 不得恢复）。报告 docs/class_func_reports/Dispatcher_BuyCeraShopLimitItem/
// _checkUserState.md 两段已读（Ghidra C 的 iVar2 - iVar3 < 0x1e ⇒ 我方三目条件）。
// ORIG 0x0821f7b2 `setle %al; test %al,%al; je`——isInInterval 值在 %al、无栈槽
// ⇒ 按 P0 双推律原码无该具名局部，还原为三目表达式直传。
// 残差 = N96+N18a 封顶（4.4 对整数比较恒直跳，不物化 setCC）。
// 读数：清绑前 [identical=6 ae=6 合计12 near=0 diff=2]，本函数 identical/ae；
//       清绑后 [identical=6 ae=5 合计11 near=0 diff=3]，verdict=DIFF
//       orig51/our49（缺 setle+test 两条）。
int Dispatcher_BuyCeraShopLimitItem::_checkUserState(CUser* user) const
{
    int ret = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x39);
    if (ret != 0)
        return ret;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return 0x13;
    if (user->CheckInTrade())
        return 0x13;
    // [N96 清绑] 原 `register bool isInInterval asm("al")` 已删（P0/N96 全域禁绑）。
    // ORIG 0x0821f7b2 `setle %al; test %al,%al; je`——isInInterval 值在 %al、无
    // 栈槽 ⇒ 按 P0 双推律原码无该具名局部，还原为三目表达式直传（残差见函数头判词）。
    // [L31 round3 试形 · P225] 三目形被 4.4 折成 `jg` 直跳（旧记录 N96/N18a）；改用
    // `while (cond) { return X; }` 形试探 4.1 的 `setle %al; test %al,%al; je` 物化。
    while (GlobalData::s_systemTime_.getCurSec()
           - user->getLastItemLimitEditionBuyTime() < 0x1e)
    {
        return 9;
    }
    return 0;
}