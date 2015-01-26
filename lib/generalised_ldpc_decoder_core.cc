#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "generalised_ldpc_decoder_core.h"

using namespace std;

generalised_ldpc_decoder_core::generalised_ldpc_decoder_core(const char* pcmat_filename, int chan_length) :
    ldpc_decoder_core(pcmat_filename)
{
    dprn = vector< vector< vector<float> > > (num_cols, vector< vector<float> > (num_cols, vector<float> (num_rows, 0)));

    d_L = chan_length;

    pprn00 = vector< vector<float> > (num_cols, vector<float> (num_cols, 1));
    pprn10 = vector< vector<float> > (num_cols, vector<float> (num_cols, 1));
    pprn01 = vector< vector<float> > (num_cols, vector<float> (num_cols, 1));
    pprn11 = vector< vector<float> > (num_cols, vector<float> (num_cols, 1));
}


vector<unsigned char>
generalised_ldpc_decoder_core::decode_ldpc(vector<float> codeword, int num_iterations)
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

                if(abs(temp) > 0.99999)
                {
                    if(signbit(temp)) temp = -0.99999; else temp = 0.99999;
                }
                if(temp == 0)
                {
                    temp = 1e-10;
                    fprintf(stderr, "Warning, zero dcq, substituted\n");
                }

                dpr[H->get_indices_for_row(j)[a]][j] = 2 * atanh(temp);

                //--------------------PAIRWISE STUFF------------------------------------------------
                if(loop == (num_iterations - 1) || d_was_decoded)
                {
                    for(int b = 0; b < H->get_number_of_indices_in_row(j); b++)
                    {
                        //fprintf(stderr, "1 - %d\n", a);
                        float num;
                        if(a != b)
                        {
                            num = tanh(dcq[H->get_indices_for_row(j)[a]][j]/2.0) *
                                tanh(dcq[H->get_indices_for_row(j)[b]][j]/2.0);
                        }else{
                            num = tanh(dcq[H->get_indices_for_row(j)[a]][j]/2.0);
                        }
                        //fprintf(stderr, "2 - %d\n", b);
                        //if(isnan(num)) fprintf(stderr, "nan = num dcq = %1.2f\n", dcq[H->get_indices_for_row(j)[a]][j]);

                        num = tanh_half_u[j] / num;

                        if(abs(num) > 0.99999)
                        {
                            if(signbit(num)) num = -0.99999; else num = 0.99999;
                        }

                        dprn[H->get_indices_for_row(j)[a]][H->get_indices_for_row(j)[b]][j] = 2 * atanh(num);
                    }
                }
                //-----------------END PAIRWISE STUFF
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

        /*for(int i = 0; i < num_cols; i++)
        {
            fprintf(stderr, "%f ", ppr[i]);
        }
        fprintf(stderr, "\n");*/

        for(int i=0; i < num_cols; i++)
            for(int a = 0; a < H->get_number_of_indices_in_col(i); a++)
                dcq[i][H->get_indices_for_col(i)[a]] = ppr[i] - dpr[i][H->get_indices_for_col(i)[a]];

        if(loop == (num_iterations - 1) || d_was_decoded)
        {
            //fprintf(stderr, "pprns\n");
            for(int i1 = 0; i1 < num_cols; i1++)
            {
                for(int i2 = 0; i2 < num_cols; i2++)
                {
                    if(abs(i1-i2) < d_L)
                    {
                        float pp00 = (1 - eP[i1]) * (1 - eP[i2]);
                        float pp01 = (1 - eP[i1]) * eP[i2];
                        float pp10 = eP[i1] * (1 - eP[i2]);
                        float pp11 = eP[i1] * eP[i2];
                        //fprintf(stderr, "go!\n");
                        vector<int> uni = H->get_indices_for_2_cols(i1, i2);
                        //fprintf(stderr, "come back!\n");
                        pprn00[i1][i2] = 1;
                        pprn01[i1][i2] = 1;
                        pprn10[i1][i2] = 1;
                        pprn11[i1][i2] = 1;

                        for(int j = 0; j < (int)uni.size(); j++)
                        {
                            //fprintf(stderr, "uni[%d] = %i\n", j, uni[j]);

                            if(dprn[i1][i2][uni[j]] != 0)
                            {
                                if(i1 != i2)
                                {
                                    float temp = exp(dprn[i1][i2][uni[j]]);
                                    //if(dprn[i1][i2][uni[j]]) fprintf(stderr, "nan = dprn\n");
                                    //if(isnan(pprn00[i1][i2])) fprintf(stderr, "nan = temp\n");

                                    if(temp < 0) fprintf(stderr, "emergency\n");

                                    pprn00[i1][i2] = pprn00[i1][i2] * (1 - 1/(temp + 1));
                                    pprn01[i1][i2] = pprn01[i1][i2] * (1/(temp + 1));
                                    pprn10[i1][i2] = pprn10[i1][i2] * (1/(temp + 1));
                                    pprn11[i1][i2] = pprn11[i1][i2] * (1 - 1/(temp + 1));
                                }else{
                                    pprn01[i1][i2] = 0;
                                    pprn10[i1][i2] = 0;
                                    pprn11[i1][i2] = 1/(exp(ppr[i1]) + 1);
                                    pprn00[i1][i2] = 1 - pprn11[i1][i2];
                                }
                            }

                            if(i1 != i2)
                            {
                                pprn00[i1][i2] = pprn00[i1][i2] * pp00;
                                pprn10[i1][i2] = pprn10[i1][i2] * pp10;
                                pprn01[i1][i2] = pprn01[i1][i2] * pp01;
                                pprn11[i1][i2] = pprn11[i1][i2] * pp11;
                            }

                        }

                    }
                }
            }
            //fprintf(stderr, "normalise\n");
            for(int i1 = 0; i1 < num_cols; i1++)
            {
                for(int i2 = 0; i2 < num_cols; i2++)
                {
                    if(abs(i1-i2) < d_L)
                    {

                        float alpha = 1 / (pprn00[i1][i2] + pprn01[i1][i2] + pprn10[i1][i2] + pprn11[i1][i2]);
                        /*if(isnan(alpha))
                        {
                            //fprintf(stderr, "nan found on alpha\n");
                            pprn00[i1][i2] = 0.25;
                            pprn10[i1][i2] = 0.25;
                            pprn01[i1][i2] = 0.25;
                            pprn11[i1][i2] = 0.25;
                        }*/

                        pprn00[i1][i2] = pprn00[i1][i2] * alpha;
                        pprn10[i1][i2] = pprn10[i1][i2] * alpha;
                        pprn01[i1][i2] = pprn01[i1][i2] * alpha;
                        pprn11[i1][i2] = pprn11[i1][i2] * alpha;
                        /*if(isnan(pprn00[i1][i2])) fprintf(stderr, "nan = pprn00\n");
                        if(isnan(pprn10[i1][i2])) fprintf(stderr, "nan = pprn10\n");
                        if(isnan(pprn01[i1][i2])) fprintf(stderr, "nan = pprn01\n");
                        if(isnan(pprn11[i1][i2])) fprintf(stderr, "nan = pprn11\n");*/

                    }
                }
            }
        }
        if(d_was_decoded)
            return chat;
        //---------------------------------------------------------------------------------------
        if(H->parity_check(chat) == 0)
        {

            fprintf(stderr, "Solved LDPC packet %d in %d iterations (%d 1-bits).\n", num_decoded, loop + 1, cwtot);
            num_decoded++;
            d_was_decoded = true;
            //TODO: need to output posterior probs
            if(loop == num_iterations - 1)
                return chat;
        }

    }
    fprintf(stderr, "Failed decoding LDPC packet %d\n", num_decoded);
    d_was_decoded = false;
    return chat; //best guess

}
