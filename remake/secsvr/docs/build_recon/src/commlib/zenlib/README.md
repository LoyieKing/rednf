# src/commlib/zenlib/ —— zen 基础库

- 40~44 CU；内容：锁/线程/事件反应器/定时器/共享内存/进程通信/MySQL 封装/TraceLog 等
  （`zen_*` 前缀）
- include：`src/commlib/zenlib` + `import/include/opensource`（mysql、rapidxml）
- 构建产物：`output/commlib/zenlib/release/include`（DWARF 证实），推断另有 `release/lib`
- 依赖：无内部模块依赖；第三方：mysqlclient 5.0.92、rapidxml
