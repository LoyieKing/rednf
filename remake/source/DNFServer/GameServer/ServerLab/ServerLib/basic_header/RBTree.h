#ifndef NSL_RBTREE_H_
#define NSL_RBTREE_H_

#include <boost/pool/pool.hpp>

#include "TraceLog.h"

namespace nsl {

namespace changes_meaning {

enum ENUM_RBCOLOR
{
    RB_RED = 0,
    RB_BLACK = 1
};

template <class TKey, class TData, int N>
struct RBNode
{
    RBNode<TKey, TData, N>* mParent;
    RBNode<TKey, TData, N>* mLeftChild;
    RBNode<TKey, TData, N>* mRightChild;
    unsigned char mColor;
    TKey mKey;
    TData mData;

    void init()
    {
        mParent = NULL;
        mLeftChild = NULL;
        mRightChild = NULL;
        mColor = changes_meaning::RB_RED;
    }

    void SetColorBlack()
    {
        mColor = changes_meaning::RB_BLACK;
    }

    void SetColorRed()
    {
        mColor = changes_meaning::RB_RED;
    }

    RBNode<TKey, TData, N>* getSibling()
    {
        if (mParent != NULL)
        {
            if (mParent->mLeftChild == this)
            {
                return mParent->mRightChild;
            }
            return mParent->mLeftChild;
        }
        return NULL;
    }

    RBNode<TKey, TData, N>* getGrandParent()
    {
        if (mParent != NULL)
        {
            return mParent->mParent;
        }
        return NULL;
    }

    RBNode<TKey, TData, N>* getUncle()
    {
        RBNode<TKey, TData, N>* grandParent = getGrandParent();
        if (grandParent != NULL)
        {
            if (mParent == grandParent->mLeftChild)
            {
                return grandParent->mRightChild;
            }
            return grandParent->mLeftChild;
        }
        return NULL;
    }

    TData* GetData()
    {
        return &mData;
    }

    RBNode<TKey, TData, N>* GetParent()
    {
        return mParent;
    }

    RBNode<TKey, TData, N>* GetLeftChild()
    {
        return mLeftChild;
    }

    RBNode<TKey, TData, N>* GetRightChild()
    {
        return mRightChild;
    }
};

} // namespace changes_meaning

enum ENUM_RBTREE_INSERT_CASE
{
    RBTREE_INSERT_CASE_NOT_DEFINED = 0,
    RBTREE_INSERT_CASE_NOT_ROOT = 1,
    RBTREE_INSERT_CASE_PARENT_RED = 2,
    RBTREE_INSERT_CASE_UNCLE_NOT_RED = 3
};

enum ENUM_RBTREE_DELETE_CASE
{
    RBTREE_DELETE_CASE_NOT_DEFINED = 0,
    RBTREE_DELETE_CASE_CHILD_BLACK = 1,
    RBTREE_DELETE_CASE_NOT_ROOT = 2,
    RBTREE_DELETE_CASE_SIBLING_BALCK = 3,
    RBTREE_DELETE_CASE_PARENT_RED = 4
};

template <class TKey, class TData, int N>
class RBTree
{
public:
    typedef changes_meaning::RBNode<TKey, TData, N> RBNodeType;

    template <class TRBNode, class TDataType>
    class RBIterator
    {
    public:
        enum ENUM_RBITERATOR_TRAVERSAL
        {
            RBITERATOR_TRAVERSAL_NOT_DEFINE = -1,
            RBITERATOR_TRAVERSAL_PREORDER = 0,
            RBITERATOR_TRAVERSAL_INORDER = 1,
            RBITERATOR_TRAVERSAL_POSTORDER = 2
        };

        enum ENUM_RBITERATOR_FLOW
        {
            RBITERATOR_FLOW_NOT_DEFINE = -1,
            RBITERATOR_FLOW_PARENT = 0,
            RBITERATOR_FLOW_RIGHTCHILD = 1
        };

