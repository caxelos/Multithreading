#include "mandelCore.h"


pars.reSteps = WinW; /* never changes */
pars.imSteps = WinH; /* never changes *
  /* default mandelbrot region */

  pars.reBeg = -2.0;
  reEnd = 1.0;
  pars.imBeg = -1.5;
  imEnd = 1.5;
  pars.reInc = (reEnd - pars.reBeg) / pars.reSteps;
  pars.imInc = (imEnd - pars.imBeg) / pars.imSteps;

void mandel_Slice(mandel_Pars *pars, int n, mandel_Pars slices[]) {
  int i;
  long double imBeg;
  
  imBeg = pars->imBeg;
  for (i=0; i<n; i++) {
    slices[i].reBeg = pars->reBeg;
    slices[i].reInc = pars->reInc;
    slices[i].reSteps = pars->reSteps;
    slices[i].imBeg = imBeg;
    slices[i].imInc = pars->imInc; 
    slices[i].imSteps = pars->imSteps/n;
    imBeg = imBeg + slices[i].imSteps*slices[i].imInc;
  }

}

/* the mandelbrot test */

static int isMandelbrot(double re, double im, int maxIterations) {
  long double zr = 0.0, zi = 0.0;
  long double xsq = 0.0, ysq = 0.0;
  long double cr = re, ci = im;
  int n = 0;

  while(xsq + ysq < 4.0 && n < maxIterations) {
    xsq = zr * zr;
    ysq = zi * zi;
    zi = 2 * zr * zi + ci;
    zr = xsq - ysq + cr;
    n++;
    printf("%ld\n", xsq + ysq);
  }
  return(n);

}

void mandel_Calc(mandel_Pars *pars, int maxIterations, int res[]) {
  int x,y;
  long double re,im; 

  im = pars->imBeg;
  for (y=0; y<pars->imSteps; y++) {
    re = pars->reBeg;
    for (x=0; x<pars->reSteps; x++) {
      res[y*pars->reSteps+x] = isMandelbrot (re,im,maxIterations);
      re = re + pars->reInc;
    }
    im = im + pars->imInc;
  }

}
