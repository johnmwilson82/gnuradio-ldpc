#ifndef GENERALISED_LDPC_DECODER_H
#define GENERALISED_LDPC_DECODER_H

#include <vector>
#include "ldpc_decoder_core.h"

/*
 * A generalised LDPC decoder which outputs pairwise probabilities
 * Blind turbo equalization using a generalized LDPC decoder
 * Gunther, J. ; Ankapura, M. ; Moon, T.
 */
class generalised_ldpc_decoder_core : public ldpc_decoder_core
{
private:
    std::vector< std::vector<float> > pprn01;
    std::vector< std::vector<float> > pprn00;
    std::vector< std::vector<float> > pprn10;
    std::vector< std::vector<float> > pprn11;

    std::vector< std::vector< std::vector<float> > > dprn;

public:
    generalised_ldpc_decoder_core(const char *pcmat_filename, int num_iterations);
    ~generalised_ldpc_decoder_core();

    virtual std::vector<unsigned char> decode_ldpc(std::vector<float> codeword, int num_iterations);

    std::vector< std::vector<float> > get_pw_posteriors00() { return pprn00; };
    std::vector< std::vector<float> > get_pw_posteriors10() { return pprn10; };
    std::vector< std::vector<float> > get_pw_posteriors01() { return pprn01; };
    std::vector< std::vector<float> > get_pw_posteriors11() { return pprn11; };
};

#endif
