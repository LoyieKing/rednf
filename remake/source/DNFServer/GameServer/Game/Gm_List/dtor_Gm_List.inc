// ORIG 0x08299948：D1 为 deleting 形态（cmpl $0,0x8(%ebp); je;
// call D1(自身递归); call _ZdlPv）——源码为 `delete this` 展开。
Gm_List::~Gm_List() { delete this; }
