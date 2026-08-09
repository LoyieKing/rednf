# rapidxml 1.13

## 版本确认

- 上游发布包：`rapidxml-1.13.zip`（SourceForge：
  https://sourceforge.net/projects/rapidxml/files/rapidxml/rapidxml%201.13/rapidxml-1.13.zip/download ）
- 包内四个头：`rapidxml.hpp`、`rapidxml_print.hpp`、`rapidxml_utils.hpp`、
  `rapidxml_iterators.hpp`（+ license.txt、manual.html）。

## 与二进制对照

secsvr 三件套的 DWARF include 闭包只用了三个头（`rapidxml.hpp` / `rapidxml_print.hpp` /
`rapidxml_utils.hpp`），与本包一致（迭代器头未引用）。构建重构报告
（`../docs/df_secsvr_build_recon.md` §2）通过宏族 `RAPIDXML_PARSE_ERROR` 等确认
版本族为 1.x；1.13 是 rapidxml 最终发布版（此后上游不再维护），与 2010s 中期的
CI 构建时间吻合。

## 使用

`import/include/opensource/rapidxml/rapidxml/` 下的三个头是本包的拷贝
（`rapidxml_iterators.hpp` 一并放入以便独立引用时可用）。

原始发布 zip 保留于 `../rapidxml-1.13.zip`。
