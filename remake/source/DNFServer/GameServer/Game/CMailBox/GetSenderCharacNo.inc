unsigned int CMailBox::CMail::GetSenderCharacNo() const { return m_senderCharacNo; }

// ---- CMailBox::CMail::._398（命名类 t_398；成员 +0x00 unsigned int m_packageMoney /
//      +0x04 Inven_Item m_item）----
// ORIG _ZN8WongWork8CMailBox5CMail5._398C1Ev t 0x085519cc (0x16)：仅 Inven_Item 默认
// 构造（切片 9 条无 packageMoney 清零——原码不初始化该字段，P131 口径）。GCC 对
// ctor/dtor 的 asm 别名无效（硬限制），故以命名类 `t_398`（自然 mangled `5t_398*` >
// 别名 `5._398*`，P203 附注条件②）重建，末尾 `.set`+四件套令点号别名成标注名。
