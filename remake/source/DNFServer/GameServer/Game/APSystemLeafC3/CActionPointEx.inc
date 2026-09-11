CActionPointEx::CActionPointEx()
    : m_ap()          // ORIG 0x0889567c `call _ZN8APSystem12_ActionPointC1Ev`
    , m_mapKey(0)     // ORIG 0x08895684 `movw $0x0,0xc(%eax)`（mem-init：序在 vector C1 之前）
    , m_field0e(0)    // ORIG 0x0889568d `movb $0x0,0xe(%eax)`
    , m_levelMin(0)   // ORIG 0x08895694 `movb $0x0,0xf(%eax)`
    , m_levelMax(0)   // ORIG 0x0889569b `movb $0x0,0x10(%eax)`
    , m_weekday(7)    // ORIG 0x088956b0 `movl $0x7,0x20(%eax)`（S5AradDisp 已落地）
