#include <iostream>
#include <fstream>
#include <stdexcept>
#include "ldpc_matrix_utils.h"
#include <cstring>
using namespace std;

ldpc_p_matrix::ldpc_p_matrix(const char *filename)
{
    /* filename must point to a file of M+1 lines, the first of which is the number of rows in P, then each successive
    line will have the number of indices in that check first followed by the indices themselves.*/
    int num_rows;
    vector<int> row;
    int num_indices;
    int index;

    ifstream pc_file (filename); //open file given by filename
    if (pc_file.fail()) throw std::runtime_error ("ldpc_p_matrix::can't open file");

    pc_file >> num_rows >> d_num_cols;

    assert(num_rows > 0);
    for(int i=0; i < num_rows; i++)
    {

        pc_file >> num_indices; //first number is number of indices
        row.clear();

        for(int j=0; j<num_indices; j++)
        {
            pc_file >> index;
            row.push_back(index - 1);
        }

        d_checks.push_back(row);
    }
}

ldpc_p_matrix::~ldpc_p_matrix()
{

}

vector<unsigned char> ldpc_p_matrix::encode_ldpc(vector<unsigned char> message)
{
    vector<unsigned char> codeword(d_checks.size()+message.size());
    encode_ldpc(message, codeword.data());
    return codeword;
}

void
ldpc_p_matrix::encode_ldpc(vector<unsigned char> message, unsigned char *codeword)
{
    int temp;
    memcpy(codeword + d_checks.size(), message.data(), message.size());
    for(int i = 0; i < (int)d_checks.size(); i++)
    {
        temp = 0;
        for(int j = 0; j < (int)d_checks[i].size(); j++)
        {
            temp ^= message[d_checks[i][j]];
        }
        codeword[i] = temp;
    }
}

ldpc_parity_check_matrix::ldpc_parity_check_matrix(const char* filename)
{
    /*filename points to a file of M+N+1 lines, the first is two numbers denoting M and N, the next M lines are lists
    of indices of the M rows of the matrix as above (where the first number in the line is the number of indices in that
    line), the second N lines are the lists of indices for the N columns*/

    int num_rows, num_cols;
    vector<int> data;
    int num_indices;
    int index;

    ifstream pc_file (filename); //open file given by filename
    if (pc_file.fail()) throw std::runtime_error ("ldpc_parity_check_matrix::can't open file");

    pc_file >> num_rows >> num_cols;
    assert(num_rows > 0);
    assert(num_cols > 0);

    for(int i=0; i < num_rows; i++)
    {
        pc_file >> num_indices; //first number is number of indices

        for(int j=0; j<num_indices; j++)
        {
            pc_file >> index;
            data.push_back(index - 1);
        }

        d_rows.push_back(data);
        data.clear();
    }

    for(int i=0; i < num_cols; i++)
    {

        pc_file >> num_indices; //first number is number of indices

        for(int j=0; j<num_indices; j++)
        {
            pc_file >> index;
            data.push_back(index - 1);
        }

        d_cols.push_back(data);
        data.clear();
    }

}

ldpc_parity_check_matrix::~ldpc_parity_check_matrix()
{
}

int ldpc_parity_check_matrix::parity_check(vector<unsigned char> codeword)
{
    assert(codeword.size() == d_cols.size());
    return parity_check(codeword.data());
}

int ldpc_parity_check_matrix::parity_check(unsigned char* codeword)
{
    int xor_tot = 0;
    int temp_data = 0;

    for(int i=0; i < (int)d_rows.size(); i++)
    {
        temp_data = 0;
        for(int j=0; j < (int)d_rows[i].size(); j++)
        {
            temp_data ^= (codeword[d_rows[i][j]] & 0x1);
        }

        xor_tot += temp_data;
    }
    return xor_tot;
}

void ldpc_parity_check_matrix::print_matrix()
{
    int n, oldn;

    for(int i=0; i < (int)d_rows.size(); i++)
    {
        oldn = -1;

        assert(d_rows[i].size() > 0);

        for(int j=0; j < (int)d_rows[i].size(); j++)
        {
            n = d_rows[i][j];
            for(int k=0; k< (n - oldn - 1); k++)
                cout << "0 ";

            cout << "1 ";

            oldn = n;
        }

        for(int j = 0; j < ((int)d_cols.size() - oldn - 1); j++)
        {
            cout << "0 ";
        }

        cout << endl;

    }
}

vector <int> ldpc_parity_check_matrix::get_indices_for_2_cols(int col1, int col2)
{
    assert(col1 >= 0 && col1 < (int)d_cols.size());
    assert(col2 >= 0 && col2 < (int)d_cols.size());

    vector<int> uni = get_indices_for_col(col1);
    for(int i = 0; i < get_number_of_indices_in_col(col2); i++)
    {
        int ck = 0;
        int ind = get_indices_for_col(col2)[i];
        for(int j = 0; j < get_number_of_indices_in_col(col1); j++)
        {
            if(ind == get_indices_for_col(col1)[j])
                ck++;
        }
        if(ck == 0)
            uni.push_back(ind);
    }
    return uni;
}

ldpc_fc_rows::ldpc_fc_rows(const char *filename, int seed)
{
    /* filename must point to a file of M+1 lines, the first of which is the number of rows in P, then each successive
    line will have the number of indices in that check first followed by the indices themselves.*/
    int num_rows;
    vector<int> row;
    int num_indices;
    int index;

    ifstream pc_file (filename); //open file given by filename
    if (pc_file.fail()) throw std::runtime_error ("ldpc_fc_rows::can't open file");

    pc_file >> num_rows >> d_num_cols;

    fprintf(stderr, "Loading ldpc_fc_rows (%s)...\n", filename);

    assert(num_rows > 0);
    for(int i=0; i < num_rows; i++)
    {

        pc_file >> num_indices; //first number is number of indices
        row.clear();

        for(int j=0; j<num_indices; j++)
        {
            pc_file >> index;
            row.push_back(index - 1);
        }
        d_checks.push_back(row);
    }

    for(int i =0; i < (int)d_checks[0].size(); i++)
    {
        fprintf(stderr, "%d ", d_checks[0][i]);
    }

    fprintf(stderr, "\n");

    d_row_ptr = 0;
    d_ri = new randint(seed);

    pc_file.close();
}

ldpc_fc_rows::~ldpc_fc_rows()
{
    delete d_ri;
}

vector <int> ldpc_fc_rows::get_new_inds(int length)
{
    //fprintf(stderr, "i - %d\n", d_checks[d_row_ptr].size());

    vector <int> p(d_checks[d_row_ptr].size());
    vector <int> out(length);
    vector <int> in = get_checks_indices(d_row_ptr);
    //generate random permutation
    //from http://rgrig.blogspot.com/2005/04/random-permutation.html

    for (int i = 0; i < (int)d_checks[d_row_ptr].size(); ++i) {
        int j = d_ri->gen() % (i + 1);
        p[i] = p[j];
        p[j] = i;
    }

    for(int i=0; i < length; i++)
    {
        out[i] = in[p[i]];
    }

    d_row_ptr++;

    if(d_row_ptr >= get_number_of_checks()) d_row_ptr = 0;

    return out;
}
