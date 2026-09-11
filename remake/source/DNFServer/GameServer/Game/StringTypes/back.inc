char CharString::back() const
{
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad6edc）：`if(length())` 形在 4.4
    // 恒物化 `test %eax,%eax; setne %al; test %al,%al; je`（P292 int callee 侧），
    // ORIG 4.1 为直判 `test %eax,%eax; je`。改三目 `cond ? a : b`（P47 族）
    // 即复现直判 + 零分支后置岛布局，逐条与 ORIG 全等。
    return length() ? buffer_[length() - 1] : '\0';
}