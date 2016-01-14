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

import numpy
import math
import scipy
from gnuradio import gr, digital, channels
from gnuradio import blocks
from gnuradio import analog
import snr
import sys
import csv

try:
    import pylab
except ImportError:
    print "Error: could not import pylab (http://matplotlib.sourceforge.net/)"
    sys.exit(1)

# Number of samples to use
N = 10000
# After this many samples, a tag containing the SNR (key='snr') will be sent
ntag = 10000
# The running-average coefficient
alpha = 0.001
# QPSK
bits = (2*scipy.complex64(scipy.random.randint(0, 2, N)) - 1) + \
     1j*(2*scipy.complex64(scipy.random.randint(0, 2, N)) - 1)

general_snr = list()
snr_svr = list()
snr_m2m4 = list()
snr_skew = list()
snr_simple = list()
snr_real_value = list()


def noise_voltage(SNR):
    """ Calculate noise voltage from SNR assuming unit signal power
    :param SNR: signal to noise ratio
    """
    return 10 ** (float(-SNR) / 20)


def simulate_general_est(SNR_range):
    """ All the work's done here: create flow graph, run, read out estimated SNR
    :param SNR_range: range of SNR for which noise voltage will be calculated
    """
    est_snr = []
    for SNR in SNR_range:
        noise_volt = noise_voltage(SNR)

        src = blocks.vector_source_c(bits.tolist(), False)
        # noise_voltage, frequency_offset, epsilon, taps, noise_seed, block_tags
        chn = snr.channel_model_snr(0.0, 0.0, 1.0, [1, ],
                                    0)
        chn.set_noise_voltage(noise_volt)
        est = snr.mpsk_receiver_snr_est_cc(ntag, alpha)
        sink = blocks.null_sink(gr.sizeof_gr_complex)

        tb = gr.top_block()
        tb.connect(src, chn, est, sink)
        tb.run()

        est_snr.append(est.snr())
    return est_snr


def simulate_other_est(SNR_range, est_type):
    """ Create flow graph, run, read out estimated SNR with usage of gnuradio implemented estimator
    :param SNR_range: range of SNR for which noise voltage will be calculated
    """
    est_snr = []
    for SNR in SNR_range:
        noise_volt = noise_voltage(SNR)

        src = blocks.vector_source_c(bits.tolist(), False)
        # noise_voltage, frequency_offset, epsilon, taps, noise_seed, block_tags
        chn = snr.channel_model_snr(0.0, 0.0, 1.0, [1, ],
                                    0)
        est = digital.mpsk_snr_est_cc(est_type, ntag, alpha)
        sink = blocks.null_sink(gr.sizeof_gr_complex)

        chn.set_noise_voltage(noise_volt)
        tb = gr.top_block()
        tb.connect(src, chn, est, sink)
        tb.run()

        est_snr.append(est.snr())
    return est_snr


if __name__ == "__main__":
    EbN0_min = -5.0
    EbN0_max = 20.0
    SNR_step = 2.5
    SNR_range = scipy.arange(EbN0_min, EbN0_max + SNR_step, SNR_step)
    for SNR in SNR_range:
        print "SNR: %0.2f dB" % SNR
        noise_volt = noise_voltage(SNR)
        print "Noise voltage = %0.2f V" % noise_volt

    print "Simulating of general estimator"
    general_snr = simulate_general_est(SNR_range)

    print "Simulating of svr estimator"
    snr_svr = simulate_other_est(SNR_range, digital.SNR_EST_SVR)

    print "Simulating of m2m4 estimator"
    snr_m2m4 = simulate_other_est(SNR_range, digital.SNR_EST_M2M4)

    print "Simulating of skew estimator"
    snr_skew = simulate_other_est(SNR_range, digital.SNR_EST_SKEW)

    print "Simulating of simple estimator"
    snr_simple = simulate_other_est(SNR_range, digital.SNR_EST_SIMPLE)

    f1 = pylab.figure(1)
    s1 = f1.add_subplot(1, 1, 1)
    s1.plot(SNR_range, SNR_range, "g-o", linewidth=2, label="SNR rzeczywisty")
    s1.plot(SNR_range, snr_svr, "b-o", linewidth=2, label="Estymator SVR")
    s1.plot(SNR_range, snr_m2m4, "c-o", linewidth=2, label="Estymator M2M4")
    s1.plot(SNR_range, snr_simple, "r-o", linewidth=2, label="Estymator prosty")
    s1.plot(SNR_range, general_snr, "k-o", linewidth=2, label="Metoda automatyczna")
    samples = u' próbek'
    s1.set_title('Wyniki symulacji dla modulacji QPSK, przy ' + str(N) + samples)
    s1.set_xlabel('SNR [dB]')
    s1.set_ylabel('SNR estymowany [dB]')
    handles, labels = s1.get_legend_handles_labels()
    s1.legend(handles, labels, loc='lower left', fontsize='11', bbox_to_anchor=(0.60, 0.02))
    s1.grid(True)
    pylab.show()
    
    path = '/home/kchrusci/Workspace/Simulations/QPSK/data/'

    filename_general = path + 'general.csv'
    with open(filename_general, 'wb') as out_file:
        writer = csv.writer(out_file, delimiter='\n')
        writer.writerow(general_snr)

    filename_svr = path + 'svr.csv'
    with open(filename_svr, 'wb') as out_file:
        writer = csv.writer(out_file, delimiter='\n')
        writer.writerow(snr_svr)

    filename_m2m4 = path + 'mm4.csv'
    with open(filename_m2m4, 'wb') as out_file:
        writer = csv.writer(out_file, delimiter='\n')
        writer.writerow(snr_m2m4)

    filename_skew = path + 'skew.csv'
    with open(filename_skew, 'wb') as out_file:
        writer = csv.writer(out_file, delimiter='\n')
        writer.writerow(snr_skew)

    filename_simple = path + 'simple.csv'
    with open(filename_simple, 'wb') as out_file:
        writer = csv.writer(out_file, delimiter='\n')
        writer.writerow(snr_simple)
