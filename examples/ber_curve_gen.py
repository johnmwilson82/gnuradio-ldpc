#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Ber Curve Gen
# Generated: Sun Feb  1 11:32:40 2015
##################################################

# Call XInitThreads as the _very_ first thing.
# After some Qt import, it's too late
import ctypes
import sys
if sys.platform.startswith('linux'):
    try:
        x11 = ctypes.cdll.LoadLibrary('libX11.so')
        x11.XInitThreads()
    except:
        print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import eng_notation
from gnuradio import fec
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import ldpc
import numpy
import sip
import sys

from distutils.version import StrictVersion
class ber_curve_gen(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Ber Curve Gen")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Ber Curve Gen")
        try:
             self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
             pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "ber_curve_gen")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.framebits = framebits = 4096
        self.samp_rate_0 = samp_rate_0 = 35000000
        self.rate = rate = 2
        self.polys = polys = [79, 109]
        self.k = k = 7
        self.esno_0 = esno_0 = numpy.arange(0, 8, .5) 
        
        self.enc_ldpc = enc_ldpc = map( (lambda b: map( ( lambda a: ldpc.ldpc_encoder("/home/john/gnuradio/gnuradio-ldpc/examples/408.pmat")), range(0,1) ) ), range(0,16));     
        
        self.enc_dummy = enc_dummy = map((lambda b: map((lambda a: fec.dummy_encoder_make(framebits)), range(0,1))), range(0,16))     
        
        self.dec_ldpc = dec_ldpc = map( (lambda b: map( ( lambda a: ldpc.ldpc_decoder("/home/john/gnuradio/gnuradio-ldpc/examples/408.pcmat", 50, 0)), range(0,1) ) ), range(0,16));     
        
        self.dec_dummy = dec_dummy = map((lambda b: map((lambda a: fec.dummy_decoder.make(framebits)), range(0,1))), range(0,16))     

        ##################################################
        # Blocks
        ##################################################
        self.qtgui_bercurve_sink_0 =   qtgui.ber_sink_b(
            esno_0, #range of esnos
            2, #number of curves
            1000, #ensure at least
            -10, #cutoff
            [], #indiv. curve names
          )
        self.qtgui_bercurve_sink_0.set_update_time(0.10)
        self.qtgui_bercurve_sink_0.set_y_axis(-10, 0)
        self.qtgui_bercurve_sink_0.set_x_axis(esno_0[0], esno_0[-1])
        
        labels = ["None", "LDPC", "CC (K=7, Rate=2)", "", "",
                  "", "", "", "", ""]
        widths = [2, 2, 2, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "magenta", "red", "red",
                  "red", "red", "red", "red", "red"]
        styles = [1, 2, 5, 0, 0,
                  0, 0, 0, 0, 0]
        markers = [0, 1, 0, 0, 0,
                   0, 0, 0, 0, 0]
        alphas = [1, 1, 1, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(2):
            if len(labels[i]) == 0:
                self.qtgui_bercurve_sink_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_bercurve_sink_0.set_line_label(i, labels[i])
            self.qtgui_bercurve_sink_0.set_line_width(i, widths[i])
            self.qtgui_bercurve_sink_0.set_line_color(i, colors[i])
            self.qtgui_bercurve_sink_0.set_line_style(i, styles[i])
            self.qtgui_bercurve_sink_0.set_line_marker(i, markers[i])
            self.qtgui_bercurve_sink_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_bercurve_sink_0_win = sip.wrapinstance(self.qtgui_bercurve_sink_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_bercurve_sink_0_win)
        self.fec_bercurve_generator_0_0_0 = fec.bercurve_generator(
        	enc_dummy, #size
        	dec_dummy, #name
        	esno_0, #range of esnos
        	samp_rate_0, #throttle
                "capillary", #threading mode
        	'11', #puncture pattern
                -100 # noise gen. seed
        )
          
        self.fec_bercurve_generator_0_0 = fec.bercurve_generator(
        	enc_ldpc, #size
        	dec_ldpc, #name
        	esno_0, #range of esnos
        	samp_rate_0, #throttle
                "capillary", #threading mode
        	'11', #puncture pattern
                -100 # noise gen. seed
        )
          

        ##################################################
        # Connections
        ##################################################
        self.connect((self.fec_bercurve_generator_0_0, 0), (self.qtgui_bercurve_sink_0, 32))    
        self.connect((self.fec_bercurve_generator_0_0, 1), (self.qtgui_bercurve_sink_0, 33))    
        self.connect((self.fec_bercurve_generator_0_0, 2), (self.qtgui_bercurve_sink_0, 34))    
        self.connect((self.fec_bercurve_generator_0_0, 3), (self.qtgui_bercurve_sink_0, 35))    
        self.connect((self.fec_bercurve_generator_0_0, 4), (self.qtgui_bercurve_sink_0, 36))    
        self.connect((self.fec_bercurve_generator_0_0, 5), (self.qtgui_bercurve_sink_0, 37))    
        self.connect((self.fec_bercurve_generator_0_0, 6), (self.qtgui_bercurve_sink_0, 38))    
        self.connect((self.fec_bercurve_generator_0_0, 7), (self.qtgui_bercurve_sink_0, 39))    
        self.connect((self.fec_bercurve_generator_0_0, 8), (self.qtgui_bercurve_sink_0, 40))    
        self.connect((self.fec_bercurve_generator_0_0, 9), (self.qtgui_bercurve_sink_0, 41))    
        self.connect((self.fec_bercurve_generator_0_0, 10), (self.qtgui_bercurve_sink_0, 42))    
        self.connect((self.fec_bercurve_generator_0_0, 11), (self.qtgui_bercurve_sink_0, 43))    
        self.connect((self.fec_bercurve_generator_0_0, 12), (self.qtgui_bercurve_sink_0, 44))    
        self.connect((self.fec_bercurve_generator_0_0, 13), (self.qtgui_bercurve_sink_0, 45))    
        self.connect((self.fec_bercurve_generator_0_0, 14), (self.qtgui_bercurve_sink_0, 46))    
        self.connect((self.fec_bercurve_generator_0_0, 15), (self.qtgui_bercurve_sink_0, 47))    
        self.connect((self.fec_bercurve_generator_0_0, 16), (self.qtgui_bercurve_sink_0, 48))    
        self.connect((self.fec_bercurve_generator_0_0, 17), (self.qtgui_bercurve_sink_0, 49))    
        self.connect((self.fec_bercurve_generator_0_0, 18), (self.qtgui_bercurve_sink_0, 50))    
        self.connect((self.fec_bercurve_generator_0_0, 19), (self.qtgui_bercurve_sink_0, 51))    
        self.connect((self.fec_bercurve_generator_0_0, 20), (self.qtgui_bercurve_sink_0, 52))    
        self.connect((self.fec_bercurve_generator_0_0, 21), (self.qtgui_bercurve_sink_0, 53))    
        self.connect((self.fec_bercurve_generator_0_0, 22), (self.qtgui_bercurve_sink_0, 54))    
        self.connect((self.fec_bercurve_generator_0_0, 23), (self.qtgui_bercurve_sink_0, 55))    
        self.connect((self.fec_bercurve_generator_0_0, 24), (self.qtgui_bercurve_sink_0, 56))    
        self.connect((self.fec_bercurve_generator_0_0, 25), (self.qtgui_bercurve_sink_0, 57))    
        self.connect((self.fec_bercurve_generator_0_0, 26), (self.qtgui_bercurve_sink_0, 58))    
        self.connect((self.fec_bercurve_generator_0_0, 27), (self.qtgui_bercurve_sink_0, 59))    
        self.connect((self.fec_bercurve_generator_0_0, 28), (self.qtgui_bercurve_sink_0, 60))    
        self.connect((self.fec_bercurve_generator_0_0, 29), (self.qtgui_bercurve_sink_0, 61))    
        self.connect((self.fec_bercurve_generator_0_0, 30), (self.qtgui_bercurve_sink_0, 62))    
        self.connect((self.fec_bercurve_generator_0_0, 31), (self.qtgui_bercurve_sink_0, 63))    
        self.connect((self.fec_bercurve_generator_0_0_0, 0), (self.qtgui_bercurve_sink_0, 0))    
        self.connect((self.fec_bercurve_generator_0_0_0, 1), (self.qtgui_bercurve_sink_0, 1))    
        self.connect((self.fec_bercurve_generator_0_0_0, 2), (self.qtgui_bercurve_sink_0, 2))    
        self.connect((self.fec_bercurve_generator_0_0_0, 3), (self.qtgui_bercurve_sink_0, 3))    
        self.connect((self.fec_bercurve_generator_0_0_0, 4), (self.qtgui_bercurve_sink_0, 4))    
        self.connect((self.fec_bercurve_generator_0_0_0, 5), (self.qtgui_bercurve_sink_0, 5))    
        self.connect((self.fec_bercurve_generator_0_0_0, 6), (self.qtgui_bercurve_sink_0, 6))    
        self.connect((self.fec_bercurve_generator_0_0_0, 7), (self.qtgui_bercurve_sink_0, 7))    
        self.connect((self.fec_bercurve_generator_0_0_0, 8), (self.qtgui_bercurve_sink_0, 8))    
        self.connect((self.fec_bercurve_generator_0_0_0, 9), (self.qtgui_bercurve_sink_0, 9))    
        self.connect((self.fec_bercurve_generator_0_0_0, 10), (self.qtgui_bercurve_sink_0, 10))    
        self.connect((self.fec_bercurve_generator_0_0_0, 11), (self.qtgui_bercurve_sink_0, 11))    
        self.connect((self.fec_bercurve_generator_0_0_0, 12), (self.qtgui_bercurve_sink_0, 12))    
        self.connect((self.fec_bercurve_generator_0_0_0, 13), (self.qtgui_bercurve_sink_0, 13))    
        self.connect((self.fec_bercurve_generator_0_0_0, 14), (self.qtgui_bercurve_sink_0, 14))    
        self.connect((self.fec_bercurve_generator_0_0_0, 15), (self.qtgui_bercurve_sink_0, 15))    
        self.connect((self.fec_bercurve_generator_0_0_0, 16), (self.qtgui_bercurve_sink_0, 16))    
        self.connect((self.fec_bercurve_generator_0_0_0, 17), (self.qtgui_bercurve_sink_0, 17))    
        self.connect((self.fec_bercurve_generator_0_0_0, 18), (self.qtgui_bercurve_sink_0, 18))    
        self.connect((self.fec_bercurve_generator_0_0_0, 19), (self.qtgui_bercurve_sink_0, 19))    
        self.connect((self.fec_bercurve_generator_0_0_0, 20), (self.qtgui_bercurve_sink_0, 20))    
        self.connect((self.fec_bercurve_generator_0_0_0, 21), (self.qtgui_bercurve_sink_0, 21))    
        self.connect((self.fec_bercurve_generator_0_0_0, 22), (self.qtgui_bercurve_sink_0, 22))    
        self.connect((self.fec_bercurve_generator_0_0_0, 23), (self.qtgui_bercurve_sink_0, 23))    
        self.connect((self.fec_bercurve_generator_0_0_0, 24), (self.qtgui_bercurve_sink_0, 24))    
        self.connect((self.fec_bercurve_generator_0_0_0, 25), (self.qtgui_bercurve_sink_0, 25))    
        self.connect((self.fec_bercurve_generator_0_0_0, 26), (self.qtgui_bercurve_sink_0, 26))    
        self.connect((self.fec_bercurve_generator_0_0_0, 27), (self.qtgui_bercurve_sink_0, 27))    
        self.connect((self.fec_bercurve_generator_0_0_0, 28), (self.qtgui_bercurve_sink_0, 28))    
        self.connect((self.fec_bercurve_generator_0_0_0, 29), (self.qtgui_bercurve_sink_0, 29))    
        self.connect((self.fec_bercurve_generator_0_0_0, 30), (self.qtgui_bercurve_sink_0, 30))    
        self.connect((self.fec_bercurve_generator_0_0_0, 31), (self.qtgui_bercurve_sink_0, 31))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "ber_curve_gen")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_framebits(self):
        return self.framebits

    def set_framebits(self, framebits):
        self.framebits = framebits

    def get_samp_rate_0(self):
        return self.samp_rate_0

    def set_samp_rate_0(self, samp_rate_0):
        self.samp_rate_0 = samp_rate_0

    def get_rate(self):
        return self.rate

    def set_rate(self, rate):
        self.rate = rate

    def get_polys(self):
        return self.polys

    def set_polys(self, polys):
        self.polys = polys

    def get_k(self):
        return self.k

    def set_k(self, k):
        self.k = k

    def get_esno_0(self):
        return self.esno_0

    def set_esno_0(self, esno_0):
        self.esno_0 = esno_0

    def get_enc_ldpc(self):
        return self.enc_ldpc

    def set_enc_ldpc(self, enc_ldpc):
        self.enc_ldpc = enc_ldpc

    def get_enc_dummy(self):
        return self.enc_dummy

    def set_enc_dummy(self, enc_dummy):
        self.enc_dummy = enc_dummy

    def get_dec_ldpc(self):
        return self.dec_ldpc

    def set_dec_ldpc(self, dec_ldpc):
        self.dec_ldpc = dec_ldpc

    def get_dec_dummy(self):
        return self.dec_dummy

    def set_dec_dummy(self, dec_dummy):
        self.dec_dummy = dec_dummy

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    if(StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0")):
        Qt.QApplication.setGraphicsSystem(gr.prefs().get_string('qtgui','style','raster'))
    qapp = Qt.QApplication(sys.argv)
    tb = ber_curve_gen()
    tb.start()
    tb.show()
    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()
    tb = None #to clean up Qt widgets
