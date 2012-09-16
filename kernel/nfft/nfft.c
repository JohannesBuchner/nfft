/**
 * Simple and fast computation of the NDFT.
 * authors: D. Potts, S. Kunis (c) 2002-2006
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "options.h"
#include "window_defines.h"

#include "nfft3.h"

/** direct computation of non equispaced fourier transforms
 *  ndft_trafo, ndft_conjugated, ndft_adjoint, ndft_transposed
 *  require O(M_total N^d) arithemtical operations
 *
 * direct computation of the ndft_trafo and ndft_conjugated, formula (1.1)
 * ndft_trafo:
 * for j=0,...,M_total-1                                                       
 *  f[j] = sum_{k in I_N^d} f_hat[k] * exp(-2 (pi) k x[j])
 * ndft_conjugated:
 * for j=0,...,M_total-1                                                       
 *  f[j] = sum_{k in I_N^d} f_hat[k] * exp(+2 (pi) k x[j])
 *
 * direct computation of the ndft_adjoint and ndft_transposed, formula (1.2)
 * ndft_adjoint:
 * for k in I_N^d
 *  f_hat[k] = sum_{j=0}^{M_total-1} f[j] * exp(+2(pi) k x[j])
 * ndft_transposed:
 * for k in I_N^d
 *  f_hat[k] = sum_{j=0}^{M_total-1} f[j] * exp(-2(pi) k x[j])
 */

/** macros and small sub routines for the direct transforms
 */
#define MACRO_ndft_init_result_trafo memset(f,0,ths->M_total*                 \
                                            sizeof(double complex));
#define MACRO_ndft_init_result_conjugated MACRO_ndft_init_result_trafo
#define MACRO_ndft_init_result_adjoint memset(f_hat,0,ths->N_total*           \
					      sizeof(double complex));
#define MACRO_ndft_init_result_transposed MACRO_ndft_init_result_adjoint

#define MACRO_ndft_sign_trafo      +2*PI*ths->x[j*ths->d+t]
#define MACRO_ndft_sign_conjugated -2*PI*ths->x[j*ths->d+t]
#define MACRO_ndft_sign_adjoint    +2*PI*ths->x[j*ths->d+t]
#define MACRO_ndft_sign_transposed -2*PI*ths->x[j*ths->d+t]

#define MACRO_init_k_N_Omega_x(which_one) {                                   \
for(t=0; t<ths->d; t++)                                                       \
  {                                                                           \
    k[t]=-ths->N[t]/2;                                                        \
    x[t]= MACRO_ndft_sign_ ## which_one;                                      \
    Omega[t+1]=k[t]*x[t]+Omega[t];                                            \
  }                                                                           \
omega=Omega[ths->d];                                                          \
}                                                                             \

#define MACRO_count_k_N_Omega {                                               \
for(t = ths->d-1; (t >= 1) && (k[t] == ths->N[t]/2-1); t--)                   \
  k[t]-= ths->N[t]-1;                                                         \
                                                                              \
k[t]++;                                                                       \
                                                                              \
for(t2 = t; t2<ths->d; t2++)                                                  \
  Omega[t2+1]=k[t2]*x[t2]+Omega[t2];                                          \
                                                                              \
omega=Omega[ths->d];                                                          \
}

#define MACRO_ndft_compute_trafo (*fj) += (*f_hat_k)*cexp(-I*omega);

#define MACRO_ndft_compute_conjugated MACRO_ndft_compute_trafo

#define MACRO_ndft_compute_adjoint (*f_hat_k) += (*fj)*cexp(+I*omega);

#define MACRO_ndft_compute_transposed MACRO_ndft_compute_adjoint

#define MACRO_ndft(which_one)                                                 \
void ndft_ ## which_one (nfft_plan *ths)                                      \
{                                                                             \
  int j;                                /**< index over all nodes           */\
  int t,t2;                             /**< index for dimensions           */\
  int k_L;                              /**< plain index for summation      */\
  double complex *f_hat, *f;            /**< dito                           */\
  double complex *f_hat_k;              /**< actual Fourier coefficient     */\
  double complex *fj;                   /**< actual sample                  */\
  double x[ths->d];                     /**< actual node x[d*j+t]           */\
  int k[ths->d];                        /**< multi index for summation      */\
  double omega, Omega[ths->d+1];        /**< sign times 2*pi*k*x            */\
                                                                              \
  f_hat=ths->f_hat; f=ths->f;                                                 \
                                                                              \
  MACRO_ndft_init_result_ ## which_one                                        \
                                                                              \
  if(ths->d==1) /* univariate case (due to performance) */                    \
    {                                                                         \
      t=0;                                                                    \
      for(j=0, fj = f; j<ths->M_total; j++, fj++)                             \
        {                                                                     \
	  for(k_L=0, f_hat_k = f_hat; k_L<ths->N_total; k_L++, f_hat_k++)     \
	    {                                                                 \
	      omega=(k_L-ths->N_total/2)* MACRO_ndft_sign_ ## which_one;      \
              MACRO_ndft_compute_ ## which_one;                               \
	    }                                                                 \
        }                                                                     \
    }                                                                         \
  else /* multivariate case */					              \
    {                                                                         \
      Omega[0]=0;                                                             \
      for(j=0, fj=f; j<ths->M_total; j++, fj++)                               \
        {                                                                     \
          MACRO_init_k_N_Omega_x(which_one);                                  \
          for(k_L=0, f_hat_k=f_hat; k_L<ths->N_total; k_L++, f_hat_k++)       \
	    {                                                                 \
              MACRO_ndft_compute_ ## which_one;                               \
	      MACRO_count_k_N_Omega;                                          \
	    } /* for(k_L) */                                                  \
        } /* for(j) */                                                        \
    } /* else */                                                              \
} /* ndft_trafo */


