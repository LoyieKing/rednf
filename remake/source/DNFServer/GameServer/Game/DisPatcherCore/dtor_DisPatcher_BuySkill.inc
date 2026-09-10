DisPatcher_BuySkill::~DisPatcher_BuySkill() {}
// ORIG 0x081bf8be T _ZN19DisPatcher_BuySkill11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_BuySkill/check_error.md 逐块映射）：user 空 → -1；
// 遍历 msg 技能项（count@+0xe signed char，entries@+0xf stride 3），
// 技能 id ∈ (0xc7,0xd0) 且 ServiceRestrictManager::isRestricted(user,1,0x1e) → 0xd1。
