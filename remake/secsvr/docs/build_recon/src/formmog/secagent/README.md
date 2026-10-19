# src/formmog/secagent/ —— secagent 专属代码

- 16 CU：secagent_main / secagent_app / secagent_msg_handler / secagent_antibot_* /
  secagent_dpsdk_info_mgr / secagent_file_load / secagent_game_exe /
  secagent_gamesvr_channel / secagent_bill / secagent_predefine / secagent_restrict /
  secagent_timer_handler / secagent_user_mgr
- 职责：反外挂 agent（TSS 客户端侧），含 AntiBot/DP 消息处理、用户/频道管理、
  限制/封禁、账单与监控
- 启动：`./secagent`

## 当前状态（2026-10-20）

- `secagent_main.cpp:178-183` 的 `init_cfg_info()` 与 `main()` **均为空**；按当前源码构建的
  secagent 不会执行启动逻辑。
- 本树约 16/150 个 `.cpp` 仍带 DWARF stub 标记；`TenHash.cpp` 仍有 HAVAL/RIPEMD/GOST TODO。
- `dnf_secsvr/` 没有检入可复现的 secagent 构建入口；`/tmp/secagent_rebuilt` 已不存在。
- 结论：**不可运行，不能替换原版 secagent**。

## TODO

- [ ] 实现 `main()` / `init_cfg_info()`：参数、配置、`SecAgentApp` 生命周期、信号与日志。
- [ ] 核对 `SecAgentApp::init/proc/process_recv_appframe` 与 `SecAgentGamesvrChannel` 的启动时序。
- [ ] 补齐消息处理、用户/频道、限制/封禁、账单、定时器与文件加载的未实现函数。
- [ ] 纳入统一构建入口，产出可复验 ELF；在真实 game + secsvr 会话中验收共享内存/AntiBot/DP 链路。
