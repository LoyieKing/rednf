int getGrowTypeIndex(const char* name);

// stBloodWarGrowTable / stBloodWarBloodPointTable —— importBloodWarEtcScript 形参类型
// （mangled P19stBloodWarGrowTable / P25stBloodWarBloodPointTable）。GCC 4.4 对
// typedef 到模板实例会展开 typedef 名，唯派生结构体才能产生 ORIG mangled。
// stBloodWarGrowTable 亦定义于 WarRoom.cpp（ImportWarRoomWPTable 形参，布局一致；
// 纯派生 struct 无成员函数，链接无符号冲突）。此处为 importBloodWarEtcScript
// 提供本 TU 视图。
