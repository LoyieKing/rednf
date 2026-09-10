// [N1F 2026-09-11 §6.5 串盲区] ORIG .rodata `_ZZN27InterSelectMobileAuthReward11makeRequestEijbE19__PRETTY_FUNCTION__`
//   = `static void InterSelectMobileAuthReward::makeRequest(int, memberIdentificationNumber_t, bool)`
//   （strall 审计实读；我方原写 `unsigned int` ⇒ 串内容不符）。mangled `...Eijb` 不变
//   （typedef 不入 mangled）。同 typedef 先例：CGuildServerProxy.cpp:151 / DB_Login.cpp:89 /
//   CUser.cpp:780 / W8LT_DBTask/makeRequest_3.inc:1（同型重复 typedef 合法）。
typedef unsigned int memberIdentificationNumber_t;
void InterSelectMobileAuthReward::makeRequest(int param_1, memberIdentificationNumber_t param_2,
