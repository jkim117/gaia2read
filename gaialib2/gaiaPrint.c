#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "gaiastar.h"
#include "gaia2ret.h"
#include "sllist.h"

// format for printing float
local void printFloat(FILE* out, float val, char* format){//format should have a space in front of it
  if (fabs(3.55-val)<1e-7)
    {
      char *naFormat;
      if(strcmp(format," %14.10f")==0)
	naFormat = " %14s";
      else
	naFormat = " %9s";
      char *na = "n/a";
      fprintf(out,naFormat,na);
    }
  else
    fprintf(out,format,val);
}

// format for printing double
local void printDouble(FILE* out, double val, char* format){//format should have a space in front of it
  if (fabs(3.55-val)<1e-7){
    char *naFormat;
    if(strcmp(format," %14.10f")==0)
      naFormat = " %14s";
    else
      naFormat = " %9s";
    char *na = "n/a";
    fprintf(out,naFormat,na);
  }
  else
    fprintf(out,format,val);
}

// default print options
local void print_common(FILE* out, const gaiastar* star, long id,IDType type){
  if(id==0)
    id = star->source_id;
  if(type==GAIA)
    fprintf(out, "%ld",id);
  else if(type==TMASS)
    {
      char tmassID[16];
      char str[16];
      sprintf(str,"%ld",id);
      if(str[0]=='1')
	{
	  tmassID[0]=str[1];
	  tmassID[1]=str[2];
	  tmassID[2]=str[3];
	  tmassID[3]=str[4];
	  tmassID[4]=str[5];
	  tmassID[5]=str[6];
	  tmassID[6]=str[7];
	  tmassID[7]=str[8];
	  tmassID[8]='+';
	  tmassID[9]=str[9];
	  tmassID[10]=str[10];
	  tmassID[11]=str[11];
	  tmassID[12]=str[12];
	  tmassID[13]=str[13];
	  tmassID[14]=str[14];
	  tmassID[15]=str[15];
	}
      else
	{
	  tmassID[0]=str[1];
          tmassID[1]=str[2];
          tmassID[2]=str[3];
          tmassID[3]=str[4];
          tmassID[4]=str[5];
          tmassID[5]=str[6];
          tmassID[6]=str[7];
          tmassID[7]=str[8];
          tmassID[8]='-';
          tmassID[9]=str[9];
          tmassID[10]=str[10];
          tmassID[11]=str[11];
          tmassID[12]=str[12];
          tmassID[13]=str[13];
          tmassID[14]=str[14];
          tmassID[15]=str[15];

	}
      fprintf(out,"%s",tmassID);

    }
  else if(type==HAT)
    {
      char hatID[20];
      char str[20];
      sprintf(str,"%ld",id);
      hatID[0]=str[strlen(str)-3];
      hatID[1]=str[strlen(str)-2];
      hatID[2]=str[strlen(str)-1];
      hatID[3]='-';

      unsigned int i;
      for(i = 0; i<strlen(str)-3;i++)
	{
	  hatID[4+i]=str[i];
	}
      hatID[i+4]='\0';
      fprintf(out,"%s",hatID);
      
    }
  printDouble(out,star->ra," %14.10f");
  printDouble(out,star->dec," %14.10f");
  printDouble(out,star->ra_error," %14.10f");
  printDouble(out,star->dec_error," %14.10f");
  printDouble(out,star->parallax," %9.4f");
  printDouble(out,star->parallax_error," %9.4f");
  printDouble(out,star->pmra," %14.10f");
  printDouble(out,star->pmdec," %14.10f");
  printDouble(out,star->pmra_error," %14.10f");
  printDouble(out,star->pmdec_error," %14.10f");
  printDouble(out,star->ref_epoch," %5.1f");
  printDouble(out,star->astrometric_excess_noise," %14.10f");
  printDouble(out,star->astrometric_excess_noise_sig," %14.10f");
  if(star->astrometric_primary_flag)
    fprintf(out," true");
  else
    fprintf(out," false");
}

// calls default print options
void gaiastar_print(FILE* out, const gaiastar* star, long id,IDType type)
{
  print_common( out, star, id,type);
  fendl( out );
}

