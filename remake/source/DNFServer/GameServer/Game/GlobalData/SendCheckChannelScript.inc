// ORIG SendCheckChannelScript 0x081066ea（0x453 字节）：桥接后校验/拉取
// channel_info.etc 并落盘。stat 失败且非 ENOENT → 0；version 文件读写两次；
// CHECK_SCRIPT_VERSION(cat1/id5) 应答尾包写入 version；GET_SCRIPT(cat2/id9)
// 应答尾包为脚本体（fwrite size-0xb 字节）。
// [SweepCAutoPunish 2026-09-10] 285→**291**（ORIG 289，余 +2）：三处 `if(fp==0)`
// 改 P295 while 形，复现 ORIG 0x081067e4/0x081069d7/0x08106ae8 的
// `cmpl $0,-0x20; sete %al; test %al,%al; je`（探针 /tmp/gdprobe/p3.cpp f2/f3；
// if 形恒折 `jne`）。余 +2 = fgets 行剥离循环的 continue 岛布局（ORIG
// `jne <body>` 回边 vs 我方 `jne; jmp; nop`，P223 边界）+ 3 条装载序，
// 无 jcc 语义差、p167 CLEAR（仅 C1/C2 别名）。
// [GDataWriter 2026-09-11 · **语义修正 + 减少 4 条**] ORIG 0x0810684b..0x08106869 逐条：
//   `mov -0x18(%ebp),%eax; movzbl -0x4d4(%ebp,%eax,1),%eax; cmp $0xa,%al; je <linePos=0>;
//    …cmp $0x23,%al; jne 0x08106870`（0x08106870 = fgets **循环条件头**）
// ⇒ 非注释行**不 break**（继续读下一行，line 保留末行）；旧写 `continue`+`break` 会在
// 首个普通行提前退出 ⇒ version 内容真语义 bug（AE 归一化看不见）。已改为单一
// `if (a=='\n'||a=='#') linePos=0;` ⇒ 实测 295→**291**（ORIG 289，gap +6→+2）。
bool CmmChannelProxy::SendCheckChannelScript()
