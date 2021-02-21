// Checks for an intersection between a ray and a sphere
// The sphere center is given by sphere.xyz and its radius is sphere.w
void intersectSphere(Ray ray, inout RayHit bestHit, Material material, float4 sphere)
{
    float3 d = normalize(ray.direction);
    float3 o = ray.origin;
    float3 c = sphere.xyz;
    float r = sphere.w;
    float3 oc = o - c;

    float A = 1;
    float B = 2 * dot(oc, d);
    float C = dot(oc, oc) - r * r;
    float discriminant = B * B - 4 * A * C;

    float t = -1;

    // Hit sphere
    if (discriminant >= 0)
    {
        float t1 = (-B + sqrt(discriminant)) / (2 * A);
        float t2 = (-B - sqrt(discriminant)) / (2 * A);
        if (t1 > 0) t = t1;
        if (t2 > 0 && t2 < t1) t = t2;

        if (t >= bestHit.distance || t == -1) return; // Check if new hit is further away or behind ray origin

        float3 hit_pos = o + d * t;
        bestHit.position = hit_pos;
        bestHit.distance = t;
        bestHit.normal = normalize(hit_pos - c);
        bestHit.material = material;
    }
}

// Checks for an intersection between a ray and a plane
// The plane passes through point c and has a surface normal n
void intersectPlane(Ray ray, inout RayHit bestHit, Material material, float3 c, float3 n)
{
    n = normalize(n);
    float3 d = normalize(ray.direction);
    float3 o = ray.origin;

    if (dot(d, n) >= 0) return; // Behind plane

    float t = (-dot((o - c), n)) / dot(d, n);
    if (t >= bestHit.distance || t < 0) return;
    float3 hit_pos = o + d * t;

    bestHit.position = hit_pos;
    bestHit.distance = t;
    bestHit.normal = n;
    bestHit.material = material;
}

// Checks for an intersection between a ray and a plane
// The plane passes through point c and has a surface normal n
// The material returned is either m1 or m2 in a way that creates a checkerboard pattern 
void intersectPlaneCheckered(Ray ray, inout RayHit bestHit, Material m1, Material m2, float3 c, float3 n)
{
    n = normalize(n);
    intersectPlane(ray, bestHit, m1, c, n);
    if(isinf(bestHit.distance)) return;

    // Sum of floor of relevant coordinates
    float3 mask = 1 - abs(n);
    float temp = dot(floor(2 * bestHit.position), mask);

    // Sum of floors even -> m1, odd -> m2
    if (temp % 2 != 0) bestHit.material = m2;
}


// Checks for an intersection between a ray and a triangle
// The triangle is defined by points a, b, c
void intersectTriangle(Ray ray, inout RayHit bestHit, Material material, float3 a, float3 b, float3 c)
{
    // Intersection with triangle plane
    float3 n = normalize(cross((a - c), (b - c)));
    RayHit temp = CreateRayHit();
    intersectPlane(ray, temp, material, c, n);
    if (isinf(temp.distance)) return;

    // Check if inside triangle
    float3 p = temp.position;
    bool cond1 = dot(cross((b - a), (p - a)), n) >= 0;
    bool cond2 = dot(cross((c - b), (p - b)), n) >= 0;
    bool cond3 = dot(cross((a - c), (p - c)), n) >= 0;
    if (!cond1 || !cond2 || !cond3) return; 
    if (temp.distance >= bestHit.distance) return;

    bestHit = temp;
}


// Checks for an intersection between a ray and a 2D circle
// The circle center is given by circle.xyz, its radius is circle.w and its orientation vector is n 
void intersectCircle(Ray ray, inout RayHit bestHit, Material material, float4 circle, float3 n)
{
    RayHit temp = CreateRayHit();
    intersectPlane(ray, temp, material, circle.xyz, n);
    if (isinf(temp.distance)) return;

    bool cond1 = distance(circle.xyz, temp.position) <= circle.w;
    if (!cond1 || temp.distance >= bestHit.distance) return;

    bestHit = temp;
}


// Checks for an intersection between a ray and a cylinder aligned with the Y axis
// The cylinder center is given by cylinder.xyz, its radius is cylinder.w and its height is h
void intersectCylinderY(Ray ray, inout RayHit bestHit, Material material, float4 cylinder, float h)
{
    // Top and bottom circles
    float4 top_circle = float4(cylinder.xyz + float3(0, h / 2, 0), cylinder.w);
    float4 bot_circle = float4(cylinder.xyz + float3(0, -h / 2, 0), cylinder.w);
    float3 top_n = float3(0, 1, 0);
    float3 bot_n = -top_n;

    RayHit topHit = CreateRayHit();
    intersectCircle(ray, topHit, material, top_circle, top_n);
    RayHit botHit = CreateRayHit();
    intersectCircle(ray, topHit, material, bot_circle, bot_n);

    // Calculate intersection with cylinder
    float d_x = ray.direction.x;
    float d_z = ray.direction.z;
    float oc_x = ray.origin.x - cylinder.x;
    float oc_z = ray.origin.z - cylinder.z;
    float r = cylinder.w;

    // Discriminant
    float A = d_x * d_x + d_z * d_z;
    float B = 2 * (d_x * oc_x + d_z * oc_z);
    float C = oc_x * oc_x + oc_z * oc_z - r * r;
    float discriminant = B * B - 4 * A * C;

    float t = -1;
    RayHit cylinderHit = CreateRayHit();
    if (discriminant >= 0)
    {
        float t1 = (-B + sqrt(discriminant)) / (2 * A);
        float t2 = (-B - sqrt(discriminant)) / (2 * A);
        if (t1 > 0) t = t1;
        if (t2 > 0 && t2 < t1) t = t2;

        float3 hit_pos = ray.origin + ray.direction * t;
        bool cond1 = t >= bestHit.distance || t == -1;
        bool out_of_range = hit_pos.y > top_circle.y || hit_pos.y < bot_circle.y; // Above or below cylinder

        if (!cond1 && !out_of_range)
        {
            bestHit.position = hit_pos;
            bestHit.distance = t;
            bestHit.normal = normalize(hit_pos - float3(cylinder.x, hit_pos.y, cylinder.z));
            bestHit.material = material;
        }
    }

    // Circle hit behind or in front of cylinder
    if (topHit.distance < botHit.distance && topHit.distance < bestHit.distance) bestHit = topHit;
    if (botHit.distance < topHit.distance && botHit.distance < bestHit.distance) bestHit = botHit;
}
