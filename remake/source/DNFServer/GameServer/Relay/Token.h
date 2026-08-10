#ifndef TOKEN_H_
#define TOKEN_H_

#define MAX_TOKEN 256

// Token：n_token@0 / seps@4[52] / m_token@0x38[256]，大小 1080（与 auction/channel 一致）
class Token
{
public:
    Token();
    ~Token();
    bool get_token(char* str, int size);
    char* get_context(int idx0);
    void free_token(int idx0);
    int get_n_token();

private:
    int n_token;
    char seps[52];
    char* m_token[MAX_TOKEN];
};

#endif // TOKEN_H_
