321123788 kublakhan
312481294 idan2468

Part 1 Question 6:
As a result of creating separate vertices for each triangle, all the vertices
of a single triangle now contain the same normal. This is different from
when triangles shared vertices, and each vertex stored the mean of the normals
of the triangles that it belongs to. 
When interpolating the normals in the new model where each triangle has
unique vertices, the result is the same as the original normals because they
were the same. This means that every vertex contains the same normal and as
such will receive the same color.
