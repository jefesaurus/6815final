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
  //cumulative = cumulative / max_val;
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

float GetSeamEnergy(const Image &cumulative_energy, vector<int> seam, bool vertical) {
  if (vertical) {
    return cumulative_energy(seam[0], 0);
  } else {
    return cumulative_energy(0, seam[0]);
  }
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

vector<Image> ShrinkImageOptimalOrdering(const Image &in, float horizontal_factor, float vertical_factor) {
  int new_width = in.width() * horizontal_factor;
  int new_height = in.height() * vertical_factor;
  int width_removed = in.width() - new_width;
  int height_removed = in.height() - new_height;

  Image energy_removed = Image(width_removed + 1, height_removed + 1, 1);

  vector< vector<Image> > image_map(energy_removed.height(), vector<Image>(energy_removed.width(), Image(0)));
  vector<Image> above_row(energy_removed.width(), Image(0));
  vector<Image> above_row_energy(energy_removed.width(), Image(0));

  above_row[0] = Image(in);
  above_row_energy[0] = Energy(above_row[0]);

  // First Row: Remove vertical seams
  for (int x = 1; x < energy_removed.width(); x++) {
    Image cumulative_energy = ComputeCumulativeEnergy(above_row_energy[x - 1], true);
    vector<int> seam = GetOptimalSeam(cumulative_energy, true);
    above_row[x] = CarveSeam(above_row[x - 1], seam, true);
    above_row_energy[x] = Energy(above_row[x]);

    float seam_energy = GetSeamEnergy(cumulative_energy, seam, true);
    energy_removed(x, 0) = energy_removed(x - 1, 0) + seam_energy;
  }

  vector<Image> out;

  for (int y = 1; y < energy_removed.height(); y++) {
    vector<Image> current_row(energy_removed.width(), Image(0));
    vector<Image> current_row_energy(energy_removed.width(), Image(0));

    Image cumulative_energy = ComputeCumulativeEnergy(above_row_energy[0], false);
    vector<int> seam = GetOptimalSeam(cumulative_energy, false);
    current_row[0] = CarveSeam(above_row[0], seam, false);
    current_row_energy[0] = Energy(current_row[0]);


    float seam_energy = GetSeamEnergy(cumulative_energy, seam, false);
    energy_removed(0, y) = energy_removed(0, y - 1) + seam_energy;

    for (int x = 1; x < energy_removed.width(); x++) {
      printf("%f, %f\n", x/(float)energy_removed.width(), y/(float)energy_removed.height());

      // From above: Horizontal
      Image cumulative_energy = ComputeCumulativeEnergy(above_row_energy[x], false);
      vector<int> hori_seam = GetOptimalSeam(cumulative_energy, false);
      float top_energy = GetSeamEnergy(cumulative_energy, hori_seam, false) + energy_removed(x, y - 1);

      // From left: Vertical
      cumulative_energy = ComputeCumulativeEnergy(current_row_energy[x - 1], true);
      vector<int> vert_seam = GetOptimalSeam(cumulative_energy, true);
      float left_energy = GetSeamEnergy(cumulative_energy, vert_seam, true) + energy_removed(x - 1, y);

      if (top_energy < left_energy) {
        current_row[x] = CarveSeam(above_row[x], hori_seam, false);
        energy_removed(x, y) = top_energy;
      } else {
        current_row[x] = CarveSeam(current_row[x - 1], vert_seam, true);
        energy_removed(x, y) = left_energy;
      }
      current_row_energy[x] = Energy(current_row[x]);
    }
    above_row = current_row;
    above_row_energy = current_row_energy;
  }

  // Normalize for visualization
  energy_removed = energy_removed / energy_removed.max();

  // Trace path.
  int x = width_removed;
  int y = height_removed;
  while (x > 0 || y > 0) {
    energy_removed(x, y) = 1.0;

    if (x == 0) {
      y -= 1;
    } else if (y == 0) {
      x -= 1;
    } else if (energy_removed(x - 1, y) < energy_removed(x, y - 1)) {
      x -= 1;
    } else {
      y -= 1;
    }
  }
  energy_removed(0, 0, 0) = 1.0;

  out.push_back(above_row[width_removed]);
  out.push_back(energy_removed);
  return out;
}

Image AddSeam(const Image &im, vector<int> seam, bool vertical, int channel, float value) {
  if (vertical) {
    Image out = Image(im.width() + 1, im.height(), im.channels());
    for (int y = 0; y < out.height(); y++) {
      for (int x = 0; x < seam[y]; x++) {
        for (int c = 0; c < im.channels(); c++) {
          out(x, y, c) = im(x, y, c);
        }
      }
      out(seam[y], y, channel) = value;
      for (int x = seam[y] + 1; x < out.width(); x++) {
        for (int c = 0; c < im.channels(); c++) {
          out(x, y, c) = im(x - 1, y, c);
        }
      }
    }
    return out;
  } else {
    Image out = Image(im.width(), im.height() + 1, im.channels());
    for (int x = 0; x < out.width(); x++) {
      for (int y = 0; y < seam[x]; y++) {
        for (int c = 0; c < im.channels(); c++) {
          out(x, y, c) = im(x, y, c);
        }
      }
      out(x, seam[x], channel) = value;
      for (int y = seam[x] + 1; y < out.height(); y++) {
        for (int c = 0; c < im.channels(); c++) {
          out(x, y, c) = im(x, y - 1, c);
        }
      }
    }
    return out;
  }
}

Image GetSeamImage(const Image &in, float horizontal_factor, float vertical_factor) {
  int new_width = in.width() * horizontal_factor;
  int new_height = in.height() * vertical_factor;

  Image current = Image(in);
  vector<vector<int> > seams;
  vector<bool> seam_vertical;

  while (current.height() > new_height || current.width() > new_width) {
    // Because this can take a while, we print the current progress along the two dimensions
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
    seams.push_back(seam);
    seam_vertical.push_back(vertical);
  }

  // At this point, current is the shrunk image.
  // So we replay the seams back onto a blac image of that size.
  Image seam_img = Image(current.width(), current.height(), 3);
  for (int i = 0; i < seams.size(); i++) {
    vector<int> current_seam = seams.back();
    bool vertical = seam_vertical.back();
    seam_img = AddSeam(seam_img, current_seam, vertical, (vertical) ? 0 : 2, ((float) i + 1.0) / (seams.size() + 1.0));
    seams.pop_back();
    seam_vertical.pop_back();
  }

  return seam_img;
}

// Basic bilateral image rescaling
// Used to keep all of the frames the same size for a video.
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
