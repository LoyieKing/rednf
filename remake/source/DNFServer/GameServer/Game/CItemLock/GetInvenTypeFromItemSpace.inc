int GetInvenTypeFromItemSpace(ENUM_ITEMSPACE space);   // ORIG W 0x080f7845（switch 映射；权威定义 CTitleBook.cpp）

// [b28d-solo-CItemLock] DB 区块 8 字节条目的 POD 块拷贝视图（布局镜像
// stItemLockInfo）。stItemLockInfo 带用户 ctor（非 POD）⇒ 4.4 对其赋值恒逐字段
// 「读-写」交替、且对 +4 成员先发 `add $4`；**两 int 成员**的 POD 视图令 4.4 发
// ORIG 的 movdi 展开「先读高字、再读低字、后连写两字」（探针
// /tmp/b28d_solo_CItemLock/probe_copy3.cpp v11/v12 逐条同形）。平凡 ctor/op=/dtor
// ⇒ 零符号发射，sizeof/偏移/mangled 不受影响。
