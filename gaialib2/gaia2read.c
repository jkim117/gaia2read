/******************************************************************************
 *  Name:    Jason Kim
 *  Program : Princeton USRP
 *
 *  Version: 1.1.2
 * 
 *  Description: Includes main method. Takes command line arguments, forwards queries to other programs, and writes output
 *
 ******************************************************************************/
#define _GNU_SOURCE
#include "utils.h"
#include "myargs.h"
#include "gaia2ret.h"
#include "astrio.h"
#include "sllist.h"
#include "astrometry.h"
#include "gaiaPrint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//*********************cmd-line arguments*********************
//options, ra and dec position with size and circular or rectangular plus
//
//includes option flag to show gaia id, hat id, 2mass id
//includes option flag to request based on input of gaia id, hat id, or 2 mass id

enum {
    //arg_catpath,
    arg_header,
    //arg_outphot,
    //arg_estphot,
    arg_extra,
    arg_idrequest,
    arg_idtype,
    arg_idfile,
    arg_precess,
    arg_pm,
    arg_cmdline
};

// array of longoptions (see myargs.c)
myoptlong longoptions[] =
{
    { "ra",             required_argument,  'r'         },
    { "dec",            required_argument,  'd'         },
    { "pos",            required_argument,  'p'         },
    { "size",           required_argument,  's'         },
    { "circ",           no_argument,        'c'         },
    { "id",             required_argument,  'g'         },
    { "idtype",         required_argument,   arg_idtype },
    //{ "cat",            required_argument,  arg_catpath },
    //{ "catalog",        required_argument,  arg_catpath },//
    { "header",         no_argument,        arg_header  },
    { "extra",          no_argument,        arg_extra   },
    { "idrequest",     required_argument,  arg_idrequest  },
    { "idfile",         required_argument,  arg_idfile  },
    { "precess",        required_argument,  arg_precess },
    { "pm",             optional_argument,  arg_pm      },
    { "cmdline",        no_argument,        arg_cmdline },
    { "out",            required_argument,  'o'         },
    { "version",        no_argument,        'v'         },
    { "help",           no_argument,        'h'         },
    { 0, 0, 0 }
};

// local functions
static int     add_star_to_list( sllist** ids, const char* id, bool isfile, IDType inputIDType);
static void     usage();
static void     help();
static void     version();

