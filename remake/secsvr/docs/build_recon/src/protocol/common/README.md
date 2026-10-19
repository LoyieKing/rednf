# src/protocol/common/ —— 通用协议（TDR 生成代码 + 手写封装）

- 12~14 CU：TdrBufUtil / TdrIO / TdrParse / TdrTime / TdrTypeUtil / TdrXml /
  comm_conf_{cfgsdk,framework,gunnersvr,svcid,zerg} / comm_proto_public_{cfgsvr,logsvr,monitorsvr}
- 基于 tsf4g-TDR（import/include/tencore/tsf4g/tsf4g_base/tdr），
  `comm_conf_*` 是 XML 配置的 TDR 结构（大量匿名嵌套 struct，桩中已内联展开）
- 构建产物：`output/protocol/common/release/include`
