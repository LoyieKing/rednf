# mysqlclient 5.0.92

## 版本确认

- 上游发布包：`mysql-5.0.92.tar.gz`（downloads.mysql.com：
  https://downloads.mysql.com/archives/get/p/23/file/mysql-5.0.92.tar.gz ）
- 完整解包源码保留于本目录（122 MB，含 `include/`、`libmysql/`、`libmysql_r/` 客户端）。
- 版本证据（二进制 .debug_macinfo）：`MYSQL_VERSION_ID=50092`、
  `MYSQL_SERVER_VERSION="5.0.92"`；源码包 `configure` 中 `VERSION=5.0.92`、
  `configure.in` 计算 `MYSQL_VERSION_ID=50092`。

## 二进制实际引用的头（7 个）

DWARF include 闭包：`mysql.h`、`mysql_com.h`、`mysql_time.h`、`mysql_version.h`、
`my_alloc.h`、`my_list.h`、`typelib.h`。

- 6 个直接来自本包 `include/`。
- `mysql_version.h` 是 configure 时由 `include/mysql_version.h.in` 生成，本目录
  `include-generated/mysql_version.h` 按 5.0.92 默认配置生成（见该文件头注释）。

## 静态链接说明

原始二进制静态链接了 libmysqlclient（mysql_* 符号在 ELF 内定义）。后续还原 zenlib
的 `zen_mysql_*` 封装与服务 DB 访问时需要 libmysqlclient 静态库；本包保留
`libmysql/` 与 `libmysql_r/` 完整源码，构建阶段再评估（5.0.92 用 autotools，
需在 i586/32 位环境编译客户端库，仅取 client 部分，不构建 mysqld）。
