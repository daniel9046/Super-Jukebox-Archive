/*============================================================================
	A static library to make using FFT.DLL easier than putting
	the FFT code into your own app.
	Why is it so freaky?  Because I wrote it in five minutes, of
	course.  What do you care?  You don't need to look at it.
	Just compile it. =D
	
	Murphy McCauley 08/01/99
============================================================================*/

#include <windows.h>
#include "FFTLib.h"

typedef void (WINAPI *FFTDOUBLE)(unsigned, int, double *, double *, double *, double *);
typedef void (WINAPI *FFTFLOAT)(unsigned, int, float *, float *, float *, float *);
typedef double (WINAPI *INDEXTOFREQ)(unsigned, unsigned);

HANDLE		hFFTDLL;
FFTDOUBLE	fpFftDouble;
FFTFLOAT	fpFftFloat;
INDEXTOFREQ	fpIndexToFreq;


// The other three prototypes are in FFTLib.h (where they belong).
BOOL LoadFFTDLL ();
void UnloadFFTDLL ();


BOOL LoadFFTDLL (void) {
	// Returns True if the library is loaded, False otherwise.
	BOOL	AllIsWell = TRUE;

	if (hFFTDLL) {
		return TRUE;
	}

	if ((hFFTDLL = LoadLibrary("FFT.DLL")) < (HANDLE)32) {
		hFFTDLL = 0;
    	return FALSE; // Get outta here if we can't load DLL.
	}

	if ((fpFftDouble = (FFTDOUBLE)GetProcAddress(hFFTDLL, "fft_double")) == NULL) {
    	AllIsWell = FALSE;
	} else {
		if ((fpFftFloat = (FFTFLOAT)GetProcAddress(hFFTDLL, "fft_float")) == NULL) {
			AllIsWell = FALSE;
		} else {
			if ((fpIndexToFreq = (INDEXTOFREQ)GetProcAddress(hFFTDLL, "Index_to_frequency")) == NULL) {
				AllIsWell = FALSE;
			}
		}
	}

	if (AllIsWell == FALSE) {
		// Ack!  Bad news.  Unload the DLL.
		UnloadFFTDLL();
		return FALSE;
	}

	return TRUE;

}

void UnloadFFTDLL () {
	if (hFFTDLL) {
		FreeLibrary(hFFTDLL);
		hFFTDLL = 0;
	}
}

BOOL fftDouble (
    unsigned  NumSamples,
    int       InverseTransform,
    double   *RealIn,
    double   *ImagIn,
    double   *RealOut,
    double   *ImagOut ) {
	
	BOOL Success;
	if (LoadFFTDLL()) {
		Success = TRUE;
 		fpFftDouble(NumSamples, InverseTransform, RealIn, ImagIn, RealOut, ImagOut);
	} else {
		Success = FALSE;
	}

	return Success;
}

BOOL fftFloat (
    unsigned  NumSamples,
    int       InverseTransform,
    float   *RealIn,
    float   *ImagIn,
    float   *RealOut,
    float   *ImagOut ) {
	
	BOOL Success;
	if (LoadFFTDLL()) {
		Success = TRUE;
 		fpFftFloat(NumSamples, InverseTransform, RealIn, ImagIn, RealOut, ImagOut);
	} else {
		Success = FALSE;
	}

	return Success;
}

double indexToFrequency ( unsigned NumSamples, unsigned Index ) {
	double ReturnVal = -1; // returns -1 if it fails.  Stupid, but easy.

	if (LoadFFTDLL()) {
		ReturnVal = fpIndexToFreq(NumSamples, Index);		
	}

	return ReturnVal;
}
