#ifndef AUCTION_ISTATE_H_
#define AUCTION_ISTATE_H_

#include <stdio.h>

class IState
{
public:
    enum STATE_COMMON
    {
        STATE_STAND = 0,
        STATE_ATTACK = 1,
        STATE_MOVE = 2,
        STATE_CAST = 3
    };

    IState()
    {
    }
    virtual void enter() = 0;
    virtual void exit() = 0;

    int mCurState;
};

class Stand : public IState
{
public:
    Stand()
    {
    }
    virtual void enter()
    {
        mCurState = 0;
        puts("\xbc\xad \xc0\xd6\xb4\xc2\xb4\xd9.");
    }
    virtual void exit()
    {
        puts("\xbc\xad \xc0\xd6\xb4\xc2 \xb5\xbf\xc0\xdb\xc0\xbb \xb1\xd7\xb8\xb8\xc7\xcf\xb0\xed \xb4\xd9\xb8\xa5\xb0\xc9 \xc7\xd2\xb2\xa8\xb4\xd9.");
    }
};

class Move : public IState
{
public:
    Move()
    {
    }
    virtual void enter()
    {
        mCurState = 1;
        puts("\xbf\xf2\xc1\xf7\xc0\xce\xb4\xd9.");
    }
    virtual void exit()
    {
        puts("\xbf\xf2\xc1\xf7\xc0\xd3\xc0\xbb \xb8\xd8\xc3\xe1\xb4\xd9");
    }
};

class Attack : public IState
{
public:
    Attack()
    {
    }
    virtual void enter()
    {
        mCurState = 2;
        puts("\xb0\xf8\xb0\xdd\xc7\xd1\xb4\xd9.");
    }
    virtual void exit()
    {
        puts("\xb0\xf8\xb0\xdd\xc0\xbb \xb8\xd8\xc3\xe1\xb4\xd9.");
    }
};

class Cast : public IState
{
public:
    Cast()
    {
    }
    virtual void enter()
    {
        mCurState = 3;
        puts("\xb8\xb6\xb9\xfd\xc0\xbb \xbd\xf0\xb4\xd9");
    }
    virtual void exit()
    {
        puts("\xb8\xb6\xb9\xfd\xc0\xbb \xb8\xd8\xc3\xe1\xb4\xd9");
    }
};

#endif // AUCTION_ISTATE_H_
