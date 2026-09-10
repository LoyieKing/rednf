Dispatcher_BingoQuiz::~Dispatcher_BingoQuiz() {}
// ORIG getBingoData() 返回 void*；check_error 按 +0x04/+0x05 char 字段单级读取
// （df_game_r 0x0825eb5d movzbl 0x5(%eax),%eax / 0x0825eb72 movzbl 0x4(%eax),%eax）。
// ORIG send 族（0x081e3056 等）：param 指针落 -0xc 槽，字段两次 mov 0x4(%eax) 单级读。
