#define _GNU_SOURCE

#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gaiastar.h"
#include "sllist.h"
#include "gaia2cat.h"
#include "gaia2ret.h"
#include "utils.h"

char catpath_Gaia2[MAX_LINE] = { '\0' };
char catpath_Gaia2Bin[MAX_LINE] = { '\0' };
char catpath_Gaia2Mass[MAX_LINE] = { '\0' };


int STARSIZE = sizeof(gaiastar);


// -------------------------------------------------------------------------- +

void gaia2_setpath( const char* path )
{
    strncpy( catpath_Gaia2, path, MAX_LINE );
    if ( catpath_Gaia2[strlen( catpath_Gaia2 ) - 1] != '/' ) {
      strcat( catpath_Gaia2, "/" );
    }

    strcpy(catpath_Gaia2Bin,catpath_Gaia2);
    strcat(catpath_Gaia2Bin,"Gaia2Bin/");

    strcpy(catpath_Gaia2Mass,catpath_Gaia2);
    strcat(catpath_Gaia2Mass,"Gaia2Mass/");

    return;
}

// -------------------------------------------------------------------------- +

void gaia2_getpath()
{
    if ( !*catpath_Gaia2 ) {
        const char* defpath = "./";

        const char* home = getenv( "HOME" );
        if ( !home ) {
            gaia2_setpath( defpath );
	    return;
        }

        strcpy( catpath_Gaia2, home );
        strcat( catpath_Gaia2, "/.gaia2readrc" );

        FILE* rc = fopen( catpath_Gaia2, "r" );
        if ( !rc ) {
	  gaia2_setpath( defpath );
	  return;
        }

        char* line = malloc( MAX_LINE * sizeof *line );
        size_t len = 0;
        while ( getline( &line, &len, rc ) > 0 ) {
            char* comment = strchr( line, '#' );
            if ( comment ) {
                *comment = '\0';
            }

            char* path = ( char* )empty_string( line );
            if ( path && *( path = trim_right( path ) ) ) {
	      gaia2_setpath( path );
	      break;
            }
        }

        free( line );
        fclose( rc );
    }
    return;
}

// -------------------------------------------------------------------------- +

// FINDING STARS IN GAIA DR2 BASED ON RA AND DEC RANGE:
// Stars are grouped into 0.2 degree zones in zone files in the sortedBin folder. There are 900 zones.
// Within each zone file, stars are sorted according to ra. In addition, there are 1440 ints at the beginning of each file
// These ints tell the program how many stars are within each ra zone, each ra zone is 0.25 degrees and there are 1440 of them.
// The search works by identifying which zone files must be searched through according to the prescribed dec range
// Within each of those zone files, the program narrows down which ra zones must be looked at for the starting and end of the prescribed ra range
// Within each of the ra zones, the program then conducts a binary search to exactly identify the location of the start and end of the ra range
// The program then returns every star within those ranges.

// This method is the main recursive part of the binary search
int recursiveSearch(FILE *zFile, int rzIndex1, int rzIndex2, double ra,bool minormax)
{
  int nStars = (rzIndex2-rzIndex1)/STARSIZE;
  int middleStar = nStars/2;
  int raMiddleIndex = rzIndex1+middleStar*STARSIZE+16;
  if(raMiddleIndex>=rzIndex2)
    return rzIndex2;
  if(raMiddleIndex<=rzIndex1)
    return rzIndex1;
  fseek(zFile,raMiddleIndex,SEEK_SET);
  double raMiddle;
  fread((void*)(&raMiddle),sizeof(double),1,zFile);

  if (raMiddle<ra)
    {
      fseek(zFile,raMiddleIndex+STARSIZE,SEEK_SET);
      double raMiddlePlus1;
      fread((void*)(&raMiddlePlus1),sizeof(double),1,zFile);

      if(raMiddlePlus1>ra)
        {
          if (minormax)
            return rzIndex1+(middleStar+1)*STARSIZE;
          return rzIndex1+(middleStar)*STARSIZE;
        }
        return recursiveSearch(zFile, rzIndex1+(middleStar+1)*STARSIZE,rzIndex2, ra,minormax);
    }
  if (raMiddle>ra)
    {
      fseek(zFile,raMiddleIndex-STARSIZE,SEEK_SET);
      double raMiddleMinus1;
      fread((void*)(&raMiddleMinus1),sizeof(double),1,zFile);

      if (raMiddleMinus1<ra)
        {
          if (!minormax)
            return rzIndex1+(middleStar-1)*STARSIZE;
          return rzIndex1+(middleStar)*STARSIZE;
        }
        return recursiveSearch(zFile, rzIndex1, rzIndex1+(middleStar-1)*STARSIZE, ra,minormax);
    }
  else
    return rzIndex1+(middleStar)*STARSIZE;
}

