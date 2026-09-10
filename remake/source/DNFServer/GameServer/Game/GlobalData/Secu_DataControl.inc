Secu_DataControl::Secu_DataControl() {}  // ORIG 0x82aa6ba：空体（纯 ret，无成员初始化）

// encryptAES_ 定义在 MouseRegisterEJ.cpp（保留方，0x148 更近 ORIG 0x14d）；此 TU 仅调用。
char* encryptAES_(CRijndael* cipher, char* inData, char* outHex);

// Secu_DataControl::UpdateSecurityServiceState —— ORIG T 0x08286e40
// [0x08286e40,0x08287081]。service==0：SecurityCard 已激活→init；
// service==1：CPad 已激活→若已认证则 flag=1，否则将 msg 截 8 字节 AES 加密后
// setFailCnt(0)+setPassword；任一分支生效则发送 0x159 通知包（service/flag）。
// [b28d-18 N78] EXTRA memset×2：4.1 内联清零 vs 4.4 call（N78） | ORIG 0x08286e40
// [b28d-solo-GlobalData] DIFF 封顶：p167 EXTRA memset×2 = N78（4.1 内联清零/4.4 call）；
// MISSING 空 ⇒ 语义无缺口。
