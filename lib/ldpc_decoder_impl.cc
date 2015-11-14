#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ldpc_decoder_impl.h"
#include <gnuradio/math.h>
#include <assert.h>
#include <stdexcept>
#include <string.h>
#include <gnuradio/fec/generic_decoder.h>

namespace gr {
  namespace ldpc {
      ldpc_decoder::sptr
      ldpc_decoder::make(const char* filename,
                         int num_iterations,
                         int message_length)
      {
        return ldpc_decoder::sptr
          (new ldpc_decoder_impl (filename, num_iterations,
                                  message_length));
      }

      ldpc_decoder_impl::ldpc_decoder_impl (const char* filename,
                                            int num_iterations,
                                            int message_length)
          //reads from a p_matrix file as described in matrix_utils
        : generic_decoder ("ldpc_decoder"),
          d_decoder(filename),
          d_message_length(message_length),
          d_num_iterations(num_iterations)
      {
          d_num_parity_checks = d_decoder.get_number_of_checks();
          d_codeword_length = d_decoder.get_codeword_length();
          d_max_frame_size = d_message_length;

          d_circbuff_in = boost::circular_buffer<float>(2 * d_codeword_length);

          GR_LOG_INFO(d_logger, boost::format("message length = %1%\nnum parity checks = %2%\n" \
                                              "codeword length = %3%\n") % \
                                              d_message_length % d_num_parity_checks % \
                                              d_codeword_length);
          d_frame_size = d_max_frame_size;

          d_rate = (double) d_message_length /
                   (double) d_codeword_length;
      }

      ldpc_decoder_impl::~ldpc_decoder_impl ()
      {
      }

      std::vector<float> ldpc_decoder_impl::prob_to_llr(std::vector<float> codeword)
      {
          std::vector<float> out(codeword.size());
          float sigma = 0.5;
          for(int i = 0; i < out.size(); i++)
          {
              float cw = codeword[i];

              out[i] = (-1*cw)/(2*sigma*sigma);

              /*if (i < 10)
                fprintf(stdout, ".. %f, %f, %f \n", codeword[i], lr, out[i]);
              fflush(stdout);*/
          }
          return out;
      }

      void ldpc_decoder_impl::decode (std::vector<float> codeword, unsigned char *out)
      {
          unsigned char out_codeword[d_codeword_length];
          codeword = prob_to_llr(codeword);
          d_decoder.decode_ldpc(codeword, d_num_iterations, out_codeword);
          memcpy(out, &out_codeword[d_codeword_length-d_message_length], d_message_length);
          d_posterior_llrs = d_decoder.get_posteriors();
      }

      bool ldpc_decoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;
        if(frame_size > d_max_frame_size) {
          GR_LOG_INFO(d_logger, boost::format("ldpc_decoder: tried to set frame to %1%; max possible is %2%") \
                      % frame_size % d_max_frame_size);
          frame_size = d_max_frame_size;
          ret = false;
        }
        GR_LOG_INFO(d_logger, boost::format("Set frame size - %1%") % frame_size);
        d_frame_size = frame_size;

        return ret;
      }

      void
      ldpc_decoder_impl::generic_work (void *inbuffer, void *outbuffer)
      {
        const float *in = (const float *) inbuffer;
        unsigned char *out = (unsigned char *) outbuffer;

        d_circbuff_in.insert(d_circbuff_in.end(), in,
                in + (int)(d_frame_size * 1.0/d_rate * sizeof(unsigned char)));
        if (d_circbuff_in.size() >= d_codeword_length) {
          static int i = 0;
          i += 1;
          /*GR_LOG_INFO(d_logger, boost::format("decoder i = %1%, %2%") \
                  % i % (int)(d_frame_size * 1.0/d_rate * sizeof(unsigned char)));
          GR_LOG_INFO(d_logger, boost::format("decder ?? %1%") % (int)d_circbuff_in.size());*/

          // We have enough elements now so decode a message
          std::vector<float> codeword(d_circbuff_in.begin(),
                                      d_circbuff_in.begin() + d_codeword_length);

          decode(codeword, out);
          // remove the old codeword from the buffer
          d_circbuff_in.erase(d_circbuff_in.begin(),
                              d_circbuff_in.begin() + d_codeword_length);
        }
      }

      int
      ldpc_decoder_impl::get_output_size() {
          return d_message_length;
      }

      int
      ldpc_decoder_impl::get_input_size() {
          return d_codeword_length;
      }

      double
      ldpc_decoder_impl::rate() {
          return d_rate;
      }

      std::vector<float>
      ldpc_decoder_impl::get_posterior_llrs() {
          return d_posterior_llrs; };

      std::vector<unsigned char>
      ldpc_decoder_impl::get_chat() {
          return d_chat; };

      int
      ldpc_decoder_impl::get_message_length() {
          return d_message_length; };

      int
      ldpc_decoder_impl::get_codeword_length() {
          return d_codeword_length; };

      bool
      ldpc_decoder_impl::was_decoded() {
          return d_decoder.was_decoded(); };

      void
      ldpc_decoder_impl::print_matrix() {
          d_decoder.print_matrix(); };

  } /* namespace ldpc */
} /* namespace gr */
