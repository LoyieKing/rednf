StringResource::~StringResource() {}

// ---------------------------------------------------------------------------
// parseStringLine —— ORIG t _ZL15parseStringLinePcPiPS_ @0x08ad490f。
// 行格式 "<key数字> > <value>"；'/' 开头("//")为注释；'\r'/'\n' 开头空行；
// 返回：0=成功（*key=atoi(key 区)、*value=value 起）、-1=key 含非数字、
// -2=key 位数>11。value 内 "\n" 转义为真实换行并左移，
// 其余 '\' 删除前缀字符（与 ORIG 循环形态一致），行结束('\r'/'\n')截断。
// 错误码经调用方 "Parse error : line:%d reason:%d" 记入 error。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource AE: 155=155 逐条全等（P264 归一后） | ORIG 0x08ad490f]
