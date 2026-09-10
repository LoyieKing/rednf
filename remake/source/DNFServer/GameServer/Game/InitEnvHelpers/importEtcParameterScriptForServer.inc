bool importEtcParameterScriptForServer(void);
    // _Z33importEtcParameterScriptForServerv

// ---------------------------------------------------------------------------
// CreatureCommonScript（ORIG C1 0x08096ab4 / clear 0x08096a2c / D1 0x08096b22）
//   clear：+0x00..+0x14 六个 int 清零 → vector<int>@+0x18/.clear()
//          → vector<int>@+0x24/.clear() → +0x30=1 → +0x34=1 → +0x38=100(0x64)
//          → +0x3c=0
//   C1：vector@+0x18 构造 → vector@+0x24 构造 → clear()
//   D1：~vector@+0x24 → ~vector@+0x18（成员逆序析构）
//   尺寸按 ORIG 调用方栈对象 0x58（Ghidra local [88]）补尾垫。
// ---------------------------------------------------------------------------
