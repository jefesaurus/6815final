#include "a11_pset.h"
#include "matrix.h"
#include <math.h>
#include <ctime>

using namespace std;
// Put your function definitions here

// Gets the gradient magnitude and sums over all the channels
// returning a single channel image.
Image Energy(const Image &im) {
  Image energy = gradientMagnitude(im);
  Image out = Image(im.width(), im.height(), 1);
  for (int channel = 0; channel < energy.channels(); channel++) {
    for (int y = 0; y < energy.height(); y++) {
      for (int x = 0; x < energy.width(); x++) {
        out(x, y) += energy(x, y, channel);
      }
    }
  }
  // out = out / energy.channels();
  return out; 
}

Image ComputeCumulativeEnergy(const Image &im, bool vertical) {
  Image cumulative = Image(im.width(), im.height(), 1);
  float t1, t2, t3;
  float max_val = 0.0;

  if (vertical) {
    for (int y = im.height() - 1; y >= 0; y--) {
      for (int x = im.width() - 1; x >= 0; x--) {
        cumulative(x, y) = im(x, y);
        t1 = cumulative.smartAccessor(x, y + 1, 0, true);
        t2 = cumulative.smartAccessor(x - 1, y + 1, 0, true);
        t3 = cumulative.smartAccessor(x + 1, y + 1, 0, true);
        t1 = (t1 <= t2) ? t1 : t2;
        cumulative(x, y) += (t1 <= t3) ? t1 : t3;
      }
    }
    for (int x = 0; x < im.width(); x++) {
      if (cumulative(x, 0) > max_val) {
        max_val = cumulative(x, 0);
      }
    }
  } else {
    for (int x = im.width() - 1; x >= 0; x--) {
      for (int y = im.height() - 1; y >= 0; y--) {
        cumulative(x, y) = im(x, y);
        t1 = cumulative.smartAccessor(x + 1, y, 0, true);
        t2 = cumulative.smartAccessor(x + 1, y - 1, 0, true);
        t3 = cumulative.smartAccessor(x + 1, y + 1, 0, true);
        t1 = (t1 <= t2) ? t1 : t2;
        cumulative(x, y) += (t1 <= t3) ? t1 : t3;
      }
    }
    for (int y = 0; y < im.height(); y++) {
      if (cumulative(0, y) > max_val) {
       max_val = cumulative(0, y);
      }
    }
  }
  cumulative = cumulative / max_val;
  return cumulative;
}

// Returns the indices of the columns/rows of the optimal seam
// Bottom to top if !vertical, right to left if vertical
vector<int> GetOptimalSeam(const Image &energy, bool vertical) {
  vector<int> out;
  int current_min_index = 0;
  if (vertical) {
    // Find the minimum cumulative pixel at the bottom first.
    for (int x = 1; x < energy.width(); x++) {
      if (energy(x, 0) < energy(current_min_index, 0)) {
        current_min_index = x;
      }
    }
    out.push_back(current_min_index);

    for (int y = 1;  y < energy.height(); y++) {
      current_min_index = out.back();
      if (energy.smartAccessor(current_min_index, y, 0, true) > energy.smartAccessor(out.back() - 1, y, 0, true)) {
        current_min_index = out.back() - 1;
      }
      if (energy.smartAccessor(current_min_index, y, 0, true) > energy.smartAccessor(out.back() + 1, y, 0, true)) {
        current_min_index = out.back() + 1;
      }
      out.push_back(current_min_index);
    }
  } else {
    for (int y = 1; y < energy.height(); y++) {
      if (energy(0, y) < energy(0, current_min_index)) {
        current_min_index = y;
      }
    }
    out.push_back(current_min_index);

    for (int x = 1; x < energy.width(); x++) {
      current_min_index = out.back();
      if (energy.smartAccessor(x, current_min_index, 0, true) > energy.smartAccessor(x, out.back() - 1, 0, true)) {
        current_min_index = out.back() - 1;
      }
      if (energy.smartAccessor(x, current_min_index, 0, true) > energy.smartAccessor(x, out.back() + 1, 0, true)) {
        current_min_index = out.back() + 1;
      }
      out.push_back(current_min_index);
    }
  }
  return out;
}