/** user routines
 */
MACRO_ndft(trafo)
MACRO_ndft(adjoint)

/** fast computation of non equispaced fourier transforms
 *  require O(N^d log(N) + M_total) arithemtical operations
 *
 * fast computation of the nfft_trafo and nfft_conjugated, formula (1.1)
 * nfft_trafo:
 * for j=0,...,M_total-1                                                       
 *  f[j] = sum_{k in I_N^d} f_hat[k] * exp(-2 (pi) k x[j])
 * nfft_conjugated:
 * for j=0,...,M_total-1                                                       
 *  f[j] = sum_{k in I_N^d} f_hat[k] * exp(+2 (pi) k x[j])
 *
 * direct computation of the nfft_adjoint and nfft_transposed, formula (1.2)
 * nfft_adjoint:
 * for k in I_N^d
 *  f_hat[k] = sum_{j=0}^{M_total-1} f[j] * exp(+2(pi) k x[j])
 * nfft_transposed:
 * for k in I_N^d
 *  f_hat[k] = sum_{j=0}^{M_total-1} f[j] * exp(-2(pi) k x[j])
 */

/** macros and small sub routines for the fast transforms
 */

/** computes 2m+2 indices for the matrix B
 */
void nfft_uo(nfft_plan *ths,int j,int *up,int *op,int act_dim)
{
  double c;
  int u,o;

  c = ths->x[j*ths->d+act_dim] * ths->n[act_dim];
  u = c; o = c;

  if(c < 0)                  
    u = u-1;                  
  else
    o = o+1;

  u = u - (ths->m); o = o + (ths->m);

  up[0]=u; op[0]=o;
}

#define MACRO_nfft_D_compute_A {                                              \
 g_hat[k_plain[ths->d]] = f_hat[ks_plain[ths->d]] * c_phi_inv_k[ths->d];      \
}

#define MACRO_nfft_D_compute_T {                                              \
 f_hat[ks_plain[ths->d]] = g_hat[k_plain[ths->d]] * c_phi_inv_k[ths->d];      \
}

#define MACRO_nfft_D_init_result_A  memset(g_hat,0,ths->n_total*              \
					   sizeof(double complex));
#define MACRO_nfft_D_init_result_T memset(f_hat,0,ths->N_total*               \
                                           sizeof(double complex));

#define MACRO_with_PRE_PHI_HUT * ths->c_phi_inv[t2][ks[t2]];
#define MACRO_without_PRE_PHI_HUT / (PHI_HUT(ks[t2]-ths->N[t2]/2,t2));

#define MACRO_init_k_ks {                                                     \
  for(t = ths->d-1; t>=0; t--)                                                \
    {                                                                         \
      kp[t]= 0;                                                               \
      k[t] = 0;                                                               \
      ks[t] = ths->N[t]/2;                                                    \
    }                                                                         \
  t++;                                                                        \
}

#define MACRO_update_c_phi_inv_k(which_one) {                                 \
  for(t2=t; t2<ths->d; t2++)                                                  \
    {                                                                         \
      c_phi_inv_k[t2+1]= c_phi_inv_k[t2] MACRO_ ##which_one;                  \
      ks_plain[t2+1]= ks_plain[t2]*ths->N[t2]+ks[t2];                         \
      k_plain[t2+1]= k_plain[t2]*ths->n[t2]+k[t2];                            \
    }                                                                         \
}

#define MACRO_count_k_ks {                                                    \
  for(t=ths->d-1; (t>0)&& (kp[t]==ths->N[t]-1); t--)                          \
    {                                                                         \
      kp[t]= 0;                                                               \
      k[t]= 0;                                                                \
      ks[t]= ths->N[t]/2;                                                     \
    }                                                                         \
                                                                              \
  kp[t]++; k[t]++; ks[t]++;                                                   \
  if(kp[t]==ths->N[t]/2)                                                      \
    {                                                                         \
      k[t]= ths->n[t]-ths->N[t]/2;                                            \
      ks[t]= 0;                                                               \
    }                                                                         \
}                                                                             \


/** sub routines for the fast transforms
 *  matrix vector multiplication with \f$D, D^T\f$
 */
