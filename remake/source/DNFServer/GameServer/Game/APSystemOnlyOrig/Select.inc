// Select —— 报告全文转写。ORIG T 0x08123b9e。
// [b28d-27] DIFF N82/N89：ORIG 0x08123b9e；p167 CLEAR（21/21），残差为 SQL/STL
// [b28d-solo-APSystemOnlyOrig] 判词：**gap -3→0**（本轮还原：列号具名局部 `col++` 后置自增、5 次 get_ubyte 复用同一 flag 槽 -0x39、col 后死局部 -0x14、apSum 直传 `&sig.m_apSum`、blobLen unsigned、尾部 `if(!Parse)return false; return true;`；245=245）。残差 = N93附（nRows `jne false岛` vs 4.4 `je+mov$0+jmp`）。p167 CLEAR 21/21。
// 实例与帧/槽分配，语义闭合。
bool DB_LoadActionPoint::Select(_SIG_LOAD_ACTION_POINT& sig)
