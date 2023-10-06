#pragma once

#include "utils.h"

#include <array>

class perlin
{
private:
    static const int n_point = 256;
    std::array<vec3, perlin::n_point> ranv;
    std::array<int, perlin::n_point> perm_x;
    std::array<int, perlin::n_point> perm_y;
    std::array<int, perlin::n_point> perm_z;

    void perlin_generate_perm(std::array<int, perlin::n_point> &p)
    {
        for (int i = 0; i < n_point; i++)
        {
            p[i] = i;
            int j = random_int(0, i - 1);
            std::swap(p[i], p[j]);
        }
    }

    static double trilinear_interp(vec3 c[2][2][2], double u, double v, double w)
    {
        auto uu = smoothstep(0, 1, u);
        auto vv = smoothstep(0, 1, v);
        auto ww = smoothstep(0, 1, w);
        auto sum = 0.0;
        for (int i = 0; i <= 1; i++)
        {
            double prod1 = (i * uu + (1 - i) * (1 - uu));
            for (int j = 0; j <= 1; j++)
            {
                double prod2 = (j * vv + (1 - j) * (1 - vv));
                for (int k = 0; k <= 1; k++)
                {
                    double prod3 = (k * ww + (1 - k) * (1 - ww));
                    vec3 weight_v(u - i, v - j, w - k);
                    sum += prod1 * prod2 * prod3 * dot(c[i][j][k], weight_v);
                }
            }
        }
        return sum;
    }

public:
    perlin()
    {
        for (int i = 0; i < n_point; i++)
            ranv[i] = random_unit_vector();
        
        perlin_generate_perm(perm_x);
        perlin_generate_perm(perm_y);
        perlin_generate_perm(perm_z);
    }

    double noise(const point3 &p) const
    {
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());
        auto i = static_cast<int>(floor(p.x()));
        auto j = static_cast<int>(floor(p.y()));
        auto k = static_cast<int>(floor(p.z()));
        vec3 c[2][2][2];

        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                    c[di][dj][dk] = ranv[
                        perm_x[(i + di) & (n_point - 1)] ^
                        perm_y[(j + dj) & (n_point - 1)] ^
                        perm_z[(k + dk) & (n_point - 1)]
                    ];
        
        return trilinear_interp(c, u, v, w);
    }

    double turb(const point3 &p, int depth = 7) const
    {
        auto sum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++)
        {
            sum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(sum);
    }
};