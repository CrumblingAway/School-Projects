using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;


public class CCMeshData
{
    public List<Vector3> points; // Original mesh points
    public List<Vector4> faces; // Original mesh quad faces
    public List<Vector4> edges; // Original mesh edges
    public List<Vector3> facePoints; // Face points, as described in the Catmull-Clark algorithm
    public List<Vector3> edgePoints; // Edge points, as described in the Catmull-Clark algorithm
    public List<Vector3> newPoints; // New locations of the original mesh points, according to Catmull-Clark
    public Dictionary<int[], int[]> pairToEdgeMapping; // Mapping of {face point, original point} to edge points that connect them in divided mesh
}


public static class CatmullClark
{
    // Returns a QuadMeshData representing the input mesh after one iteration of Catmull-Clark subdivision.
    public static QuadMeshData Subdivide(QuadMeshData quadMeshData)
    {
        // Create and initialize a CCMeshData corresponding to the given QuadMeshData
        CCMeshData meshData = new CCMeshData();
        meshData.points = quadMeshData.vertices;
        meshData.faces = quadMeshData.quads;
        meshData.edges = GetEdges(meshData);
        meshData.facePoints = GetFacePoints(meshData);
        meshData.edgePoints = GetEdgePoints(meshData);
        meshData.newPoints = GetNewPoints(meshData);

        // Combine facePoints, edgePoints and newPoints into a subdivided QuadMeshData
        return CreateNewQuadMeshData(meshData);
    }


    // Creating the Quad list for the new mesh after the subdivision
    private static List<Vector4> QuadsFromDict(Dictionary<int[], int[]> dict)
    {
        return (from keyValPair in dict
            let p1 = keyValPair.Key[0]
            let p2 = keyValPair.Value[1]
            let p3 = keyValPair.Key[1]
            let p4 = keyValPair.Value[0]
            select new Vector4(p1, p2, p3, p4)).ToList();
    }

    //Creating the new mesh after the subdivision
    private static QuadMeshData CreateNewQuadMeshData(CCMeshData mesh)
    {
        var vertices = mesh.newPoints.Concat(mesh.edgePoints.Concat(mesh.facePoints)).ToList();
        var quads = QuadsFromDict(mesh.pairToEdgeMapping);

        return new QuadMeshData(vertices, quads);
    }


    //Comparator used for EdgeDict in GetEdges func to compare it's keys so they represent the same edge  
    class EdgeComparer : EqualityComparer<int[]>
    {
        public override bool Equals(int[] x, int[] y)
        {
            return (x[0] == y[0] && x[1] == y[1]) || (x[0] == y[1] && x[1] == y[0]);
        }

        public override int GetHashCode(int[] obj)
        {
            return obj[0].GetHashCode() + obj[1].GetHashCode();
        }
    }

    // Returns a list of all edges in the mesh defined by given points and faces.
    // Each edge is represented by Vector4(p1, p2, f1, f2)
    // p1, p2 are the edge vertices
    // f1, f2 are faces incident to the edge. If the edge belongs to one face only, f2 is -1
    public static List<Vector4> GetEdges(CCMeshData mesh)
    {
        var edgeDict = new Dictionary<int[], Vector4>(new EdgeComparer());

        // Iterate over faces, add as values to dictionary[current edge]
        for (var faceIdx = 0; faceIdx < mesh.faces.Count; ++faceIdx)
        {
            for (var idx = 0; idx < 4; ++idx)
            {
                var currentKey = new int[2]
                    {(int) mesh.faces[faceIdx][idx % 4], (int) mesh.faces[faceIdx][(idx + 1) % 4]};
                if (edgeDict.ContainsKey(currentKey))
                {
                    edgeDict[currentKey] = new Vector4(edgeDict[currentKey].x, edgeDict[currentKey].y,
                        edgeDict[currentKey].z, faceIdx);
                }
                else
                {
                    edgeDict[currentKey] = new Vector4(currentKey[0], currentKey[1], faceIdx, -1);
                }
            }
        }

        return edgeDict.Values.ToList();
    }

    // Returns a list of "face points" for the given CCMeshData, as described in the Catmull-Clark algorithm 
    public static List<Vector3> GetFacePoints(CCMeshData mesh)
    {
        return mesh.faces.Select(face => (mesh.points[(int) face[0]] + mesh.points[(int) face[1]] +
                                          mesh.points[(int) face[2]] + mesh.points[(int) face[3]]) / 4).ToList();
    }

