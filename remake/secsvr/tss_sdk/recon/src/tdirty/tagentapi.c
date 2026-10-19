/* Reconstructed from libtss_sdk.so.1.0.394 (lib_src/taa/tagentapi.c).
   SysV shm + semaphore based agent-API.  Shared memory id 0xffee0001,
   size 0x100000, blocks of 0x1000; header: size, blocksize, ctime, magic
   0x8899, count, then a 128 byte "Create by TAGENT_API" tag.  Each block
   holds {u32 id, u32 busid} at offset 0 and a 0x40 byte header. */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define TAA_SHM_KEY 0xffee0001
#define TAA_SHM_SIZE 0x100000
#define TAA_BLOCK_SIZE 0x1000
#define TAA_MAGIC 0x8899
#define TAA_MAX_BLOCKS 0xff

typedef struct tagTAA {
    void *pShm;
    void *pSem;
    void *pCur;
} tagTAA;

typedef struct tagTAABlock {
    unsigned int uiId;
    unsigned int uiBusId;
} tagTAABlock;

typedef struct tagTAASem {
    int iSemId;
} tagTAASem;

typedef struct tagTAAShmHead {
    unsigned int uiSize;
    unsigned int uiBlockSize;
    unsigned int uiTime;
    unsigned int uiMagic;
    unsigned int uiCount;
    char szName[128];
} tagTAAShmHead;

