LevelUpRewardScript::LevelUpRewardScript() {}

// ORIG W 0x08985eec：string ctor → 五个标量写入 → Action_SendMail ctor →
// str.clear → mail.reset。标量经初始化列表写在 string 与 mail 的隐式构造之间，
// 复现 ORIG 交错顺序。
