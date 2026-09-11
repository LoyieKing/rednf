//
// 50. CUser::getInformNoticeFlag(int) T 0x0868fa76
// int CUser::getInformNoticeFlag(int idx)
// {
//     return (unsigned char)m_informNoticeFlags[idx];              // movzbl 直返（P9）
// }
//
//
// ============================================================================
// body_C.cpp — CUser 批次C 可粘贴定义（草稿，未写入仓库文件）
// 集成步骤：
//   1) 先按 fields_C.tsv 在 CUser.h / CMCAPManager.h / _Charac_info 落地字段；
//   2) 将本文件内容并入 CUser.cpp 对应区段；签名与 ORIG mangled 一一对应；
//   3) NOTE(FIELD ...) 注释指明对应 fields_C.tsv 行，字段落地后可删除。
// 取证：docs/class_func_reports/CUser/<方法>.md（##汇编+##反编译C 完整读取），
//      及 17 个无报告符号的 objdump 直读（地址见 meta_C.tsv「报告缺失@...」行）。
// ============================================================================
//
//
// ============================ 简单访问器 / 状态门 ============================
//
// ORIG 0x084ecd42 W：movzwl 返回 ushort（P9）
// unsigned short CUser::GetSlotIDX() const
// {
//     return (unsigned short)m_field704a4;        // NOTE(FIELD 改名 m_slotIDX)
// }
//
// ORIG 0x082308e8 W：movzbl 返回 byte（P9/P11）
// unsigned char CUser::GetReceivedRequestType() const
// {
//     return (unsigned char)m_field8ec28;         // NOTE(FIELD 改名 m_receivedRequestType)
// }
//
// ORIG 0x0863be54 W
// unsigned char CUser::getPcRoomAuth() const
// {
//     return m_pcRoomAuth;                        // NOTE(FIELD +0x8cef1 unsigned char)
// }
//
// ORIG 0x082300d0 W：sete 布尔规范化
// bool CUser::IsState(ch_state state) const
// {
//     return m_chState == (int)state;
// }
//
// ORIG 0x084ecb00 W：ORIG 体为 mov $0,%eax; ret —— 忠实恒假返回
// bool CUser::IsPCRoomUser() const
// {
//     return false;
// }
//
// ORIG 0x08328220 W：+0x8e098 字节 == 0xff 判定
// bool CUser::IsOKChildrensDayEventCondition()
// {
    // NOTE(FIELD childrensDayEventState = +0x8e098 unsigned char，现 m_pad8e098[0])
//     unsigned char v = *(const unsigned char*)((const char*)this + 0x8e098);
//     if (v == 0xff) {
//         return true;
//     }
//     return false;
// }
//
// ORIG 0x0822fbe6 W：转发雇佣兵管理器等待态判定（返回值透传）
// bool CUser::IsWaitingStateCharac(unsigned int characNo)
// {
//     return m_mercenaryMgr.IsWaitingStateCharac(characNo);
// }
//
// ============================ hack-trade 计数组（m_379 @0x8d1a8）============
//
// ORIG 0x08532078 W：cnt=+0x8d1b4
// int CUser::getTotalHackUserTradeCnt() const
// {
//     return m_379.m_hackTradeCnt;                      // NOTE(FIELD 改名 m_hackTradeCnt)
// }
//
// ORIG 0x08532078 区（Gold）：edx:eax 双寄存器返回 ⇒ 组合 64 位
// long long CUser::getTotalHackUserTradeGold() const
// {
//     unsigned int lo = (unsigned int)m_379.m_hackTradeGoldLo;   // NOTE(FIELD m_hackTradeGoldLo @0x8d1ac)
//     unsigned int hi = (unsigned int)m_379.m_hackTradeGoldHi;   // NOTE(FIELD m_hackTradeGoldHi @0x8d1b0)
//     return (long long)(((unsigned long long)hi << 32) | lo);
// }
//
// ORIG 0x086972ee W
// void CUser::incTotalHackUserTradeCnt(unsigned int n)
// {
//     m_379.m_hackTradeCnt = (int)((unsigned int)m_379.m_hackTradeCnt + n);
// }
//
// ORIG 0x086972b6 W：64 位加法（lo 用 add、hi 用 adc）
// void CUser::incTotalHackUserTradeGold(unsigned long long gold)
// {
//     unsigned long long cur =
//         (((unsigned long long)(unsigned int)m_379.m_hackTradeGoldHi) << 32) |
//         (unsigned int)m_379.m_hackTradeGoldLo;
//     cur += gold;
//     m_379.m_hackTradeGoldLo = (int)(unsigned int)(cur & 0xffffffffu);
//     m_379.m_hackTradeGoldHi = (int)(unsigned int)(cur >> 32);
// }
//
// ============================ 计数器族 ======================================
//
// ORIG 0x080fdf56 W：自增并返回新值
// int CUser::inc_multiboxLotteryItemFailCnt()
// {
    // NOTE(FIELD m_multiboxLotteryFailCnt @0x703bc int)
//     ++m_multiboxLotteryFailCnt;
//     return m_multiboxLotteryFailCnt;
// }
//
// ORIG 0x08110dd0 W：byte 自增（objdump 逐指令转写）
void CUser::incPcRoomPlayTimeRewardCnt()
{
    ++m_pcRoomPlayTimeRewardCnt;                // +0x8eba4 unsigned char
}