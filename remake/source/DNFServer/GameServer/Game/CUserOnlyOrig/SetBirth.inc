// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0867ecf0：P75 封顶——P319 整型加法+回 cast 已复现
// src 两段 `mov 0xc(%ebp),%eax; add $0x2,%eax`；残余为 4.4 先算 dest lea 致 src 中转 %edx
// （ORIG 先压 src 后 lea dest），属实参装载时刻跨版本差。
void CUser::SetBirth(const char* birth)