#define MACRO_nfft_D(which_one)                                               \
inline void nfft_D_ ## which_one (nfft_plan *ths)                             \
{                                                                             \
  int t, t2;                            /**< index dimensions               */\
  int k_L;                              /**< plain index                    */\
  int kp[ths->d];                       /**< multi index (simple)           */\
  int k[ths->d];                        /**< multi index in g_hat           */\
  int ks[ths->d];                       /**< multi index in f_hat, c_phi_inv*/\
  double c_phi_inv_k[ths->d+1];         /**< postfix product of PHI_HUT     */\
  int k_plain[ths->d+1];                /**< postfix plain index            */\
  int ks_plain[ths->d+1];               /**< postfix plain index            */\
  double complex *f_hat, *g_hat;        /**< local copy                     */\
                                                                              \
  f_hat=ths->f_hat; g_hat=ths->g_hat;                                         \
  MACRO_nfft_D_init_result_ ## which_one;                                     \
                                                                              \
  c_phi_inv_k[0]=1;                                                           \
  k_plain[0]=0;                                                               \
  ks_plain[0]=0;                                                              \
                                                                              \
  if(ths->nfft_flags & PRE_PHI_HUT)                                           \
    {                                                                         \
      MACRO_init_k_ks;                                                        \
                                                                              \
      for(k_L=0; k_L<ths->N_total; k_L++)                                     \
	{                                                                     \
          MACRO_update_c_phi_inv_k(with_PRE_PHI_HUT);                         \
                                                                              \
	  MACRO_nfft_D_compute_ ## which_one;                                 \
	                                                                      \
	  MACRO_count_k_ks;                                                   \
	} /* for(k_L) */                                                      \
    } /* if(PRE_PHI_HUT) */                                                   \
  else                                                                        \
    {                                                                         \
      MACRO_init_k_ks;                                                        \
                                                                              \
      for(k_L=0; k_L<ths->N_total; k_L++)                                     \
	{                                                                     \
          MACRO_update_c_phi_inv_k(without_PRE_PHI_HUT);                      \
                                                                              \
	  MACRO_nfft_D_compute_ ## which_one;                                 \
	                                                                      \
	  MACRO_count_k_ks;                                                   \
	} /* for(k_L) */                                                      \
    } /* else(PRE_PHI_HUT) */                                                 \
} /* nfft_D */

MACRO_nfft_D(A)
MACRO_nfft_D(T)

/** sub routines for the fast transforms
 *  matrix vector multiplication with \f$B, B^{\rm T}\f$
 */ 
#define MACRO_nfft_B_init_result_A  memset(f,0,ths->M_total*                  \
                                           sizeof(double complex));
#define MACRO_nfft_B_init_result_T memset(g,0,ths->n_total*                   \
                                          sizeof(double complex));

#define MACRO_nfft_B_PRE_FULL_PSI_compute_A {                                 \
  (*fj) += ths->psi[ix] * g[ths->psi_index_g[ix]];			      \
}

#define MACRO_nfft_B_PRE_FULL_PSI_compute_T {                                 \
  g[ths->psi_index_g[ix]] += ths->psi[ix] * (*fj);                            \
}

#define MACRO_nfft_B_compute_A {                                              \
  (*fj) += phi_prod[ths->d] * g[ll_plain[ths->d]];                            \
}

#define MACRO_nfft_B_compute_T {                                              \
  g[ll_plain[ths->d]] += phi_prod[ths->d] * (*fj);                            \
}

#define MACRO_with_FG_PSI fg_psi[t2][lj[t2]]

#define MACRO_with_PRE_PSI     ths->psi[(j*ths->d+t2)*(2*ths->m+2)+lj[t2]]

#define MACRO_without_PRE_PSI  PHI(ths->x[j*ths->d+t2]-                       \
                                   ((double)l[t2])/ths->n[t2], t2)

#define MACRO_init_uo_l_lj_t {                                                \
  for(t = ths->d-1; t>=0; t--)                                                \
    {                                                                         \
      nfft_uo(ths,j,&u[t],&o[t],t);                                           \
      l[t] = u[t];                                                            \
      lj[t] = 0;                                                              \
    } /* for(t) */                                                            \
  t++;                                                                        \
}

#define MACRO_update_phi_prod_ll_plain(which_one) {                           \
  for(t2=t; t2<ths->d; t2++)                                                  \
    {                                                                         \
      phi_prod[t2+1]=phi_prod[t2]* MACRO_ ## which_one;                       \
      ll_plain[t2+1]=ll_plain[t2]*ths->n[t2] +(l[t2]+ths->n[t2])%ths->n[t2];  \
    } /* for(t2) */                                                           \
}

#define MACRO_count_uo_l_lj_t {                                               \
  for(t = ths->d-1; (t>0)&&(l[t]==o[t]); t--)                                 \
    {                                                                         \
      l[t] = u[t];                                                            \
      lj[t] = 0;                                                              \
    } /* for(t) */                                                            \
                                                                              \
  l[t]++;                                                                     \
  lj[t]++;                                                                    \
}

