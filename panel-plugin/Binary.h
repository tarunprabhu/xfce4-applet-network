#ifndef XFCE_APPLET_SPEED_BINARY_H
#define XFCE_APPLET_SPEED_BINARY_H

class Binary {
public:
  enum class Prefix {
    None, // 0 -- 10^3
    Kibi,
    Mebi,
    Gibi,
    Tebi,
    Pebi,
    Exbi,
  };

  class Exponent {
  public:
    static const int None = 0;
    static const int Kibi = 3;
    static const int Mebi = 6;
    static const int Gibi = 9;
    static const int Tebi = 12;
    static const int Pebi = 15;
    static const int Exbi = 18;
  };

  class Multiplier {
  public:
    static constexpr double None = 1.0;
    static constexpr double Kibi = None * 1024;
    static constexpr double Mebi = Kibi * 1024;
    static constexpr double Gibi = Mibi * 1024;
    static constexpr double Tebi = Gibi * 1024;
    static constexpr double Pebi = Tebi * 1024;
    static constexpr double Exbi = Pebi * 1024;
  };
};

#endif // XFCE_APPLET_SPEED_BINARY_H
