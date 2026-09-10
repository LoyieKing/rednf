Dispatcher_Expand_Equipslot_Flag_Update::~Dispatcher_Expand_Equipslot_Flag_Update() {}
// ORIG 0x081d5f10（报告 Dispatcher_Expand_Equipslot_Flag_Update/process.md）：
// user==0 → 1；state<3 → 0x13；UpdateEquipslotFlag==0x13 → LineFunc(0x3df3)；
// 否则 param.m_4=(unsigned char)结果；返 0。
