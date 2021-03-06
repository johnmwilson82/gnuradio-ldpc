#ifndef ldpc_MATRIX_UTILS_H
#define ldpc_MATRIX_UTILS_H

#include <vector>
#include <assert.h>
#include "ldpc_randint.h"

class ldpc_p_matrix
{
    /*this class deals with the non-sparse parity check section of an LDPC generator matrix, i.e. P in most literature
    An LDPC generator matrix is in the form:    [P]
                                                [I]
    Where I the R*R identity matrix (where R=N-M, N=codeword length, R=message length and M=number of parity checks)
    and P is the R*M parity check matrix. It is generated by performing Gaussian elimination on the sparse parity check
    matrix. The Gaussian elimination must be performed in Matlab or similar prior to loading into the LDPC generator
    routine as on the fly Gaussian elimination at the transmitter is deemed too complicated to warrant performing on-chip.

    An ASCII file format is defined to allow this class to read data in.

    Generation of an LDPC codeword will involve generating the parity bits by performing modulo-2 multiplication of the
    message vector bits defined by the parity checks and appending the original message vector right after*/

private:
    std::vector< std::vector<int> > d_checks; //a 2-d vector where the checks are stored

public:
    ldpc_p_matrix(const char* filename); //constructor for parity checks
    ~ldpc_p_matrix(); //destructor

    int d_num_cols;

    //void load_parity_checks_from_file(const char* filename);
    const std::vector<int> & get_checks_indices(int row_num) { //returns a vector containing parity checks for that row
                                assert(row_num >= 0 && row_num < (int)d_checks.size());
                                return d_checks[row_num];
                                }

    int get_number_of_checks() { return d_checks.size(); }; //return number of parity checks
    int get_message_length() { return d_num_cols; };

    int get_number_of_indices_in_check(int row_num) {
                                assert(row_num >= 0 && row_num < (int)d_checks.size());
                                return d_checks[row_num].size();
                                }

    std::vector<unsigned char> encode_ldpc(std::vector<unsigned char> message);
    void encode_ldpc(std::vector<unsigned char> message, unsigned char *codeword);

    std::vector<unsigned char> encode_ldpc_message_first(std::vector<unsigned char> message);
};

class ldpc_parity_check_matrix
{
    /*This class defines an LDPC parity check matrix in terms of its rows and columns, as above an ASCII file will
    be used to read matrix data in, which will be similar to the alist format used by Mackay. This class will store
    vectors for both row and column data, which are used by an LDPC decoder to perform the horizontal and vertical
    steps in the BP algorithm.

    The only operation that will be coded into this class will be the parity check equation finding x for a given
    codeword c where Hc=x.*/

private:
    std::vector< std::vector<int> > d_rows;
    std::vector< std::vector<int> > d_cols;

public:
    ldpc_parity_check_matrix(const char* filename);

    ~ldpc_parity_check_matrix();

    const std::vector<int> & get_indices_for_row(int row_num){
                                return d_rows[row_num];
                                };

    const std::vector<int> & get_indices_for_col(int col_num){
                                return d_cols[col_num];
                                };

    int get_number_of_indices_in_row(int row_num){
                                return d_rows[row_num].size();
                                };

    int get_number_of_indices_in_col(int col_num){
                                return d_cols[col_num].size();
                                };

    std::vector<int> get_indices_for_2_cols(int col1, int col2);

    int num_rows() { return d_rows.size(); };
    int num_cols() { return d_cols.size(); };

    int parity_check(std::vector<unsigned char> codeword);
    int parity_check(unsigned char *codeword);

    void print_matrix();

};

class ldpc_fc_rows
{

private:
    std::vector< std::vector<int> > d_checks; //a 2-d vector where the checks are stored

public:
    ldpc_fc_rows(const char* filename, int seed); //constructor for parity checks

    ~ldpc_fc_rows(); //destructor

    int d_num_cols;

    int d_row_ptr;

    //void load_parity_checks_from_file(const char* filename);
    const std::vector<int> & get_checks_indices(int row_num) { //returns a vector containing parity checks for that row
                                assert(row_num >= 0 && row_num < (int)d_checks.size());
                                return d_checks[row_num];
                                }

    int get_number_of_checks() { return d_checks.size(); }; //return number of parity checks
    int get_message_length() { return d_num_cols; };

    int get_number_of_indices_in_check(int row_num) {   assert(row_num >= 0 && row_num < (int)d_checks.size());
                                return d_checks[row_num].size();
                                }

    std::vector <int> get_new_inds(int length);


    randint *d_ri;

};
#endif
