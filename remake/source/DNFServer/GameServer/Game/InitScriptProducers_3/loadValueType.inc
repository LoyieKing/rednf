int loadValueType(const std::string& token);

// g_equipmentStatInfoMap —— ORIG 未修饰名 B 0x095000a0（RDARScriptOldEquip 域
// 全局，本 TU 为其唯一定义点；消费方经同名 extern "C" 引用）。
// 类型证据：报告 _lt_global_gt_/__static_initialization_and_destruction_0_
// 08a588b8.md —— ORIG 静态初始化为 map<unsigned int, STEquipmentStatInfo>
// 默认构造并 __cxa_atexit 注册析构（本定义的动态初始化与之等价）。
