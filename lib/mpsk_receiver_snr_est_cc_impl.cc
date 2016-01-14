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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "mpsk_receiver_snr_est_cc_impl.h"
#include <cstdio>

namespace gr {
  namespace snr {

    mpsk_receiver_snr_est_cc::sptr
    mpsk_receiver_snr_est_cc::make(int tag_nsamples,
         double alpha)
    {
      return gnuradio::get_initial_sptr
   (new mpsk_receiver_snr_est_cc_impl(tag_nsamples, alpha));
    }

    mpsk_receiver_snr_est_cc_impl::mpsk_receiver_snr_est_cc_impl(int tag_nsamples,
         double alpha)
      : gr::sync_block("mpsk_receiver_snr_est_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      d_snr_est = NULL;
 
      d_nsamples = tag_nsamples;
      d_count = 0;

      // Set the running-average coefficient
      set_alpha(alpha);

      // Set estimator to use
      set_est();

      // At least 1 estimator has to look back
      set_history(2);

      std::stringstream str;
      str << name() << unique_id();
      d_me = pmt::string_to_symbol(str.str());
      d_key = pmt::string_to_symbol("snr");  
    }

    mpsk_receiver_snr_est_cc_impl::~mpsk_receiver_snr_est_cc_impl()
    {
      if(d_snr_est)
  delete d_snr_est;
    }

    int
    mpsk_receiver_snr_est_cc_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      // This is a pass-through block; copy input to output
      memcpy(output_items[0], input_items[0],
      noutput_items * sizeof(gr_complex));

        const gr_complex *in = (const gr_complex *) input_items[0];

      // Update, calculate, and issue an SNR tag every d_nsamples
      int index = 0, x = 0;
      int64_t nwritten = nitems_written(0);
      while(index + (d_nsamples - d_count) < noutput_items) {
  x = d_nsamples -  d_count;
  nwritten += x;

  // Count current SNR level to choose proper algorithm
  d_snr_level = count_level(x, &in[index], d_alpha);

  // Update the SNR estimate registers from the current input
  d_snr_est->update_general(x, &in[index], d_snr_level);

  // Issue a tag with SNR data
  pmt::pmt_t pmt_snr = pmt::from_double(d_snr_est->snr_general(d_snr_level));
  add_item_tag(0,      // stream ID
         nwritten,     // tag's sample number
         d_key,        // snr key
         pmt_snr,      // SNR
         d_me);        // block src id

  index += x;
  d_count = 0;
      }

      // Keep track of remaining items and update estimators
      x = noutput_items - index;
      d_count += x;
      d_snr_est->update_general(x , &in[index], d_snr_level);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

    double
    mpsk_receiver_snr_est_cc_impl::snr()
    {
      return d_snr_est->snr_general(d_snr_level);
    }

    double
    mpsk_receiver_snr_est_cc_impl::level()
    {
      return d_snr_level;
    }

    int
    mpsk_receiver_snr_est_cc_impl::tag_nsample() const
    {
      return d_nsamples;
    }

    double
    mpsk_receiver_snr_est_cc_impl::alpha() const
    {
      return d_alpha;
    }

    void
    mpsk_receiver_snr_est_cc_impl::set_tag_nsample(int n)
    {
      if(n > 0) {
    d_nsamples = n;
    d_count = 0; // reset state
      }
      else
  throw std::invalid_argument("mpsk_receiver_snr_est_cc_impl: tag_nsamples can't be <= 0\n");
    }

    void
    mpsk_receiver_snr_est_cc_impl::set_alpha(double alpha)
    {
      if((alpha >= 0) && (alpha <= 1.0)) {
  d_alpha = alpha;
  if(d_snr_est)
    d_snr_est->set_alpha(d_alpha);
      }
      else
  throw std::invalid_argument("mpsk_receiver_snr_est_cc_impl: alpha must be in [0,1]\n");
    }

    void
    mpsk_receiver_snr_est_cc_impl::set_est()
    {
      if(d_snr_est)
  delete d_snr_est;
  // Create general SNR estimator
  d_snr_est = new mpsk_snr_est_general(d_alpha);
    }

    double
    mpsk_receiver_snr_est_cc_impl::count_level(int noutput_items,
           const gr_complex *input, double alpha)
    {
      // Use simple algorithm to calculate current SNR level
      d_y1 = 0;
      d_y2 = 0;
      d_alpha = alpha;
      d_beta = 1.0 - alpha;
      for(int i = 0; i < noutput_items; i++) {
        double x = abs(input[i+1]);
        double x1 = abs(input[i]);
        double y1 = (x*x)*(x1*x1);
        d_y1 = d_alpha*y1 + d_beta*d_y1;

        double y2 = x*x*x*x;
        d_y2 = d_alpha*y2 + d_beta*d_y2;
        }
      double x = d_y1 / (d_y2 - d_y1);
      return 10.0*log10(x - 1 + sqrt(x*(x-1)));
    }


  } /* namespace snr */
} /* namespace gr */

