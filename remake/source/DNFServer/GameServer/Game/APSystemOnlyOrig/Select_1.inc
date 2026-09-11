// Select —— 报告全文转写。ORIG T 0x08124910。
// [b28d-27] DIFF N82/N89：ORIG 0x08124910；p167 CLEAR（18/18），残差为 SQL/STL
// [b28d-solo-APSystemOnlyOrig] 判词：**gap -4→-1**（本轮还原：medalCount=size() 先于 minAp=99999、strToday/strYesterday 16 字节、get_uint 直传 &sig.m_apSum；165 vs 166）。残差 = N93附（nRows `jle` vs 4.4 `jg+mov$0+jmp`）。p167 CLEAR 18/18。
// 实例与帧/槽分配，语义闭合。
bool DB_LoadRewardMedal::Select(_SIG_LOAD_REWARD_MEDAL& sig)
