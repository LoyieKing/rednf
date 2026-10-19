# src/commsvr/gunnersvr/ —— gunnersvr 专属代码

- 9 CU：gunner_app / gunner_main / gunner_channel_mgr / gunner_file_scanner /
  gunner_predefine / gunner_reporter / gunner_statinfo_fetcher / gunner_timer_handler /
  gunner_tool
- 职责：TSS 平台对外安全接入 + 文件扫描（安全服务入口）
- 启动：`./gunnersvr -t30 -i1`（-t 超时 / -i 间隔，来自 main 参数解析）

## 当前状态（2026-10-20）

- `gunner_main.cpp` 的入口已实现，但整个 gunnersvr 树仍有约 7/88 个 `.cpp` 带 DWARF stub 标记。
- `dnf_secsvr/` 没有检入可复现的 gunnersvr 构建入口；`/tmp/gunnersvr_rebuilt` 已不存在。
- 结论：**结构/入口基本齐全，行为与可部署 ELF 未验收**。

## TODO

- [ ] 逐个确认剩余 stub TU 是否被链接路径使用；补齐未实现函数。
- [ ] 纳入统一构建入口，产出可复验 ELF，记录符号与 SHA-256。
- [ ] 对文件扫描、上报、统计与超时路径做真实运行/协议验收。
