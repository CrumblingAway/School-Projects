321123788 kublakhan
312481294 idan2468

intersectPlaneCheckered implementation
---------------------------------------------------------------------------------
Since we may assume that the plane is axis-alligned, that means that its normal
has to be either [1, 0, 0], [0, 1, 0], or [0, 0, 1]. 
Let's assume that the releveant plane is YZ, which means that its normal is 
[1, 0, 0]. In that case the X component doesn't matter when calculating the 
material at a coordinate. 
The way we calculate the material is the folliwng: given a coordinate (x, y) we
multiply it by 2, take the floor function of it, and return m1 if the modulo of
their sum is 0 (i.e. floor(2x + 2y) % 2 == 0), otherwise we return m2. This works 
because the sides of each square on the grid are given to be 0.5, so when we 
multiply by 2 we get the size 1, from there we take the floor and all the pixels
in a given square get the same material.
---------------------------------------------------------------------------------


intersectCylinder implementation
---------------------------------------------------------------------------------
Given the cylinder formula from the PDF we can derive a quadratic formula just
like in the recitations with the following parameters:

Define oc_x = o_x - c_x, oc_z = o_z - c_z where o is the origin of the ray, c is
is the center of the cylinder and _x, _z are x and z coordinates.

A = d_x * d_x + d_z * d_z;
B = B = 2 * (d_x * oc_x + d_z * oc_z);
C = oc_x * oc_x + oc_z * oc_z - r * r;

With these we did the same as in intersectSphere. We also calculated the hit on 
the top and bottom disks of the cylinder and took the closest hit.
---------------------------------------------------------------------------------