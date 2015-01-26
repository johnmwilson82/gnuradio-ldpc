#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ldpc_encoder_impl.h"
#include <assert.h>
#include <stdexcept>
#include <string.h>
#include <gnuradio/fec/generic_encoder.h>

namespace gr {
  namespace ldpc {

      ldpc_encoder::sptr
      ldpc_encoder::make(const char* filename)
      {
        return ldpc_encoder::sptr
          (new ldpc_encoder_impl(filename));
      }

      /* ldpc_encoder_impl works with modulated symbols instead of raw data,
       * provide an incoming symbol via python, the desired payload size,
       * a pointer to an array of complex preamble symbols and the length
       * of this array
       */

      ldpc_encoder_impl::ldpc_encoder_impl (const char* filename)
          //reads from a p_matrix file as described in grj_matrix_utils
        : generic_encoder("ldpc_encoder"),
          d_p_matrix(filename)

      {
          d_message_length = d_p_matrix.get_message_length();
          d_num_parity_checks = d_p_matrix.get_number_of_checks();
          d_codeword_length = d_message_length + d_num_parity_checks;
          d_max_frame_size = d_message_length;

          GR_LOG_INFO(d_logger, boost::format("Message length = %1%; Codeword length = %2%") \
                  % d_message_length % d_codeword_length);

          d_circbuff_in = boost::circular_buffer<unsigned char>(2 * d_message_length);
          d_frame_size = d_max_frame_size;

          d_rate = (double) d_message_length /
                   (double) d_codeword_length;

      }

      ldpc_encoder_impl::~ldpc_encoder_impl ()
      {
      }

      bool
      ldpc_encoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;
        if(frame_size > d_max_frame_size) {
          GR_LOG_INFO(d_logger, boost::format("ldpc_encoder: tried to set frame to %1%; max possible is %2%") \
                      % frame_size % d_max_frame_size);
          frame_size = d_max_frame_size;
          ret = false;
        }

        d_frame_size = frame_size;

        return ret;
      }

      void
      ldpc_encoder_impl::generic_work(void *in_buffer, void *out_buffer)
      {
        const unsigned char *in = (const unsigned char *) in_buffer;
        unsigned char *out = (unsigned char *) out_buffer;

        d_circbuff_in.insert(d_circbuff_in.end(), in, in + d_frame_size);

        if (d_circbuff_in.size() >= d_message_length) {
          static int i = 0;
          i += 1;
          GR_LOG_INFO(d_logger, boost::format("encoder %d") % i);

          // We have enough elements now so decode a message
          std::vector<unsigned char> message(d_circbuff_in.begin(),
                                             d_circbuff_in.begin() + d_message_length);

          d_p_matrix.encode_ldpc(message, out);

          // remove the old codeword from the buffer
          d_circbuff_in.erase(d_circbuff_in.begin(),
                              d_circbuff_in.begin() + d_message_length);
        }
      }

      double
      ldpc_encoder_impl::rate() {
          return 1.0/d_rate;
      }

      int
      ldpc_encoder_impl::get_message_length() {
          return d_message_length;
      }

      int
      ldpc_encoder_impl::get_output_size() {
          return d_codeword_length;
      }

      int
      ldpc_encoder_impl::get_input_size() {
          return d_message_length;
      }

  } //namespace fec
} //namespace gr
