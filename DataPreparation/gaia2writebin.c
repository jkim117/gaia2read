#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//local functions
// string concatenation
char *concat(const char *s1, const char *s2);
bool checkCount(int count94);
double starASCIIrd(char *str);
int writeBin(double dec, double starArray[], unsigned long long *sourceID);
int *readFile(FILE* catfile);
FILE *popen(const char *command, const char *mode);
char * strsep(char **stringp, const char *delim);

// main method
int main(void)
{
   bool already = true;

  // PART 1: OBTAIN DATA AND PUT STARS INTO ZONE FILES
  char* catpath = "/home/jkim/catalogs/GaiaDR2/";
  const char prefix[] = "zcat ";
  char* cmd;
  DIR *d;
  struct dirent *dir;
  d = opendir(catpath);
  if (d)
    {
      while ((dir = readdir(d)) != NULL)
        {
          if ((dir->d_name)[0]!='G')
            continue;
          //printf("%s\n", dir->d_name);                                                                                                                                                                    
          char *fileName = concat(catpath, dir->d_name);
	  
	  //if (!strcmp(fileName, "/home/jkim/catalogs/GaiaDR2/GaiaSource_5879405436152890368_5879427980482735104.csv.gz"))
	     already = false;
      	  if (already)
	    continue;
          cmd = malloc(sizeof(prefix) + strlen(fileName) + 1);
          sprintf(cmd, "%s%s", prefix, fileName);

          FILE *catfile = popen(cmd,"r");
          printf("%s\n", fileName);

          if ( catfile == NULL )
            {
              printf("error: could not open file\n");
            }
	  readFile(catfile);
          fclose(catfile);
	  
        }
      closedir(d);
    }

  // PART 2: INSPECT EACH ZONE FILE AND SORT STARS BASED ON RA



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

bool checkCount(int count94)
{
  int badNums[46]={0,1,2,3,11,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,34,36,37,38,39,40,41,42,43,44,45,62,63,64,65,68,69,70,71,73,74,75,76,77,87};
  for (int i = 0; i < 46; i++)
    {
      if (count94==badNums[i])
	return false;
    }
  return true;
}

double starASCIIrd(char *str)
{
  char *token;
  if (str[strlen(str)-1]=='\n')
    {
      token = (char*) malloc(strlen(str));
      strncpy(token,str,strlen(str)-1);
	}
  else
    token = str;
  if (strcmp(token, "")==0)
    return 3.55;
  if (strcmp(token, "NOT_AVAILABLE")==0)
    return 4.55;
  if (strcmp(token, "VARIABLE")==0)
    return 5.55;
  if (strcmp(token, "false")==0)
    return 6.55;
  if (strcmp(token, "true")==0)
    return 7.55;                                                                                                                                                                                        
  return atof(token);
}

int writeBin(double dec, double starArray[], unsigned long long *sourceID)
{
  double dPos = dec + 90.0;
  int zone = (int)((dPos/0.2) + 1);
  if (dPos == 180.0)
    zone = 900;
  char zString [4];
  sprintf(zString,"%d",zone);
  char* outName = concat("/home/jkim/work/Gaia2Bin/binFiles2/z", zString);
  FILE *outFile = fopen(outName,"ab");
  fwrite(sourceID,sizeof(sourceID),1,outFile);
  fwrite(starArray,sizeof(starArray[0]),48,outFile);
  fclose(outFile);
  return 0;
}


int *readFile(FILE* catfile)
{
  unsigned long long sourceID;
   char buf[2000];
   bool headCount = false;
  while(fgets(buf,1999,catfile)!=NULL)
    //  while(fscanf(catfile,"%s",buf)==1)
    {
      if (!headCount)
	{
	  headCount=true;
	  continue;
	}
     
      int count94 = 0;
      int count = 0;
      double starArray[48];
      char *token;
      char *point[]={buf};

      while((token=strsep(point,","))!=NULL)
	{
	  if (checkCount(count94))
	    {
	      starArray[count] = starASCIIrd(token);
	      count++;
	    }
	  if (count94==2)
	    sourceID = strtoull(token, NULL,0);

	  count94++;
	  token = strtok(NULL,",");
	}
      writeBin(starArray[3],starArray,&sourceID);
    }

  return 0;
}         
