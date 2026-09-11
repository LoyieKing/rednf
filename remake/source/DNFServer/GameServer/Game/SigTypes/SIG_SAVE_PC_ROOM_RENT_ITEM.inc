// ORIG 权威两层结构（条目类型 RENT_INFO，容器 RENT_ITEM）：
//   PC_ROOM_INFO::clear  @0x0848f9a2：+0 int=0 / +4,+5,+6 byte=0 / +8 int=-1。
//   DIMENSION_INFO::clear@0x0848f9e4：+0 int=0 / +4..+7 byte=0。
//   ITEM::clear 0x8694c9a：for i<=0 调 INFO::clear(&entry[i])。
//   ITEM::clear 0x8694cd2：for i<=0xa 调 INFO::clear（entry[11] 不动）。
// C1 类外定义（头文件仅声明）：生成 ORIG 导出符号
// _ZN26SIG_SAVE_PC_ROOM_RENT_ITEMC1Ev（0x8491cbe，仅构造 entry[0]）/
// _ZN28SIG_SAVE_DIMENSION_RENT_ITEMC1Ev（0x8491cf2，构造 entry[0..10]）。
SIG_SAVE_PC_ROOM_RENT_ITEM::SIG_SAVE_PC_ROOM_RENT_ITEM() {}
