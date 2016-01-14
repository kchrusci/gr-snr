/* -*- c++ -*- */

#define SNR_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "snr_swig_doc.i"

%{
#include "snr/channel_model_snr.h"
#include "snr/mpsk_receiver_snr_est_cc.h"
%}

%include "snr/channel_model_snr.h"
GR_SWIG_BLOCK_MAGIC2(snr, channel_model_snr);
%include "snr/mpsk_receiver_snr_est_cc.h"
GR_SWIG_BLOCK_MAGIC2(snr, mpsk_receiver_snr_est_cc);
