#ifndef COLOR_H
#define COLOR_H

#include <cmath>
#include <vector>
#include <algorithm>

class color {
  public:
    double r;
    double g;
    double b;


    
    double luminance() const {
      return double (r*0.2126+g*0.7152+b*0.0722);
    }

    
    double hue() const {
      double maxVal = std::max({r,g,b});
      double minVal = std::min({r,g,b});
      double delta = maxVal - minVal;

      if (delta == 0) return 0.0;  

      double hue = 0.0;
      if (maxVal == r) {
        hue = (g - b) / delta;
        if (hue < 0) hue += 6.0;
      } else if (maxVal == g) {
        hue = (b - r) / delta + 2.0;
      } else if (maxVal == b) {
        hue = (r - g) / delta + 4.0;
      }

      return hue / 6.0;
    }
    double saturation() const {
      double maxVal = std::max({r, g, b});
      double minVal = std::min({r, g, b});

      if (maxVal == minVal) {
          return 0.0;
      }

      return (maxVal - minVal) / maxVal;
    }

    double clamp(double x) const {
      if (x < 0.0) return 0.0;
      if (x > 1.0) return 1.0;
      return x;
    }

    void transform(int colorspace) {
      r = int(clamp(r)*colorspace) ;
      g = int(clamp(g)*colorspace) ;
      b = int(clamp(b)*colorspace) ;
    }

    void to_gamma() {
      r = std::pow(r,1/2.2);
      g = std::pow(g,1/2.2);
      b = std::pow(b,1/2.2);
    }

    void to_linear() {
      r = std::pow(r,2.2);
      g = std::pow(g,2.2);
      b = std::pow(b,2.2);
    }

    void write(std::ostream& os) const {
        os << static_cast<int>(r) << ' '
           << static_cast<int>(g) << ' '
           << static_cast<int>(b) << '\n';
    }


    color(double x,double y,double z) {
      r=x;
      g=y;
      b=z;
    }

    void absolute_grayscale(double middle) {
      double brightness = std::abs(middle-luminance());
      r = brightness;
      g = brightness;
      b = brightness;
    }

    void maneskin(double middle) {
      double brightness = middle-luminance();
      if (middle > 0) {
        r = brightness;
        g = 0.0;
        b = 0.0;
      } else {
        r = 0.0;
        g = brightness*-0.4;
        b = brightness*-1;
      }
    }

    void sepia() {
      double tempr = (r * .393) + (g *.769) + (b * .189);
      double tempg = (r * .349) + (g *.686) + (b * .168);
      double tempb = (r * .272) + (g *.534) + (b * .131);
      r = tempr;
      g = tempg;
      b = tempb;
    }

    void binary_grayscale(double min, double max, double onval, double offval) {
      if (min <= luminance() && luminance() <= max) {
        r = onval;
        g = onval;
        b = onval;
      } else {
        r = offval;
        g = offval;
        b = offval;
      }
    }
    
    // Comparisons between colors
    bool operator<(const color &other) const {
      return luminance() < other.luminance();
    }
    
    bool operator<=(const color& other) const {
        return luminance() <= other.luminance();
    }

    bool operator>(const color& other) const {
        return luminance() > other.luminance();
    }

    bool operator>=(const color& other) const {
        return luminance() >= other.luminance();
    }

    bool operator==(const color& other) const {
        return luminance() == other.luminance();
    }

    bool operator!=(const color& other) const {
        return luminance() != other.luminance();
    }

    // Comparisons between color and double
    bool operator<(double threshold) const {
        return luminance() < threshold;
    }

    bool operator<=(double threshold) const {
        return luminance() <= threshold;
    }

    bool operator>(double threshold) const {
        return luminance() > threshold;
    }

    bool operator>=(double threshold) const {
        return luminance() >= threshold;
    }

    bool operator==(double threshold) const {
        return luminance() == threshold;
    }

    bool operator!=(double threshold) const {
        return luminance() != threshold;
    }
    // Comparisons between double and color
    friend bool operator<(double threshold, const color& c) {
        return threshold < c.luminance();
    }

    friend bool operator<=(double threshold, const color& c) {
        return threshold <= c.luminance();
    }

    friend bool operator>(double threshold, const color& c) {
        return threshold > c.luminance();
    }

    friend bool operator>=(double threshold, const color& c) {
        return threshold >= c.luminance();
    }

    friend bool operator==(double threshold, const color& c) {
        return threshold == c.luminance();
    }

    friend bool operator!=(double threshold, const color& c) {
        return threshold != c.luminance();
    }

};
#endif