#define MACRO_nfft_B(which_one)                                               \
inline void nfft_B_ ## which_one (nfft_plan *ths)                             \
{                                                                             \
  int lprod;                            /**< 'regular bandwidth' of matrix B*/\
  int u[ths->d], o[ths->d];             /**< multi band with respect to x_j */\
  int t, t2;                            /**< index dimensions               */\
  int j;                                /**< index nodes                    */\
  int l_L, ix;                          /**< index one row of B             */\
  int l[ths->d];                        /**< multi index u<=l<=o            */\
  int lj[ths->d];                       /**< multi index 0<=lj<u+o+1        */\
  int ll_plain[ths->d+1];               /**< postfix plain index in g       */\
  double phi_prod[ths->d+1];            /**< postfix product of PHI         */\
  double complex *f, *g;                /**< local copy                     */\
  double complex *fj;                   /**< local copy                     */\
  double y[ths->d];                                                           \
  double fg_psi[ths->d][2*ths->m+2];                                          \
  double fg_exp_l[ths->d][2*ths->m+2];                                        \
  int l_fg,lj_fg;                                                             \
  double tmpEXP1, tmpEXP2, tmpEXP2sq, tmp1, tmp2, tmp3;                       \
  double ip_w;                                                                \
  int ip_u;                                                                   \
  int ip_s=ths->K/(ths->m+1);                                                 \
                                                                              \
  f=ths->f; g=ths->g;                                                         \
                                                                              \
  MACRO_nfft_B_init_result_ ## which_one;                                     \
                                                                              \
  if(ths->nfft_flags & PRE_FULL_PSI)                                          \
    {                                                                         \
      for(ix=0, j=0, fj=f; j<ths->M_total; j++, fj++)                         \
        for(l_L=0; l_L<ths->psi_index_f[j]; l_L++, ix++)                      \
	  MACRO_nfft_B_PRE_FULL_PSI_compute_ ## which_one;                    \
      return;                                                                 \
    }                                                                         \
                                                                              \
  phi_prod[0]=1;                                                              \
  ll_plain[0]=0;                                                              \
                                                                              \
  for(t=0,lprod = 1; t<ths->d; t++)                                           \
    lprod *= (2*ths->m+2);                                                    \
                                                                              \
  if(ths->nfft_flags & PRE_PSI)                                               \
    {                                                                         \
      for(j=0, fj=f; j<ths->M_total; j++, fj++)                               \
	{                                                                     \
          MACRO_init_uo_l_lj_t;                                               \
                                                                              \
	  for(l_L=0; l_L<lprod; l_L++)                                        \
	    {                                                                 \
              MACRO_update_phi_prod_ll_plain(with_PRE_PSI);                   \
                                                                              \
	      MACRO_nfft_B_compute_ ## which_one;                             \
		                                                              \
	      MACRO_count_uo_l_lj_t;                                          \
            } /* for(l_L) */                                                  \
	} /* for(j) */                                                        \
      return;                                                                 \
    } /* if(PRE_PSI) */                                                       \
                                                                              \
  if(ths->nfft_flags & PRE_FG_PSI)                                            \
    {                                                                         \
      for(t2=0; t2<ths->d; t2++)                                              \
        {                                                                     \
          tmpEXP2 = exp(-1.0/ths->b[t2]);                                     \
          tmpEXP2sq = tmpEXP2*tmpEXP2;                                        \
          tmp2 = 1.0;                                                         \
          tmp3 = 1.0;                                                         \
          fg_exp_l[t2][0] = 1.0;                                              \
          for(lj_fg=1; lj_fg <= (2*ths->m+2); lj_fg++)                        \
            {                                                                 \
              tmp3 = tmp2*tmpEXP2;                                            \
              tmp2 *= tmpEXP2sq;                                              \
              fg_exp_l[t2][lj_fg] = fg_exp_l[t2][lj_fg-1]*tmp3;               \
            }                                                                 \
        }                                                                     \
      for(j=0, fj=f; j<ths->M_total; j++, fj++)                               \
	{                                                                     \
          MACRO_init_uo_l_lj_t;                                               \
                                                                              \
          for(t2=0; t2<ths->d; t2++)                                          \
            {                                                                 \
              fg_psi[t2][0] = ths->psi[2*(j*ths->d+t2)];                      \
              tmpEXP1 = ths->psi[2*(j*ths->d+t2)+1];                          \
              tmp1 = 1.0;                                                     \
              for(l_fg=u[t2]+1, lj_fg=1; l_fg <= o[t2]; l_fg++, lj_fg++)      \
                {                                                             \
                  tmp1 *= tmpEXP1;                                            \
                  fg_psi[t2][lj_fg] = fg_psi[t2][0]*tmp1*fg_exp_l[t2][lj_fg]; \
                }                                                             \
            }                                                                 \
                                                                              \
	  for(l_L=0; l_L<lprod; l_L++)                                        \
	    {                                                                 \
              MACRO_update_phi_prod_ll_plain(with_FG_PSI);                    \
                                                                              \
	      MACRO_nfft_B_compute_ ## which_one;                             \
		                                                              \
	      MACRO_count_uo_l_lj_t;                                          \
            } /* for(l_L) */                                                  \
	} /* for(j) */                                                        \
      return;                                                                 \
    } /* if(PRE_FG_PSI) */                                                    \
                                                                              \
  if(ths->nfft_flags & FG_PSI)                                                \
    {                                                                         \
      for(t2=0; t2<ths->d; t2++)                                              \
        {                                                                     \
          tmpEXP2 = exp(-1.0/ths->b[t2]);                                     \
          tmpEXP2sq = tmpEXP2*tmpEXP2;                                        \
          tmp2 = 1.0;                                                         \
          tmp3 = 1.0;                                                         \
          fg_exp_l[t2][0] = 1.0;                                              \
          for(lj_fg=1; lj_fg <= (2*ths->m+2); lj_fg++)                        \
            {                                                                 \
              tmp3 = tmp2*tmpEXP2;                                            \
              tmp2 *= tmpEXP2sq;                                              \
              fg_exp_l[t2][lj_fg] = fg_exp_l[t2][lj_fg-1]*tmp3;               \
            }                                                                 \
        }                                                                     \
      for(j=0, fj=f; j<ths->M_total; j++, fj++)                               \
	{                                                                     \
          MACRO_init_uo_l_lj_t;                                               \
                                                                              \
          for(t2=0; t2<ths->d; t2++)                                          \
            {                                                                 \
              fg_psi[t2][0] =                                                 \
                (PHI((ths->x[j*ths->d+t2]-((double)u[t2])/ths->n[t2]),t2));   \
                                                                              \
              tmpEXP1 = exp(2.0*(ths->n[t2]*ths->x[j*ths->d+t2] - u[t2])      \
                      / ths->b[t2]);                                          \
              tmp1 = 1.0;                                                     \
              for(l_fg=u[t2]+1, lj_fg=1; l_fg <= o[t2]; l_fg++, lj_fg++)      \
                {                                                             \
                  tmp1 *= tmpEXP1;                                            \
                  fg_psi[t2][lj_fg] = fg_psi[t2][0]*tmp1*fg_exp_l[t2][lj_fg]; \
                }                                                             \
            }                                                                 \
                                                                              \
	  for(l_L=0; l_L<lprod; l_L++)                                        \
	    {                                                                 \
              MACRO_update_phi_prod_ll_plain(with_FG_PSI);                    \
                                                                              \
	      MACRO_nfft_B_compute_ ## which_one;                             \
		                                                              \
	      MACRO_count_uo_l_lj_t;                                          \
            } /* for(l_L) */                                                  \
	} /* for(j) */                                                        \
      return;                                                                 \
    } /* if(FG_PSI) */                                                        \
                                                                              \
                                                                              \
  if(ths->nfft_flags & PRE_LIN_PSI)                                           \
    {                                                                         \
      for(j=0, fj=f; j<ths->M_total; j++, fj++)                               \
	{                                                                     \
          MACRO_init_uo_l_lj_t;                                               \
                                                                              \
          for(t2=0; t2<ths->d; t2++)                                          \
            {                                                                 \
              y[t2] = ((ths->n[t2]*ths->x[j*ths->d+t2]-                       \
                          (double)u[t2]) * ((double)ths->K))/(ths->m+1);      \
              ip_u  = (int)floor(y[t2]);                                      \
              ip_w  = y[t2]-ip_u;                                             \
              for(l_fg=u[t2], lj_fg=0; l_fg <= o[t2]; l_fg++, lj_fg++)        \
                {                                                             \
                  fg_psi[t2][lj_fg] = ths->psi[(ths->K+1)*t2+                 \
					       abs(ip_u-lj_fg*ip_s)]*         \
                                      (1-ip_w) +                              \
                                      ths->psi[(ths->K+1)*t2+                 \
					       abs(ip_u-lj_fg*ip_s+1)]*       \
                                       (ip_w);                                \
              }                                                               \
            }                                                                 \
                                                                              \
	  for(l_L=0; l_L<lprod; l_L++)                                        \
	    {                                                                 \
              MACRO_update_phi_prod_ll_plain(with_FG_PSI);                    \
                                                                              \
	      MACRO_nfft_B_compute_ ## which_one;                             \
		                                                              \
	      MACRO_count_uo_l_lj_t;                                          \
            } /* for(l_L) */                                                  \
	} /* for(j) */                                                        \
      return;                                                                 \
    } /* if(PRE_LIN_PSI) */                                                   \
                                                                              \
  /* no precomputed psi at all */                                             \
  for(j=0, fj=f; j<ths->M_total; j++, fj++)                                   \
    {                                                                         \
      MACRO_init_uo_l_lj_t;                                                   \
	                                                                      \
      for(l_L=0; l_L<lprod; l_L++)                                            \
     	{                                                                     \
          MACRO_update_phi_prod_ll_plain(without_PRE_PSI);                    \
                                                                              \
          MACRO_nfft_B_compute_ ## which_one;                                 \
		                                                              \
          MACRO_count_uo_l_lj_t;                                              \
	} /* for(l_L) */                                                      \
    } /* for(j) */                                                            \
} /* nfft_B */                                                                \

