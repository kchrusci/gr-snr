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


#ifndef INCLUDED_SNR_MPSK_RECEIVER_SNR_EST_CC_H
#define INCLUDED_SNR_MPSK_RECEIVER_SNR_EST_CC_H

#include <snr/api.h>
#include <gnuradio/sync_block.h>
#include <snr/mpsk_snr_est.h>

namespace gr {
  namespace snr {

    /*!
     * \brief A block for computing SNR of a signal.
     * \ingroup snr
     * \details
     * This block can be used to monitor and retrieve estimations of
     * the signal SNR. It is designed to work in a flowgraph and
     * passes all incoming data along to its output.
     * The block is designed for use with M-PSK signals
     * especially and with AWGN channel.
     */
    class SNR_API mpsk_receiver_snr_est_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<mpsk_receiver_snr_est_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of snr::mpsk_receiver_snr_est_cc.   
       *  \param tag_nsamples: after this many samples, a tag containing
       *  the SNR (key='snr') will be sent
       *  \param alpha: the update rate of internal running average
       *  calculations
       */
      static sptr make(int tag_nsamples=10000,
        double alpha=0.001);
      //! Return the estimated signal-to-noise ratio in decibels
      virtual double snr() = 0;

      //! Return approximate SNR level used to choose proper estimator
      virtual double level() = 0;

      //! Return how many samples between SNR tags
      virtual int tag_nsample() const = 0;

      //! Return the running-average coefficient
      virtual double alpha() const = 0;

      //! Set the number of samples between SNR tags
      virtual void set_tag_nsample(int n) = 0;

      //! Set the running-average coefficient
      virtual void set_alpha(double alpha) = 0;

      //! Set estimator to use
      virtual void set_est() = 0;

      //! Count current SNR level
      virtual double count_level(int noutput_items,
           const gr_complex *input,
           double alpha) = 0;

    };

  } // namespace snr
} // namespace gr

#endif /* INCLUDED_SNR_MPSK_RECEIVER_SNR_EST_CC_H */

