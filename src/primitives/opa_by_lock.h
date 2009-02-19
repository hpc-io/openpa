#ifndef OPA_ATOMICS_BY_LOCK_H_INCLUDED
#define OPA_ATOMICS_BY_LOCK_H_INCLUDED

/* FIXME For now we rely on pthreads for our IPC locks.  This is fairly
   portable, although it is obviously not 100% portable.  Some day when we
   refactor the OPA_Process_locks code we should be able to use that again. */
#if defined(HAVE_PTHREAD_H)
#include <pthread.h>

/* defined in mpidu_atomic_primitives.c */
pthread_mutex_t *OPA_emulation_lock;

/*
    These macros are analogous to the MPIDU_THREAD_XXX_CS_{ENTER,EXIT} macros.
    TODO Consider putting debugging macros in here that utilize 'msg'.
*/
/* FIXME these make less sense now that OPA is not inside of MPICH2.  Is there a
   simpler name/scheme that could be used here instead? [goodell@ 2009-02-19] */
#define OPA_IPC_SINGLE_CS_ENTER(msg)          \
    do {                                        \
        OPA_assert(OPA_emulation_lock);    \
        pthread_mutex_lock(OPA_emulation_lock);     \
    } while (0)

#define OPA_IPC_SINGLE_CS_EXIT(msg)           \
    do {                                        \
        OPA_assert(OPA_emulation_lock);    \
        pthread_mutex_unlock(OPA_emulation_lock);   \
    } while (0)

typedef struct { volatile int v;  } OPA_int_t;
typedef struct { int * volatile v; } OPA_ptr_t;

/*
    Emulated atomic primitives
    --------------------------

    These are versions of the atomic primitives that emulate the proper behavior
    via the use of an inter-process lock.  For more information on their
    individual behavior, please see the comment on the corresponding top level
    function.

    In general, these emulated primitives should _not_ be used.  Most algorithms
    can be more efficiently implemented by putting most or all of the algorithm
    inside of a single critical section.  These emulated primitives exist to
    ensure that there is always a fallback if no machine-dependent version of a
    particular operation has been defined.  They also serve as a very readable
    reference for the exact semantics of our MPIDU_Atomic ops.
*/

/* We don't actually implement CAS natively, but we do support all the non-LL/SC
   primitives including CAS. */
#define MPIDU_ATOMIC_UNIVERSAL_PRIMITIVE MPIDU_ATOMIC_CAS

static inline int OPA_load(OPA_int_t *ptr)
{
    int retval;
    OPA_IPC_SINGLE_CS_ENTER("atomic_add");
    retval = ptr->v;
    OPA_IPC_SINGLE_CS_EXIT("atomic_add");
    return retval;
}

static inline void OPA_store(OPA_int_t *ptr, int val)
{
    OPA_IPC_SINGLE_CS_ENTER("atomic_add");
    ptr->v = val;
    OPA_IPC_SINGLE_CS_EXIT("atomic_add");
}

static inline void *OPA_load_ptr(OPA_ptr_t *ptr)
{
    int *retval;
    OPA_IPC_SINGLE_CS_ENTER("atomic_add");
    retval = ptr->v;
    OPA_IPC_SINGLE_CS_EXIT("atomic_add");
    return retval;
}

static inline void OPA_store_ptr(OPA_ptr_t *ptr, void *val)
{
    OPA_IPC_SINGLE_CS_ENTER("atomic_add");
    ptr->v = val;
    OPA_IPC_SINGLE_CS_EXIT("atomic_add");
}


static inline void OPA_add(OPA_int_t *ptr, int val)
{
    OPA_IPC_SINGLE_CS_ENTER("atomic_add");
    ptr->v += val;
    OPA_IPC_SINGLE_CS_EXIT("atomic_add");
}

static inline void *OPA_cas_ptr(OPA_ptr_t *ptr, int *oldv, int *newv)
{
    int *prev;
    OPA_IPC_SINGLE_CS_ENTER("atomic_cas");
    prev = ptr->v;
    if (prev == oldv) {
        ptr->v = newv;
    }
    OPA_IPC_SINGLE_CS_EXIT("atomic_cas");
    return prev;
}

static inline int OPA_cas_int(OPA_int_t *ptr, int oldv, int newv)
{
    int prev;
    OPA_IPC_SINGLE_CS_ENTER("atomic_cas");
    prev = ptr->v;
    if (prev == oldv) {
        ptr->v = newv;
    }
    OPA_IPC_SINGLE_CS_EXIT("atomic_cas");
    return prev;
}

static inline int OPA_decr_and_test(OPA_int_t *ptr)
{
    int new_val;
    OPA_IPC_SINGLE_CS_ENTER("atomic_decr_and_test");
    new_val = --(ptr->v);
    OPA_IPC_SINGLE_CS_EXIT("atomic_decr_and_test");
    return (0 == new_val);
}

static inline void OPA_decr(OPA_int_t *ptr)
{
    OPA_IPC_SINGLE_CS_ENTER("atomic_decr");
    --(ptr->v);
    OPA_IPC_SINGLE_CS_EXIT("atomic_decr");
}

static inline int OPA_fetch_and_add(OPA_int_t *ptr, int val)
{
    int prev;
    OPA_IPC_SINGLE_CS_ENTER("atomic_fetch_and_add");
    prev = ptr->v;
    ptr->v += val;
    OPA_IPC_SINGLE_CS_EXIT("atomic_fetch_and_add");
    return prev;
}

static inline int OPA_fetch_and_decr(OPA_int_t *ptr)
{
    int prev;
    OPA_IPC_SINGLE_CS_ENTER("atomic_fetch_and_decr");
    prev = ptr->v;
    --(ptr->v);
    OPA_IPC_SINGLE_CS_EXIT("atomic_fetch_and_decr");
    return prev;
}

static inline int OPA_fetch_and_incr(OPA_int_t *ptr)
{
    int prev;
    OPA_IPC_SINGLE_CS_ENTER("atomic_fetch_and_incr");
    prev = ptr->v;
    ++(ptr->v);
    OPA_IPC_SINGLE_CS_EXIT("atomic_fetch_and_incr");
    return prev;
}

static inline void OPA_incr(OPA_int_t *ptr)
{
    OPA_IPC_SINGLE_CS_ENTER("atomic_incr");
    ++(ptr->v);
    OPA_IPC_SINGLE_CS_EXIT("atomic_incr");
}

static inline void *OPA_swap_ptr(OPA_ptr_t *ptr, void *val)
{
    int *prev;
    OPA_IPC_SINGLE_CS_ENTER("atomic_swap_ptr");
    prev = ptr->v;
    ptr->v = val;
    OPA_IPC_SINGLE_CS_EXIT("atomic_swap_ptr");
    return prev;
}

static inline int OPA_swap_int(OPA_int_t *ptr, int val)
{
    int prev;
    OPA_IPC_SINGLE_CS_ENTER("atomic_swap_int");
    prev = ptr->v;
    ptr->v = val;
    OPA_IPC_SINGLE_CS_EXIT("atomic_swap_int");
    return (int)prev;
}

#endif /* defined(HAVE_PTHREAD_H) */
#endif /* !defined(OPA_BY_LOCK_H_INCLUDED) */