MACRO_nfft_B(A)
MACRO_nfft_B(T)


/** user routines
 */
void nfft_trafo(nfft_plan *ths)
{
  /* use ths->my_fftw_plan1 */
  ths->g_hat=ths->g1;
  ths->g=ths->g2;
 
  /** form \f$ \hat g_k = \frac{\hat f_k}{c_k\left(\phi\right)} \text{ for }
   *  k \in I_N \f$
   */ 
  TIC(0)
  nfft_D_A(ths);
  TOC(0)

  /** compute by d-variate discrete Fourier transform
   *  \f$ g_l = \sum_{k \in I_N} \hat g_k {\rm e}^{-2\pi {\rm i} \frac{kl}{n}}
   *  \text{ for } l \in I_n \f$
   */
  TIC_FFTW(1)
  fftw_execute(ths->my_fftw_plan1);
  TOC_FFTW(1)

  /** set \f$ f_j =\sum_{l \in I_n,m(x_j)} g_l \psi\left(x_j-\frac{l}{n}\right)
   *  \text{ for } j=0,\hdots,M_total-1 \f$
   */
  TIC(2)
  nfft_B_A(ths);
  TOC(2)
} /* nfft_trafo */

void nfft_adjoint(nfft_plan *ths)
{
  /* use ths->my_fftw_plan2 */
  ths->g_hat=ths->g1;
  ths->g=ths->g2;
  
  /** set \f$ g_l = \sum_{j=0}^{M_total-1} f_j \psi\left(x_j-\frac{l}{n}\right)
   *  \text{ for } l \in I_n,m(x_j) \f$
   */
  TIC(2)
  nfft_B_T(ths);
  TOC(2)
 
  /** compute by d-variate discrete Fourier transform
   *  \f$ \hat g_k = \sum_{l \in I_n} g_l {\rm e}^{+2\pi {\rm i} \frac{kl}{n}}
   *  \text{ for }  k \in I_N\f$
   */
  TIC_FFTW(1)
  fftw_execute(ths->my_fftw_plan2);
  TOC_FFTW(1)
 
  /** form \f$ \hat f_k = \frac{\hat g_k}{c_k\left(\phi\right)} \text{ for }
   *  k \in I_N \f$
   */
  TIC(0)
  nfft_D_T(ths);
  TOC(0)
} /* nfft_adjoint */

