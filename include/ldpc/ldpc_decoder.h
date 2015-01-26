#ifndef INCLUDED_LDPC_DECODER_H
#define INCLUDED_LDPC_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/decoder.h>

namespace gr {
  namespace ldpc {
      /*!
       * \brief Convolutional Code Decoding class.
       * \ingroup error_coding_blk
       */

      class FEC_API ldpc_decoder : virtual public gr::fec::generic_decoder
      {
      public:

        typedef boost::shared_ptr<gr::fec::generic_decoder> sptr;
        /*!
         * Build an LDPC decoding FEC API object.
         *
         * \param pcmat_filename Filename to a parity check matrix
         * \param num_iterations Maximum number of iterations of the
         *                       LDPC decoder
         * \param message_length The encoded message length
         */
        static sptr make
          (const char* pcmat_filename, int num_iterations,
           int message_length);

        /*!
         * Sets the uncoded frame size to \p frame_size. If \p
         * frame_size is greater than the value given to the
         * constructor, the frame size will be capped by that initial
         * value and this function will return false. Otherwise, it
         * returns true.
         */
        virtual bool set_frame_size(unsigned int frame_size) = 0;

        /*!
         * Returns the coding rate of this encoder.
         */
        virtual double rate() = 0;

        virtual int get_message_length() = 0;
        virtual int get_codeword_length() = 0;
        virtual bool was_decoded() = 0;
        virtual void print_matrix() = 0;
      };
  } /* namespace ldpc */
} /* namespace gr */

#endif /* INCLUDED_LDPC_CC_DECODER_H */
