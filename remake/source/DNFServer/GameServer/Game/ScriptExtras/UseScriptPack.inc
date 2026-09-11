// ORIG T 0x088bb7cf：`movzbl 0x93f4d24,%eax; pop %ebp; ret` 直返字节。
// [D01 属主更新 2026-09-10 · 采纳 E07 实测] 旧 b28d-lane16 判词「返回类型 bool→char」
// **作废**：单改返回类型时「bool 返回 + char 字段」会被 4.4 插 `test %al,%al;
// setne %al`（本体 identical −1，已实测回退）；按 P248 档②/P300 **源字段与返回类型
// 成对 bool 化**后返回表达式本身已是 bool ⇒ 直返形两全：本体仍 `movzbl mem; ret`
// （size 0xc = ORIG 0x088bb7cf 0xc）、消费点 LinkMiscSecuTimerSolo.cpp::
// findFileWithServerToken 182→**181 = ORIG（gap 0）**。跨 TU extern 声明同步 bool
// （该点跨 TU 访问 ORIG TU-local static，受 P189 豁免形不变）。
bool UseScriptPack()
{
    return s_bUseScriptPack;
}