#include "basicImageManipulation.h"
#include "filtering.h"
#include "a11_pset.h"
#include "Image.h"
#include "ImageException.h"
#include "messageassert.h"
#include "matrix.h"
#include <ctime>
#include <iostream>
// #include <unistd.h>

using namespace std;

// Generic example shrinking only in X
void TestShrinkImageMatterhorn() {
  Image im("./Input/matterhorn.png");
  Image shrunk = ShrinkImage(im, .6, 1.0);
  shrunk.write("./Output/matterhorn_shrunk");
}

// Generates a sequence of video frames of the process
void TestShrinkImageSequenceFace() {
  Image in("./Input/glalonde.png");
  int orig_width = in.width(); 
  int orig_height = in.height(); 
  float factor = .25;
  vector<Image> shrunk_seq = ShrinkImageSequence(in, factor, factor);
  
  char buffer [50];
  for (int n = 0; n < shrunk_seq.size(); n++){
    sprintf(buffer, "./Output/seq/glalonde_shrunk_%08d.png", n);
    Image rescaled = Rescale(shrunk_seq[n], orig_width, orig_height);
    rescaled.write(buffer);
  }
}

// The first output is the actual optimal output
// The second is the energy map
void TestOptimal() {
  Image in("./Input/cat.png");
  vector<Image> shrunk = ShrinkImageOptimalOrdering(in, .8, .8);
  shrunk[0].write("./Output/cat_optimalshrink.png");
  shrunk[1].write("./Output/cat_energyremoved.png");
}


/* CAT IMAGE TEST SEQUENCE */

// The first step is to get the energy value for each pixel.
void TestEnergyCat() {
  Image im("./Input/cat.png");
  Image energy = Energy(im);
  energy = energy / energy.max();
  energy.write("./Output/cat_energy.png");
}

// Next accumulate the energy for "connected" pixels along the vertical dimension
void TestVerticalCumulativeEnergyCat() {
  Image im("./Input/cat.png");
  Image energy = Energy(im);
  Image cumulative_energy = ComputeCumulativeEnergy(energy, true);
  cumulative_energy = cumulative_energy / cumulative_energy.max();
  cumulative_energy.write("./Output/cat_vertical.png");
}

// Next, find the optimal seam (with the lowest energy)
// In this test, we highlight that seam on the original image.
void TestVerticalSeamCat() {
  Image im("./Input/cat.png");
  Image energy = Energy(im);
  Image cumulative_energy = ComputeCumulativeEnergy(energy, true);
  vector<int> seam = GetOptimalSeam(cumulative_energy, true);
  Image highlighted = HighlightSeam(im, seam, true);
  highlighted.write("./Output/cat_vertical_seam_highlit.png");
}

// Finally, remove that seam from the original image.
void TestVerticalSeamCarvedCat() {
  Image im("./Input/cat.png");
  Image energy = Energy(im);
  Image cumulative_energy = ComputeCumulativeEnergy(energy, true);
  vector<int> seam = GetOptimalSeam(cumulative_energy, true);
  Image highlighted = HighlightSeam(im, seam, true);
  Image removed = CarveSeam(im, seam, true);
  removed.write("./Output/cat_vertical_seam_carved.png");
}

// The final test is just the above repeated to satisfaction.
void TestShrinkCat() {
  Image im("./Input/cat.png");
  Image shrunk = ShrinkImage(im, .6, .6);
  shrunk.write("./Output/cat_shrunk.png");
}

// This generates the seam index for the given retargeting scheme
// using the same arguments as a regular shrink call
void TestSeamImage() {
  Image im("./Input/cat.png");
  Image shrunk = GetSeamImage(im, .01, .01);
  shrunk.write("./Output/cat_seams_many.png");
}


// Your code **has to compile** on the submission system with all of the test cases
// uncommented! We will not grade the contents of this main function
int main() {
  // Basic exmaple
  TestShrinkImageMatterhorn();

  // The main tests
  TestEnergyCat();
  TestVerticalCumulativeEnergyCat();
  TestVerticalSeamCat();
  TestVerticalSeamCarvedCat();
  TestShrinkCat();

  // These ones take a really long time:
  //TestSeamImage();
  //TestOptimal();
  //TestShrinkImageSequenceFace();

  return 0;
}
