#ifndef REDNF_CPACKETPARSER_H_
#define REDNF_CPACKETPARSER_H_

// 原始二进制中存在 CPacketParser 构造/析构（mangled C1/C2/D1/D2，均空实现），
// 位于 PacketParser.cpp TU（_GLOBAL__I__ZN13CPacketParserC2Ev 桩）。
class CPacketParser {
public:
    CPacketParser();
    ~CPacketParser();
};

#endif  // REDNF_CPACKETPARSER_H_
