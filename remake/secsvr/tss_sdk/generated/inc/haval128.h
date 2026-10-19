// Auto-generated header stub from DWARF info
// Original path: inc/haval128.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_HAVAL128_H_H_
#define SECSVR_INC_HAVAL128_H_H_

// sizeof = 4
struct CHaval128 { // line 5
    // sizeof = 208
    struct ._31 {
    public:
    short unsigned int passes;
    short unsigned int hashLength;
    unsigned int digest[];
    unsigned char block[];
    unsigned int occupied;
    unsigned int bitCount[];
    unsigned int temp[];
    };
private:
CFrog::._31 *m_pctx;
public:
CHaval128(); // line 17
~CHaval128(); // line 18
void havalInit(); // line 20
int havalUpdate(unsigned char *arg0, unsigned int arg1); // line 21
int havalFinal(unsigned char *arg0); // line 22
private:
void havalTransform3(unsigned int *arg0, unsigned char *arg1, unsigned int *arg2); // line 24
};
namespace CHaval128 {
    // sizeof = 208
    struct ._31 { // line 8
    public:
    short unsigned int passes;
    short unsigned int hashLength;
    unsigned int digest[];
    unsigned char block[];
    unsigned int occupied;
    unsigned int bitCount[];
    unsigned int temp[];
    };
    // sizeof = 208
    struct ._0 { // line 8
    public:
    short unsigned int passes;
    short unsigned int hashLength;
    unsigned int digest[];
    unsigned char block[];
    unsigned int occupied;
    unsigned int bitCount[];
    unsigned int temp[];
    };
} // namespace CHaval128

#endif // SECSVR_INC_HAVAL128_H_H_
