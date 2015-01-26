#ifndef INCLUDED_LDPC_ENCODER_IMPL_H
#define INCLUDED_LDPC_ENCODER_IMPL_H

#include "ldpc_matrix_utils.h"
#include <stdio.h>
#include <ldpc/ldpc_encoder.h>
#include <boost/circular_buffer.hpp>

namespace gr {
  namespace ldpc {

      class FEC_API ldpc_encoder_impl : public ldpc_encoder
      {
      private:
        int get_output_size();
        int get_input_size();

        void generic_work (void *inbuffer, void *outbuffer);

        double d_rate;
        int d_message_length;
        int d_codeword_length;
        int d_num_parity_checks;
        int d_max_frame_size;
        int d_frame_size;
        std::vector<unsigned char> codeword;

        ldpc_p_matrix d_p_matrix;

        boost::circular_buffer<unsigned char> d_circbuff_in;

      public:
        ldpc_encoder_impl (const char* filename);
        ~ldpc_encoder_impl();

        int get_message_length();

        bool set_frame_size(unsigned int frame_size);
        double rate();
      };

  } //namespace fec
} //namespace gr

#endif /* INCLUDED_LDPC_ENCODER_IMPL_H */
