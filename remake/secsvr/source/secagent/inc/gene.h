// Auto-generated header stub from DWARF info
// Original path: inc/gene.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_GENE_H_H_
#define SECSVR_INC_GENE_H_H_

// sizeof = 8
struct CGene { // line 7
private:
void *m_pSys;
void *m_pLocal;
public:
CGene(); // line 9
~CGene(); // line 10
void Init(void *arg0, void *arg1); // line 12
void UnInit(); // line 13
void Gene(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3); // line 15
private:
void SetGeneKey(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned char *arg3); // line 17
};

#endif // SECSVR_INC_GENE_H_H_
