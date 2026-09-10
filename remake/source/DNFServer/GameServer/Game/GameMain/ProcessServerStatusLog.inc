void ProcessServerStatusLog();               // _Z22ProcessServerStatusLogv

// ProcessAntiBotInputWrapper() —— ORIG 0x80eddb0（0xe1 字节，逐指令转写）：
// 计时包裹 s_antibotChecker_ 的虚调用（vtbl+0x10）；耗时 >500ms 记 logFormat(42)。
// [b28d-28] DIFF | ORIG 0x080eddb0 | P282/N7：虚调用偏移 vptr+0x10 已核（word[4]）；
// ORIG 无判空（P171 照抄）已修 + 死局部 `limit=500` 已补；残差仅 fn 落 %edx（ORIG）
// vs %eax（4.4.7 间接 call 恒 %eax）⇒ N7 封顶（P230：4.4.6 后端可解，待路由）。
// [b28d-solo-GameMain] NEAR | 63=63 | 修法：函数指针值在调用点 cast（旧形经
// `CheckerFn*` 解引用多发一条 `mov (%eax),%eax`）；残差 N7（间接 call 目标 4.4.7
// 恒 %eax vs ORIG %edx）。**446 后端实测反劣**（-B cc1plus446bin/ 编译本 TU：
// 本件 NEAR→DIFF，TU near 3→2 / diff 12→13，AE 合计 263 不变）⇒ 不推荐路由，
// 按 N7 在 4.4.7 封顶。
