#ifndef GAIA_STAR_H__
#define GAIA_STAR_H__

#include <stdbool.h>

#include "sllist.h"

typedef struct
{
  // This data is the default show                                                                                                                                                                                                                                              
  long source_id;
  double ref_epoch;
  double ra;
  double ra_error;
  double dec;
  double dec_error;
  double parallax;
  double parallax_error;
  double pmra;
  double pmra_error;
  double pmdec;
  double pmdec_error;
  double astrometric_excess_noise;
  double astrometric_excess_noise_sig;
  bool astrometric_primary_flag;
  //109 bytes                                                                                                                                                                                                                                                                   

  // This data below is optional                                                                                                                                                                                                                                                
  int phot_g_n_obs;
  double phot_g_mean_flux;
  double phot_g_mean_flux_error;
  float phot_g_mean_flux_over_error;
  float phot_g_mean_mag;
  int phot_bp_n_obs;
  double phot_bp_mean_flux;
  double phot_bp_mean_flux_error;
  float phot_bp_mean_flux_over_error;
  float phot_bp_mean_mag;
  int phot_rp_n_obs;
  double phot_rp_mean_flux;
  double phot_rp_mean_flux_error;
  float phot_rp_mean_flux_over_error;
  float phot_rp_mean_mag;
  float phot_bp_rp_excess_factor;
  double radial_velocity;
  double radial_velocity_error;
  bool phot_variable_flag;//false for NOT_AVAILABLE and true for VARIABLE                                                                                                                                                                                                       
  float teff_val;
  float teff_percentile_lower;
  float teff_percentile_upper;
  float a_g_val;
  float a_g_percentile_lower;
  float a_g_percentile_upper;
  float e_bp_min_rp_val;
  float e_bp_min_rp_percentile_lower;
  float e_bp_min_rp_percentile_upper;
  float radius_val;
  float radius_percentile_lower;
  float radius_percentile_upper;
  float lum_val;
  float lum_percentile_lower;
  float lum_percentile_upper;
  //165 bytes                                                                                                                                                                                                                                                                   
  //total 296 bytes including struct alignment                                                                                                                                                                                                                                                           

} gaiastar;


// comparing stars based on ra for sort
int starcmp(const void * a, const void * b);

// precess list according to given equinox
void gaia2_precesslist(gaiastar stars[], double JD/* Julian days */,int count);

#endif