        RBIterator()
        {
            init();
        }

        RBIterator(ENUM_RBITERATOR_TRAVERSAL traversal)
        {
            init();
            mTraversal = traversal;
        }

        ~RBIterator()
        {
        }

        RBIterator& operator=(const RBIterator& other)
        {
            mpRbnode = other.mpRbnode;
            mTraversal = other.mTraversal;
            mFlow = other.mFlow;
            return *this;
        }

        void init()
        {
            mpRbnode = NULL;
            mTraversal = (ENUM_RBITERATOR_TRAVERSAL)-1;
            mFlow = (ENUM_RBITERATOR_FLOW)-1;
        }

        RBIterator& operator++()
        {
            if (mTraversal == RBITERATOR_TRAVERSAL_NOT_DEFINE)
            {
                G_TraceLog()->sysLog(8, "OPERATOR++, err RBITERATOR_TRAVERSAL_NOT_DEFINE");
            }
            if (mpRbnode != NULL && mTraversal != RBITERATOR_TRAVERSAL_PREORDER &&
                mTraversal == RBITERATOR_TRAVERSAL_INORDER)
            {
                bool bValidChild = false;
                switch (mFlow)
                {
                case RBITERATOR_FLOW_PARENT:
                    if (mpRbnode->GetParent() != NULL)
                    {
                        if (mpRbnode == mpRbnode->GetParent()->GetRightChild())
                        {
                            while (mpRbnode == mpRbnode->GetParent()->GetRightChild())
                            {
                                mpRbnode = mpRbnode->GetParent();
                                if (mpRbnode->GetParent() == NULL)
                                {
                                    bValidChild = true;
                                    break;
                                }
                            }
                        }
                        if (!bValidChild)
                        {
                            mpRbnode = mpRbnode->GetParent();
                            if (mpRbnode->GetRightChild() != NULL)
                            {
                                mFlow = RBITERATOR_FLOW_RIGHTCHILD;
                            }
                        }
                        else
                        {
                            mpRbnode = NULL;
                        }
                    }
                    else
                    {
                        mpRbnode = NULL;
                    }
                    break;
                case RBITERATOR_FLOW_RIGHTCHILD:
                    mpRbnode = mpRbnode->GetRightChild();
                    bValidChild = false;
                    if (mpRbnode->GetLeftChild() != NULL)
                    {
                        bValidChild = true;
                    }
                    if (bValidChild)
                    {
                        while (mpRbnode->GetLeftChild() != NULL)
                        {
                            mpRbnode = mpRbnode->GetLeftChild();
                        }
                        if (mpRbnode->GetRightChild() != NULL)
                        {
                            mFlow = RBITERATOR_FLOW_RIGHTCHILD;
                        }
                        else
                        {
                            mFlow = RBITERATOR_FLOW_PARENT;
                        }
                    }
                    else
                    {
                        if (mpRbnode->GetRightChild() != NULL)
                        {
                            mFlow = RBITERATOR_FLOW_RIGHTCHILD;
                        }
                        else
                        {
                            mFlow = RBITERATOR_FLOW_PARENT;
                        }
                    }
                    break;
                default:
                    G_TraceLog()->sysLog(8, "OPERATOR++, err default");
                    break;
                }
            }
            return *this;
        }

        TDataType* GetDataPtr()
        {
            if (mpRbnode != NULL)
            {
                return mpRbnode->GetData();
            }
            return NULL;
        }