/** initialisation of direct transform 
 */
void nfft_precompute_phi_hut(nfft_plan *ths)
{
  int ks[ths->d];                       /**< index over all frequencies      */
  int t;                                /**< index over all dimensions       */

  ths->c_phi_inv = (double**) fftw_malloc(ths->d*sizeof(double*));

  for(t=0; t<ths->d; t++)
    {
      ths->c_phi_inv[t]= (double*)fftw_malloc(ths->N[t]*sizeof(double));
      for(ks[t]=0; ks[t]<ths->N[t]; ks[t]++)  
	ths->c_phi_inv[t][ks[t]]= 1.0/(PHI_HUT(ks[t]-ths->N[t]/2,t));
    }
} /* nfft_phi_hut */

/** create a lookup table, but NOT for each node
 *  good idea K=2^xx
 *  TODO: estimate K, call from init
 *  assumes an EVEN window function
 */
void nfft_precompute_lin_psi(nfft_plan *ths)
{
  int t;                                /**< index over all dimensions       */
  int j;                                /**< index over all nodes            */
  double step;                          /**< step size in [0,(m+1)/n]        */

  for (t=0; t<ths->d; t++)
    {
      step=((double)(ths->m+1))/(ths->K*ths->n[t]);
      for(j=0;j<=ths->K;j++)
	{
	  ths->psi[(ths->K+1)*t + j] = PHI(j*step,t);
	} /* for(j) */
    } /* for(t) */
}

void nfft_precompute_fg_psi(nfft_plan *ths)
{
  int t;                                /**< index over all dimensions       */
  int j;                                /**< index over all nodes            */
  int u, o;                             /**< depends on x_j                  */
  
  for (t=0; t<ths->d; t++)
    for(j=0;j<ths->M_total;j++)
      {
	nfft_uo(ths,j,&u,&o,t);

        ths->psi[2*(j*ths->d+t)]=
            (PHI((ths->x[j*ths->d+t]-((double)u)/ths->n[t]),t));

        ths->psi[2*(j*ths->d+t)+1]=
            exp(2.0*(ths->n[t]*ths->x[j*ths->d+t] - u) / ths->b[t]);   
      } /* for(j) */
  /* for(t) */
} /* nfft_precompute_fg_psi */

void nfft_precompute_psi(nfft_plan *ths)
{
  int t;                                /**< index over all dimensions       */
  int j;                                /**< index over all nodes            */
  int l;                                /**< index u<=l<=o                   */
  int lj;                               /**< index 0<=lj<u+o+1               */
  int u, o;                             /**< depends on x_j                  */
  
  for (t=0; t<ths->d; t++)
    for(j=0;j<ths->M_total;j++)
      {
	nfft_uo(ths,j,&u,&o,t);
	
	for(l=u, lj=0; l <= o; l++, lj++)
	  ths->psi[(j*ths->d+t)*(2*ths->m+2)+lj]=
	    (PHI((ths->x[j*ths->d+t]-((double)l)/ths->n[t]),t));
      } /* for(j) */
  /* for(t) */
} /* nfft_precompute_psi */