// Binary Search method for finding star in ra and dec range
int binarySearch(FILE *zFile, int raZone, double ra, bool minormax)//true for min false for max
{
  fseek(zFile,4*raZone,SEEK_SET);
  int rzIndex2;
  fread((void*)(&rzIndex2),sizeof(int),1,zFile);
  rzIndex2=rzIndex2*STARSIZE+4*1440;

  int rzIndex1;
  if (raZone==0)
    rzIndex1=4*1440;
  else
    {
      fseek(zFile,4*(raZone-1),SEEK_SET);
      fread((void*)(&rzIndex1),sizeof(int),1,zFile);
      rzIndex1=rzIndex1*STARSIZE+4*1440;
    }
  //rzIndex1 is the index for the start of this zone. rzIndex2 is the index for the start of the next zone
  int nStars = (rzIndex2-rzIndex1)/STARSIZE;

  int middleStar = nStars/2;
  int raMiddleIndex = rzIndex1+middleStar*STARSIZE+16;
  if(raMiddleIndex>=rzIndex2)
    return rzIndex2;
  if(raMiddleIndex<=rzIndex1)
    return rzIndex1;
  fseek(zFile,raMiddleIndex,SEEK_SET);
  double raMiddle;
  fread((void*)(&raMiddle),sizeof(double),1,zFile);
  if (raMiddle<ra)
    {
      fseek(zFile,raMiddleIndex+STARSIZE,SEEK_SET);
      double raMiddlePlus1;
      fread((void*)(&raMiddlePlus1),sizeof(double),1,zFile);

      if(raMiddlePlus1>ra)
        {
          if (minormax)
            return rzIndex1+(middleStar+1)*STARSIZE;
          return rzIndex1+(middleStar)*STARSIZE;
        }
      return recursiveSearch(zFile, rzIndex1+(middleStar+1)*STARSIZE,rzIndex2, ra,minormax);
    }
  if (raMiddle>ra)
    {
      fseek(zFile,raMiddleIndex-STARSIZE,SEEK_SET);
      double raMiddleMinus1;
      fread((void*)(&raMiddleMinus1),sizeof(double),1,zFile);

      if (raMiddleMinus1<ra)
        {
          if (!minormax)
            return rzIndex1+(middleStar-1)*STARSIZE;
          return rzIndex1+(middleStar)*STARSIZE;
        }
      return recursiveSearch(zFile, rzIndex1, rzIndex1+(middleStar-1)*STARSIZE, ra,minormax);
    }
  else
  return rzIndex1+(middleStar)*STARSIZE;
}

