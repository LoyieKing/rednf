STMonsterScript::~STMonsterScript() {}

// ORIG op= W 0x08377994 为编译器合成版（无自赋值检查、rep movsl 分组拷贝、
// 尾部 eax=this），头文件中以 = default 声明，由编译器按成员序生成。

// ---- STMonsterEquipment 成员（ORIG W 0x08368d92/0x08368d7e）----
