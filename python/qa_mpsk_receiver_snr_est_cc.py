#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2015 Karolina Chrusciel
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks, analog
import math
import snr

class qa_mpsk_receiver_snr_est_cc(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        N = 1000                # number of samples to use
        fs = 1000               # baseband sampling rate
        freq = 100              # frequency of source signal
        noise_volt = 0.18       # noise voltage [V]

        signal = analog.sig_source_c(fs, analog.GR_SIN_WAVE, freq, 1)
        head = blocks.head(gr.sizeof_gr_complex, N)
        op = snr.channel_model_snr(0.0, 0.0, 1.0, [1,], 0) # noise_voltage, frequency_offset, epsilon, taps, noise_seed, block_tags
        est = snr.mpsk_receiver_snr_est_cc(1000, 0.001) # ntag, alpha
        snk = blocks.vector_sink_c()

        op.set_noise_voltage(0.18)
        op.set_frequency_offset(0.0)
        op.set_taps([1,])
        op.set_timing_offset(1.0)

        self.tb.connect(signal, head, op, est, snk)
        self.tb.run()

        est_snr = est.snr()
        exp_snr = 20*math.log10(1/noise_volt)


if __name__ == '__main__':
    gr_unittest.run(qa_mpsk_receiver_snr_est_cc, "qa_mpsk_receiver_snr_est_cc.xml")