union semun_compat {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#define TAA_NAME_LITERAL "Create by TAGENT_API"

void __attribute__((regparm(3)))
agent_api_getid(tagTAABlock *pBlock, unsigned int *puiId,
                unsigned int *puiBusId) {
    *puiId = pBlock->uiId;
    *puiBusId = pBlock->uiBusId;
}

int semreset(void *pSem) {
    return 0;
}

tagTAASem *semcreate(int iKey, int iInitVal) {
    tagTAASem *pSem = (tagTAASem *)malloc(4);
    if (pSem == 0)
        return 0;

    pSem->iSemId = semget(iKey, 1, 0x1b6);
    if (pSem->iSemId < 0) {
        pSem->iSemId = semget(iKey, 1, 0x7b6);
        if (pSem->iSemId < 0)
            return 0;
    }

    union semun_compat su;
    su.val = iInitVal;
    if (semctl(pSem->iSemId, 0, 0x10 /* SETVAL */, su) < 0)
        return 0;
    return pSem;
}

void semdestroy(void *pHandle) {
    tagTAASem *pSem = (tagTAASem *)pHandle;
    if (pSem == 0)
        return;
    semctl(pSem->iSemId, 0, 0 /* IPC_RMID */, 0);
    free(pSem);
}

int semrelease(void *pHandle) {
    tagTAASem *pSem = (tagTAASem *)pHandle;
    struct sembuf op;
    if (pSem == 0)
        return 0;
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0x1000;
    if (semop(pSem->iSemId, &op, 1) < 0)
        return -1;
    return 0;
}

int semacquire(void *pHandle) {
    tagTAASem *pSem = (tagTAASem *)pHandle;
    struct sembuf op;
    if (pSem == 0)
        return 0;
    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0x1000;
    if (semop(pSem->iSemId, &op, 1) < 0)
        return -1;
    return 0;
}

void *agent_api_get_freebufptr(void *pHandle, unsigned int uiId,
                               unsigned int uiBusId) {
    tagTAA *pTA = (tagTAA *)pHandle;
    tagTAAShmHead *pHead;
    unsigned int uiBlockId;
    unsigned int uiBlockBusId;
    unsigned int i;

    if (pTA == 0)
        return 0;
    if (pTA->pShm == 0)
        return 0;

    pHead = (tagTAAShmHead *)pTA->pCur;
    if (pHead == 0)
        return 0;

    semacquire(pTA->pSem);

    if ((int)pHead->uiCount <= 0) {
        semrelease(pTA->pSem);
        return 0;
    }

    for (i = 0; i < pHead->uiCount; i++) {
        tagTAABlock *pBlock = (tagTAABlock *)((char *)pTA->pShm +
                                              TAA_BLOCK_SIZE +
                                              i * TAA_BLOCK_SIZE);
        agent_api_getid(pBlock, &uiBlockId, &uiBlockBusId);
        if (uiBlockId == uiId && uiBlockBusId == uiBusId) {
            semrelease(pTA->pSem);
            return (char *)pBlock + 0x40;
        }
    }

    semrelease(pTA->pSem);
    return 0;
}

int agent_api_get_blocks(void *pHandle, void **pBlocks,
                         unsigned int *puiCount) {
    tagTAA *pTA = (tagTAA *)pHandle;
    tagTAAShmHead *pHead;
    unsigned int i;

    if (pTA == 0)
        return -1;
    if (pTA->pShm == 0)
        return -1;

    pHead = (tagTAAShmHead *)pTA->pCur;
    if (pHead == 0)
        return -1;

    if (*puiCount < pHead->uiCount)
        return -1;
    *puiCount = pHead->uiCount;

    semacquire(pTA->pSem);
    for (i = 0; i < pHead->uiCount; i++)
        pBlocks[i] = (char *)pTA->pShm + (i + 1) * TAA_BLOCK_SIZE;
    semrelease(pTA->pSem);
    return 0;
}

int agent_api_get_bussid(void *pHandle, unsigned int uiId,
                         unsigned int *puiBusIds, unsigned int *puiCount) {
    tagTAA *pTA = (tagTAA *)pHandle;
    tagTAAShmHead *pHead;
    unsigned int uiBlockId;
    unsigned int uiBlockBusId;
    unsigned int i;

    if (pTA == 0)
        return -1;
    if (pTA->pShm == 0)
        return -1;

    pHead = (tagTAAShmHead *)pTA->pCur;
    if (pHead == 0)
        return -1;

    if (*puiCount < pHead->uiCount)
        return -1;
    *puiCount = 0;

    semacquire(pTA->pSem);

    for (i = 0; i < pHead->uiCount; i++) {
        tagTAABlock *pBlock = (tagTAABlock *)((char *)pTA->pShm +
                                              TAA_BLOCK_SIZE +
                                              i * TAA_BLOCK_SIZE);
        agent_api_getid(pBlock, &uiBlockId, &uiBlockBusId);
        if (uiBlockId == uiId) {
            puiBusIds[*puiCount] = uiBlockBusId;
            *puiCount += 1;
        }
    }

    semrelease(pTA->pSem);
    return 0;
}

int agent_api_unregister(void *pHandle, unsigned int uiId,
                         unsigned int uiBusId) {
    tagTAA *pTA = (tagTAA *)pHandle;
    tagTAAShmHead *pHead;
    unsigned int uiBlockId;
    unsigned int uiBlockBusId;
    unsigned int i;

    if (pTA == 0)
        return -1;
    if (pTA->pShm == 0)
        return -1;

    pHead = (tagTAAShmHead *)pTA->pCur;
    if (pHead == 0)
        return -1;

    semacquire(pTA->pSem);

    for (i = 0; i < pHead->uiCount; i++) {
        tagTAABlock *pBlock = (tagTAABlock *)((char *)pTA->pShm +
                                              TAA_BLOCK_SIZE +
                                              i * TAA_BLOCK_SIZE);
        agent_api_getid(pBlock, &uiBlockId, &uiBlockBusId);
        if (uiBlockId == uiId && uiBlockBusId == uiBusId) {
            if (i != pHead->uiCount - 1) {
                tagTAABlock *pLast =
                    (tagTAABlock *)((char *)pTA->pShm + TAA_BLOCK_SIZE +
                                    (pHead->uiCount - 1) * TAA_BLOCK_SIZE);
                memcpy(pBlock, pLast, TAA_BLOCK_SIZE);
            }
            pHead->uiCount -= 1;
            break;
        }
    }

    semrelease(pTA->pSem);
    return 0;
}

int agent_api_register(void *pHandle, unsigned int uiId,
                       unsigned int uiBusId) {
    tagTAA *pTA = (tagTAA *)pHandle;
    tagTAAShmHead *pHead;
    unsigned int uiBlockId;
    unsigned int uiBlockBusId;
    unsigned int i;

    if (pTA == 0)
        return -1;
    if (pTA->pShm == 0)
        return -1;

    pHead = (tagTAAShmHead *)pTA->pCur;
    if (pHead == 0)
        return -1;

    semacquire(pTA->pSem);

    for (i = 0; i < pHead->uiCount; i++) {
        tagTAABlock *pBlock = (tagTAABlock *)((char *)pTA->pShm +
                                              TAA_BLOCK_SIZE +
                                              i * TAA_BLOCK_SIZE);
        agent_api_getid(pBlock, &uiBlockId, &uiBlockBusId);
        if (uiBlockId == uiId && uiBlockBusId == uiBusId)
            break;
    }

    if ((int)pHead->uiCount > TAA_MAX_BLOCKS - 1) {
        semrelease(pTA->pSem);
        return -2;
    }

    if (i == pHead->uiCount) {
        tagTAABlock *pBlock = (tagTAABlock *)((char *)pTA->pShm +
                                              TAA_BLOCK_SIZE +
                                              i * TAA_BLOCK_SIZE);
        memset(pBlock, 0, TAA_BLOCK_SIZE);
        pBlock->uiId = uiId;
        pBlock->uiBusId = uiBusId;
        pHead->uiCount += 1;
    }

    semrelease(pTA->pSem);
    return 0;
}

void agent_api_destroy(void *pHandle) {
    tagTAA *pTA = (tagTAA *)pHandle;
    if (pTA == 0)
        return;
    if (pTA->pShm != 0) {
        shmdt(pTA->pShm);
        pTA->pShm = 0;
    }
    if (pTA->pSem != 0) {
        semdestroy(pTA->pSem);
        pTA->pSem = 0;
    }
    free(pTA);
}

int agent_api_init(void **ppHandle) {
    tagTAA *pTA;
    tagTAAShmHead *pHead;
    int iShmId;
    void *pShm;

    pTA = (tagTAA *)malloc(12);
    if (pTA == 0)
        return -1;

    pTA->pShm = 0;
    pTA->pSem = 0;
    pTA->pCur = 0;

    pTA->pSem = semcreate(TAA_SHM_KEY, 1);
    if (pTA->pSem == 0) {
        free(pTA);
        return -1;
    }

    iShmId = shmget(TAA_SHM_KEY, TAA_SHM_SIZE, 0x5b6);
    if (iShmId < 0) {
        if (errno != ENOENT)
            goto fail_sem;

        iShmId = shmget(TAA_SHM_KEY, TAA_SHM_SIZE, 0x7b6);
        if (iShmId < 0) {
            if (errno != EEXIST)
                goto fail_sem;

            iShmId = shmget(TAA_SHM_KEY, 0, 0x1b6);
            if (iShmId < 0)
                goto fail_sem;
            if (shmctl(iShmId, IPC_RMID, 0) != 0)
                goto fail_sem;

            iShmId = shmget(TAA_SHM_KEY, TAA_SHM_SIZE, 0x7b6);
            if (iShmId < 0)
                goto fail_sem;
        }
    }

    pShm = shmat(iShmId, 0, 0);
    if (pShm == (void *)-1) {
        pTA->pShm = 0;
        goto fail_sem;
    }

    pTA->pShm = pShm;
    if (pShm == 0)
        goto fail_sem;

    pTA->pCur = pShm;

    semacquire(pTA->pSem);

    pHead = (tagTAAShmHead *)pTA->pCur;
    if (pHead->uiMagic != TAA_MAGIC) {
        pHead->uiSize = TAA_SHM_SIZE;
        pHead->uiBlockSize = TAA_BLOCK_SIZE;
        pHead->uiTime = (unsigned int)time(0);
        pHead->uiMagic = TAA_MAGIC;
        pHead->uiCount = 0;
    }

    strncpy(pHead->szName, TAA_NAME_LITERAL, 0x80);
    semrelease(pTA->pSem);

    *ppHandle = pTA;
    return 0;

fail_sem:
    if (pTA->pSem != 0)
        semdestroy(pTA->pSem);
    free(pTA);
    return -1;
}

#if defined(__cplusplus)
}
#endif
