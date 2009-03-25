/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef MPIDU_ATOMIC_EMULATED_H_INCLUDED
#define MPIDU_ATOMIC_EMULATED_H_INCLUDED

/* Functions emulated using other atomics

   This header should be included at the bottom of any atomic
   primitives header that needs to implement an atomic op in terms of
   another atomic.
*/

static inline int OPA_fetch_and_add_by_cas(OPA_int_t *ptr, int val)
{
    int cmp;
    int prev = OPA_load(ptr);

    do {
        cmp = prev;
        prev = OPA_cas_int(ptr, cmp, prev + val);
    } while (prev != cmp);

    return prev;
}

static inline int OPA_fetch_and_incr_by_faa(OPA_int_t *ptr)
{
    return OPA_fetch_and_add(ptr, 1);
}

static inline int OPA_fetch_and_decr_by_faa(OPA_int_t *ptr)
{
    return OPA_fetch_and_add(ptr, -1);
}

static inline int OPA_decr_and_test_by_fad(OPA_int_t *ptr)
{
    return OPA_fetch_and_decr(ptr) == 1;
}

static inline void OPA_add_by_faa(OPA_int_t *ptr, int val)
{
    OPA_fetch_and_add(ptr, val);
}

static inline void OPA_incr_by_add(OPA_int_t *ptr)
{
    OPA_add(ptr, 1);
}

static inline void OPA_incr_by_fai(OPA_int_t *ptr)
{
    OPA_fetch_and_incr(ptr);
}

static inline void OPA_decr_by_add(OPA_int_t *ptr)
{
    OPA_add(ptr, -1);
}

static inline void OPA_decr_by_fad(OPA_int_t *ptr)
{
    OPA_fetch_and_decr(ptr);
}


/* Swap using CAS */

static inline int *OPA_swap_ptr_by_cas(OPA_ptr_t *ptr, int *val)
{
    int *cmp;
    int *prev = OPA_load_ptr(ptr);

    do {
        cmp = prev;
        prev = OPA_cas_ptr(ptr, cmp, val);
    } while (prev != cmp);

    return prev;
}

static inline int OPA_swap_int_by_cas(OPA_int_t *ptr, int val)
{
    int cmp;
    int prev = OPA_load(ptr);

    do {
        cmp = prev;
        prev = OPA_cas_int(ptr, cmp, val);
    } while (prev != cmp);

    return prev;
}


/* Emulating using LL/SC */

#ifdef ATOMIC_LL_SC_SUPPORTED
static inline int OPA_fetch_and_add_by_llsc(OPA_int_t *ptr, int val)
{
    int prev;
    do {
        prev = OPA_LL_int(ptr);
    } while (!OPA_SC_int(ptr, prev + val));
    return prev;
}


static inline void OPA_add_by_llsc(int *ptr, int val)
{
    OPA_fetch_and_add_by_llsc(ptr, val);
}

static inline void OPA_incr_by_llsc(OPA_int_t *ptr)
{
    OPA_add_by_llsc(ptr, 1);
}

static inline void OPA_decr_by_llsc(OPA_int_t *ptr)
{
    OPA_add_by_llsc(ptr, -1);
}


static inline int OPA_fetch_and_decr_by_llsc(OPA_int_t *ptr)
{
    return OPA_fetch_and_add_by_llsc(ptr, -1);
}

static inline int OPA_fetch_and_incr_by_llsc(OPA_int_t *ptr)
{
    return OPA_fetch_and_add_by_llsc(ptr, 1);
}

static inline int OPA_decr_and_test_by_llsc(OPA_int_t *ptr)
{
    int prev = OPA_fetch_and_decr_by_llsc(ptr);
    return prev == 1;
}

static inline int *OPA_cas_ptr_by_llsc(OPA_ptr_t *ptr, int *oldv, int *newv)
{
    int *prev;
    do {
        prev = OPA_LL_ptr(ptr);
    } while (prev == oldv && !OPA_SC_ptr(ptr, newv));
    return prev;
}

static inline int OPA_cas_int_by_llsc(OPA_int_t *ptr, int oldv, int newv)
{
    int prev;
    do {
        prev = OPA_LL_int(ptr);
    } while (prev == oldv && !OPA_SC_int(ptr, newv));
    return prev;
}


static inline int *OPA_swap_ptr_by_llsc(int * volatile *ptr, int *val)
{
    int *prev;
    do {
        prev = OPA_LL_ptr(ptr);
    } while (!OPA_SC_ptr(ptr, val));
    return prev;
}

static inline int OPA_swap_int_by_llsc(OPA_int_t *ptr, int val)
{
    int prev;
    do {
        prev = OPA_LL_int(ptr);
    } while (!OPA_SC_int(ptr, val));
    return prev;
}

#endif /* ATOMIC_LL_SC_SUPPORTED */


#endif /* MPIDU_ATOMIC_EMULATED_H_INCLUDED */
