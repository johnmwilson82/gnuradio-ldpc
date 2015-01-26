//ldpc_randint.h
//defines simple random int class

#ifndef _LDPC_RANDINT_H
#define _LDPC_RANDINT_H

class randint
{
    int d_lastint;

   public:
    randint(int seed) {d_lastint = seed;};
    void reseed(int seed) {d_lastint = seed;};
    int gen() { return(((d_lastint = d_lastint * 214013L + 2531011L) >> 16) & 0x7fff); };
};

#endif
