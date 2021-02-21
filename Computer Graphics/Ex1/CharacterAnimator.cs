using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterAnimator : MonoBehaviour
{
    public TextAsset BVHFile; // The BVH file that defines the animation and skeleton
    public bool animate; // Indicates whether or not the animation should be running

    private BVHData data; // BVH data of the BVHFile will be loaded here
    private int currFrame = 0; // Current frame of the animation
    private float currTime = 0f;

    // Start is called before the first frame update
    void Start()
    {
        BVHParser parser = new BVHParser();
        data = parser.Parse(BVHFile);
        CreateJoint(data.rootJoint, Vector3.zero);
    }

    // Returns a Matrix4x4 representing a rotation aligning the up direction of an object with the given v
    Matrix4x4 RotateTowardsVector(Vector3 v)
    {
        v.Normalize();
        var degRotateZ = Mathf.Atan2(v.x, v.y) * Mathf.Rad2Deg;
        var degRotateX = Mathf.Atan2(v.z, v.y) * Mathf.Rad2Deg;
        var transformToZY = MatrixUtils.RotateX(degRotateX);
        var transformToXY = MatrixUtils.RotateZ(-degRotateZ);
        return transformToXY * transformToZY;
    }

    // Creates a Cylinder GameObject between two given points in 3D space
    GameObject CreateCylinderBetweenPoints(Vector3 p1, Vector3 p2, float diameter)
    {
        // Your code here
        var cylinder = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
        var height = Vector3.Distance(p1, p2);

        // Calculate Scale, Rotation and Translation matrices
        var S = MatrixUtils.Scale(new Vector3(diameter, height / 2, diameter));
        var R = RotateTowardsVector(p2 - p1);
        var T = MatrixUtils.Translate((p2 + p1) / 2);
        MatrixUtils.ApplyTransform(cylinder, T * R * S);
        return cylinder;
    }

    // Creates a GameObject representing a given BVHJoint and recursively creates GameObjects for it's child joints
    GameObject CreateJoint(BVHJoint joint, Vector3 parentPosition)
    {
        joint.gameObject = new GameObject();
        joint.gameObject.name = joint.name;

        var translateMatrix = MatrixUtils.Translate(parentPosition + joint.offset);
        if (joint == data.rootJoint) Debug.Log(translateMatrix + " " + joint.offset);
        var scaleFactor = joint.name == "Head" ? new Vector3(8, 8, 8) : new Vector3(2, 2, 2);
        var scalingMatrix = MatrixUtils.Scale(scaleFactor);


        var newSphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        newSphere.transform.parent = joint.gameObject.transform;

        MatrixUtils.ApplyTransform(newSphere, scalingMatrix);
        MatrixUtils.ApplyTransform(joint.gameObject, translateMatrix);

        foreach (var child in joint.children)
        {
            var childJoint = CreateJoint(child, joint.gameObject.transform.position);
            var cylinder = CreateCylinderBetweenPoints(childJoint.gameObject.transform.position,
                joint.gameObject.transform.position, .5f);
            cylinder.transform.parent = joint.gameObject.transform;
        }

        return joint.gameObject;
    }

    // Gets the Rotation Matrix to the current animation frame
    private Matrix4x4 getRMatrix(BVHJoint joint, float[] keyframe)
    {
        var xRotation = keyframe[joint.rotationChannels.x];
        var yRotation = keyframe[joint.rotationChannels.y];
        var zRotation = keyframe[joint.rotationChannels.z];

        Matrix4x4[] rotationMatrices = new Matrix4x4[3];
        rotationMatrices[joint.rotationOrder.x] = MatrixUtils.RotateX(xRotation);
        rotationMatrices[joint.rotationOrder.y] = MatrixUtils.RotateY(yRotation);
        rotationMatrices[joint.rotationOrder.z] = MatrixUtils.RotateZ(zRotation);

        var R = rotationMatrices[0] * rotationMatrices[1] * rotationMatrices[2];
        return R;
    }

    // Transforms BVHJoint according to the keyframe channel data, and recursively transforms its children
    private void TransformJoint(BVHJoint joint, Matrix4x4 parentTransform, float[] keyframe)
    {
        var S = MatrixUtils.Scale(Vector3.one);
        var rootPos = new Vector3(keyframe[joint.positionChannels.x], keyframe[joint.positionChannels.y],
            keyframe[joint.positionChannels.z]);
        var T = joint != data.rootJoint ? MatrixUtils.Translate(joint.offset) : MatrixUtils.Translate(rootPos);
        var R = !joint.isEndSite ? getRMatrix(joint, keyframe) : Matrix4x4.identity;
        var M = T * R * S;
        var newTransform = parentTransform * M;
        MatrixUtils.ApplyTransform(joint.gameObject, newTransform);

        foreach (var child in joint.children)
        {
            TransformJoint(child, newTransform, keyframe);
        }
    }

    // Updates the current frame of the animation by the given BVH file
    private void updateCurrentFrame()
    {
        currTime += Time.deltaTime;
        if (currTime >= data.frameLength)
        {
            currFrame++;

            if (currFrame >= data.numFrames) currFrame = 0;
            currTime = 0;
            TransformJoint(data.rootJoint, MatrixUtils.Scale(Vector3.one), data.keyframes[currFrame]);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (animate)
        {
            updateCurrentFrame();
        }
    }
}