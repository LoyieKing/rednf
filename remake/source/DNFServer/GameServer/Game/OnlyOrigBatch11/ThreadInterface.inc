ThreadInterface::ThreadInterface() {}

// ---- [b28c-l23 代落 C28L18 spec] begin / dispatch_proxy ------------------
// ORIG T 0x08630b8a（nm 实证强符号 ⇒ 外联，头内联只产 W/P119）：
// `lea 0x4(this),%edx` + (esp)=&m_tid / 0x4=NULL / 0x8=&dispatch_proxy /
// 0xc=this → call pthread_create → `mov %eax,-0xc` 具名槽往返 →
// `cmpl $0,-0xc; je OK`；失败块 cerr 四段（串 @0x08ce6620、" " @0x08ce664c、
// operator<<(int)(rc)、endl）；OK 块 `m_tidCopy = m_tid`（+8 ← +4）后
// `mov $1,%eax`（两路恒返 true）。
