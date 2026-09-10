WongWork::CWorldDrop::CWorldDrop(int count) : CItemGenerator(count) {}

// ORIG W 0x0853a81c（虚槽 +0x04 覆写）：mov $0x0,%eax; leave; ret —— ORIG
// 即空实现恒返回 0（真实装载入口是非虚双参重载），忠实转写，非占位桩。
