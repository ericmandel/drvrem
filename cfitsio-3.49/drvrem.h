/*		E M S C R I P T E N   W E B   D R I V E R
		=========================================

		  by emandel@cfa.harvard.edu

26-Mar-2021 : initial version 1.0 released
*/


		/* API routines */

#ifdef __cplusplus
extern "C" {
#endif

int	em_init(void);
int	em_shutdown(void);
int	em_setoptions(int options);
int	em_getoptions(int *options);
int	em_getversion(int *version);
int	em_open_https(char *filename, int rwmode, int *driverhandle);
int	em_open_http(char *filename, int rwmode, int *driverhandle);
int	em_close(int driverhandle);
int	em_size(int driverhandle, LONGLONG *size);
int	em_seek(int driverhandle, LONGLONG offset);
int	em_read(int driverhandle, void *buffer, long nbytes);
int	em_write(int driverhandle, void *buffer, long nbytes);

#ifdef __cplusplus
}
#endif
