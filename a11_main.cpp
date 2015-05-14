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
// Put your tests cases here! Make at least three of them.

void test_horizontal_seam() {
  Image im("./Input/copley.png");

  Image energy = Energy(im);
  Image cumulative_energy = ComputeCumulativeEnergy(energy, false);
  cumulative_energy.write("./Output/copley_cumulative_horizontal.png");

  vector<int> seam = GetOptimalSeam(cumulative_energy, false);
  Image highlighted = HighlightSeam(im, seam, false);
  highlighted.write("./Output/copley_seam_horizontal.png");

  Image removed = CarveSeam(im, seam, false);
  removed.write("./Output/copley_carved_horizontal.png");
}

void test_vertical_seam() {
  Image im("./Input/copley.png");

  Image energy = Energy(im);
  Image cumulative_energy = ComputeCumulativeEnergy(energy, true);
  cumulative_energy.write("./Output/copley_cumulative_vertical.png");

  vector<int> seam = GetOptimalSeam(cumulative_energy, true);
  Image highlighted = HighlightSeam(im, seam, true);
  highlighted.write("./Output/copley_seam_vertical.png");

  Image removed = CarveSeam(im, seam, true);
  removed.write("./Output/copley_carved_vertical.png");
}

void TestShrinkImage() {
  Image im("./Input/copley.png");
  Image shrunk = ShrinkImage(im, .95, .95);
  shrunk.write("./Output/seq/copley_shrunk");
}

void TestShrinkImageBurj() {
  Image im("./Input/burj.png");
  Image shrunk = ShrinkImage(im, .5, .5);
  shrunk.write("./Output/burj_shrunk");
}

void TestShrinkImageMatterhorn() {
  Image im("./Input/matterhorn2.png");
  Image shrunk = ShrinkImage(im, .6, 1.0);
  shrunk.write("./Output/matterhorn2_shrunk");
}

void TestShrinkImageSequence() {
  Image in("./Input/copley.png");
  vector<Image> shrunk_seq = ShrinkImageSequence(in, .25, .25);
  
  char buffer [50];
  for (int n = 0; n < shrunk_seq.size(); n++){
    sprintf(buffer, "./Output/seq/copley_shrunk_%d.png", n);
    shrunk_seq[n].write(buffer);
  }
}

void TestShrinkImageSequenceFace() {
  Image in("./Input/pose_small.png");
  int orig_width = in.width(); 
  int orig_height = in.height(); 
  float factor = .25;
  vector<Image> shrunk_seq = ShrinkImageSequence(in, factor, factor);
  
  char buffer [50];
  for (int n = 0; n < shrunk_seq.size(); n++){
    sprintf(buffer, "./Output/seq/pose_shrunk_%08d.png", n);
    Image rescaled = Rescale(shrunk_seq[n], orig_width, orig_height);
    rescaled.write(buffer);
  }
}

void TestShrinkImageSequenceFace2() {
  Image in("./Input/pose_2_small.png");
  int orig_width = in.width(); 
  int orig_height = in.height(); 
  float factor = .25;
  vector<Image> shrunk_seq = ShrinkImageSequence(in, factor, factor);
  
  char buffer [50];
  for (int n = 0; n < shrunk_seq.size(); n++){
    sprintf(buffer, "./Output/seq/pose_2_shrunk_%08d.png", n);
    Image rescaled = Rescale(shrunk_seq[n], orig_width, orig_height);
    rescaled.write(buffer);
  }
}

void TestShrinkImageSequenceFace3() {
  Image in("./Input/pose_3_small.png");
  int orig_width = in.width(); 
  int orig_height = in.height(); 
  float factor = .25;
  vector<Image> shrunk_seq = ShrinkImageSequence(in, factor, factor);
  
  char buffer [50];
  for (int n = 0; n < shrunk_seq.size(); n++){
    sprintf(buffer, "./Output/seq/pose_3_shrunk_%08d.png", n);
    Image rescaled = Rescale(shrunk_seq[n], orig_width, orig_height);
    rescaled.write(buffer);
  }
}

void TestShrinkImageSequenceFace4() {
  Image in("./Input/glalonde_small.png");
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



void TestRescale() {
  Image in("./Input/pose_small.png");
  Image shrunk = Rescale(in, 50, 50);
  shrunk.write("./Output/test_rescale.png");
}


// Your code **has to compile** on the submission system with all of the test cases
// uncommented! We will not grade the contents of this main function
int main() {
  //test_horizontal_seam();
  //test_vertical_seam();
  //TestShrinkImage();
  //TestShrinkImageSequenceFace4();
  //TestRescale();
  //TestShrinkImageBurj();
  TestShrinkImageMatterhorn();
  return 0;
}
