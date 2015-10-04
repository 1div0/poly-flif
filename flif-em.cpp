#include "flif_config.h"

#include "common.h"
#include "flif-dec.h"

#include <emscripten.h>

// TODO: Remove this (v_printf and related) from here and flif.cpp and move to common.cpp
#include <stdarg.h>

static int verbosity = 1;

void v_printf(const int v, const char *format, ...) {
    if (verbosity < v) return;
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    fflush(stdout);
    va_end(args);
}

void showImage(Image firstImage) {
  int numPlanes = firstImage.numPlanes();
  printf("Num decoded planes: %d", numPlanes);

  EM_ASM_({
    var canvas = document.getElementById('canvas');
    canvas.width = $0;
    canvas.height = $1;
    window.ctx = canvas.getContext('2d');
    window.imgData = window.ctx.getImageData(0,0,1,$0);
  }, firstImage.cols(), firstImage.rows());
      
  for (int i = 0; i < firstImage.cols(); i++) {
    for (int j = 0; j < firstImage.rows(); j++) {
      ColorVal r = firstImage(0, j, i);
      ColorVal g = firstImage(1, j, i);
      ColorVal b = firstImage(2, j, i);
      ColorVal a = numPlanes > 3 ? firstImage(3, j, i) : 255;
      EM_ASM_({
        var indx = $4 * 4;
        var imgData = window.imgData;
        imgData.data[indx + 0] = $0;
        imgData.data[indx + 1] = $1;
        imgData.data[indx + 2] = $2;
        imgData.data[indx + 3] = $3;
      }, r, g, b, a, j);
    }
    EM_ASM_({
        window.ctx.putImageData(imgData, $0, 0);
    }, i);
      
  }

  EM_ASM({
    window.ctx = undefined;
    window.imgData = undefined;
  });
}

extern "C" {

int mainy(int truncate, const char *fname) {
  printf("Hello new world !\n");
  Images images;
  int quality = 100;
  int scale = 1;
  if (!decode(fname, images, quality, scale, truncate)) return 3;
  printf("Num decoded images: %d\n", images.size());
  Image firstImage = images[0];
  showImage(firstImage);
  return 0;
}

}
