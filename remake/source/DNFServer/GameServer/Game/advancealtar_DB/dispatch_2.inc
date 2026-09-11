// [report dispatch.md / objdump 0x81412b2]
// [b28d-solo-advancealtar_DB] dead-test 族封顶（orig 312 / ours 308，clean resid=4）：
//   ORIG 两处 insert 分支 `mysql->exec(true)` 后各留 `xor $0x1,%eax; test %al,%al`
//   （4.1 对未使用 bool 返回值的归一化残留，P168a③ dead test）；4.4 tree-DCE 删除。
//   P167 CLEAR（31/31）、语义等价。
bool DB_StatisticAdvanceAltar::dispatch(int /*uid*/, int /*uniqueId*/, Stream* stream)
