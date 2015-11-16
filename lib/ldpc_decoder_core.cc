#include "ldpc_decoder_core.h"
#include <gnuradio/math.h>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

ldpc_decoder_core::ldpc_decoder_core( const char* pcmat_filename )
{
    H = new ldpc_parity_check_matrix(pcmat_filename);
    num_cols = H->num_cols();
    num_rows = H->num_rows();

    dcq = vector< vector<float> > (num_cols, vector<float> (num_rows));
    dpr = vector< vector<float> > (num_cols, vector<float> (num_rows));

    P = vector<float> (num_cols);
    eP = vector<float> (num_cols);

    d_was_decoded = false;

    num_decoded = 1;

}

ldpc_decoder_core::~ldpc_decoder_core()
{
    delete H;
}

vector<unsigned char>
ldpc_decoder_core::decode_ldpc(vector<float> codeword, int num_iterations)
{
    std::vector<unsigned char> chat;
    decode_ldpc(codeword, num_iterations, chat.data());
    return chat;
}

void
ldpc_decoder_core::initialize_msg()
{
    for(int i = 0; i < num_cols; i++)
    {
        //eP[i] = 1/(exp(P[i]) + 1);
        for(int a = 0; a < H->get_number_of_indices_in_col(i); a++)
            dcq[i][H->get_indices_for_col(i)[a]] = P[i];
    }
}
void
ldpc_decoder_core::horizontal_step()
{
    for(int j=0; j < num_rows; j++)
    {
        float _tanh_half_u = 1;
        const std::vector<int> & indices = H->get_indices_for_row(j);

        for(std::vector<int>::const_iterator it = indices.begin(); it != indices.end(); ++it)
            _tanh_half_u = _tanh_half_u * tanh(dcq[*it][j]/2.0);

        for(std::vector<int>::const_iterator it = indices.begin(); it != indices.end(); ++it)
        {
            float temp = _tanh_half_u / tanh(dcq[*it][j]/2.0);
            dpr[*it][j] = 2 * atanh(temp);
        }
    }
}

void
ldpc_decoder_core::vertical_step(unsigned char* chat)
{
    for(int i=0; i < num_cols; i++)
    {
        float _ppr = P[i];
        const std::vector<int> & indices = H->get_indices_for_col(i);
        for(std::vector<int>::const_iterator it = indices.begin(); it != indices.end(); ++it)
            _ppr += dpr[i][*it];

        chat[i] = (_ppr < 0.0) ? 1 : 0;

        for(std::vector<int>::const_iterator it = indices.begin(); it != indices.end(); ++it)
            dcq[i][*it] = _ppr - dpr[i][*it];
    }
}

void
ldpc_decoder_core::decode_ldpc(vector<float> codeword, int num_iterations, unsigned char* chat)
{
    P = codeword;
    /*this takes a vector of LLRs as the argument codeword and performs the BP LDPC decoder algorithm
    for num_iterations*/
    d_was_decoded = false;

    initialize_msg();
    //main decoder loop
    for(int loop = 0; loop < num_iterations; loop++)
    {
        horizontal_step();
        vertical_step(chat);

        if(d_was_decoded)
            return;
        //---------------------------------------------------------------------------------------
        if(H->parity_check(chat) == 0)
        {
            //fprintf(stdout, "Solved LDPC packet %d in %d iterations (%d 1-bits).\n", num_decoded, loop + 1, cwtot);
            //fflush(stdout);
            num_decoded++;
            d_was_decoded = true;
            //TODO: need to output posterior probs
            if(loop == num_iterations - 1)
                return;
        }

    }
    //fprintf(stdout, "Failed decoding LDPC packet %d\n", num_decoded);
    //fflush(stdout);
    d_was_decoded = false;

}