// default print options + additional info
void gaiastar_printextra(FILE* out, const gaiastar* star, long id,IDType type)
{
  print_common( out, star, id,type);

  fprintf(out," %d", star->phot_g_n_obs);
  printDouble(out,star->phot_g_mean_flux," %14.10f");
  printDouble(out,star->phot_g_mean_flux_error," %14.10f");
  printFloat(out,star->phot_g_mean_flux_over_error," %14.10f");
  printFloat(out,star->phot_g_mean_mag," %14.10f");

  fprintf(out," %d", star->phot_bp_n_obs);
  printDouble(out,star->phot_bp_mean_flux," %14.10f");
  printDouble(out,star->phot_bp_mean_flux_error," %14.10f");
  printFloat(out,star->phot_bp_mean_flux_over_error," %14.10f");
  printFloat(out,star->phot_bp_mean_mag," %14.10f");

  fprintf(out," %d", star->phot_rp_n_obs);
  printDouble(out,star->phot_rp_mean_flux," %14.10f");
  printDouble(out,star->phot_rp_mean_flux_error," %14.10f");
  printFloat(out,star->phot_rp_mean_flux_over_error," %14.10f");
  printFloat(out,star->phot_rp_mean_mag," %14.10f");

  printFloat(out,star->phot_bp_rp_excess_factor," %14.10f");
  printDouble(out,star->radial_velocity," %14.10f");
  printDouble(out,star->radial_velocity_error," %14.10f");
  if(star->phot_variable_flag)
    fprintf(out," VARIABLE");
  else
    fprintf(out," NOT_AVAILABLE");

  printFloat(out,star->teff_val," %14.10f");
  printFloat(out,star->teff_percentile_lower," %14.10f");
  printFloat(out,star->teff_percentile_upper," %14.10f");
  printFloat(out,star->a_g_val," %14.10f");
  printFloat(out,star->a_g_percentile_lower," %14.10f");
  printFloat(out,star->a_g_percentile_upper," %14.10f");
  printFloat(out,star->e_bp_min_rp_val," %14.10f");
  printFloat(out,star->e_bp_min_rp_percentile_lower," %14.10f");
  printFloat(out,star->e_bp_min_rp_percentile_upper," %14.10f");
  printFloat(out,star->radius_val," %14.10f");
  printFloat(out,star->radius_percentile_lower," %14.10f");
  printFloat(out,star->radius_percentile_upper," %14.10f");
  printFloat(out,star->lum_val," %14.10f");
  printFloat(out,star->lum_percentile_lower," %14.10f");
  printFloat(out,star->lum_percentile_upper," %14.10f");
  fprintf(out,"\n");
}

// print list of stars with Gaia IDs
void gaiastar_printlist(FILE* out, const gaiastar stars[], bool extra,int count)
{
  if ( extra ) {
    for (int i = 0; i < count; i++) {
      //fprintf(out,"GAIA ");
      const gaiastar* star = &stars[i];
      gaiastar_printextra(out, star, 0,GAIA);
    }
  }
  else {
    for (int i = 0; i < count; i++) {
      //fprintf(out,"GAIA ");
      const gaiastar* star = &stars[i];
      gaiastar_print(out, star, 0,GAIA);
    }
  }
}

// -------------------------------------------------------------------------- +                                                                                                                                                                                                 
// print list of stars with specified IDs
void gaiastar_printlist_alternateID(FILE* out, const gaiastar stars[], bool extra, const sllist* alternateIDs, IDType type,int count)
{
  if ( extra ) {
    const sllist* ids = alternateIDs;
    for (int i = 0; i < count; i++ ) {
      const gaiastar* star = &stars[i];
      const long* id = (const long*)ids->data;
      if (type==TMASS && *id!=0)
	fprintf(out,"2MASS ");
      else if(type==HAT && *id!=0)
	fprintf(out,"HAT ");
      else
	fprintf(out,"GAIA ");

      gaiastar_printextra(out, star, *id,type);
      ids = ids->next;
    }
  }
  else {
    const sllist* ids = alternateIDs;
    for (int i = 0; i<count; i++) {
      const gaiastar* star = &stars[i];
      const long* id = (const long*)ids->data;
      if (type==TMASS && *id!=0)
        fprintf(out,"2MASS ");
      else if(type==HAT && *id!=0)
	fprintf(out,"HAT ");
      else
	fprintf(out,"GAIA ");

      gaiastar_print(out, star, *id,type);
      ids = ids->next;
    }
  }
}

// print header
void gaiastar_printheader(FILE* out, bool extra, IDType outType)
{
  char *idString;
  if (outType == GAIA)
    idString = "Gaia";
  else if (outType == HAT)
    idString = "HAT";
  else
    idString = "2MASS";

  // ID and position                                                                                                                                                                                                                                                            
  fprintf(out, "%s ID[1] RA[deg][2] Dec[deg][3] RAError[mas][4] DecError[mas][5] Parallax[mas][6] Parallax_error[mas][7] PM_RA[mas/yr][8] PM_Dec[mas/year][9] PMRA_error[mas/yr][10] PMDec_error[mas/yr][11] Ref_Epoch[yr][12]",idString);
  fprintf(out, " AstExcNoise[mas][13] AstExcNoiseSig[14] AstPriFlag[15] ");

  if (!extra) {
    fendl( out );
    return;
  }


  fprintf(out, "phot_g_n_obs[16] phot_g_mean_flux[17] phot_g_mean_flux_error[18] phot_g_mean_flux_over_error[19] phot_g_mean_mag[20] phot_bp_n_obs[21] phot_bp_mean_flux[22] phot_bp_mean_flux_error[23] phot_bp_mean_flux_over_error[24] phot_bp_mean_mag[25] ");
  fprintf(out, "phot_rp_n_obs[26] phot_rp_mean_flux[27] phot_rp_mean_flux_error[28] phot_rp_mean_flux_over_error[29] phot_rp_mean_mag[30] phot_bp_rp_excess_factor[31] radial_velocity[32] radial_velocity_error[33] phot_variable_flag[34] teff_val[35] teff_percentile_lower[36] ");
  fprintf(out, "teff_percentile_upper[37] a_g_val[38] a_g_percentile_lower[39] a_g_percentile_upper[40] e_bp_min_rp_val[41] e_bp_min_rp_percentile_lower[42] e_bp_min_rp_percentile_upper[43] radius_val[44] radius_percentile_lower[45] radius_percentile_upper[46] lum_val[47] ");
  fprintf(out, "lum_percentile_lower[48] lum_percentile_upper[49]");
}

