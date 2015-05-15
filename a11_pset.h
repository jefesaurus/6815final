#ifndef __a11_pset__h
#define __a11_pset__h

using namespace std;
#include "basicImageManipulation.h"
#include "filtering.h"
#include "a11_pset.h"
#include "Image.h"
#include "ImageException.h"
#include "messageassert.h"
#include "matrix.h"
#include <iostream>
#include <math.h>

#define pi 3.1415926535897

// Put your function declarations here
Image Energy(const Image &im);

Image ComputeCumulativeEnergy(const Image &im, bool horizontal);

vector<int> GetOptimalSeam(const Image &energy, bool vertical);

float GetSeamEnergy(const Image &cumulative_energy, vector<int> seam, bool vertical);

Image HighlightSeam(const Image &in, vector<int> seam, bool vertical);

Image CarveSeam(const Image &in, vector<int> seam, bool vertical);

Image ShrinkImage(const Image &in, float horizontal_factor, float vertical_factor);


vector<Image> ShrinkImageOptimalOrdering(const Image &in, float horizontal_factor, float vertical_factor);

vector<Image> ShrinkImageSequence(const Image &in, float horizontal_factor, float vertical_factor);

Image AddSeam(const Image &im, vector<int> seam, bool vertical, int channel, float value);

Image GetSeamImage(const Image &in, float horizontal_factor, float vertical_factor);

Image Rescale(const Image &im, int desired_width, int desired_height);

#endif
