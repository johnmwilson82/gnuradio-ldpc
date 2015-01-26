/* -*- c++ -*- */
#define FEC_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ldpc_swig_doc.i"

%{
#include "ldpc/ldpc_decoder.h"
#include "ldpc/ldpc_encoder.h"
%}

%include "ldpc/ldpc_decoder.h"
%include "ldpc/ldpc_encoder.h"

GR_SWIG_BLOCK_MAGIC2(ldpc, ldpc_encoder);
GR_SWIG_BLOCK_MAGIC2(ldpc, ldpc_decoder);
