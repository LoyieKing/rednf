# src/commlib/framework/ —— 服务框架（comm 通信框架）

- 13~15 CU；内容：应用生命周期（comm_app_frame/comm_svrd_application）、收发管道
  （comm_sndrcv_*）、配置（comm_svrd_config/comm_cfgsvr_sdk）、定时器、监控上报、
  zerg 总线（comm_zerg_mmappipe）等（`comm_*` 前缀）
- include：`src/commlib/framework` + `output/commlib/zenlib/release/include` +
  `output/protocol/common/release/include` + `import/include/{opensource,tencore}`
- 构建产物：`output/commlib/framework/release/include`
- 依赖：zenlib、protocol/common、TenCrypt SDK、tsf4g-TDR
