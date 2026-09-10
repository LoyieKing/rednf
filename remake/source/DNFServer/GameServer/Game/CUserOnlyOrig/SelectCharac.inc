// [K7 2026-09-11 §6.5 PRETTY 串盲区] ORIG .rodata 实串 = `bool CUser::SelectCharac(int, int)`
// ⇒ 返回类型 bool（同 CUser.h:3047，K4CUser 同刻落头）。
bool CUser::SelectCharac(int idx, int tagIdx)
