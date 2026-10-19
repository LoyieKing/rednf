# output/ —— 各库构建产物

- 命名：`output/<lib>/release/include`（DWARF decl_file 证实被上层模块编译引用）
- 推断：每个库还有 `release/lib`（静态库归档），供最终链接
- 桩镜像：`../../source/<binary>/output/...`（按二进制各存一份）
