# src/commlib/ —— 公共库

两个库：`zenlib`（基础库，无内部依赖）与 `framework`（服务框架，依赖 zenlib 与
protocol/common 的 installed 头）。二者构建后把头安装到
`output/commlib/<lib>/release/include`，供上层模块引用。