        void begin(RBNodeType* pRoot)
        {
            if (mTraversal == RBITERATOR_TRAVERSAL_NOT_DEFINE)
            {
                G_TraceLog()->sysLog(8, "begin(), err RBITERATOR_TRAVERSAL_NOT_DEFINE");
            }
            mpRbnode = pRoot;
            if (mpRbnode != NULL)
            {
                if (mTraversal != RBITERATOR_TRAVERSAL_PREORDER &&
                    mTraversal == RBITERATOR_TRAVERSAL_INORDER)
                {
                    RBNodeType* pre = NULL;
                    while (mpRbnode != NULL)
                    {
                        pre = mpRbnode;
                        mpRbnode = mpRbnode->GetLeftChild();
                    }
                    mpRbnode = pre;
                }
                if (mpRbnode->GetRightChild() != NULL)
                {
                    mFlow = RBITERATOR_FLOW_RIGHTCHILD;
                }
                else
                {
                    mFlow = RBITERATOR_FLOW_PARENT;
                }
            }
        }

    private:
        RBNodeType* mpRbnode;
        ENUM_RBITERATOR_TRAVERSAL mTraversal;
        ENUM_RBITERATOR_FLOW mFlow;
    };

    RBTree(bool bDuplicatePermit = false)
    {
        mDuplicatePermit = bDuplicatePermit;
        mNumOfRBNode = 0;
        mpRBNodePool = new boost::pool<boost::default_user_allocator_new_delete>(0x18, 0x6000, 0);
        mpRoot = (RBNodeType*)mpRBNodePool->malloc();
        mpRBNodePool->free(mpRoot);
        mpRoot = NULL;
    }

    RBTree(const RBTree& other)
    {
        mDuplicatePermit = other.mDuplicatePermit;
        mNumOfRBNode = 0;
        mpRBNodePool = new boost::pool<boost::default_user_allocator_new_delete>(0x18, 0x6000, 0);
        mpRoot = (RBNodeType*)mpRBNodePool->malloc();
        mpRBNodePool->free(mpRoot);
        mpRoot = NULL;
    }

    ~RBTree()
    {
        Clear();
        delete mpRBNodePool;
    }

    RBTree& operator=(const RBTree& other)
    {
        mDuplicatePermit = other.mDuplicatePermit;
        return *this;
    }

