Arad_Mileage_Data::~Arad_Mileage_Data() {}

// b28d-solo-ARAD verdict=IDENTICAL/AE：补具名 `int found = find(ipg)`（P0 槽往返，
// ORIG 0x81a3598 `mov %eax,-0xc`）后全帧槽位对齐（script -0x74 / r -0x18 / ipg -0x14 /
// percent -0x10 / found -0xc / Data -0x20），154=154 指令全等；残余 diff 全为 .o
// 未 relink 的 call/jmp 与 .rodata 重定位显示（§6.3 假阳性）。
// p167：ORIG 0x81a35ca 调 push_back(Data&&)（`OS1_`）⇒ 匿名临时形
// `push_back(Data(ipg, percent))`（具名 d 选 RKS1_，差集 1/1）；改后 0/0 CLEAR。