int main(int argc, char** argv)
{
	progname = mybasename( *argv ); // see utils.c
	//*********************Initialize cmd-line arguments*********************
	skypos center;
    bool cent_ra_set        = false;
    bool cent_dec_set       = false;
    double size               = 0;
    bool is_circular        = false;
    const char* outfile     = NULL;
    sllist* ids             = NULL;
    bool print_header       = false;
    bool print_extra        = false;
    IDType inputIDType      = GAIA;
    IDType specify_idOut    = GAIA;
    bool equinox            = false;
    double JDequinox          = 0;
    bool epoch              = false;
    double JD                 = 0;
    bool print_cmdline      = false;
    const char* gID               = NULL;
    const char* idFile            = NULL;
    int idcount = 0;//number of id stars added to list
    int opt;

    while ((opt = mygetopt(argc, argv, "r:d:p:s:cg:o:vh", longoptions)) != NO_MORE_OPTIONS)
    {
        switch(opt)
        {
	        case 'r':           // --ra
	            if ( !astrio_parseRA( myoptarg, &center.RA, NULL ) ) {
	                err_ret(
	                    EXIT_FAILURE, "%s: invalid RA %s",
	                    progname, myoptarg
	                );
	            }

	            cent_ra_set = true;
	            break;

	        case 'd':           // --dec
	            if ( !astrio_parseDec( myoptarg, &center.Dec, NULL ) ) {
	                err_ret(
	                    EXIT_FAILURE, "%s: invalid Dec %s",
	                    progname, myoptarg
	                );
	            }

	            cent_dec_set = true;
	            break;

	        case 'p':           // --pos
	            if ( !astrio_parsepos( myoptarg, &center, NULL ) ) {
	                err_ret(
	                    EXIT_FAILURE, "%s: invalid position %s",
	                    progname, myoptarg
	                );
	            }

	            cent_ra_set = true;
	            cent_dec_set = true;
	            break;

	        case 's':           // --size
	            if ( !mystr2d( myoptarg, &size )  || size < 0.0 || size > 360.0 ) {
	                err_ret(
	                    EXIT_FAILURE, "%s: invalid argument %s",
	                    progname, myoptarg
	                );
	            }
	            break;

	        case 'c':           // --circ
	            is_circular = true;
	            break;

	        case 'o':           // --out
	            outfile = myoptarg;
	            break;

	        case 'g':           // --id
                gID = myoptarg;
	            break;

	        case arg_idtype:    // --what type of ids are the input
                if ( !astrio_parseID(myoptarg, &inputIDType, NULL))
                {
                    err_ret(EXIT_FAILURE, "%s: invalid ID type %s", progname, myoptarg);
                }
	        	break;

	        case arg_header:    // --header
	            print_header = true;
	            break;

            case arg_extra:    // --extra info on star
                print_extra = true;
                break;

	        case arg_idrequest:     // --hat-id
                if ( !astrio_parseID(myoptarg, &specify_idOut, NULL))
                {
                    err_ret(EXIT_FAILURE, "%s: invalid ID type %s", progname, myoptarg);
                }
	            break;

	        case arg_idfile:    // --idfile
                idFile = myoptarg;
	            break;

	        case arg_precess:   // --precess
	            {
	                if ( !astrio_text2jd( myoptarg, &JDequinox, NULL ) ) {
	                    err_print_msg( "invalid date-time %s", myoptarg );
	                    usage();
	                }//JDequinox is in years

	                equinox = true;
	            }
	            break;

	        case arg_pm:        // --pm
	            {
	                if ( !myoptarg || !astrio_text2jd( myoptarg, &JD, NULL ) ) {
	                    err_print_msg("error with arg_pm", myoptarg);
	                }//JD is in years
	                else {
	                    epoch = true;
	                }
	            }
	            break;

	        case arg_cmdline:   // --cmdline
	            print_cmdline = true;
	            break;

	        case 'h':           // --help
	            help();

	        case 'v':           // --version
	            version();

	        default:
	            usage();
        }
    }

    // collect ID information from arguments 'g' and arg_idfile
    if (gID != NULL)
        idcount = add_star_to_list( &ids, gID, false, inputIDType );
    if (idFile != NULL)
        idcount = add_star_to_list( &ids, idFile, true, inputIDType );

    // sanity check on cmd-line arguments
    if (    ( cent_ra_set && !cent_dec_set )
        ||  ( !cent_ra_set && cent_dec_set )
    ) {
        err_print_msg( "invalid input coordinate" );
        usage();
    }

    if ( cent_ra_set && size <= 0.0 ) {
        err_print_msg( "invalid or missing frame size" );
        usage();
    }


	//*********************Collect Input from Command Line*********************
    if ( myoptind == argc ) {
        // no more command line arguments
        if ( ids == NULL ) {
            if (!cent_ra_set) {
                if ( size > 0 ) {
                    err_print_msg( "nothing to search for" );
                    usage();
                }

                // full sky search
                cent_ra_set = true;
                is_circular = false;
                size = -1;
            }
        }
        else {
            if ( cent_ra_set ) {
                err_print_msg(
                    "cannot do area and ID search at the same time"
                );
                usage();
            }
        }
    }
    else {
        if ( cent_ra_set ) {
            err_print_msg(
                "cannot do area and ID search at the same time"
            );
            usage();
        }
		//This here takes the remaining command line arguments, if there are any, as star ids and then adds them to the lsit
        // add IDs to list
        for ( ; myoptind < argc; ++myoptind ) {
            const char* id = argv[myoptind];   // shortcut
            add_star_to_list( &ids, id, false, inputIDType );
        }
    }


    //*********************Initializing stars list*********************
    FILE* os = NULL;

    if ( cent_ra_set ) {
      const double* pJD = epoch ? &JD : NULL;
      int count;
      if ( !is_circular ) {
	// read square count                                                                                                                                                                                                                   
	count = starPosCount(center.RA, center.Dec, false, size, pJD);
      }
      else {
	// read circle count                                                                                                                                                                                                                 
	count = starPosCount(center.RA, center.Dec, true, size, pJD);
      }
      gaiastar *stars;
      stars=malloc(count*sizeof(gaiastar));

        if ( !is_circular ) {
            // read square
	  starPosSearch(center.RA, center.Dec, false, size, pJD,stars);
        }
        else {
            // read circle
	  starPosSearch(center.RA, center.Dec, true, size, pJD,stars);
        }

        if (count==0 ) {
            err_print_msg( "no star found" );
            exit( EXIT_FAILURE );
        }

        if ( outfile ) {
            os = fopen( outfile, "w" );
            if ( !os ) {
                err_ret( 11, "%s: cannot open file %s", progname, outfile );
            }
        }

        if ( equinox ) {
	  gaia2_precesslist( stars, JDequinox,count );
        }

        if ( !os ) {
            os = stdout;
        }

        if ( print_header ) {
            gaiastar_printheader( os, print_extra, specify_idOut);
        }

        if ( print_cmdline ) {
            fputs( "# ", os );
            myargs_print_cmdline( os, argc, argv );
        }

        if(specify_idOut==GAIA)
	  gaiastar_printlist(os, stars,print_extra,count);
        else
        {
	  sllist* altIDs = starListToIDs(stars,specify_idOut,count);
            gaiastar_printlist_alternateID(os, stars, print_extra, altIDs, specify_idOut,count);
        }
    }
    else {
        const double* pJD = epoch ? &JD : NULL;
        // get stars based on their IDs
        gaiastar *stars;
	stars = malloc(idcount*sizeof(gaiastar));
	  starsfromID(ids, pJD,stars);

	  if (idcount==0) {
            err_print_msg( "no star found" );
            exit( EXIT_FAILURE );
        }

        if ( outfile ) {
            os = fopen( outfile, "w" );
            if ( !os ) {
                err_ret( 11, "%s: cannot open file %s", progname, outfile );
            }
        }

        if ( equinox ) {
	  gaia2_precesslist( stars, JDequinox ,idcount);
        }

        if ( !os ) {
            os = stdout;
        }

        if ( print_header ) {
	  gaiastar_printheader( os, print_extra, specify_idOut);
        }

        if ( print_cmdline ) {
            fputs( "# ", os );
            myargs_print_cmdline( os, argc, argv );
        }

        if(specify_idOut==GAIA)
	  gaiastar_printlist(os, stars,print_extra,idcount);
        else
        {
	  sllist* altIDs = starListToIDs(stars,specify_idOut,idcount);
	    gaiastar_printlist_alternateID(os, stars, print_extra, altIDs, specify_idOut,idcount);
        }
    }

    if ( outfile ) {
        fclose( os );
    }


	exit(EXIT_SUCCESS);
}

