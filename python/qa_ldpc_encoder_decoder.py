from gnuradio import gr, gr_unittest
from gnuradio import blocks
import ldpc_swig as ldpc
import numpy as np
import sys, numpy
import timeit
import functools

from gnuradio.fec import extended_encoder
from gnuradio.fec import extended_decoder

class map_bb(gr.sync_block):
    def __init__(self, bitmap):
        gr.sync_block.__init__(
            self,
            name = "map_bb",
            in_sig = [numpy.int8],
            out_sig = [numpy.int8])
        self.bitmap = bitmap

    def work(self, input_items, output_items):
        output_items[0][:] = map(lambda x: self.bitmap[x], input_items[0])
        return len(output_items[0])


class _qa_helper(gr.top_block):

    def __init__(self, data_size, enc, dec, threading):
        gr.top_block.__init__(self, "_qa_helper")

        self.puncpat = puncpat = '11'

        self.enc = enc
        self.dec = dec
        self.data_size = data_size
        self.threading = threading

        self.ext_encoder = extended_encoder(enc, threading=self.threading, puncpat=self.puncpat)
        self.ext_decoder = extended_decoder(dec, threading=self.threading, ann=None,
                                           puncpat=self.puncpat, integration_period=10000)

        self.src = blocks.vector_source_b(data_size*[0, 1, 2, 3, 5, 7, 9, 13, 15, 25, 31, 45, 63, 95, 127], False)
        self.unpack = blocks.unpack_k_bits_bb(8)
        self.map = map_bb([-1, 1])
        self.to_float = blocks.char_to_float(1)
        self.snk_input = blocks.vector_sink_b()
        self.snk_output = blocks.vector_sink_b()

        self.connect(self.src, self.unpack, self.ext_encoder)
        self.connect(self.ext_encoder, self.map, self.to_float)
        self.connect(self.to_float, self.ext_decoder)
        self.connect(self.unpack, self.snk_input)
        self.connect(self.ext_decoder, self.snk_output)


class qa_ldpc_encode_decode (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def check_coders(self, enc, dec):
        threading = None
        self.test = _qa_helper(204, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in  = self.test.snk_input.data()

        return data_out == data_in

    def test_001_basic_operation(self):
        enc = ldpc.ldpc_encoder('../../examples/408.pmat')
        dec = ldpc.ldpc_decoder('../../examples/408.pcmat', 50, 204)

        self.assertTrue(self.check_coders(enc, dec))
        time = timeit.timeit(functools.partial(
            self.check_coders, enc=enc, dec=dec), number=10)
        print "ldpc time = " + str(time)

    def test_002_gnuradio_decoder(self):
        from gnuradio.fec import ldpc_encoder_make as gnuradio_ldpc_encoder
        from gnuradio.fec import ldpc_decoder_make as gnuradio_ldpc_decoder
        enc = gnuradio_ldpc_encoder('../../examples/408.alist')
        dec = gnuradio_ldpc_decoder('../../examples/408.alist')

        self.assertTrue(self.check_coders(enc, dec))
        time = timeit.timeit(functools.partial(
            self.check_coders, enc=enc, dec=dec), number=10)
        print "gnuradio ldpc time = " + str(time)


if __name__ == '__main__':
    gr_unittest.run(qa_ldpc_encode_decode, "qa_ldpc_encode_decode.xml")
