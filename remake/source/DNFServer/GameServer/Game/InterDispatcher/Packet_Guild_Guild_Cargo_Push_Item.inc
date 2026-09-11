// ORIG 0x084dd856 T dispatch_sig —— 公会仓库上架申请：校验角色/物品/期限后删除
// 背包物品并把物品内容打包转发给公会服务器代理。
// 报告 docs/class_func_reports/Inter_GuildCargoCheckPushItem/dispatch_sig.md 全文转写。
// 依赖 Packet_Guild_Guild_Cargo_Push_Item（NEEDS_DECL，见同目录 meta）。
// [c3] 旧 DnfItemInfoView（本 TU 字节区视图）已删：头内 m_item 为真实 DnfItemInfo
// 成员，视图引用别名会多占一个 lea 槽（ORIG 全为 pck+常量栈槽直写）。
// ORIG ctor 0x084e9434：PacketHeader(0x70d,0x53) → 字段清零 → DnfItemInfo(+0x1b) 构造
// → m_e/m_f 清零。m_item 为字节区（Guild 域同名类冲突），placement-new 显式构造
// （多一次 operator new 调用，形态残留；gcc 4.4.7 拒绝显式构造调用，语义与 ORIG 一致）。
Packet_Guild_Guild_Cargo_Push_Item::Packet_Guild_Guild_Cargo_Push_Item()