int posCount(double raMin, double raMax, double decMin, double decMax, testfunc tester,double ra,double dec, double frame_size,
        const double *epoch, const maglims* maglim)
{
  int count = 0;

  bool noRA0 = raMax > raMin;

  //dec zones go from 1 to 900
  double dMinPos = decMin + 90.0;
  double dMaxPos = decMax + 90.0;
  int dMinZone = (int)(dMinPos/0.2)+1;
  int dMaxZone = (int)(dMaxPos/0.2)+1;
  if (dMinPos==180.0)
    dMinZone=900;
  if (dMaxPos==180.0)
    dMaxZone=900;

  //ra zones go from 0 to 1439
  int rMinZone = (int)((raMin)/0.25);
  if(raMin == 360.0)
    rMinZone = 1439;
  int rMaxZone = (int)((raMax)/0.25);
  if(raMax == 360.0)
    rMaxZone = 1439;

  char* catpath = concat(catpath_Gaia2Bin, "sortedBin/z");

  for(int i = dMinZone; i < dMaxZone + 1; i++)
    {

      char buffer[3];
      sprintf(buffer,"%d",i);
      char *fileName = concat(catpath, buffer);

      FILE *zFile = fopen(fileName,"rb");
      if (zFile == NULL) {
        fprintf(stderr,"error: could not open file %s\n",fileName);
	exit(EXIT_FAILURE);
      }
      free(fileName);

      if(noRA0)
        {
          //for the minimum ra:
          int minIndex = binarySearch(zFile,rMinZone,raMin,true);

          //for the max ra:
          int maxIndex = binarySearch(zFile,rMaxZone,raMax,false);

          // during the initial run for gaia2writebin.c, there was a point where the program failed and stopped. Because of the vast size of the Gaia DR2,
          // I decided not to start the run from scratch, but continued where the initial run left off. However, because of this, there may be a few duplicates
          // of stars within my sortedBin files. Thus, I include a small check here to eliminate such duplicates.
          long id = 0; //test for duplicates by comparing adjacent ids.

	  for (int i = minIndex; i < maxIndex; i+=STARSIZE)
            {
              //read and store each of the stars, checking dec each time. Add the stars to a list
              fseek(zFile, i+32, SEEK_SET);
              double starDec;
              fread((void*)(&starDec), sizeof(double),1, zFile);
              if(starDec>decMax || starDec<decMin)
                continue;
              fseek(zFile,i,SEEK_SET);
              gaiastar newStar;
              fread((void*)(&newStar),sizeof(gaiastar),1,zFile);

              if (newStar.source_id==id) // testing for duplicates
                continue;
              id = newStar.source_id;

              if((*tester)(&newStar,ra,dec,frame_size, epoch, maglim))
                count++;
            }
        }
      else
        {
          long id = 0;
          //part 1: east
          int minIndex = binarySearch(zFile,0,0.0,true);
          int maxIndex = binarySearch(zFile,rMaxZone,raMax,false);

          for (int i = minIndex; i < maxIndex; i+=STARSIZE)
            {
              //read and store each of the stars, checking dec each time. Add the stars to a list

              fseek(zFile, i+32, SEEK_SET);
              double starDec;
              fread((void*)(&starDec), sizeof(double),1, zFile);
              if(starDec>decMax || starDec<decMin)
                continue;
              fseek(zFile,i,SEEK_SET);
              gaiastar newStar;
              fread((void*)(&newStar),sizeof(gaiastar),1,zFile);

              if (newStar.source_id==id)// testing for duplicates
                continue;
              id = newStar.source_id;

              if((*tester)(&newStar,ra,dec,frame_size, epoch, maglim))
		count++;
	    }

	  id = 0;

	  //part 2: west
	  minIndex = binarySearch(zFile,rMinZone,raMin,true);
	  maxIndex = binarySearch(zFile,1439,360.0,false);

          for (int i = minIndex; i < maxIndex; i+=STARSIZE)
            {
              //read and store each of the stars, checking dec each time. Add the stars to a list

              fseek(zFile, i+32, SEEK_SET);
              double starDec;
              fread((void*)(&starDec), sizeof(double),1, zFile);
              if(starDec>decMax || starDec<decMin)
                continue;
              fseek(zFile,i,SEEK_SET);
              gaiastar newStar;
              fread((void*)(&newStar),sizeof(gaiastar),1,zFile);

              if (newStar.source_id==id)// testing for duplicates
                continue;
              id = newStar.source_id;

              if((*tester)(&newStar,ra,dec,frame_size, epoch, maglim))
		count++;
	    }
	}

      fclose(zFile);


    }
  free(catpath);
  printf("%d stars found\n", count);
  return count;

}
// returns list of stars based on ra and dec range
int posQuery(double raMin, double raMax, double decMin, double decMax, testfunc tester,double ra,double dec, double frame_size,
        const double *epoch, const maglims* maglim, gaiastar stars[])
{
  int count = 0;

  bool noRA0 = raMax > raMin;

  //dec zones go from 1 to 900
  double dMinPos = decMin + 90.0;
  double dMaxPos = decMax + 90.0;
  int dMinZone = (int)(dMinPos/0.2)+1;
  int dMaxZone = (int)(dMaxPos/0.2)+1;
  if (dMinPos==180.0)
    dMinZone=900;
  if (dMaxPos==180.0)
    dMaxZone=900;

  //ra zones go from 0 to 1439
  int rMinZone = (int)((raMin)/0.25);
  if(raMin == 360.0)
    rMinZone = 1439;
  int rMaxZone = (int)((raMax)/0.25);
  if(raMax == 360.0)
    rMaxZone = 1439;

  char* catpath = concat(catpath_Gaia2Bin, "sortedBin/z");

    for(int i = dMinZone; i < dMaxZone + 1; i++)
    {

      char buffer[3];
      sprintf(buffer,"%d",i);
      char *fileName = concat(catpath, buffer);

      FILE *zFile = fopen(fileName,"rb");
      if (zFile == NULL) {
        printf("error: could not open file %s\n",fileName);
	exit(EXIT_FAILURE);
      }
      free(fileName);

        if(noRA0)
        {
          //for the minimum ra:
          int minIndex = binarySearch(zFile,rMinZone,raMin,true);

          //for the max ra:
          int maxIndex = binarySearch(zFile,rMaxZone,raMax,false);

          // during the initial run for gaia2writebin.c, there was a point where the program failed and stopped. Because of the vast size of the Gaia DR2,
          // I decided not to start the run from scratch, but continued where the initial run left off. However, because of this, there may be a few duplicates
          // of stars within my sortedBin files. Thus, I include a small check here to eliminate such duplicates.
          long id = 0; //test for duplicates by comparing adjacent ids.

           for (int i = minIndex; i < maxIndex; i+=STARSIZE)
            {
              //read and store each of the stars, checking dec each time. Add the stars to a list
              fseek(zFile, i+32, SEEK_SET);
              double starDec;
              fread((void*)(&starDec), sizeof(double),1, zFile);
              if(starDec>decMax || starDec<decMin)
                continue;
              fseek(zFile,i,SEEK_SET);
              gaiastar newStar;
              fread((void*)(&newStar),sizeof(gaiastar),1,zFile);

              if (newStar.source_id==id) // testing for duplicates
                continue;
              id = newStar.source_id;

              if((*tester)(&newStar,ra,dec,frame_size, epoch, maglim))
		{
		stars[count]=newStar;
		count++;
		}
               }
        }
         else
        {
	  long id = 0;
          //part 1: east
	  int minIndex = binarySearch(zFile,0,0.0,true);
          int maxIndex = binarySearch(zFile,rMaxZone,raMax,false);

	  for (int i = minIndex; i < maxIndex; i+=STARSIZE)
            {
              //read and store each of the stars, checking dec each time. Add the stars to a list
              fseek(zFile, i+32, SEEK_SET);
              double starDec;
              fread((void*)(&starDec), sizeof(double),1, zFile);
              if(starDec>decMax || starDec<decMin)
                continue;
              fseek(zFile,i,SEEK_SET);
              gaiastar newStar;
              fread((void*)(&newStar),sizeof(gaiastar),1,zFile);

              if (newStar.source_id==id)// testing for duplicates
                continue;
              id = newStar.source_id;

              if((*tester)(&newStar,ra,dec,frame_size, epoch, maglim))
		{
		stars[count]=newStar;
		count++;
		}
		}

            id = 0;
          //part 2: west
	     minIndex = binarySearch(zFile,rMinZone,raMin,true);
	     maxIndex = binarySearch(zFile,1439,360.0,false);

	  for (int i = minIndex; i < maxIndex; i+=STARSIZE)
            {
              //read and store each of the stars, checking dec each time. Add the stars to a list

              fseek(zFile, i+32, SEEK_SET);
              double starDec;
              fread((void*)(&starDec), sizeof(double),1, zFile);
              if(starDec>decMax || starDec<decMin)
                continue;
              fseek(zFile,i,SEEK_SET);
              gaiastar newStar;
              fread((void*)(&newStar),sizeof(gaiastar),1,zFile);

              if (newStar.source_id==id)// testing for duplicates
                continue;
              id = newStar.source_id;

              if((*tester)(&newStar,ra,dec,frame_size, epoch, maglim))
		{
		stars[count]=newStar;
		count++;
		}
		}
            }

      fclose(zFile);


      }
    free(catpath);
    return 0;


}

