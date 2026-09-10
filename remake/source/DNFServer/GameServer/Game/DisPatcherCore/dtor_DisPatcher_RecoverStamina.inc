DisPatcher_RecoverStamina::~DisPatcher_RecoverStamina() {}
// ORIG 0x081c3f4a T _ZN25DisPatcher_RecoverStamina11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_RecoverStamina/check_error.md 逐块映射）：state!=3 → 99；
// SecuProtectionField Check(user,0x2f) 非 0 直返；否则 getCurCharacR 为空 → -1。
