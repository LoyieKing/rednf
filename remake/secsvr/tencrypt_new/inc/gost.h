/* gost.h -- GOST 28147-89（由 DWARF 重建 + 实现修正）
   FUCKING_WZ_SP / FUCKING_WZ_SPKEY 的 DWARF 声明为无界多维数组（非法），
   实现使用文件级静态表（见 src/gost.cpp），此处移除成员声明。 */
#ifndef TENCRYPT_INC_GOST_H_
#define TENCRYPT_INC_GOST_H_

// sizeof = 8
struct CGost {
private:
    void *m_pSys;
    void *m_pLocal;
public:
    CGost();
    ~CGost();
    void Init(void *arg0, void *arg1);
    void UnInit();
    void Gost(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3);
private:
    void gost_dec(unsigned int *arg0, unsigned int *arg1);
    void dencry_data(unsigned int *arg0, unsigned int *arg1, unsigned int *arg2);
    void gost_enc(unsigned int *arg0, unsigned int *arg1);
    void encry_data(unsigned int *arg0, unsigned int *arg1, unsigned int *arg2);
    void gost_swap(unsigned int *arg0, unsigned int *arg1);
    unsigned int f(unsigned int arg0);
};

#endif /* TENCRYPT_INC_GOST_H_ */
