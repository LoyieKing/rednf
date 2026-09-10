int get_rand_int(int range);

// stQuestTriggerState_t::setTrigger（ORIG W 0x836dce0：9 位触发值打包）
// [E04 2026-09-10] 旧 b28d-solo-Quest 判词「N18a 封顶：4.4 无源码形态产出该冗余
// 截断」作废 —— 真因是字段为「16 位容器 9 位位域」（QuestTriggerState.h 位域建模）。
// 位域直赋 `m_triggerN = x;` 在 4.4.7 下逐条复现 ORIG：值截断 `mov %eax,%edx;
// and $0x1ff,%dx` → 读容器 `movzwl 旧; and $0xfe00,%dx` → or → `mov %dx,容器`；
// ORIG 的 `mov %edx,%ecx; and $0x1ff,%cx` 冗余截断由位域赋值自身产生。
// 探针 /tmp/qts/b.cpp setTrigger 41=41 逐条一致（改前 41/62，gap +21）。
