// ---- EquipmentParameterInfo::._119（命名类 t_119；成员 +0x00 SASPROPERTY vector /
//      +0x0c/+0x0d char / +0x10 vector<vector<float>>）----
// ORIG _ZN22EquipmentParameterInfo5._119C1Ev t 0x088fd884（仅默认构造两 vector，
// char 不初始化）；~._119 t 0x088fd8ca（逆序析构）；aSERKS0_ t 0x088fd910（向
// vector<SASPROPERTY> op=、char、char、vector<vector<float>> op= 拷贝，无 self-check）。
// GCC 对 ctor/dtor 的 asm 别名无效（硬限制），故以命名类 `t_119`(5t_119) 重建，
// 末尾 `.set` 汇编别名强制 ORIG 带点号符号（mangling 必要性，非 §4.5 违例）。
EquipmentParameterInfo::t_119::t_119()
{
    // ORIG 0x088fd884：仅默认构造 +0/+0x10 两 vector；char 保留未定义初值。
}