void nfft_precompute_full_psi(nfft_plan *ths)
{
  int t,t2;                             /**< index over all dimensions       */
  int j;                                /**< index over all nodes            */
  int l_L;                              /**< plain index 0<=l_L<lprod        */
  int l[ths->d];                        /**< multi index u<=l<=o             */
  int lj[ths->d];                       /**< multi index 0<=lj<u+o+1         */
  int ll_plain[ths->d+1];               /**< postfix plain index             */
  int lprod;                            /**< 'bandwidth' of matrix B         */
  int u[ths->d], o[ths->d];             /**< depends on x_j                  */
  
  double phi_prod[ths->d+1];

  int ix,ix_old;

  phi_prod[0]=1;
  ll_plain[0]=0;

  for(t=0,lprod = 1; t<ths->d; t++)
      lprod *= 2*ths->m+2;

  for(j=0,ix=0,ix_old=0; j<ths->M_total; j++)
    {
      MACRO_init_uo_l_lj_t;
      
      for(l_L=0; l_L<lprod; l_L++, ix++)
	{
	  MACRO_update_phi_prod_ll_plain(without_PRE_PSI);
	  
	  ths->psi_index_g[ix]=ll_plain[ths->d];
	  ths->psi[ix]=phi_prod[ths->d];
	   
	  MACRO_count_uo_l_lj_t;
	} /* for(l_L) */
      
      
      ths->psi_index_f[j]=ix-ix_old;
      ix_old=ix;
    } /* for(j) */
}

void nfft_precompute_one_psi(nfft_plan *ths)
{
  if(ths->nfft_flags & PRE_LIN_PSI)
    nfft_precompute_lin_psi(ths);
  if(ths->nfft_flags & PRE_FG_PSI)
    nfft_precompute_fg_psi(ths);
  if(ths->nfft_flags & PRE_PSI)
    nfft_precompute_psi(ths);
  if(ths->nfft_flags & PRE_FULL_PSI)
    nfft_precompute_full_psi(ths);
}


void nfft_init_help(nfft_plan *ths)
{
  int t;                                /**< index over all dimensions       */
  int lprod;                            /**< 'bandwidth' of matrix B         */

  ths->N_total=nfft_prod_int(ths->N, ths->d);
  ths->n_total=nfft_prod_int(ths->n, ths->d);

  ths->sigma = (double*) fftw_malloc(ths->d*sizeof(double));
  for(t = 0;t < ths->d; t++)
    ths->sigma[t] = ((double)ths->n[t])/ths->N[t];
  
  WINDOW_HELP_INIT;

  if(ths->nfft_flags & MALLOC_X)
    ths->x = (double*)fftw_malloc(ths->d*ths->M_total*sizeof(double));

  if(ths->nfft_flags & MALLOC_F_HAT)
    ths->f_hat = (double complex*)fftw_malloc(ths->N_total*sizeof(double complex));

  if(ths->nfft_flags & MALLOC_F)
    ths->f = (double complex*)fftw_malloc(ths->M_total*sizeof(double complex));

  if(ths->nfft_flags & PRE_PHI_HUT)
    nfft_precompute_phi_hut(ths);

  if(ths->nfft_flags & PRE_LIN_PSI)
  {
      ths->K=(1U<< 10)*(ths->m+1);
      ths->psi = (double*) fftw_malloc((ths->K+1)*ths->d*sizeof(double));
  }

  if(ths->nfft_flags & PRE_FG_PSI)
    ths->psi = (double*) fftw_malloc(ths->M_total*ths->d*2*sizeof(double));

  if(ths->nfft_flags & PRE_PSI)
    ths->psi = (double*) fftw_malloc(ths->M_total*ths->d*
				     (2*ths->m+2)*sizeof(double));

  if(ths->nfft_flags & PRE_FULL_PSI)
  {
      for(t=0,lprod = 1; t<ths->d; t++)
	  lprod *= 2*ths->m+2;
      
      ths->psi = (double*) fftw_malloc(ths->M_total*lprod*sizeof(double));

      ths->psi_index_f = (int*) fftw_malloc(ths->M_total*sizeof(int));
      ths->psi_index_g = (int*) fftw_malloc(ths->M_total*lprod*sizeof(int));
  }

  if(ths->nfft_flags & FFTW_INIT)
      {  
	ths->g1=(double complex*)fftw_malloc(ths->n_total*
					     sizeof(double complex));

	if(ths->nfft_flags & FFT_OUT_OF_PLACE)
	    ths->g2 = (double complex*) fftw_malloc(ths->n_total*
					     sizeof(double complex));
	else
	    ths->g2 = ths->g1;
	
	ths->my_fftw_plan1 = 
	    fftw_plan_dft(ths->d, ths->n, ths->g1, ths->g2,
			  FFTW_FORWARD, ths->fftw_flags);
	ths->my_fftw_plan2 = 
	    fftw_plan_dft(ths->d, ths->n, ths->g2, ths->g1,
			  FFTW_BACKWARD, ths->fftw_flags);
      }
}

