// ---------------------------------------------------------------------------
// 11. CUser::setSkillCommandData @0x08654924
//     memcpy(&m_skillCommands[0],src,200)→解析 50×4B 条目入 getSkillCommand()
//     返回的 std::map<int,STSkillCommandData>→末尾 memset 清零。
//     条目位提取(+0xb/+0xc/+0xd/+0xe 相对 this+0x703d0):
//       cmd = e[3]&0x3f<<8|e[0]; count=e[3]>>5;
//       key[0]=(u16@e+1>>6)&7; key[1]=e[2]>>1&7; key[2]=e[2]>>4&7;
//       key[3]=(u32@e+1>>15)&7; key[4]=e[3]>>2&7(-0x4e/-0x4d 两组照录其位序)。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08654924：p167=M0/E3:_ZNKSt17_Rb_tree_iteratorISt4pairIKi18STSkillCommandDataEEneERKS4_;_ZSt9make_pairIRiR18STSkillCommandDataESt4pairINSt17__decay_and_stripIT_E6__typeENS4_IT0_E6__typeEEOS5_OS8_;_ZNSt4pairIKi18STSkillCommandDataEC1IiS1_EEOS_IT_T0_E，gap=26；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08654924：N82/N89 封顶——map/pair 模板实例差集
// （p167 M0/E3，gap 26）。
// [Lane6 2026-09-10 P172/P183 一试] 删具名指针 `e` 改宏展开表达式直取 → 实测**反劣**
// （200→233 vs ORIG 172）：4.4 对 `reinterpret_cast<uchar*>(&m_skillCommands[i])[k]`
// 恒展开完整 add 链（N4/N102 族；ORIG 的 `movzbl 0xb(%eax,%edx,4)` 单条 scaled 不可
// 复现）⇒ 已回滚为具名指针。memcpy 长度保留 ORIG 具名槽形（-0x18）。
// 附：`int size=0xc8` 局部试形亦反劣（200→204：4.4 每处回读槽 +store/load 各 1，
// 而立即数形零开销；ORIG 的「常量→槽→回读」两跳不可复现）⇒ 一并回滚为立即数。
void CUser::setSkillCommandData(char* data)
{
    // [M8COB 2026-09-11 重试+回滚] ORIG 0x0865492a..0x08654950 长度走**具名槽**两跳
    // （`movl $0xc8,-0x18(%ebp); mov -0x18(%ebp),%eax; … mov %eax,0x8(%esp)`）。
    // 实测在**当前**（位域直访 + sc 字段直写）结构下仍反劣：175→177 条 ⇒ 确认 A06 负结论；
    // 4.4 对该槽每处回读（+2 条）而 ORIG 只一跳。⇒ 保留立即数形。
    memcpy(&m_skillCommands[0], data, 200);

    std::map<int, STSkillCommandData>* cmds = reinterpret_cast<std::map<int, STSkillCommandData>*>(
        const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return;

    for (int i = 0; i < 50; ++i)
    {
        // [A06 2026-09-10 负结论·已回滚] ORIG 每次访问重算完整地址
        // （`mov -0x10(%ebp),%edx; mov 0x8(%ebp),%eax; add $0x1c0f4,%edx;
        //   movzbl 0xb(%eax,%edx,4),%ecx`，无 e 指针槽），但 4.4 对**同义表达式**
        // 两形实测均反劣：① `reinterpret_cast<const unsigned char*>(&m_skillCommands[i])[k]`
        // 逐处内联 = 200→230 条；② `((const unsigned char*)m_skillCommands)[i*4+k]` = 230→239 条
        // （4.4 恒发 add 链，不复现 4.1 的 `disp(%base,%idx,4)` 折叠）。
        // ⇒ 正解需 STCommand 内建**字节数组视角**（`union { 七个 packed 位域; unsigned char
        // m_bytes[4]; }`，sizeof 4/align 1 不变）才能出 ORIG 的单条 scaled 访存；属**头改动**，
        // 按 R2 只报告不动（见交付清单）。此前保留指针缓存形（4.4 最优，200 条）。
        // [M8COB 2026-09-11] 改用**位域直接访问**（ORIG 真形）：`m_skillCommands[i].字段`
        // 令 4.4 从 this 直接出 `disp(%base,%idx,4)` 形，省掉指针缓存槽与 add 链。
        // 判定用重算表达式（ORIG 0x08654992 `test %ax,%ax` 无槽）。
        if (m_skillCommands[i].m_command == 0)
            break;

        // [M8COB 2026-09-11 语义修正] ORIG 0x08654a13..0x08654b43 逐字节提取**五个** key，
        // 且落点即 sc 自身槽（-0x54=sc.m_command、-0x52=sc.m_count、-0x51..-0x4d=sc.m_key0..4，
        // sc = -0x54..-0x49 sizeof 12；ORIG 0x086549a1 先 call STSkillCommandData ctor @ -0x54）：
        //   -0x51 = (u16@+1>>6)&7  → m_key0(bits14-16)
        //   -0x50 = (u8 @+2>>1)&7  → m_key1(bits17-19)
        //   -0x4f = (u8 @+2>>4)&7  → m_key2(bits20-22)
        //   -0x4e = (u32@+1>>15)&7 → m_key3(bits23-25)（`mov 0xc(%eax,%edx,4),%eax; shr $0xf,%eax`）
        //   -0x4d = (u8 @+3>>2)&7  → m_key4(bits26-28)
        // 旧形 keys[4]：keys[3] 仅取 16 位 word>>15 = bit23（丢 bits24-25），且无 m_key4 ⇒
        // count≥5 时越界读栈、key3 高两位丢失。按 ORIG 修正为 sc 字段直写。
        STSkillCommandData sc;
        sc.m_command = static_cast<unsigned short>(m_skillCommands[i].m_command);
        sc.m_count   = static_cast<unsigned char>(m_skillCommands[i].m_count);
        sc.m_key0    = static_cast<unsigned char>(m_skillCommands[i].m_key0);
        sc.m_key1    = static_cast<unsigned char>(m_skillCommands[i].m_key1);
        sc.m_key2    = static_cast<unsigned char>(m_skillCommands[i].m_key2);
        sc.m_key3    = static_cast<unsigned char>(m_skillCommands[i].m_key3);
        sc.m_key4    = static_cast<unsigned char>(m_skillCommands[i].m_key4);

        // [b28d-solo-CUserOnlyOrig P167/P61] ORIG 0x08654xxx 命中
        // `make_pair<short&, STSkillCommandData&>`（非 int&）⇒ key 声明 short；
        // 且命中 `_Rb_tree_iterator::operator==`（非 !=）⇒ 原码 `if (it == end())`
        // 先走插入分支（P222/P220 极性按 ORIG 取 ==）。
        // ORIG 0x08654a64 `movzwl -0x54(%ebp),%eax; cwtl; mov %eax,-0x48` ⇒ **short 局部**
        // （源码变量持 short 位模式），`find()` 的 `const int&` 形参由隐式提升产生 int 转换
        // 临时（-0x48 槽 = 那个临时，非 key 本身）。决定性证据是**符号层**：ORIG 命中
        // `_ZSt9make_pairIRsR18STSkillCommandDataE…`（IRs = **short&**）而非 IRi；
        // [M8BigTURest 2026-09-11 04:02 复核修复] 上一版写成 `int key` ⇒ 发射
        // `_ZSt9make_pairIRiR18STSkillCommandDataE…` 与 `_ZNSt4pairIi18STSkillCommandDataEC1IRiRS0_EE…`
        // 等 **ORIG 不存在的模板实例**（违禁发明符号；基线里 short 版本是 AE），故恢复 short。
        short key = static_cast<short>(sc.m_command);        // ORIG -0x48 为 find 的 int 转换临时
        std::map<int, STSkillCommandData>::iterator it = cmds->find(key);
        if (it == cmds->end())
        {
            cmds->insert(std::make_pair(key, sc));          // make_pair(short&,data&) 收敛为值对
        }
        else
        {
            // ORIG 0x08654b06/0x08654b19 均从 sc 槽读（-0x54/-0x52），循环源 -0x51=sc.m_key0。
            it->second.m_command = sc.m_command;
            it->second.m_count   = sc.m_count;
            // [M8COB 2026-09-11] ORIG 0x08654b34..0x08654b43 是**单条 scaled 成员数组访存**：
            //   `movzbl -0x51(%ebp,%ecx,1),%ecx; mov %cl,0x7(%eax,%edx,1)`
            // 旧形 `(&sc.m_key0)[k]`/`(&it->second.m_key0)[k]` 取址再 +k ⇒ 4.4 发 add 链（多 4 条）。
            // 改用 union 的 `m_keyBytes[k]`（与匿名 struct 同位，语义等价）⇒ 直接 (base,index,1)+disp。
            for (int k = 0; k < static_cast<char>(sc.m_count); ++k)
                it->second.m_keyBytes[k] = sc.m_keyBytes[k];  // +7 起逐键写(ORIG 循环)
        }
    }
    memset(&m_skillCommands[0], 0, 200);
}