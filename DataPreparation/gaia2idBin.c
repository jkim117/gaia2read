#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include "gaiastar.h"

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

int writeBin(char *outName, long *sourceID, long *position, int *zone)
{
  FILE *outFile = fopen(outName,"ab");
  fwrite(sourceID,sizeof(long),1,outFile);
  fwrite(position,sizeof(long),1,outFile);
  fwrite(zone,sizeof(int),1,outFile);
  fclose(outFile);
  return 0;
}

int main(void)
{
  char* catpath = "/home/jkim/work/Gaia2Bin/sortedBin/";
  DIR *d;
  struct dirent *dir;
  d = opendir(catpath);

  long starCount1 = 0;
  long starCount2 = 0;
  long starCount3 = 0;
  long starCount4 = 0;
  long starCount5 = 0;
  long starCount6 = 0;
  long starCount7 = 0;
  long starCount8 = 0;
  long starCount9 = 0;

  if (d)
    {
      while ((dir = readdir(d)) != NULL)
	{
	  if ((dir->d_name)[0]!='z')
            continue;

	  char *fileName = concat(catpath, dir->d_name);
	  FILE *zFile = fopen(fileName,"rb");

	  printf("%s\n",fileName);

	  if (zFile == NULL)
	    printf("error: could not open file\n");

	  fseek(zFile,4*1439,SEEK_SET);
	  int numStars;//number of stars in this zone file
	  fread((void*)(&numStars),sizeof(int),1,zFile);

	  for (int i = 0; i < numStars; i++)
	    {
	      long position = 4*1440 + i*sizeof(gaiastar);
	      fseek(zFile,position,SEEK_SET);
	      long sourceID;
	      fread((void*)(&sourceID),sizeof(long),1,zFile);
	      char starID[20];
	      sprintf(starID,"%ld",sourceID);
	      char *zoneString = dir->d_name+1;
	      int zone = atoi(zoneString);

	      if (starID[0]=='1')
		{
		  starCount1++;
		  writeBin("/home/jkim/work/Gaia2Bin/IDST/id1", &sourceID, &position, &zone);
		}
	      else if (starID[0]=='2')
		{
		  starCount2++;
		  writeBin("/home/jkim/work/Gaia2Bin/IDST/id2", &sourceID, &position, &zone);
		}
	      else if (starID[0]=='3')
		{
		  starCount3++;
		  writeBin("/home/jkim/work/Gaia2Bin/IDST/id3", &sourceID, &position, &zone);
		}
	      else if (starID[0]=='4')
		{
		  starCount4++;
		  writeBin("/home/jkim/work/Gaia2Bin/IDST/id4", &sourceID, &position, &zone);
		}
	      else if (starID[0]=='5')
		{
		  starCount5++;
		  writeBin("/home/jkim/work/Gaia2Bin/IDST/id5", &sourceID, &position, &zone);
		}
	      else if (starID[0]=='6')
		{
		  starCount6++;
		  writeBin("/home/jkim/work/Gaia2Bin/IDST/id6", &sourceID, &position, &zone);
		}
	      else if (starID[0]=='7')
		{
		  starCount7++;
		  writeBin("/home/jkim/work/Gaia2Bin/IDST/id7", &sourceID, &position, &zone);
		}
	      else if (starID[0]=='8')
		{
		  starCount8++;
		  writeBin("/home/jkim/work/Gaia2Bin/IDST/id8", &sourceID, &position, &zone);
		}
	      else //for 9 or any outcasts
		{
		  starCount9++;
		  writeBin("/home/jkim/work/Gaia2Bin/IDST/id9", &sourceID, &position, &zone);
		}

	    }
	  fclose(zFile);


	}
    }
  printf("count1: %ld\ncount2: %ld\ncount3: %ld\ncount4: %ld\ncount5: %ld\ncount6: %ld\ncount7: %ld\ncount8: %ld\ncount9: %ld\n",starCount1,starCount2,starCount3,starCount4,starCount5,starCount6,starCount7,starCount8,starCount9);
  return 0;
}