// method to change formatted input HAT or 2MASS ID to a long form
local int toLongID(const char* id,IDType inputIDType,char* longID)
{
  if(inputIDType==HAT)
    {
      longID[strlen(id)-4]=id[0];
      longID[strlen(id)-3]=id[1];
      longID[strlen(id)-2]=id[2];
      unsigned int i;
      for(i = 4; i < strlen(id);i++)
	{
	  longID[i-4]=id[i];
	}
      longID[i-1]='\0';
      printf("%s %s\n",id,longID);
      return 0;

    }
  else if(inputIDType==TMASS)
    {
      if(id[8]=='+')
	longID[0]='1';
      else
	longID[0]='2';

      longID[1]=id[0];
      longID[2]=id[1];
      longID[3]=id[2];
      longID[4]=id[3];
      longID[5]=id[4];
      longID[6]=id[5];
      longID[7]=id[6];
      longID[8]=id[7];
      longID[9]=id[9];
      longID[10]=id[10];
      longID[11]=id[11];
      longID[12]=id[12];
      longID[13]=id[13];
      longID[14]=id[14];
      longID[15]=id[15];
      longID[16]='\0';
      return 0;
    }

  return 0;
}

int  add_star_to_list( sllist** ids, const char* id, bool isfile, IDType inputIDType)
{
  int idcount=0;
    if ( !id || !*id ) {
        return idcount;
    }

    if ( isfile || *id == '@' ) {
        const char* filename = !isfile ? &id[1] : id;
        FILE* idfile = fopen( filename, "r" );
        if ( !idfile ) {
            err_print_msg( "cannot open input ID file %s", filename );
            return idcount;
        }

        char* line = malloc( MAX_LINE * sizeof *line );
        size_t len = 0;
        while ( getline( &line, &len, idfile ) > 0 ) {
            char* comment = strchr( line, '#' );
            if ( comment ) {
                *comment = '\0';
            }
	    char idBuffer[30];
	    if(inputIDType==GAIA)
	      {
		char* idstr = toGaiaID(( char* )empty_string( line ),inputIDType,idBuffer);
		if ( idstr && *( idstr = trim_right( idstr ) ) ) {
		  slappend( ids, idstr, strlen( idstr ) + 1 );
		  idcount++;
		}
	      }
	    else
	      {
		char longid[20];
		//            char* idstr = ( char* )empty_string( line );
		toLongID(( char* )empty_string( line ),inputIDType,longid);
		char* idstr = toGaiaID(longid,inputIDType,idBuffer);
		if ( idstr && *( idstr = trim_right( idstr ) ) ) {
		  slappend( ids, idstr, strlen( idstr ) + 1 );
		  idcount++;
		}
	      }
	}

        free( line );
        fclose( idfile );
    }
    else {
      char idBuffer[30];
      if(inputIDType==GAIA)
	{
	  char *idstr = toGaiaID(id,inputIDType,idBuffer);
	  slappend( ids, idstr, strlen( idstr ) + 1 );
	}
      else
	{
	  char longid[20];

	  toLongID(id,inputIDType,longid);

	  char *idstr = toGaiaID(longid,inputIDType,idBuffer);
	  slappend( ids, idstr, strlen( idstr ) + 1 );
	}
      

	idcount++;
    }
    return idcount;
 }