void nfft_init(nfft_plan *ths, int d, int *N, int M_total)
{
  int t;                                /**< index over all dimensions       */

  ths->d = d;

  ths->N=(int*) fftw_malloc(d*sizeof(int));
  for(t = 0;t < d; t++)
    ths->N[t] = N[t];

  ths->M_total = M_total;

  ths->n = (int*) fftw_malloc(d*sizeof(int));
  for(t = 0;t < d; t++)
    ths->n[t] = 2*nfft_next_power_of_2(ths->N[t]);

  WINDOW_HELP_ESTIMATE_m;

  ths->nfft_flags = PRE_PHI_HUT| PRE_PSI| MALLOC_X| MALLOC_F_HAT| MALLOC_F|
                    FFTW_INIT| FFT_OUT_OF_PLACE;
  ths->fftw_flags= FFTW_ESTIMATE| FFTW_DESTROY_INPUT;

  nfft_init_help(ths);    
}

void nfft_init_guru(nfft_plan *ths, int d, int *N, int M_total, int *n,
			int m, unsigned nfft_flags, unsigned fftw_flags)
{
  int t;                                /**< index over all dimensions       */

  ths->d =d;
  ths->N= (int*) fftw_malloc(ths->d*sizeof(int));
  for(t=0; t<d; t++)
    ths->N[t]= N[t];
  ths->M_total= M_total;
  ths->n= (int*) fftw_malloc(ths->d*sizeof(int));
  for(t=0; t<d; t++)
    ths->n[t]= n[t];
  ths->m= m;
  ths->nfft_flags= nfft_flags;
  ths->fftw_flags= fftw_flags;

  nfft_init_help(ths);  
}

void nfft_init_1d(nfft_plan *ths, int N1, int M_total)
{
  int N[1];

  N[0]=N1;
  nfft_init(ths,1,N,M_total);
}

void nfft_init_2d(nfft_plan *ths, int N1, int N2, int M_total)
{
  int N[2];

  N[0]=N1;
  N[1]=N2;
  nfft_init(ths,2,N,M_total);
}

void nfft_init_3d(nfft_plan *ths, int N1, int N2, int N3, int M_total)
{
  int N[3];

  N[0]=N1;
  N[1]=N2;
  N[2]=N3;
  nfft_init(ths,3,N,M_total);
}

void nfft_check(nfft_plan *ths)
{
  int j;
 
  for(j=0;j<ths->M_total*ths->d;j++)
    if((ths->x[j]<-0.5) || (ths->x[j]>=0.5))
      fprintf(stderr,"nfft_check: ths->x[%d]=%e out of range [-0.5,0.5)\n",
	      j,ths->x[j]);

  for(j=0;j<ths->d;j++)
    {
      if(ths->sigma[j]<=1)
	fprintf(stderr,"nfft_check: oversampling factor too small\n");

      if(ths->N[j]<=ths->m)
	fprintf(stderr,
		"nfft_check: polynomial degree N is smaller than cut-off m\n");

      if(ths->N[j]%2==1)
	fprintf(stderr,"nfft_check: polynomial degree N has to be even\n");
    }
}

void nfft_finalize(nfft_plan *ths)
{
  int t; /* index over dimensions */

  if(ths->nfft_flags & FFTW_INIT)
    {
	fftw_destroy_plan(ths->my_fftw_plan2);
	fftw_destroy_plan(ths->my_fftw_plan1);

	if(ths->nfft_flags & FFT_OUT_OF_PLACE)
	    fftw_free(ths->g2);
	
	fftw_free(ths->g1);
    }

  if(ths->nfft_flags & PRE_FULL_PSI)
    {
      fftw_free(ths->psi_index_g);
      fftw_free(ths->psi_index_f);
      fftw_free(ths->psi);
    }

  if(ths->nfft_flags & PRE_PSI)
    fftw_free(ths->psi);  

  if(ths->nfft_flags & PRE_FG_PSI)
    fftw_free(ths->psi);

  if(ths->nfft_flags & PRE_LIN_PSI)
    fftw_free(ths->psi);
      
  if(ths->nfft_flags & PRE_PHI_HUT)
    {
      for(t=0; t<ths->d; t++)
        fftw_free(ths->c_phi_inv[t]);
      fftw_free(ths->c_phi_inv);
    }

  if(ths->nfft_flags & MALLOC_F)
    fftw_free(ths->f);

  if(ths->nfft_flags & MALLOC_F_HAT)
    fftw_free(ths->f_hat);

  if(ths->nfft_flags & MALLOC_X)
  fftw_free(ths->x);
 
  WINDOW_HELP_FINALIZE;
 
  fftw_free(ths->sigma);
  fftw_free(ths->n);
  fftw_free(ths->N);
}
