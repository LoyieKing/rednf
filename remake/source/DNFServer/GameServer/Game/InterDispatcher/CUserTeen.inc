CUserTeen::CUserTeen() {}

// ORIG CUserTeen::IsRight(CUser*) W 0x084f078c：
//   CheckLimitMiniumAge(0xf, local) 结果取反（xor $1）后 if/else 物化 0/1：
//   ORIG 0x084f07ac `xor $0x1,%eax`; 07af `test`; 07b1 `je`; 07b3 fall
//   `mov $0x0,%eax`+`jmp`; je 目标 07ba `mov $0x1,%eax`。直接 return !call()
//   缺这四行物化（技巧10 形）。
