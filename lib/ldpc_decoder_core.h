#ifndef LDPC_DECODER_H
#define LDPC_DECODER_H

#include <vector>
#include "ldpc_matrix_utils.h"

class ldpc_decoder_core
{
protected:
    ldpc_parity_check_matrix* H;

    std::vector<float> tanh_half_u;

    std::vector< std::vector<float> > dcq;
    std::vector< std::vector<float> > dpr;

    std::vector<unsigned char> chat;
    std::vector<float> P, eP;
    std::vector<float> ppr;

    int num_cols, num_rows;

    bool d_was_decoded;

    int num_decoded, d_L;

public:
    ldpc_decoder_core(const char *pcmat_filename);

    ~ldpc_decoder_core();

    /*!
     * \brief Return posterior estimate of codeword c
     * \param codeword Vector of received codeword LLRs
     * \param num_iterations Number of iterations
     */
    virtual std::vector<unsigned char> decode_ldpc(std::vector<float> codeword, int num_iterations);

    void decode_ldpc(std::vector<float> codeword, int num_iterations, unsigned char* chat);
    /*!
     * \brief Return posterior estimate of codeword c
     */
    //std::vector<unsigned char> get_chat() { return chat; };

    /*!
     * \brief Return posterior likelihoods of codeword c
     */
    std::vector<float> get_posteriors() { return ppr; };

    bool was_decoded() { return d_was_decoded; };

    void print_matrix() { H->print_matrix(); };

    int get_codeword_length() { return num_cols; };
    int get_number_of_checks() { return num_rows; };

private:
    void initialize_msg();
    void horizontal_step();
    void vertical_step(unsigned char *chat);
};
#endif