    bool Insert(const TKey& rKey, const TData& rNewData)
    {
        RBNodeType* newNode = (RBNodeType*)mpRBNodePool->malloc();
        if (newNode == NULL)
        {
            G_TraceLog()->sysLog(8, "Insert(), OUT_OF_MEMORY");
            return false;
        }
        newNode->init();
        newNode->mKey = rKey;
        newNode->mData = rNewData;
        int preFlow = 0;
        RBNodeType* curr = mpRoot;
        RBNodeType* pre = NULL;
        while (curr != NULL)
        {
            pre = curr;
            if (newNode->mKey < curr->mKey)
            {
                preFlow = 1;
                curr = curr->mLeftChild;
            }
            else
            {
                if (mDuplicatePermit != true && newNode->mKey == curr->mKey)
                {
                    return false;
                }
                preFlow = 2;
                curr = curr->mRightChild;
            }
        }
        // ORIG: switch 分发（mov preFlow; cmp $1; je; cmp $2; je; jmp default）。
        switch (preFlow)
        {
        case 1:
            pre->mLeftChild = newNode;
            newNode->mParent = pre;
            break;
        case 2:
            pre->mRightChild = newNode;
            newNode->mParent = pre;
            break;
        default:
            if (pre != NULL)
            {
                G_TraceLog()->sysLog(8, "Insert(), ROOT_INSERT_ERROR");
            }
            mpRoot = newNode;
            break;
        }

        ENUM_RBTREE_INSERT_CASE insertCase = RBTREE_INSERT_CASE_NOT_DEFINED;
        bool endLoop = false;
        while (!endLoop)
        {
            RBNodeType* uncle;
            RBNodeType* grandParent;
            switch (insertCase)
            {
            case RBTREE_INSERT_CASE_NOT_DEFINED:
                if (newNode->mParent == NULL)
                {
                    newNode->SetColorBlack();
                    endLoop = true;
                }
                else
                {
                    insertCase = RBTREE_INSERT_CASE_NOT_ROOT;
                }
                break;
            case RBTREE_INSERT_CASE_NOT_ROOT:
                if (newNode->mParent->mColor == changes_meaning::RB_BLACK)
                {
                    endLoop = true;
                }
                else
                {
                    insertCase = RBTREE_INSERT_CASE_PARENT_RED;
                }
                break;
            case RBTREE_INSERT_CASE_PARENT_RED:
            {
                uncle = newNode->getUncle();
                if (uncle != NULL && uncle->mColor == changes_meaning::RB_RED)
                {
                    newNode->mParent->SetColorBlack();
                    uncle->SetColorBlack();
                    grandParent = newNode->getGrandParent();
                    grandParent->SetColorRed();
                    insertCase = RBTREE_INSERT_CASE_NOT_DEFINED;
                    newNode = grandParent;
                }
                else
                {
                    insertCase = RBTREE_INSERT_CASE_UNCLE_NOT_RED;
                }
                break;
            }
            case RBTREE_INSERT_CASE_UNCLE_NOT_RED:
            {
                grandParent = newNode->getGrandParent();
                if (newNode->mParent->mRightChild == newNode &&
                    newNode->mParent == grandParent->mLeftChild)
                {
                    rotateLeft(newNode->mParent);
                    newNode = newNode->mLeftChild;
                }
                else if (newNode->mParent->mLeftChild == newNode &&
                         newNode->mParent == grandParent->mRightChild)
                {
                    rotateRight(newNode->mParent);
                    newNode = newNode->mRightChild;
                }
                newNode->mParent->SetColorBlack();
                grandParent->SetColorRed();
                if (newNode->mParent->mLeftChild == newNode &&
                    newNode->mParent == grandParent->mLeftChild)
                {
                    rotateRight(grandParent);
                }
                else if (newNode->mParent->mRightChild == newNode &&
                         newNode->mParent == grandParent->mRightChild)
                {
                    rotateLeft(grandParent);
                }
                else
                {
                    G_TraceLog()->sysLog(8, "Insert(), err RBTREE_INSERT_CASE_UNCLE_NOT_RED_AFERT_ALIGN");
                }
                endLoop = true;
                break;
            }
            default:
                G_TraceLog()->sysLog(8, "Insert(),err RBTREE_INSERT_CASE_DONT_REACH_HERE");
                break;
            }
        }
        mNumOfRBNode = mNumOfRBNode + 1;
        return true;
    }

