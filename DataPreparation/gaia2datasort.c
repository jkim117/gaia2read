#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gaiastar.h"

// local functions
char *concat(const char *s1, const char *s2);
int rwBin(FILE* binFile, char* buffer);

// main method
int main(void)
{
  char* catpath = "/home/jkim/work/Gaia2Bin/binFiles2/z";
  /*DIR *d;
  struct dirent *dir;
  d = opendir(catpath);
  if (d)
    {
     
        while ((dir = readdir(d)) != NULL)*/
  for(int z = 1; z < 901; z++)
    {
      char buffer[3];
      sprintf(buffer,"%d",z);
	  // if ((dir->d_name)[0]!='z')
	  // continue;                                                                                                                                                                 
          char *fileName = concat(catpath, buffer);
	  
	  printf("%s\n",fileName);
          FILE *binFile;
          binFile = fopen(fileName,"rb");

          if ( binFile == NULL )
            {
              printf("error: could not open file\n");
            }
	  rwBin(binFile,buffer);

        }
	//    closedir(d);
	// }

  return 0;
}

// string concatenation
char *concat(const char *s1, const char *s2)
{
  char *result;

  result = malloc(strlen(s1) + strlen(s2) + 1);
  if (result == NULL)
    {
      printf("Error: malloc failed in concat\n");
      exit(EXIT_FAILURE);
    }
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}


int writeSortBin(gaiastar starsArr[], int raZones[], char *fileName, int numStars)
{
  FILE *outFile = fopen(fileName,"wb");
  fwrite(raZones,sizeof(int),1440,outFile);
  fwrite(starsArr,sizeof(gaiastar),numStars,outFile);
 
  fclose(outFile);
  return 0;
}

gaiastar convertData(unsigned long long sID, double dataArray[])
{
  gaiastar st;

  st.source_id =(long) sID;
  st.ref_epoch = dataArray[0];
  st.ra = dataArray[1];
  st.ra_error = dataArray[2];
  st.dec = dataArray[3];
  st.dec_error = dataArray[4];
  st.parallax = dataArray[5];
  st.parallax_error = dataArray[6];
  st.pmra = dataArray[7];
  st.pmra_error = dataArray[8];
  st.pmdec = dataArray[9];
  st.pmdec_error = dataArray[10];
  st.astrometric_excess_noise = dataArray[11];
  st.astrometric_excess_noise_sig = dataArray[12];
  if(dataArray[13]==6.55)
    st.astrometric_primary_flag = false;
  else
    st.astrometric_primary_flag = true;

  st.phot_g_n_obs =(int) dataArray[14];
  st.phot_g_mean_flux = dataArray[15];
  st.phot_g_mean_flux_error = dataArray[16];
  st.phot_g_mean_flux_over_error =(float) dataArray[17];
  st.phot_g_mean_mag =(float) dataArray[18];
  st.phot_bp_n_obs =(int) dataArray[19];
  st.phot_bp_mean_flux = dataArray[20];
  st.phot_bp_mean_flux_error = dataArray[21];
  st.phot_bp_mean_flux_over_error =(float) dataArray[22];
  st.phot_bp_mean_mag =(float) dataArray[23];
  st.phot_rp_n_obs =(int) dataArray[24];
  st.phot_rp_mean_flux = dataArray[25];
  st.phot_rp_mean_flux_error = dataArray[26];
  st.phot_rp_mean_flux_over_error =(float) dataArray[27];
  st.phot_rp_mean_mag = (float)dataArray[28];
  st.phot_bp_rp_excess_factor =(float) dataArray[29];
  st.radial_velocity = dataArray[30];
  st.radial_velocity_error = dataArray[31];
  if(dataArray[32]==4.55)
    st.phot_variable_flag = false;
  else
    st.phot_variable_flag = true;
  st.teff_val =(float) dataArray[33];
  st.teff_percentile_lower =(float) dataArray[34];
  st.teff_percentile_upper =(float) dataArray[35];
  st.a_g_val =(float) dataArray[36];
  st.a_g_percentile_lower =(float) dataArray[37];
  st.a_g_percentile_upper =(float) dataArray[38];
  st.e_bp_min_rp_val =(float) dataArray[39];
  st.e_bp_min_rp_percentile_lower =(float) dataArray[40];
  st.e_bp_min_rp_percentile_upper =(float) dataArray[41];
  st.radius_val =(float) dataArray[42];
  st.radius_percentile_lower =(float) dataArray[43];
  st.radius_percentile_upper =(float) dataArray[44];
  st.lum_val =(float) dataArray[45];
  st.lum_percentile_lower =(float) dataArray[46];
  st.lum_percentile_upper =(float) dataArray[47];
  return st;
}


int rwBin(FILE* binFile, char* buffer)
{
  int raZones[1440] = {0};

  fseek(binFile, 0L, SEEK_END);
  int numStars = ftell(binFile)/(8*49);
  rewind(binFile);
  //  gaiastar starsArr[numStars];

  gaiastar *starsArr = malloc(numStars*sizeof(gaiastar));
    if(starsArr==NULL)
    {
      printf("ERROR in MEMORY allocation");
      exit(EXIT_FAILURE);
      }
  unsigned long long sID;
  int starCount = 0;
   while(fread((void*)(&sID),sizeof(sID),1,binFile)==1)
    {
      double dataArray[48];
      fread((void*)(&dataArray),sizeof(double),48,binFile);
      gaiastar newStar = convertData(sID, dataArray);

      int zone = (int)((newStar.ra)/0.25);
      if(newStar.ra == 360.0)
	zone = 1439;
       raZones[zone]++;
              
       starsArr[starCount] =newStar;
      starCount++;
      
    }

  fclose(binFile);
    
  qsort(starsArr,starCount,sizeof(gaiastar),starcmp);
  
  int sum = 0;
  for (int i = 0; i < 1440; i++)
    {
      sum = sum + raZones[i];
      raZones[i] = sum;
    }
  //char *outFile = concat(fileName, "sort");
  char *outFile = concat("/home/jkim/work/Gaia2Bin/sortedBin/z",buffer);
  writeSortBin(starsArr, raZones,outFile, numStars);
  free(starsArr);
  return 0;
}         
