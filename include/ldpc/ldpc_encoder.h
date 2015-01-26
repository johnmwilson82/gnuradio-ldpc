#ifndef INCLUDED_LDPC_ENCODER_H
#define INCLUDED_LDPC_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/encoder.h>

namespace gr {
  namespace ldpc {
      /*!
       * \brief Convolutional Code Decoding class.
       * \ingroup error_coding_blk
       */

      class FEC_API ldpc_encoder : virtual public gr::fec::generic_encoder
      {
      public:
        typedef boost::shared_ptr<gr::fec::generic_encoder> sptr;

        /*!
         * Build an LDPC decoding FEC API object.
         *
         * \param pmat_filename Filename to a parity check matrix
         */
        static sptr make (const char* pmat_filename);

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
      };
  } /* namespace ldpc */
} /* namespace gr */

#endif /* INCLUDED_LDPC_CC_DECODER_H */
