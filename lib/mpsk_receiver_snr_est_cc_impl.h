/* -*- c++ -*- */
/* 
 * Copyright 2015 Karolina Chrusciel
 *
 * This file is part of gr-snr module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_SNR_MPSK_RECEIVER_SNR_EST_CC_IMPL_H
#define INCLUDED_SNR_MPSK_RECEIVER_SNR_EST_CC_IMPL_H

#include <snr/api.h>
#include <snr/mpsk_receiver_snr_est_cc.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace snr {

    class mpsk_receiver_snr_est_cc_impl : public mpsk_receiver_snr_est_cc
    {
     private:
      int d_nsamples, d_count;
      double d_alpha, d_beta;
      double d_y1, d_y2;
      double d_snr_level;
      mpsk_snr_est_general *d_snr_est;

      // d_key is the tag name - 'snr', d_me is the block name + unique ID
      pmt::pmt_t d_key, d_me; 

     public:
      mpsk_receiver_snr_est_cc_impl(int tag_nsamples=10000,
        double alpha=0.001);
      ~mpsk_receiver_snr_est_cc_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

     //! Return the estimated signal-to-noise ratio in decibels
     double snr();

     //! Return approximate SNR level used to choose proper estimator
     double level();
     
     //! Return how many samples between SNR tags
     int tag_nsample() const;

     //! Get the running-average coefficient
     double alpha() const;

     //! Set the number of samples between SNR tags
     void set_tag_nsample(int n);

     //! Set the running-average coefficient
     void set_alpha(double alpha);

     //! Set estimator to use
     void set_est();

      //! Count current SNR level
     double count_level(int noutput_items,
           const gr_complex *input,
           double alpha);
    };

  } // namespace snr
} // namespace gr

#endif /* INCLUDED_SNR_MPSK_RECEIVER_SNR_EST_CC_IMPL_H */

