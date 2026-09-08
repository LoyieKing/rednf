// df_game_r Game/ AradServerStateMessage（批3：从 GameStubs.cpp 迁移）。
// 布局升级（ARAD::Arad_ServerStateManager 批）：ORIG 12B packed POD。
//   ctor _ZN22AradServerStateMessageC1Ev @0x8186652：3×movl $0（+0/+4/+8）+
//   movw $0（+0）+ movl $0（+2）—— 等价全对象清零。
// 消费方证据：
//   checkContinus @0x81a95dc：value word(+0)==1 判生效、dword(+2) 对 curSec 比较；
//   insertStateNoSync/deleteState/saveServerState/sendEffectPacket：整 12B 拷贝/
//   put_binary(…,0xc)。
#ifndef DNF_ARADSERVERSTATEMESSAGE_H
#define DNF_ARADSERVERSTATEMESSAGE_H

class __attribute__((packed)) AradServerStateMessage
{
public:
    AradServerStateMessage();
    // [b28c-30] 匿名 union 双视角（P235）：ORIG C1 0x08186652 先发 3×movl $0
    // （+0/+4/+8 三段 dword 清零）再发真实字段 movw $0(+0)/movl $0(+2)；
    // 4.4 对 memset 恒发 call（N78）⇒ 以 int[3] 视角承载前三段 store。
    // 探针 /tmp/l30_probe1.cpp 逐条全等；sizeof/偏移/成员名/mangled 不变。
    union {
        struct __attribute__((packed)) {
            short         m_active;   // +0x00（==1 表示状态生效）
            unsigned int  m_endSec;   // +0x02（到期秒）
            unsigned char m_pad06[6]; // +0x06..+0x0b（发送/拷贝覆盖域，未单独访问）
        };
        int m_words[3];               // +0x00..+0x0b（ctor 三段 dword 清零视角）
    };
};

typedef char AradServerStateMessage_size_check[(sizeof(AradServerStateMessage) == 0xc) ? 1 : -1];

#endif  // DNF_ARADSERVERSTATEMESSAGE_H
