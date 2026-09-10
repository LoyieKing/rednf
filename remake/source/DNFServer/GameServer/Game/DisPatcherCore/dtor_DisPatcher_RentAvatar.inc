DisPatcher_RentAvatar::~DisPatcher_RentAvatar() {}
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 491（ORIG 480 / ours 487），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] callee 重载修正：m_itemId 本即 unsigned long，多余的 (int) 强转错绑
// _ZN12CStreamGuardlsEi；ORIG 0x081eeb65 实为 lsEm。实测 ndiff 345→342，差集一缺一多清零。
// 残差 5 处 memset→rep stos 属 N78（4.1 内联展开、4.4 O0 恒 call memset，不可翻）。
