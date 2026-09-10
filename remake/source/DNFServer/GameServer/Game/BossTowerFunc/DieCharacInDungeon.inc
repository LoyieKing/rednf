// [K6 2026-09-11·P292 档①] char→bool（配合 BossTowerFunc.h:48）：ORIG 调用点
// 0x081c2b00 `call; test %al,%al; je 主体`（单字节判定，无 setne 归一化）；
// 定义体各分支为常量 return 0/1，4.4 探针实测 bool 与 int/char 形态全等 ⇒ 体不变。
bool Global::CBossTowerFunc::DieCharacInDungeon(CUser* user)