Image HighlightSeam(const Image &in, vector<int> seam, bool vertical) {
  Image out = Image(in);
  if (vertical) {
    for (int y = 0; y < seam.size(); y++) {
      out(seam[y], y, 0) = 1.0;
      for (int c = 1; c < in.channels(); c++) {
        out(seam[y], y, c) = 0.0;
      }
    }
  } else {
    for (int x = 0; x < seam.size(); x++) {
      out(x, seam[x], 0) = 1.0;
      for (int c = 1; c < in.channels(); c++) {
        out(x, seam[x], c) = 0.0;
      }
    }
  }
  return out;
}

Image CarveSeam(const Image &in, vector<int> seam, bool vertical) {
  if (vertical) {
    Image out = Image(in.width() - 1, in.height(), in.channels());
    for (int y = 0; y < seam.size(); y++) {
      for (int x = 0; x < seam[y]; x++) {
        for (int c = 0; c < in.channels(); c++) {
          out(x, y, c) = in(x, y, c);
        }
      }
      for (int x = seam[y]; x < out.width(); x++) {
        for (int c = 0; c < in.channels(); c++) {
          out(x, y, c) = in(x+1, y, c);
        }
      }
    }
    return out;
  } else {
    Image out = Image(in.width(), in.height() - 1, in.channels());
    for (int x = 0; x < seam.size(); x++) {
      for (int y = 0; y < seam[x]; y++) {
        for (int c = 0; c < in.channels(); c++) {
          out(x, y, c) = in(x, y, c);
        }
      }
      for (int y = seam[x]; y < out.height(); y++) {
        for (int c = 0; c < in.channels(); c++) {
          out(x, y, c) = in(x, y+1, c);
        }
      }
    }
    return out;
  }
}

Image ShrinkImage(const Image &in, float horizontal_factor, float vertical_factor) {
  int new_width = in.width() * horizontal_factor;
  int new_height = in.height() * vertical_factor;

  Image current = Image(in);

  while (current.height() > new_height || current.width() > new_width) {
    printf("%d, %d\n", current.width() - new_width, current.height() - new_height);
    Image energy = Energy(current);

    bool vertical = true;
    if (current.width() / (float)new_width < current.height() / (float)new_height) {
      vertical = false;
    }
    Image cumulative_energy = ComputeCumulativeEnergy(energy, vertical);
    vector<int> seam = GetOptimalSeam(cumulative_energy, vertical);
    Image removed = CarveSeam(current, seam, vertical);
    current = removed;
  }

  return current;
}

vector<Image> ShrinkImageSequence(const Image &in, float horizontal_factor, float vertical_factor) {
  int new_width = in.width() * horizontal_factor;
  int new_height = in.height() * vertical_factor;

  vector<Image> out;
  out.push_back(in);

  while (out.back().height() > new_height || out.back().width() > new_width) {
    Image current = Image(out.back());
    printf("%d, %d\n", current.width() - new_width, current.height() - new_height);
    Image energy = Energy(current);

    bool vertical = true;
    if (current.width() / (float)new_width < current.height() / (float)new_height) {
      vertical = false;
    }
    Image cumulative_energy = ComputeCumulativeEnergy(energy, vertical);
    vector<int> seam = GetOptimalSeam(cumulative_energy, vertical);
    Image removed = CarveSeam(current, seam, vertical);
    out.push_back(removed);
  }

  return out;
}

Image Rescale(const Image &im, int desired_width, int desired_height) {
  Image im2(desired_width, desired_height, im.channels());

  for(int x = 0; x < desired_width; x++){
    for(int y = 0; y < desired_height; y++){
      for(int z = 0; z < im.channels(); z++){
        im2(x,y,z) = interpolateLin(im, ((float)x/desired_width)*im.width(), ((float)y/desired_height)*im.height(), z);
      }
    }
  }

  return im2;
}
