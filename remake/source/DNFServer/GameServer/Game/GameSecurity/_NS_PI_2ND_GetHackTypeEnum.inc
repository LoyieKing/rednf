// ===========================================================================
// _NS_PI_2ND_GetHackTypeEnum  ORIG W 0x08275af3 (0x17b6)
// §6.2 巨件核销：真修——原 static 前缀表查找版 mangled 为 _ZL26…（32 条），
// ORIG 权威符号为 weak _Z26…（跳表展开 1230 条）。去 static 并按 ORIG 结构
// 全量重建：if(type==1)/else if(type==0) + 内层 switch(cmd)（报告缺失，
// nm 定址后 objdump -d 0x08275af3..0x082772a9 补证）。
//   · type==1 跳表 @0x8bff190（0x25f 项）、type==0 跳表 @0x8bffb0c（0x233 项）；
//   · case 按返回值分组（同值 cmd 共享叶子 mov$N;jmp）；
//   · 指向 default（0x082772a2 mov$0）的 285/282 个 cmd 不列 case——
//     ORIG 源码未写这些 case，与越界共用 return 0；
//   · cmd→枚举映射逐项对账一致。
// ===========================================================================
// ===========================================================================
// _NS_PI_2ND_GetHackType  ORIG W 0x8277359：v = 2*(enum + 0x3e8) + type，
// 上限 0xe0f（jbe 保持）。
// 【P210】定义前置：调用者须在 _NS_PI_2ND_GetHackTypeEnum 定义之前，
// 否则 4.4 按 callee 帧精确继承 outgoing，0x38 保守大帧塌缩为 0x20。
// ===========================================================================
int _NS_PI_2ND_GetHackTypeEnum(unsigned char type, short cmd);