    // Returns a list of "edge points" for the given CCMeshData, as described in the Catmull-Clark algorithm
    public static List<Vector3> GetEdgePoints(CCMeshData mesh)
    {
        mesh.pairToEdgeMapping = new Dictionary<int[], int[]>(new EdgeComparer());

        return mesh.edges.Select((edge, idx) =>
        {
            var edgesOffset = mesh.points.Count; // offset of edge points in final QuadMeshData vertices
            var facesOffset = edgesOffset + mesh.edges.Count; // offset of face points in final QuadMeshData vertices
            var p1 = (int) edge[0];
            var p2 = (int) edge[1];
            var f1 = (int) edge[2];
            var f2 = (int) edge[3];
			
			// Add pairs (f1, p1), (p2, f1) (and if f2 exists (p1, f2), (f2, p2)) and their connecting edge points
			// (f1, p1) and (f2, p2) added in reverse order from (p2, f1) and (p1, f2) to get a clockwise sequence
            var pair1 = new int[2] {f1 + facesOffset, p1};
            var pair2 = new int[2] {p2, f1 + facesOffset};
            var pairs = new List<int[]>() {pair1, pair2};
            if (f2 != -1)
            {
                var pair3 = new int[2] {p1, f2 + facesOffset};
                var pair4 = new int[2] {f2 + facesOffset, p2};
                pairs.AddRange(new[] {pair3, pair4});
            }

			// Map to index of edge point being calculated
            foreach (var pair in pairs)
            {
                mesh.pairToEdgeMapping[pair] = mesh.pairToEdgeMapping.ContainsKey(pair)
                    ? new int[2] {mesh.pairToEdgeMapping[pair][0], idx + edgesOffset}
                    : new int[] {idx + edgesOffset};
            }
			
			// Calculate edge point
            var numToDivide = 3;
            var lastFacePoint = Vector3.zero;
            if (f2 != -1)
            {
                numToDivide = 4;
                lastFacePoint = mesh.facePoints[f2];
            }

            return (mesh.points[p1] + mesh.points[p2] + mesh.facePoints[f1] + lastFacePoint) / numToDivide;
        }).ToList();
    }

    // Creating a dictionary mapping from new points to their corresponding edge midpoints
    private static Dictionary<int, List<Vector3>> CreatePointsToEdgeMidpointsMap(CCMeshData mesh)
    {
        Dictionary<int, List<Vector3>> pointsToAvgEdgesMap = new Dictionary<int, List<Vector3>>();
        foreach (var edge in mesh.edges)
        {
            var p1 = (int) edge[0];
            var p2 = (int) edge[1];
            var avg = (mesh.points[p1] + mesh.points[p2]) / 2;
            if (!pointsToAvgEdgesMap.ContainsKey(p1))
            {
                pointsToAvgEdgesMap[p1] = new List<Vector3>();
            }

            if (!pointsToAvgEdgesMap.ContainsKey(p2))
            {
                pointsToAvgEdgesMap[p2] = new List<Vector3>();
            }

            pointsToAvgEdgesMap[p1].Add(avg);
            pointsToAvgEdgesMap[p2].Add(avg);
        }

        return pointsToAvgEdgesMap;
    }
	
    // Creating a dictionary mapping from new points to their corresponding face points
    private static Dictionary<int, List<Vector3>> CreatePointToFacePointMap(CCMeshData mesh)
    {
        Dictionary<int, List<Vector3>> pointToFacePointMap = new Dictionary<int, List<Vector3>>();
        for (var i = 0; i < mesh.faces.Count; i++)
        {
            var face = mesh.faces[i];
            for (var j = 0; j < 4; j++)
            {
                var p = (int) face[j];
                if (!pointToFacePointMap.ContainsKey(p))
                {
                    pointToFacePointMap[p] = new List<Vector3>();
                }

                pointToFacePointMap[p].Add(mesh.facePoints[i]);
            }
        }

        return pointToFacePointMap;
    }

    // Returns a list of new locations of the original points for the given CCMeshData, as described in the CC algorithm 
    public static List<Vector3> GetNewPoints(CCMeshData mesh)
    {
        var pointsToEdgePointsMap = CreatePointsToEdgeMidpointsMap(mesh);
        var pointToFacePointMap = CreatePointToFacePointMap(mesh);

        return mesh.points.Select((p, i) =>
        {
            var midPoint = pointsToEdgePointsMap[i];
            var facePoint = pointToFacePointMap[i];
            var r = Vector3.zero;
            var f = Vector3.zero;
            r = midPoint.Aggregate(r, (current, point) => current + point);
            f = facePoint.Aggregate(f, (current, point) => current + point);
            var n = midPoint.Count;
            r /= n;
            f /= n;
            return (f + 2 * r + (n - 3) * p) / n;
        }).ToList();
    }
}