/*// main method for testing                                                                                                                                                                                                                                                      
int main(void)
{
  //testing gaia2datasort                                                                                                                                                                                                                                                       
    char* fileName = "/home/jkim/work/Gaia2Bin/binFiles/z731sort";                                                                                                                                                                                                           
  FILE *file = fopen(fileName,"rb");                                                                                                                                                                                                                                            
  for(int i = 0; i < 1440; i++)                                                                                                                                                                                                                                                 
    {                                                                                                                                                                                                                                                                           
      int index;                                                                                                                                                                                                                                                                
      fread((void*)(&index),sizeof(int),1,file);                                                                                                                                                                                                                                
      //printf("%d\n",index);                                                                                                                                                                                                                                                   
                                                                                                                                                                                                                                                                                
      }                                                                                                                                                                                                                                                                         
                                                                                                                                                                                                                                                                                
  for(int i = 0; i < 30; i++)                                                                                                                                                                                                                                                   
    {                                                                                                                                                                                                                                                                           
      gaiastar nStar;                                                                                                                                                                                                                                                           
      fread((void*)(&nStar),sizeof(gaiastar),1,file);                                                                                                                                                                                                                           
      printf("%ld %f %f\n",nStar.source_id,nStar.dec,nStar.ra);                                                                                                                                                                                                                 
    }                                                                                                                                                                                                                                                                           
    fclose(file);                                                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                
    fileName = "/home/jkim/work/Gaia2Bin/binFiles/z732sort";                                                                                                                                                                                                                    
    file = fopen(fileName,"rb");                                                                                                                                                                                                                                                
    for(int i = 0; i < 1440; i++)                                                                                                                                                                                                                                              
    {                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                
      int index;                                                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                
      fread((void*)(&index),sizeof(int),1,file);                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                
      //      printf("%d\n",index);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
                                                                                                                                                                                                                                                                                
      }                                                                                                                                                                                                                                                                         
                                                                                                                                                                                                                                                                                
    for(int i = 0; i < 30; i++)                                                                                                                                                                                                                                                 
      {                                                                                                                                                                                                                                                                         
        gaiastar nStar;                                                                                                                                                                                                                                                         
        fread((void*)(&nStar),sizeof(gaiastar),1,file);                                                                                                                                                                                                                         
        printf("%ld %f %f\n",nStar.source_id,nStar.dec,nStar.ra);                                                                                                                                                                                                               
      }                                                                                                                                                                                                                                                                         
      fclose(file);

  //testing for gaia2cat                                                                                                                                                                                                                                                        
    double raMin = 101.75;                                                                                                                                                                                                                                                    
  double raMax = 101.81;                                                                                                                                                                                                                                                        
  double decMin = 56.0;                                                                                                                                                                                                                                                         
  double decMax = 56.21;

  sllist* testList = posQuery(raMin,raMax,decMin, decMax);                                                                                                                                                                                                                      
   for(const sllist* it = testList; it !=NULL; it = it->next)                                                                                                                                                                                                                   
    {                                                                                                                                                                                                                                                                           
      const gaiastar* star = (const gaiastar*)it->data;                                                                                                                                                                                                                         
      printf("%ld %f %f\n",star->source_id,star->ra,star->dec);                                                                                                                                                                                                                 
      }

  //testing binary search                                                                                                                                                                                                                                                       
   double raTarget = 101.81;                                                                                                                                                                                                                                               
  fileName = "/home/jkim/work/Gaia2Bin/binFiles/z731sort";                                                                                                                                                                                                                      
  FILE *zFile = fopen(fileName,"rb");                                                                                                                                                                                                                                           
  if (zFile == NULL)                                                                                                                                                                                                                                                            
    printf("error: could not open file\n");                                                                                                                                                                                                                                     
  int rZone = (int)((raTarget)/0.25);                                                                                                                                                                                                                                           
  if(raTarget == 360.0)                                                                                                                                                                                                                                                         
    rZone= 1439;                                                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                                
  int minIndex = binarySearch(zFile,rZone,raTarget,true);                                                                                                                                                                                                                       
                                                                                                                                                                                                                                                                                
  fseek(zFile,minIndex,SEEK_SET);                                                                                                                                                                                                                                               
  gaiastar newStar;                                                                                                                                                                                                                                                             
  fread((void*)(&newStar),sizeof(gaiastar),1,zFile);                                                                                                                                                                                                                            
  printf("%ld %f %f\n",newStar.source_id,newStar.ra,newStar.dec);
  return 0;
}*/
                         




