// A simple, self-contained Perlin Noise implementation.
// (Based on the classic Java implementation by Ken Perlin)
#pragma once
#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

class PerlinNoise
{
  public:
    PerlinNoise()
    {
        reseed(2025);
    }

    void reseed(unsigned int seed)
    {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::default_random_engine engine(seed);
        std::shuffle(p.begin(), p.end(), engine);
        p.insert(p.end(), p.begin(), p.end());
    }

    double noise(double x, double y, double z)
    {
        int X = (int)floor(x) & 255;
        int Y = (int)floor(y) & 255;
        int Z = (int)floor(z) & 255;

        x -= floor(x);
        y -= floor(y);
        z -= floor(z);

        double u = fade(x);
        double v = fade(y);
        double w = fade(z);

        int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
        int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

        return lerp(
            w,
            lerp(
                v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)),
                lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))
            ),
            lerp(
                v,
                lerp(
                    u, grad(p[AA + 1], x, y, z - 1),
                    grad(p[BA + 1], x - 1, y, z - 1)
                ),
                lerp(
                    u, grad(p[AB + 1], x, y - 1, z - 1),
                    grad(p[BB + 1], x - 1, y - 1, z - 1)
                )
            )
        );
    }

  private:
    static double fade(double t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    static double lerp(double t, double a, double b)
    {
        return a + t * (b - a);
    }
    static double grad(int hash, double x, double y, double z)
    {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    std::vector<int> p;
};