void usage()
{
    static const char* helpstr[] =
{
"",
"Usage:\n",
"  gaia2read [options] <ID...>",
" or",
"  gaia2read [options] --size|-s <size> --pos|-p <center>",
" or",
"  gaia2read [options] [--pm [<epoch>]] --fits|-f <FITS>",
"",
" Options:",
" --ra|-r <ra>          : RA of the field center: [deg] or [HH:MM:SS.SSS]",
" --dec|-d <dec>        : Dec of the field center: [deg] or [DD:MM:SS.SSS]",
" --pos|-p <pos>        : coordinates of the field center (same as -r and -d)",
" --size|-s <size>      : [deg] length of the box or radius of circle",
" --circ|-c             : circular field instead of box",
" --id|-g <ID>          : retrieve source <ID> from catalogue",
" --idtype              : GAIA, HAT, TMASS, the type of ID that the input is given in",
//" --cat <path>          : top-level catalog path",
" --header              : print header",
" --extra               : print extra values including phot information and luminosity/radius",
" --idrequest           : GAIA, HAT, TMASS, the type of ID that the output gives",
" --idfile <path>       : read IDs (see option -g) from file",
" --precess <equinox>   : apply correction for precession for a given equinox",
" --pm <epoch>          : apply correction for proper motions. Epoch is in years",
" --out|-o <file>       : output file",
" --cmdline             : prints command line first",
" --version|-v          : prints out version",
" --help|-h             : print help screen",
NULL
};

    display_longtext( stderr, helpstr );
    exit( EXIT_FAILURE );
}

void help()
{
    static const char* helpstr[] =
{
"",
"Usage:\n",
"  gaia2read [options] <ID...>",
" or",
"  gaia2read [options] --size|-s <size> --pos|-p <center>",
" or",
"  gaia2read [options] [--pm [<epoch>]] --fits|-f <FITS>",
"",
" Options:",
" --ra|-r <ra>          : RA of the field center: [deg] or [HH:MM:SS.SSS]",
" --dec|-d <dec>        : Dec of the field center: [deg] or [DD:MM:SS.SSS]",
" --pos|-p <pos>        : coordinates of the field center (same as -r and -d)",
" --size|-s <size>      : [deg] length of the box or radius of circle",
" --circ|-c             : circular field instead of box",
" --id|-g <ID>          : retrieve source <ID> from catalogue",
" --idtype              : GAIA, HAT, TMASS, the type of ID that the input is given in",
//" --cat <path>          : top-level catalog path",
" --header              : print header",
" --extra               : print extra values including phot information and luminosity/radius",
" --idrequest           : GAIA, HAT, TMASS, the type of ID that the output gives",
" --idfile <path>       : read IDs (see option -g) from file",
" --precess <equinox>   : apply correction for precession for a given equinox",
" --pm <epoch>          : apply correction for proper motions. Epoch is in years",
" --out|-o <file>       : output file",
" --cmdline             : prints command line first",
" --version|-v          : prints out version",
" --help|-h             : print help screen",
NULL
};

    display_longtext( stdout, helpstr );
    exit( EXIT_SUCCESS );
}

void version()
{
    static const char* vString[] = 
    {
        "",
        "Name:    Jason Kim",
        "Program : Princeton USRP",
        "",
        "Version: 1.1.2",
        "",
        "Description: Includes main method. Takes command line arguments, forwards queries to other programs, and writes output",
        NULL
    };
    display_longtext( stdout, vString );
    exit( EXIT_SUCCESS );
}

