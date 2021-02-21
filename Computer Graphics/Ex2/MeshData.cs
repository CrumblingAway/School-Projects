using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class MeshData
{
    public List<Vector3> vertices; // The vertices of the mesh 
    public List<int> triangles; // Indices of vertices that make up the mesh faces
    public Vector3[] normals; // The normals of the mesh, one per vertex

    // Class initializer
    public MeshData()
    {
        vertices = new List<Vector3>();
        triangles = new List<int>();
    }

    // Returns a Unity Mesh of this MeshData that can be rendered
    public Mesh ToUnityMesh()
    {
        Mesh mesh = new Mesh
        {
            vertices = vertices.ToArray(),
            triangles = triangles.ToArray(),
            normals = normals
        };

        return mesh;
    }

    // Calculates surface normals for each vertex, according to face orientation
    public void CalculateNormals()
    {
        normals = new Vector3[vertices.Count];
        for (var i = 0; i < vertices.Count; ++i)
        {
            var currentNormal = Vector3.zero;
            for (var j = 0; j < triangles.Count; j += 3)
            {
                var currentRange = triangles.GetRange(j, 3);
                if (currentRange.Contains(i))
                {
                    var p1 = vertices[currentRange[0]];
                    var p2 = vertices[currentRange[1]];
                    var p3 = vertices[currentRange[2]];
                    currentNormal += Vector3.Cross(p1 - p2, p1 - p3);
                }
            }

            currentNormal.Normalize();
            normals[i] = currentNormal;
        }
    }

    // Edits mesh such that each face has a unique set of 3 vertices
    public void MakeFlatShaded()
    {
        List<Vector3> newVertices = new List<Vector3>();
        for (var j = 0; j < triangles.Count; j += 3)
        {
            var p1 = vertices[triangles[j]];
            var p2 = vertices[triangles[j+1]];
            var p3 = vertices[triangles[j+2]];
            newVertices.AddRange(new Vector3[] { p1, p2, p3 });
        }
        vertices = newVertices;
    }
}