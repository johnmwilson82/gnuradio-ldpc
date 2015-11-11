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

    chat = vector<unsigned char> (num_cols);
    P = vector<float> (num_cols);
    eP = vector<float> (num_cols);
    ppr = vector<float> (num_cols);

    tanh_half_u = vector<float> (num_rows);
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
ldpc_decoder_core::decode_ldpc(vector<float> codeword, int num_iterations, unsigned char* chat)
{
    /*this takes a vector of LLRs as the argument codeword and performs the BP LDPC decoder algorithm
    for num_iterations*/
    P = codeword;
    d_was_decoded = false;

    //initialise q messages
    for(int i = 0; i < num_cols; i++)
    {
        eP[i] = 1/(exp(P[i]) + 1);
        for(int a = 0; a < H->get_number_of_indices_in_col(i); a++)
            dcq[i][H->get_indices_for_col(i)[a]] = P[i];
    }

    //main decoder loop
    for(int loop = 0; loop < num_iterations; loop++)
    {
        //horizontal step
        //calculate r messages
        for(int j=0; j < num_rows; j++)
        {
            tanh_half_u[j] = 1;
            for(int i = 0; i < H->get_number_of_indices_in_row(j); i++)
                tanh_half_u[j] = tanh_half_u[j] * tanh(dcq[H->get_indices_for_row(j)[i]][j]/2.0);
        }

        for(int j=0; j < num_rows; j++)
        {
            for(int a = 0; a < H->get_number_of_indices_in_row(j); a++)
            {
                float temp = tanh_half_u[j] / tanh(dcq[H->get_indices_for_row(j)[a]][j]/2.0);

                temp = gr::branchless_clip(temp, 0.99999);

                if(temp == 0)
                {
                    temp = 1e-10;
                    fprintf(stdout, "Warning, zero dcq, substituted\n");
                }

                dpr[H->get_indices_for_row(j)[a]][j] = 2 * atanh(temp);
            }
        }

        ppr = P;

        int cwtot = 0;
        for(int i=0; i < num_cols; i++)
        {
            for(int j = 0; j < H->get_number_of_indices_in_col(i); j++)
                ppr[i] += dpr[i][H->get_indices_for_col(i)[j]];

            if(ppr[i] < 0) chat[i] = 1; else chat[i] = 0;
            cwtot += chat[i];
        }

        for(int i=0; i < num_cols; i++)
            for(int a = 0; a < H->get_number_of_indices_in_col(i); a++)
                dcq[i][H->get_indices_for_col(i)[a]] = ppr[i] - dpr[i][H->get_indices_for_col(i)[a]];

        if(d_was_decoded)
            return;
        //---------------------------------------------------------------------------------------
        if(H->parity_check(chat) == 0)
        {

            fprintf(stdout, "Solved LDPC packet %d in %d iterations (%d 1-bits).\n", num_decoded, loop + 1, cwtot);
            fflush(stdout);
            num_decoded++;
            d_was_decoded = true;
            //TODO: need to output posterior probs
            if(loop == num_iterations - 1)
                return;
        }

    }
    fprintf(stdout, "Failed decoding LDPC packet %d\n", num_decoded);
    fflush(stdout);
    d_was_decoded = false;

}

