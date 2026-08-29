// CaptchaGen 数据表（提取自 df_game_r ORIG .rodata，2026-08-16）。
//   CaptchaGen::kKoreanGeneral    VA 0x08b2bca0  (978 x u16, 计数 @ 0x08b2c444)
//   CaptchaGen::kJapaneseHirakana VA 0x08b2c460 ( 72 x u16, 计数 @ 0x08b2c4f0)
//   CaptchaGen::kTaiwanAlphabet   VA 0x08b2c500  ( 26 x u16, 计数 @ 0x08b2c534)
//   XorRand4::ms_table0..3 VA 0x08e32cc0/0x08e330c0/0x08e334c0/0x08e338c0
//
// 数据定义位置（类静态成员，mangled 与 ORIG 一致）：
//   CaptchaGen::kKoreanGeneral/kJapaneseHirakana/kTaiwanAlphabet 及 3 计数
//     → GlobalData.cpp（CaptchaGen 类外定义）。
//   XorRand4::ms_table0..3 → XorRand.cpp（XorRand8 同文件已定义）。
// 本头不再定义任何数据，仅保留注释供引用方查阅。
#ifndef GAME_CAPTCHAGENDATA_H_
#define GAME_CAPTCHAGENDATA_H_

#endif  // GAME_CAPTCHAGENDATA_H_
