# src/commsvr/zergsvr/ —— zergsvr 专属代码

- 15 CU：zerg_main / zerg_application / zerg_accept_handler / zerg_app_handler /
  zerg_auto_connect / zerg_bill / zerg_buf_storage / zerg_comm_manager / zerg_configure /
  zerg_console_handler / zerg_ip_restrict / zerg_predefine / zerg_service_info_set /
  zerg_tcp_ctrl_handler / zerg_udp_ctrl_handler
- 职责：zerg 内部通信/控制服务
- 启动：`./zergsvr -t30 -i1`

## 当前状态（2026-10-20）

- `zerg_main.cpp` 的入口已实现，但整个 zergsvr 树仍有约 9/98 个 `.cpp` 带 DWARF stub 标记。
- `dnf_secsvr/` 没有检入可复现的 zergsvr 构建入口；`/tmp/zergsvr_rebuilt` 已不存在。
- 结论：**结构/入口基本齐全，行为与可部署 ELF 未验收**。

## TODO

- [ ] 补齐剩余 stub TU，重点覆盖 TCP/UDP 控制、自动连接、计费与 IP 限制。
- [ ] 纳入统一构建入口，产出可复验 ELF，记录符号与 SHA-256。
- [ ] 与 gunnersvr/secagent 做真实连接、超时、重启与协议验收。
