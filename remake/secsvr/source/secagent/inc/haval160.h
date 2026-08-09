// Auto-generated header stub from DWARF info
// Original path: inc/haval160.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_HAVAL160_H_H_
#define SECSVR_INC_HAVAL160_H_H_

// sizeof = 4
struct CHaval160 { // line 6
    // sizeof = 208
    struct ._32 {
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
CFrog::._32 *m_pctx;
public:
CHaval160(); // line 18
~CHaval160(); // line 19
int havalInit(); // line 21
int havalUpdate(unsigned char *arg0, unsigned int arg1); // line 22
int havalFinal(unsigned char *arg0); // line 23
void havalTransform3(unsigned int *arg0, unsigned char *arg1, unsigned int *arg2); // line 25
};
namespace CHaval160 {
    // sizeof = 208
    struct ._32 { // line 9
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
    struct ._0 { // line 9
    public:
    short unsigned int passes;
    short unsigned int hashLength;
    unsigned int digest[];
    unsigned char block[];
    unsigned int occupied;
    unsigned int bitCount[];
    unsigned int temp[];
    };
} // namespace CHaval160

#endif // SECSVR_INC_HAVAL160_H_H_
