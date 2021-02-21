#ifndef CG_RANDOM_INCLUDED
    // Upgrade NOTE: excluded shader from DX11, OpenGL ES 2.0 because it uses unsized arrays
    #pragma exclude_renderers d3d11 gles
    // Upgrade NOTE: excluded shader from DX11 because it uses wrong array syntax (type[size] name)
    #pragma exclude_renderers d3d11
    #define CG_RANDOM_INCLUDED

    // Returns a psuedo-random float between -1 and 1 for a given float c
    float random(float c)
    {
        return -1.0 + 2.0 * frac(43758.5453123 * sin(c));
    }

    // Returns a psuedo-random float2 with componenets between -1 and 1 for a given float2 c 
    float2 random2(float2 c)
    {
        c = float2(dot(c, float2(127.1, 311.7)), dot(c, float2(269.5, 183.3)));

        float2 v = -1.0 + 2.0 * frac(43758.5453123 * sin(c));
        return v;
    }

    // Returns a psuedo-random float3 with componenets between -1 and 1 for a given float3 c 
    float3 random3(float3 c)
    {
        float j = 4096.0 * sin(dot(c, float3(17.0, 59.4, 15.0)));
        float3 r;
        r.z = frac(512.0*j);
        j *= .125;
        r.x = frac(512.0*j);
        j *= .125;
        r.y = frac(512.0*j);
        r = -1.0 + 2.0 * r;
        return r.yzx;
    }

    // Interpolates a given array v of 4 float2 values using bicubic interpolation
    // at the given ratio t (a float2 with components between 0 and 1)
    //
    // [0]=====o==[1]
    //         |
    //         t
    //         |
    // [2]=====o==[3]
    //
    float bicubicInterpolation(float2 v[4], float2 t)
    {
        float2 u = t * t * (3.0 - 2.0 * t); // Cubic interpolation

        // Interpolate in the x direction
        float x1 = lerp(v[0], v[1], u.x);
        float x2 = lerp(v[2], v[3], u.x);

        // Interpolate in the y direction and return
        return lerp(x1, x2, u.y);
    }

    // Interpolates a given array v of 4 float2 values using biquintic interpolation
    // at the given ratio t (a float2 with components between 0 and 1)
    float biquinticInterpolation(float2 v[4], float2 t)
    {
        float2 u = t * t * t * (6 * t * t - 15 * t + 10); // Quintic Interpolation

        // Interpolate in the x direction
        float x1 = lerp(v[0], v[1], u.x);
        float x2 = lerp(v[2], v[3], u.x);

        // Interpolate in the y direction
        return lerp(x1, x2, u.y);
    }

    // Interpolates a given array v of 8 float3 values using triquintic interpolation
    // at the given ratio t (a float3 with components between 0 and 1)
    float triquinticInterpolation(float3 v[8], float3 t)
    {
        float3 u = 6 * pow(t, 5) - 15 * pow(t, 4) + 10 * pow(t, 3); // Quintic Interpolation

        // Interpolate in the x direction
        float x1 = lerp(v[0], v[1], u.x);
        float x2 = lerp(v[2], v[3], u.x);
        float x3 = lerp(v[4], v[5], u.x);
        float x4 = lerp(v[6], v[7], u.x);

        // Interpolate in the y direction
        float x5 = lerp(x1, x2, u.y);
        float x6 = lerp(x3, x4, u.y);

        // Interpolate in the z direction
        return lerp(x5, x6, u.z);
    }

    // Returns the value of a 2D value noise function at the given coordinates c
    float value2d(float2 c)
    {
        float2 corner = floor(c);
        float2 corner1 = random2(float2(corner.x, corner.y));
        float2 corner2 = random2(float2(corner.x + 1, corner.y));
        float2 corner3 = random2(float2(corner.x, corner.y + 1));
        float2 corner4 = random2(float2(corner.x + 1, corner.y + 1));
        
        float2 t = frac(c);
        float2 v[] = {corner1, corner2, corner3, corner4};
        return bicubicInterpolation(v, t);
    }

    // Returns the value of a 2D Perlin noise function at the given coordinates c
    float perlin2d(float2 c)
    {
        float2 corner1 = floor(c);
        float2 corners[4] = {corner1, 
                             float2(corner1.x + 1.0, corner1.y),
                             float2(corner1.x, corner1.y + 1.0),
                             float2(corner1.x + 1.0, corner1.y + 1.0)};

        // Calculate gradients and "distances" for dot product
        float2 grads[4];
        float2 dists[4];
        float2 v[4];

        for (int i = 0; i < 4; ++i)
        {
            grads[i] = random2(corners[i]);
            dists[i] = c - corners[i];
            v[i] = dot(grads[i], dists[i]);
        }
        float2 t = frac(c);

        return biquinticInterpolation(v, t);
    }

    // Returns the value of a 3D Perlin noise function at the given coordinates c
    float perlin3d(float3 c)
    {
        float3 corner1 = floor(c);
        float3 corners[8] = {corner1,
                            float3(corner1.x + 1.0, corner1.y, corner1.z),
                            float3(corner1.x, corner1.y + 1.0, corner1.z),
                            float3(corner1.x + 1.0, corner1.y + 1.0,corner1.z),
                            float3(corner1.x, corner1.y, corner1.z + 1.0),
                            float3(corner1.x + 1.0, corner1.y ,corner1.z + 1.0),
                            float3(corner1.x, corner1.y + 1.0,corner1.z + 1.0),
                            float3(corner1.x + 1.0, corner1.y + 1.0,corner1.z + 1.0)};

        // Calculate gradients and "distances" for dot product
        float3 grads[8];
        float3 dists[8];
        float3 v[8];

        for (int i = 0; i < 8; ++i)
        {
            grads[i] = random3(corners[i]);
            dists[i] = c - corners[i];
            v[i] = dot(grads[i], dists[i]);
        }

        float3 t = frac(c);
        return triquinticInterpolation(v, t);
    }


#endif // CG_RANDOM_INCLUDED
