#include <string.h>
#include <stdio.h>
#include "fitsio.h"

int tablist(char *file)
{
    fitsfile *fptr;      /* FITS file pointer, defined in fitsio.h */
    char *val, value[1000], nullstr[]="*";
    char keyword[FLEN_KEYWORD], colname[FLEN_VALUE];
    int status = 0;   /*  CFITSIO status value MUST be initialized to zero!  */
    int hdunum, hdutype, ncols, ii, anynul, dispwidth[1000];
    int firstcol, lastcol = 0, linewidth;
    long jj, nrows;

    if (!fits_open_file(&fptr, file, READONLY, &status))
    {
      if ( fits_get_hdu_num(fptr, &hdunum) == 1 )
          /* This is the primary array;  try to move to the */
          /* first extension and see if it is a table */
          fits_movabs_hdu(fptr, 2, &hdutype, &status);
       else 
          fits_get_hdu_type(fptr, &hdutype, &status); /* Get the HDU type */

      if (hdutype == IMAGE_HDU) 
          printf("Error: this program only displays tables, not images\n");
      else  
      {
        fits_get_num_rows(fptr, &nrows, &status);
        fits_get_num_cols(fptr, &ncols, &status);

        /* find the number of columns that will fit within 80 characters */
        while(lastcol < ncols) {
          linewidth = 0;
          firstcol = lastcol+1;
          for (lastcol = firstcol; lastcol <= ncols; lastcol++) {
             fits_get_col_display_width
                (fptr, lastcol, &dispwidth[lastcol], &status);
             linewidth += dispwidth[lastcol] + 1;
             if (linewidth > 80)break;  
          }
          if (lastcol > firstcol)lastcol--;  /* the last col didn't fit */

          /* print column names as column headers */
          printf("\n    ");
          for (ii = firstcol; ii <= lastcol; ii++) {
              fits_make_keyn("TTYPE", ii, keyword, &status);
              fits_read_key(fptr, TSTRING, keyword, colname, NULL, &status);
              colname[dispwidth[ii]] = '\0';  /* truncate long names */
              printf("%*s ",dispwidth[ii], colname); 
	      fflush(stdout);
          }
          printf("\n");  /* terminate header line */

          /* print each column, row by row (there are faster ways to do this) */
          val = value; 
          for (jj = 1; jj <= nrows && !status; jj++) {
              printf("%4ld ", jj);
              for (ii = firstcol; ii <= lastcol; ii++)
              {
                  /* read value as a string, regardless of intrinsic datatype */
                  if (fits_read_col_str (fptr,ii,jj, 1, 1, nullstr,
                      &val, &anynul, &status) )
                     break;  /* jump out of loop on error */

                  printf("%-*s ",dispwidth[ii], value);
		  fflush(stdout);
              }
              printf("\n");
          }
        }
      }
      fits_close_file(fptr, &status);
    } 

    if (status) fits_report_error(stderr, status); /* print any error message */
    return(status);
}
