# src/ —— 主工程源码树

- 路径证据：`/data/secci/ci/jenkins/workspace/g3_release_suse32*/src/...`（三个 tag 同构）
- 工具链：GCC 4.1.0 SUSE，32 位 i586，-O2 级别，-DNDEBUG，-pthread
- 桩镜像：`../../source/gunnersvr/src`、`../../source/zergsvr/src`、`../../source/secagent/src`
  （同一棵树在不同 tag 下编译，桩按二进制各存一份）
- 模块：commlib/{zenlib,framework}、commsvr/{gunnersvr,zergsvr}、
  formmog/secagent、protocol/{common,formmog}

> 状态（2026-10-20）：本目录是结构镜像，不是可运行源码。gunnersvr/zergsvr 的 `main` 已实现，
> 但仍有 stub TU；secagent 的 `main()` 为空。完成度与 TODO 见 [`../../README.md`](../../README.md)。
