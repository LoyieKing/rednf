// b28d-solo-Dispatchers verdict=DIFF(routed) [S206 收敛 2→1 块] ORIG 0x0813f92a
//   [N-P63] do-while→while 形（P25）已消除入口 jmp 差异；残余仅 ORIG 0x813fa6a
//   体尾 `jmp +1(nop 中继)`（4.1 单源尾岛，P223 负例：4.4 恒 jump-thread 合并）。
void Dispatcher_StartGame::send(CUser* user, ParamBase& param)
