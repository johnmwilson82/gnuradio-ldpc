#ifndef INCLUDED_ldpc_decoder_H
#define INCLUDED_ldpc_decoder_H

#include "ldpc_decoder_core.h"
#include "ldpc/ldpc_decoder.h"
#include <stdio.h>
#include <boost/circular_buffer.hpp>

namespace gr {
  namespace ldpc {

      class FEC_API ldpc_decoder_impl : public ldpc_decoder
      {
      private:
        //plug into the generic fec api
        int get_output_size();
        int get_input_size();

        double d_rate;
        int d_message_length;
        int d_num_parity_checks;
        int d_codeword_length;
        int d_num_iterations;
        int d_max_frame_size;
        int d_frame_size;
        boost::circular_buffer<float> d_circbuff_in;

        std::vector<float> d_posterior_llrs;
        std::vector<unsigned char> d_chat;
        ldpc_decoder_core d_decoder;

      public:
        ldpc_decoder_impl (const char* filename, int num_iterations, int message_length);
        ~ldpc_decoder_impl();

        void reload_decoder(const char* filename, int num_iterations, int message_length) {
                              d_num_iterations = num_iterations;
                              d_message_length = message_length;
                              d_decoder = ldpc_decoder_core(filename); };

        void set_num_iterations(int num_iterations) {
            d_num_iterations = num_iterations; };

        void decode (std::vector<float> codeword, unsigned char* out);

        std::vector<float> get_posterior_llrs();
        std::vector<unsigned char> get_chat();

        int get_message_length();
        int get_codeword_length();
        bool was_decoded();
        void print_matrix();

        // Inherited from generic_decoer
        void generic_work(void *inbuffer, void *outbuffer);
        double rate();
        bool set_frame_size(unsigned int frame_size);
      };
  }
}

#endif /* INCLUDED_grj_ldpc_decoder_H */
