#ifndef DNF_SIGNAL_TRANSLATOR_H_
#define DNF_SIGNAL_TRANSLATOR_H_

class CApplication;
class CSignal;

// ---- CSignalTranslator ----
class CSignalTranslator
{
public:
    CSignalTranslator();
    ~CSignalTranslator();
    // ORIG 返回类型为 bool：init_signal 中 !regist_signal(...) 编译为
    // xor $1,%eax（4.4.6-3/4.4.7 对 char 返回会做 sete 物化，与 ORIG 不符）。
    bool regist_signal(int sig, void (*handler)(int));
    void clear();
    void init_signal();
    void init_handler(CApplication* app);
    void init(CApplication* app);
    CSignal* getSignal(int idx) const;
    CSignal* m_signals[0x1a];  // +0（0x68 字节）
};

CSignalTranslator* CSignalTranslatorInstance();
void signal_handler(int sig);

#endif  // DNF_SIGNAL_TRANSLATOR_H_
