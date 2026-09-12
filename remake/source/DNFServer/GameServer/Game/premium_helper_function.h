#ifndef GAME_PREMIUM_HELPER_FUNCTION_H_
#define GAME_PREMIUM_HELPER_FUNCTION_H_

// ---------------------------------------------------------------------------
// df_game_r —— premium_helper_function（免费命名空间，6 函数，全部被
// SpecialItemHandler::respond_special_item_premium 使用）。
// ORIG 地址见 docs/class_func_reports/premium_helper_function/README.md。
// 权威定义均实现于 CCeraShop_Special.cpp。
// ---------------------------------------------------------------------------

class CUser;

namespace premium_helper_function
{
// 0x827cff0  _ZN23premium_helper_function16cal_premium_termEibi
// if(flag) term *= (byte)CDataManager+0x877e；return term * extra。
int cal_premium_term(int term, bool flag, int extra);

// 0x827d024
void cal_end_time(CUser*, long&, long&, long, int, bool, int);
// 0x827d0f5
int get_target_server(int type);
// 0x827d127
void send_db(CUser*, int, long, long, int);
// 0x827d29d （R7a04 更正：调用点 0x0827ddf5 `call; xor $1,%eax; test %al,%al; je` ⇒ bool；
//   定义侧尾 `sete %al` 直返 —— setcc 结果本身即 bool，改 bool 零形态变化）
bool is_notify(CUser*, int);
// 0x827d2df
void write_log(CUser*, int);
}

#endif  // GAME_PREMIUM_HELPER_FUNCTION_H_
