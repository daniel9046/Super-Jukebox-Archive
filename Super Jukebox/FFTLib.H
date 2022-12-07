/*============================================================================
	FFTLib.H  -  Murphy McCauley <MurphyMc@Concentric.NET>
	Derived from Don Cross's Fourier.H	

	Contains prototypes for use with FFT.LIB.
	For info on what they actually do -- check the corresponding
	prototypes in Fourier.h.

	08/01/99
============================================================================*/

/* You may need to uncomment this next line. */
//#include <windows.h>

/*
** fftDouble() and fftFloat() attempt to load FFT.DLL and then call
** fft_double() and fft_float().  They return FALSE if the library
** couldn't be loaded and TRUE otherwise.
*/

#ifdef __cplusplus
extern "C"{
#endif

BOOL fftDouble (
    unsigned  NumSamples,          /* must be a power of 2 */
    int       InverseTransform,    /* 0=forward FFT, 1=inverse FFT */
    double   *RealIn,              /* array of input's real samples */
    double   *ImagIn,         /* array of input's imag samples */
    double   *RealOut,             /* array of output's reals */
    double   *ImagOut );      /* array of output's imaginaries */


BOOL fftFloat (
    unsigned  NumSamples,          /* must be a power of 2 */
    int       InverseTransform,    /* 0=forward FFT, 1=inverse FFT */
    float    *RealIn,              /* array of input's real samples */
    float    *ImaginaryIn,         /* array of input's imag samples */
    float    *RealOut,             /* array of output's reals */
    float    *ImaginaryOut );      /* array of output's imaginaries */


/*
** indexToFrequency() attempts to load FFT.DLL and then call 
** Index_to_frequency().  It returns -1 if the library couldn't be loaded,
** or the real result otherwise.  Kind of dumb, but...
*/
double indexToFrequency ( unsigned NumSamples, unsigned Index );

#ifdef __cplusplus
}
#endif