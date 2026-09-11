// ORIG 0x088aef52 parseAbility：P333 首次建表状态机（21 项静态映射惰性注册）。
// switch(key) 每个 case 体前半是「!inited ⇒ 注册该项、无 break、fallthrough 到下一
// case」，后半是「inited ⇒ 正常动作 break」；未初始化时 key 取哨兵 0（case 0 与
// case 0x46 共享入口），注册链走完落入 default（此时 inited 仍为 false，其
// `if (inited) return false` 不触发），再由公共尾 leave + inited=true + goto 循环头
// 重查，第二次进入 switch 才按 map 查得的 key 执行正常动作；default 同时承担
// find 未命中（key==-1）的 false 返回。
// [DualArith 2026-09-09] gap 172→0（orig 977 / ours 977；基线 ours 805）。
// [L4fEventAvatar 2026-09-11] AE 收口（977/977，clean 逐行相等）：①inited 分支里
//   `find` 命中判定改三目整体赋值 `key = (it != end) ? it->second : -1;`
//   （ORIG 0x088aef7c 真支算完 jmp 汇合 + 假支 mov $imm + 唯一落槽）；②循环尾
//   `if (!inited) { leave; inited = true; } else { return true; }` 且**不写
//   continue**（ORIG 0x088b00c9 的 `jmp 0x088aef74` 即 do-while 尾部回跳；写
//   continue 会被 4.4 把该 jmp 排到 `mov $0x1,%eax` 之后并多 2 条 jmp 中继）。
// p167 修正版（call rel32 + nm 区间 + .plt 反查，唯一认读口径）：MISSING 全为 C2 变体、
// EXTRA 全为 C1 变体（pair/map/SpinLock/const_iterator/STAvatarHiddenAbilityVariation
// ctor），严格 1:1 镜像、call 总数 150/150 ⇒ 按 Main 2026-09-09 裁定② clone 别名对
// 豁免，判 CLEAR、不排期。旧版 p167 / call_multiset_diff.py 读数已弃用（本件无旧版清单）。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
bool ARAD::SCRIPT::AvatarFixedHiddenOption::parseAbility(
