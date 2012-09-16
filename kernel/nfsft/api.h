/** 
 * \file api.h
 * \brief Header file with internal API of the NFSFT module
 * \author Jens Keiner
 */
#ifndef API_H
#define API_H

#include "config.h"
#include "nfft3.h"
#include <fftw3.h>

/** \addtogroup nfsft
 * \{
 */

/* "Default exponent of maximum bandwidth" */
#define BWEXP_MAX 10

/* "Default maximum bandwidth" */
#define BW_MAX 1024

#define ROW(k) (k*(wisdom.N_MAX+2))
#define ROWK(k) (k*(wisdom.N_MAX+2)+k)

#ifdef HAVE_STDBOOL_H
#  include <stdbool.h>
#else
#  warning ISO C99 bool type not available. Defining own bool type.
typedef enum {false = 0,true = 1} bool;
#endif

//#define FIRST_L (int)floor(ntilde/(double)plength)
//#define LAST_L (int)ceil((Mtilde+1)/(double)plength)-1


/** 
 * Wisdom structure 
 */
struct nfsft_wisdom
{
  /** Indicates wether the structure has been initialized. */ 
  bool initialized;
  unsigned int flags;
  /** Stores precomputation flags. */
  /** The maximum bandwidth /f$N_{\text{max}} \in \mathbb{N}_0/f$ */
  int N_MAX;
  /** The logarithm /f$t = \log_2 N_{\text{max}}/f$ of the maximum bandwidth */
  int T_MAX;
  
  /* Data for the direct algorithms */
  
  /** 
   * Precomputed recursion coefficients /f$\alpha_k^n/f$ for /f$k = 0,/ldots,
   * N_{\text{max}}; n=-k,/ldots,k/f$ of associated Legendre-functions 
   * /f$P_k^n/f$ 
   */
  double *alpha;
  /** 
   * Precomputed recursion coefficients /f$\beta_k^n/f$ for /f$k = 0,/ldots,
   * N_{\text{max}}; n=-k,/ldots,k/f$ of associated Legendre-functions 
   * /f$P_k^n/f$ 
   */
  double *beta;
  /** 
   * Precomputed recursion coefficients /f$\gamma_k^n/f$ for /f$k = 0,/ldots,
   * N_{\text{max}}; n=-k,/ldots,k/f$ of associated Legendre-functions 
   * /f$P_k^n/f$ 
   */
  double *gamma;

  /* Data for fast algorithms. */

  /** The threshold /f$\kappa/f$ */
  double threshold;
  /** Structure for \e discrete \e polynomial \e transform (\e DPT) */
  fpt_set set;
};
/* \} */
#endif
