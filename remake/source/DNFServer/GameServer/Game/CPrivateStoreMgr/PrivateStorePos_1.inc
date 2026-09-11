// ORIG W 0x85cb72c：zone 基类构造（init 列表）后，+0x2/+0x4 每 store 重读
// 0x8(%ebp)（P29 函数体赋值形：`mov this→%eax; movzwl val→%edx; mov %dx,off`），
// init 列表会被 4.4 展成值先装载（RHS-first）与 ORIG LHS-first 相反。
inline PrivateStorePos::PrivateStorePos(unsigned char a, unsigned char b,