    bool Remove(const TKey& rKey)
    {
        RBNodeType* target = findNode(rKey);
        if (target != NULL)
        {
            if (target->mLeftChild != NULL && target->mRightChild != NULL)
            {
                RBNodeType* rightmost = target->mLeftChild;
                RBNodeType* next;
                for (; rightmost != NULL; rightmost = rightmost->mRightChild)
                {
                    next = rightmost;
                }
                target->mKey = next->mKey;
                target->mData = next->mData;
                target = next;
            }
            RBNodeType* child = NULL;
            if (target->mRightChild != NULL)
            {
                child = target->mRightChild;
            }
            else
            {
                child = target->mLeftChild;
            }
            if (target->mParent != NULL)
            {
                if (target->mParent->mLeftChild == target)
                {
                    target->mParent->mLeftChild = child;
                }
                else
                {
                    target->mParent->mRightChild = child;
                }
            }
            else
            {
                mpRoot = child;
            }
            if (child != NULL)
            {
                child->mParent = target->mParent;
            }
            if (child != NULL)
            {
                ENUM_RBTREE_DELETE_CASE deleteCase = RBTREE_DELETE_CASE_NOT_DEFINED;
                bool endLoop = false;
                RBNodeType* pNode = target;
                RBNodeType* sibling;
                while (!endLoop)
                {
                    switch (deleteCase)
                    {
                    case RBTREE_DELETE_CASE_NOT_DEFINED:
                        if (pNode->mColor == changes_meaning::RB_BLACK)
                        {
                            if (child->mColor == changes_meaning::RB_RED)
                            {
                                child->SetColorBlack();
                                endLoop = true;
                            }
                            else
                            {
                                pNode = child;
                                deleteCase = RBTREE_DELETE_CASE_CHILD_BLACK;
                            }
                        }
                        else
                        {
                            endLoop = true;
                        }
                        break;
                    case RBTREE_DELETE_CASE_CHILD_BLACK:
                        if (pNode->mParent != NULL)
                        {
                            deleteCase = RBTREE_DELETE_CASE_NOT_ROOT;
                        }
                        else
                        {
                            endLoop = true;
                        }
                        break;
                    case RBTREE_DELETE_CASE_NOT_ROOT:
                        sibling = pNode->getSibling();
                        if (sibling != NULL)
                        {
                            if (sibling->mColor == changes_meaning::RB_RED)
                            {
                                pNode->mParent->SetColorRed();
                                sibling->SetColorBlack();
                                if (pNode->mParent->mLeftChild == pNode)
                                {
                                    rotateLeft(pNode->mParent);
                                }
                                else
                                {
                                    rotateRight(pNode->mParent);
                                }
                            }
                            deleteCase = RBTREE_DELETE_CASE_SIBLING_BALCK;
                        }
                        else
                        {
                            endLoop = true;
                        }
                        break;
                    case RBTREE_DELETE_CASE_SIBLING_BALCK:
                        if (pNode->mParent->mColor == changes_meaning::RB_BLACK &&
                            sibling->mColor == changes_meaning::RB_BLACK &&
                            (sibling->mLeftChild == NULL ||
                             sibling->mLeftChild->mColor == changes_meaning::RB_BLACK) &&
                            (sibling->mRightChild == NULL ||
                             sibling->mRightChild->mColor == changes_meaning::RB_BLACK))
                        {
                            sibling->SetColorRed();
                            pNode = pNode->mParent;
                            deleteCase = RBTREE_DELETE_CASE_CHILD_BLACK;
                        }
                        else
                        {
                            deleteCase = RBTREE_DELETE_CASE_PARENT_RED;
                        }
                        break;
                    case RBTREE_DELETE_CASE_PARENT_RED:
                        if (pNode->mParent->mColor == changes_meaning::RB_RED &&
                            sibling->mColor == changes_meaning::RB_BLACK &&
                            (sibling->mLeftChild == NULL ||
                             sibling->mLeftChild->mColor == changes_meaning::RB_BLACK) &&
                            (sibling->mRightChild == NULL ||
                             sibling->mRightChild->mColor == changes_meaning::RB_BLACK))
                        {
                            sibling->SetColorRed();
                            pNode->mParent->SetColorBlack();
                        }
                        else
                        {
                            if (pNode->mParent->mLeftChild == pNode &&
                                sibling->mColor == changes_meaning::RB_BLACK &&
                                sibling->mLeftChild != NULL &&
                                sibling->mLeftChild->mColor == changes_meaning::RB_RED &&
                                (sibling->mRightChild == NULL ||
                                 sibling->mRightChild->mColor == changes_meaning::RB_BLACK))
                            {
                                sibling->SetColorRed();
                                sibling->mLeftChild->SetColorBlack();
                                rotateRight(sibling);
                            }
                            else if (pNode->mParent->mRightChild == pNode &&
                                     sibling->mColor == changes_meaning::RB_BLACK &&
                                     (sibling->mLeftChild == NULL ||
                                      sibling->mLeftChild->mColor == changes_meaning::RB_BLACK) &&
                                     sibling->mRightChild != NULL &&
                                     sibling->mRightChild->mColor == changes_meaning::RB_RED)
                            {
                                sibling->SetColorRed();
                                sibling->mRightChild->SetColorBlack();
                                rotateLeft(sibling);
                            }
                            sibling->mColor = pNode->mParent->mColor;
                            pNode->mParent->SetColorBlack();
                            if (pNode->mParent->mLeftChild == pNode)
                            {
                                if (sibling->mRightChild != NULL)
                                {
                                    sibling->mRightChild->SetColorBlack();
                                }
                                rotateLeft(pNode->mParent);
                            }
                            else
                            {
                                if (sibling->mLeftChild != NULL)
                                {
                                    sibling->mLeftChild->SetColorBlack();
                                }
                                rotateRight(pNode->mParent);
                            }
                        }
                        endLoop = true;
                        break;
                    default:
                        G_TraceLog()->sysLog(8, "Remove(), DONT_REACH_HERE");
                        break;
                    }
                }
            }
            mpRBNodePool->free(target);
            mNumOfRBNode = mNumOfRBNode - 1;
            return true;
        }
        return false;
    }

    RBNodeType* findNode(const TKey& rKey)
    {
        RBNodeType* curr = mpRoot;
        while (curr != NULL)
        {
            if (curr->mKey < rKey)
            {
                curr = curr->mRightChild;
            }
            else
            {
                if (curr->mKey == rKey)
                {
                    return curr;
                }
                curr = curr->mLeftChild;
            }
        }
        return NULL;
    }

    RBIterator<RBNodeType, TData> InorderBegin()
    {
        RBIterator<RBNodeType, TData> iter(
            RBIterator<RBNodeType, TData>::RBITERATOR_TRAVERSAL_INORDER);
        iter.begin(mpRoot);
        return iter;
    }

    bool rotateLeft(RBNodeType* pNode)
    {
        if (pNode->mRightChild != NULL)
        {
            RBNodeType* child = pNode->mRightChild;
            pNode->mRightChild = child->mLeftChild;
            if (child->mLeftChild != NULL)
            {
                child->mLeftChild->mParent = pNode;
            }
            child->mParent = pNode->mParent;
            if (pNode->mParent == NULL)
            {
                mpRoot = child;
            }
            else if (pNode->mParent->mLeftChild == pNode)
            {
                pNode->mParent->mLeftChild = child;
            }
            else if (pNode->mParent->mRightChild == pNode)
            {
                pNode->mParent->mRightChild = child;
            }
            else
            {
                G_TraceLog()->sysLog(8, "rotateLeft(), ERROR");
            }
            child->mLeftChild = pNode;
            pNode->mParent = child;
            return true;
        }
        return false;
    }

    bool rotateRight(RBNodeType* pNode)
    {
        if (pNode->mLeftChild != NULL)
        {
            RBNodeType* child = pNode->mLeftChild;
            pNode->mLeftChild = child->mRightChild;
            if (child->mRightChild != NULL)
            {
                child->mRightChild->mParent = pNode;
            }
            child->mParent = pNode->mParent;
            if (pNode->mParent == NULL)
            {
                mpRoot = child;
            }
            else if (pNode->mParent->mLeftChild == pNode)
            {
                pNode->mParent->mLeftChild = child;
            }
            else if (pNode->mParent->mRightChild == pNode)
            {
                pNode->mParent->mRightChild = child;
            }
            else
            {
                G_TraceLog()->sysLog(8, "rotateRight(), ERROR");
            }
            child->mRightChild = pNode;
            pNode->mParent = child;
            return true;
        }
        return false;
    }

    void Clear()
    {
        mpRBNodePool->purge_memory();
        mpRoot = NULL;
        mNumOfRBNode = 0;
    }

    unsigned int Size()
    {
        return mNumOfRBNode;
    }

private:
    bool mDuplicatePermit;
    boost::pool<boost::default_user_allocator_new_delete>* mpRBNodePool;
    RBNodeType* mpRoot;
    int mNumOfRBNode;
};

} // namespace nsl

#endif // NSL_RBTREE_H_
