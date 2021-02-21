# Initial code for ex4.
# You may change this code, but keep the functions' signatures
# You can also split the code to multiple files as long as this file's API is unchanged 

import numpy as np
from sol4_utils import *


import os
import matplotlib.pyplot as plt

from scipy.ndimage.morphology import generate_binary_structure
from scipy.ndimage.filters import maximum_filter
from scipy.ndimage import label, center_of_mass
import shutil
from imageio import imwrite

import sol4_utils

############## my imports ##############
from scipy.signal import convolve2d
from scipy.ndimage import map_coordinates
import matplotlib.collections as mcoll

############## my magic numbers ##############
derivative_filter_x = np.array([[1, 0, -1]])
derivative_filter_y = derivative_filter_x.T
harris_k = 0.04
default_desc_rad = 3


def get_derivative(im, axis=0):
    """
    :param im: grayscale image
    :param axis: 0 for x (rows) 1 for y (columns)
    :return: derivative of im along axis
    """
    if axis == 0:
        return convolve2d(im, derivative_filter_x, mode='same', boundary='symm')
    if axis == 1:
        return convolve2d(im, derivative_filter_y, mode='same', boundary='symm')


def harris_corner_detector(im):
    """
    Detects harris corners.
    Make sure the returned coordinates are x major!!!
    :param im: A 2D array representing an image.
    :return: An array with shape (N,2), where ret[i,:] are the [x,y] coordinates of the ith corner points.
    """
    im_derivative_x = get_derivative(im, axis=0)
    im_derivative_y = get_derivative(im, axis=1)

    # Construct matrix M
    blurred_der_x_squared = sol4_utils.blur_spatial(im_derivative_x ** 2, 3)
    blurred_der_y_squared = sol4_utils.blur_spatial(im_derivative_y ** 2, 3)
    blurred_der_xy = sol4_utils.blur_spatial(im_derivative_x * im_derivative_y, 3)
    blurred_der_yx = sol4_utils.blur_spatial(im_derivative_y * im_derivative_x, 3)

    # Calculate det and tr matrices and then R
    det_m = blurred_der_x_squared * blurred_der_y_squared - blurred_der_xy * blurred_der_yx
    trace_m = blurred_der_x_squared + blurred_der_y_squared
    R = non_maximum_suppression(det_m - (harris_k * (trace_m ** 2)))

    result = np.argwhere(R.T)
    return result


def sample_descriptor(im, pos, desc_rad):
    """
    Samples descriptors at the given corners.
    :param im: A 2D array representing an image.
    :param pos: An array with shape (N,2), where pos[i,:] are the [x,y] coordinates of the ith corner point.
    :param desc_rad: "Radius" of descriptors to compute.
    :return: A 3D array with shape (N,K,K) containing the ith descriptor at desc[i,:,:].
    """

    # Construct matrix of coordinates with (0, 0) in the center
    dim = desc_rad * 2 + 1
    default_frame = np.argwhere(np.ones((dim, dim))).reshape((dim, dim, 2))
    default_frame -= np.array([desc_rad, desc_rad])
    result = np.empty((pos.shape[0], dim, dim))

    for idx in range(pos.shape[0]):
        current_frame = default_frame + np.flip(pos[idx, :]) # Flip coords to treat as (i, j)
        descriptor = map_coordinates(im, current_frame.reshape((dim ** 2, 2)).T, order=1, prefilter=False)

        # Normalize
        descriptor_no_mean = descriptor - np.mean(descriptor)
        current_norm = np.linalg.norm(descriptor_no_mean)
        descriptor_normalized = np.zeros(descriptor_no_mean.shape)
        if current_norm != 0:
            descriptor_normalized = descriptor_no_mean / current_norm
        result[idx] = descriptor_normalized.reshape((dim, dim))

    return result


def find_features(pyr):
    """
    Detects and extracts feature points from a pyramid.
    :param pyr: Gaussian pyramid of a grayscale image having 3 levels.
    :return: A list containing:
                1) An array with shape (N,2) of [x,y] feature location per row found in the image.
                   These coordinates are provided at the pyramid level pyr[0].
                2) A feature descriptor array with shape (N,K,K)
    """
    # Calculate shape of windows
    # gcd_of_shape = np.gcd(pyr[0].shape[0], pyr[0].shape[1])
    n, m = 7, 7
    # if gcd_of_shape != 1:
    #     n, m = np.int(pyr[0].shape[0] / gcd_of_shape), np.int(pyr[0].shape[1] / gcd_of_shape)

    # Points and descriptors
    points_of_interest = spread_out_corners(pyr[0], n, m, 3)
    descriptor_array = sample_descriptor(pyr[2], 0.25 * points_of_interest, default_desc_rad)

    return [points_of_interest, descriptor_array]


def match_features(desc1, desc2, min_score):
    """
    Return indices of matching descriptors.
    :param desc1: A feature descriptor array with shape (N1,K,K).
    :param desc2: A feature descriptor array with shape (N2,K,K).
    :param min_score: Minimal match score.
    :return: A list containing:
                1) An array with shape (M,) and dtype int of matching indices in desc1.
                2) An array with shape (M,) and dtype int of matching indices in desc2.
    """

    # Flatten each row and multiply matrices,
    # dot_products[i, j] = dot of desc1[i] and desc2[j]
    flat_desc1 = desc1.reshape(desc1.shape[0], -1)
    flat_desc2 = desc2.reshape(desc2.shape[0], -1)
    dot_products = flat_desc1 @ flat_desc2.T

    # Construct masks for every condition, the matches are their product
    second_largest1 = np.partition(dot_products, -2, axis=1)[:, -2]
    second_largest2 = np.partition(dot_products, -2, axis=0)[-2]
    min_score_mask = (dot_products > min_score)
    second_largest_mask = np.maximum(second_largest1[:, np.newaxis], second_largest2[:, np.newaxis].T)
    final_mask = (dot_products >= second_largest_mask) & min_score_mask

    result = np.argwhere(final_mask)
    return [result[:, 0], result[:, 1]]


def apply_homography(pos1, H12):
    """
    Apply homography to inhomogenous points.
    :param pos1: An array with shape (N,2) of [x,y] point coordinates.
    :param H12: A 3x3 homography matrix.
    :return: An array with the same shape as pos1 with [x,y] point coordinates obtained from transforming pos1 using H12.
    """
    pos1 = np.hstack((pos1, np.ones((pos1.shape[0], 1))))
    transformed_pos = (H12 @ pos1.T)
    return (transformed_pos[0:2] / transformed_pos[2]).T


def ransac_homography(points1, points2, num_iter, inlier_tol, translation_only=False):
    """
    Computes homography between two sets of points using RANSAC.
    :param pos1: An array with shape (N,2) containing N rows of [x,y] coordinates of matched points in image 1.
    :param pos2: An array with shape (N,2) containing N rows of [x,y] coordinates of matched points in image 2.
    :param num_iter: Number of RANSAC iterations to perform.
    :param inlier_tol: inlier tolerance threshold.
    :param translation_only: see estimate rigid transform
    :return: A list containing:
                1) A 3x3 normalized homography matrix.
                2) An Array with shape (S,) where S is the number of inliers,
                    containing the indices in pos1/pos2 of the maximal set of inlier matches found.
    """
    point_amt = 1 if translation_only else 2
    current_inliers = np.array([])

    for _ in range(num_iter):
        # Pick points, estimate H12 and apply
        random_indices = np.random.choice(points1.shape[0], size=point_amt, replace=False)
        random_points1, random_points2 = points1[random_indices], points2[random_indices]
        current_H12 = estimate_rigid_transform(random_points1, random_points2, translation_only)
        transformed_p1 = apply_homography(points1, current_H12)

        # Calculate norm and inliers
        norms = np.linalg.norm(transformed_p1 - points2, axis=1) ** 2
        new_inliers = np.argwhere(norms < inlier_tol)
        if new_inliers.size > current_inliers.size: current_inliers = new_inliers

    # Estimate transform on inliers
    H12 = estimate_rigid_transform(points1[current_inliers.flatten()], points2[current_inliers.flatten()],
                                   translation_only)

    return [H12 / H12[2, 2], current_inliers.flatten()]


def display_matches(im1, im2, points1, points2, inliers):
    """
    Dispalay matching points.
    :param im1: A grayscale image.
    :param im2: A grayscale image.
    :parma pos1: An aray shape (N,2), containing N rows of [x,y] coordinates of matched points in im1.
    :param pos2: An aray shape (N,2), containing N rows of [x,y] coordinates of matched points in im2.
    :param inliers: An array with shape (S,) of inlier matches.
    """
    im_new = np.hstack((im1, im2))
    plt.imshow(im_new, cmap='gray')

    # Scatter interest points
    plt.scatter(points1[:, 0], points1[:, 1], marker='.', c='r')
    plt.scatter(points2[:, 0] + im1.shape[1], points2[:, 1], marker='.', c='r')

    # Separate outliers from inliers
    outlier_indices = np.setdiff1d(np.arange(points1.shape[0]), inliers)
    x_coord_outliers = np.vstack((points1[outlier_indices, 0], points2[outlier_indices, 0] + im1.shape[1]))
    y_coord_outliers = np.vstack((points1[outlier_indices, 1], points2[outlier_indices, 1]))
    x_coord_inliers = np.vstack((points1[inliers, 0], points2[inliers, 0] + im1.shape[1]))
    y_coord_inliers = np.vstack((points1[inliers, 1], points2[inliers, 1]))

    plt.plot(x_coord_outliers, y_coord_outliers, color='blue', lw='0.25')
    plt.plot(x_coord_inliers, y_coord_inliers, color='yellow', lw='0.5')
    plt.show()


def accumulate_homographies(H_succesive, m):
    """
    Convert a list of succesive homographies to a
    list of homographies to a common reference frame.
    :param H_successive: A list of M-1 3x3 homography
      matrices where H_successive[i] is a homography which transforms points
      from coordinate system i to coordinate system i+1.
    :param m: Index of the coordinate system towards which we would like to
      accumulate the given homographies.
    :return: A list of M 3x3 homography matrices,
      where H2m[i] transforms points from coordinate system i to coordinate system m
    """
    result = np.zeros((len(H_succesive) + 1, 3, 3))
    result[m] = np.eye(3)

    for idx in range(m-1, -1, -1):
        result[idx] = result[idx + 1] @ H_succesive[idx]
    for idx in range(m+1, len(H_succesive) + 1):
        result[idx] = result[idx - 1] @ np.linalg.inv(H_succesive[idx - 1])
        
    list_result = [result[h_idx] for h_idx in range(result.shape[0])]
    return list_result


def compute_bounding_box(homography, w, h):
    """
    computes bounding box of warped image under homography, without actually warping the image
    :param homography: homography
    :param w: width of the image
    :param h: height of the image
    :return: 2x2 array, where the first row is [x,y] of the top left corner,
     and the second row is the [x,y] of the bottom right corner
    """
    corners = np.array([[0, 0], [w-1, 0], [0, h-1], [w-1, h-1]])
    homography_corners = apply_homography(corners, homography)
    top_left = np.min(homography_corners, axis=0).astype(np.int)
    bottom_right = np.max(homography_corners, axis=0).astype(np.int)
    return np.array([top_left, bottom_right])


def warp_channel(image, homography):
    """
    Warps a 2D image with a given homography.
    :param image: a 2D image.
    :param homography: homograhpy.
    :return: A 2d warped image.
    """

    # Create bounds of new image
    bounding_box = compute_bounding_box(homography, image.shape[1], image.shape[0])
    x_values = np.arange(bounding_box[0, 0], bounding_box[1, 0] + 1)
    y_values = np.arange(bounding_box[0, 1], bounding_box[1, 1] + 1)
    temp = np.array(np.meshgrid(x_values, y_values)).reshape(2, -1).T

    # Backwards warping
    orig = apply_homography(temp, np.linalg.inv(homography)).T
    return map_coordinates(image, [orig[1], orig[0]], order=1, prefilter=False).reshape(y_values.shape[0],
                                                                                        x_values.shape[0])


def warp_image(image, homography):
    """
    Warps an RGB image with a given homography.
    :param image: an RGB image.
    :param homography: homograhpy.
    :return: A warped image.
    """
    return np.dstack([warp_channel(image[..., channel], homography) for channel in range(3)])


def filter_homographies_with_translation(homographies, minimum_right_translation):
    """
    Filters rigid transformations encoded as homographies by the amount of translation from left to right.
    :param homographies: homograhpies to filter.
    :param minimum_right_translation: amount of translation below which the transformation is discarded.
    :return: filtered homographies..
    """
    translation_over_thresh = [0]
    last = homographies[0][0, -1]
    for i in range(1, len(homographies)):
        if homographies[i][0, -1] - last > minimum_right_translation:
            translation_over_thresh.append(i)
            last = homographies[i][0, -1]
    return np.array(translation_over_thresh).astype(np.int)


def estimate_rigid_transform(points1, points2, translation_only=False):
    """
    Computes rigid transforming points1 towards points2, using least squares method.
    points1[i,:] corresponds to poins2[i,:]. In every point, the first coordinate is *x*.
    :param points1: array with shape (N,2). Holds coordinates of corresponding points from image 1.
    :param points2: array with shape (N,2). Holds coordinates of corresponding points from image 2.
    :param translation_only: whether to compute translation only. False (default) to compute rotation as well.
    :return: A 3x3 array with the computed homography.
    """
    centroid1 = points1.mean(axis=0)
    centroid2 = points2.mean(axis=0)

    if translation_only:
        rotation = np.eye(2)
        translation = centroid2 - centroid1

    else:
        centered_points1 = points1 - centroid1
        centered_points2 = points2 - centroid2

        sigma = centered_points2.T @ centered_points1
        U, _, Vt = np.linalg.svd(sigma)

        rotation = U @ Vt
        translation = -rotation @ centroid1 + centroid2

    H = np.eye(3)
    H[:2, :2] = rotation
    H[:2, 2] = translation
    return H


def non_maximum_suppression(image):
    """
    Finds local maximas of an image.
    :param image: A 2D array representing an image.
    :return: A boolean array with the same shape as the input image, where True indicates local maximum.
    """
    # Find local maximas.
    neighborhood = generate_binary_structure(2, 2)
    local_max = maximum_filter(image, footprint=neighborhood) == image
    local_max[image < (image.max() * 0.1)] = False

    # Erode areas to single points.
    lbs, num = label(local_max)
    centers = center_of_mass(local_max, lbs, np.arange(num) + 1)
    centers = np.stack(centers).round().astype(np.int)
    ret = np.zeros_like(image, dtype=np.bool)
    ret[centers[:, 0], centers[:, 1]] = True

    return ret


def spread_out_corners(im, m, n, radius):
    """
    Splits the image im to m by n rectangles and uses harris_corner_detector on each.
    :param im: A 2D array representing an image.
    :param m: Vertical number of rectangles.
    :param n: Horizontal number of rectangles.
    :param radius: Minimal distance of corner points from the boundary of the image.
    :return: An array with shape (N,2), where ret[i,:] are the [x,y] coordinates of the ith corner points.
    """
    corners = [np.empty((0, 2), dtype=np.int)]
    x_bound = np.linspace(0, im.shape[1], n + 1, dtype=np.int)
    y_bound = np.linspace(0, im.shape[0], m + 1, dtype=np.int)
    for i in range(n):
        for j in range(m):
            # Use Harris detector on every sub image.
            sub_im = im[y_bound[j]:y_bound[j + 1], x_bound[i]:x_bound[i + 1]]
            sub_corners = harris_corner_detector(sub_im)
            sub_corners += np.array([x_bound[i], y_bound[j]])[np.newaxis, :]
            corners.append(sub_corners)
    corners = np.vstack(corners)
    legit = ((corners[:, 0] > radius) & (corners[:, 0] < im.shape[1] - radius) &
             (corners[:, 1] > radius) & (corners[:, 1] < im.shape[0] - radius))
    ret = corners[legit, :]
    return ret


class PanoramicVideoGenerator:
    """
    Generates panorama from a set of images.
    """

    def __init__(self, data_dir, file_prefix, num_images):
        """
        The naming convention for a sequence of images is file_prefixN.jpg,
        where N is a running number 001, 002, 003...
        :param data_dir: path to input images.
        :param file_prefix: see above.
        :param num_images: number of images to produce the panoramas with.
        """
        self.file_prefix = file_prefix
        self.files = [os.path.join(data_dir, '%s%03d.jpg' % (file_prefix, i + 1)) for i in range(num_images)]
        self.files = list(filter(os.path.exists, self.files))
        self.panoramas = None
        self.homographies = None
        print('found %d images' % len(self.files))

    def align_images(self, translation_only=False):
        """
        compute homographies between all images to a common coordinate system
        :param translation_only: see estimte_rigid_transform
        """
        # Extract feature point locations and descriptors.
        points_and_descriptors = []
        for file in self.files:
            image = sol4_utils.read_image(file, 1)
            self.h, self.w = image.shape
            pyramid, _ = sol4_utils.build_gaussian_pyramid(image, 3, 7)
            points_and_descriptors.append(find_features(pyramid))

        # Compute homographies between successive pairs of images.
        Hs = []
        for i in range(len(points_and_descriptors) - 1):
            points1, points2 = points_and_descriptors[i][0], points_and_descriptors[i + 1][0]
            desc1, desc2 = points_and_descriptors[i][1], points_and_descriptors[i + 1][1]

            # Find matching feature points.
            ind1, ind2 = match_features(desc1, desc2, .7)
            points1, points2 = points1[ind1, :], points2[ind2, :]

            # Compute homography using RANSAC.
            H12, inliers = ransac_homography(points1, points2, 100, 6, translation_only)

            # Uncomment for debugging: display inliers and outliers among matching points.
            # In the submitted code this function should be commented out!
            # display_matches(self.images[i], self.images[i+1], points1 , points2, inliers)

            Hs.append(H12)

        # Compute composite homographies from the central coordinate system.
        accumulated_homographies = accumulate_homographies(Hs, (len(Hs) - 1) // 2)
        self.homographies = np.stack(accumulated_homographies)
        self.frames_for_panoramas = filter_homographies_with_translation(self.homographies, minimum_right_translation=5)
        self.homographies = self.homographies[self.frames_for_panoramas]

    def generate_panoramic_images(self, number_of_panoramas):
        """
        combine slices from input images to panoramas.
        :param number_of_panoramas: how many different slices to take from each input image
        """
        assert self.homographies is not None

        # compute bounding boxes of all warped input images in the coordinate system of the middle image (as given by the homographies)
        self.bounding_boxes = np.zeros((self.frames_for_panoramas.size, 2, 2))
        for i in range(self.frames_for_panoramas.size):
            self.bounding_boxes[i] = compute_bounding_box(self.homographies[i], self.w, self.h)

        # change our reference coordinate system to the panoramas
        # all panoramas share the same coordinate system
        global_offset = np.min(self.bounding_boxes, axis=(0, 1))
        self.bounding_boxes -= global_offset

        slice_centers = np.linspace(0, self.w, number_of_panoramas + 2, endpoint=True, dtype=np.int)[1:-1]
        warped_slice_centers = np.zeros((number_of_panoramas, self.frames_for_panoramas.size))
        # every slice is a different panorama, it indicates the slices of the input images from which the panorama
        # will be concatenated
        for i in range(slice_centers.size):
            slice_center_2d = np.array([slice_centers[i], self.h // 2])[None, :]
            # homography warps the slice center to the coordinate system of the middle image
            warped_centers = [apply_homography(slice_center_2d, h) for h in self.homographies]
            # we are actually only interested in the x coordinate of each slice center in the panoramas' coordinate system
            warped_slice_centers[i] = np.array(warped_centers)[:, :, 0].squeeze() - global_offset[0]

        panorama_size = np.max(self.bounding_boxes, axis=(0, 1)).astype(np.int) + 1

        # boundary between input images in the panorama
        x_strip_boundary = ((warped_slice_centers[:, :-1] + warped_slice_centers[:, 1:]) / 2)
        x_strip_boundary = np.hstack([np.zeros((number_of_panoramas, 1)),
                                      x_strip_boundary,
                                      np.ones((number_of_panoramas, 1)) * panorama_size[0]])
        x_strip_boundary = x_strip_boundary.round().astype(np.int)

        self.panoramas = np.zeros((number_of_panoramas, panorama_size[1], panorama_size[0], 3), dtype=np.float64)
        for i, frame_index in enumerate(self.frames_for_panoramas):
            # warp every input image once, and populate all panoramas
            image = sol4_utils.read_image(self.files[frame_index], 2)
            warped_image = warp_image(image, self.homographies[i])
            x_offset, y_offset = self.bounding_boxes[i][0].astype(np.int)
            y_bottom = y_offset + warped_image.shape[0]

            for panorama_index in range(number_of_panoramas):
                # take strip of warped image and paste to current panorama
                boundaries = x_strip_boundary[panorama_index, i:i + 2]
                image_strip = warped_image[:, boundaries[0] - x_offset: boundaries[1] - x_offset]
                x_end = boundaries[0] + image_strip.shape[1]
                self.panoramas[panorama_index, y_offset:y_bottom, boundaries[0]:x_end] = image_strip

        # crop out areas not recorded from enough angles
        # assert will fail if there is overlap in field of view between the left most image and the right most image
        crop_left = int(self.bounding_boxes[0][1, 0])
        crop_right = int(self.bounding_boxes[-1][0, 0])
        assert crop_left < crop_right, 'for testing your code with a few images do not crop.'
        print(crop_left, crop_right)
        self.panoramas = self.panoramas[:, :, crop_left:crop_right, :]

    def save_panoramas_to_video(self):
        assert self.panoramas is not None
        out_folder = 'tmp_folder_for_panoramic_frames/%s' % self.file_prefix
        try:
            shutil.rmtree(out_folder)
        except:
            print('could not remove folder')
            pass
        os.makedirs(out_folder)
        # save individual panorama images to 'tmp_folder_for_panoramic_frames'
        for i, panorama in enumerate(self.panoramas):
            imwrite('%s/panorama%02d.png' % (out_folder, i + 1), panorama)
        if os.path.exists('%s.mp4' % self.file_prefix):
            os.remove('%s.mp4' % self.file_prefix)
        # write output video to current folder
        os.system('ffmpeg -framerate 3 -i %s/panorama%%02d.png %s.mp4' %
                  (out_folder, self.file_prefix))

    def show_panorama(self, panorama_index, figsize=(20, 20)):
        assert self.panoramas is not None
        plt.figure(figsize=figsize)
        plt.imshow(self.panoramas[panorama_index].clip(0, 1))
        plt.show()


if __name__ == '__main__':
    # # Read images
    im_oxford1 = sol4_utils.read_image('external/oxford1.jpg', 1)
    im_oxford2 = sol4_utils.read_image('external/oxford2.jpg', 1)

    # Points and descriptors
    pyr1, filter_vec1 = sol4_utils.build_gaussian_pyramid(im_oxford1, 3, 5)
    points1, descriptors1 = find_features(pyr1)
    pyr2, filter_vec2 = sol4_utils.build_gaussian_pyramid(im_oxford2, 3, 5)
    points2, descriptors2 = find_features(pyr2)

    # Match features
    indices1, indices2 = match_features(descriptors1, descriptors2, 0.5)
    interest1, interest2 = points1[indices1], points2[indices2]

    # Show images and display matches
    plt.imshow(im_oxford1, cmap='gray')
    plt.scatter(interest1[:, 0], interest1[:, 1], marker='.', c='r')
    plt.show()

    plt.imshow(im_oxford2, cmap='gray')
    plt.scatter(interest2[:, 0], interest2[:, 1], marker='.', c='r')
    plt.show()

    H12, inliers = ransac_homography(interest1, interest2, 100, 6, False)
    display_matches(im_oxford1, im_oxford2, interest1, interest2, inliers)

    # Warp
    warped_oxford1 = warp_channel(im_oxford2, H12)
    plt.imshow(warped_oxford1, cmap='gray')
    plt.show()

    # m1 = np.array([[1, 2, 3], [4, 5, 6], [0, 0, 1]])
    # m2 = np.array([[1, 1, 0], [1, 0, 1], [0, 0, 1]])
    # m3 = np.array([[1, 2, 0], [0, 2, 1], [0, 0, 1]])
    # m4 = np.array([[1, 2, 0], [0, 2, 1], [0, 0, 1]])
    # m5 = np.array([[1, 2, 0], [0, 2, 1], [0, 0, 1]])
    # matrices = [m1, m2, m3, m4, m5]
    # result = accumulate_homographies(matrices, 1)
    # for mat in result:
    #     print(mat)

    # im1 = sol4_utils.read_image('external/oxford1.jpg', 1)
    # im2 = sol4_utils.read_image('external/oxford2.jpg', 1)
    # desc1 = find_features(sol4_utils.build_gaussian_pyramid(im1, 3, 3)[0])
    # desc2 = find_features(sol4_utils.build_gaussian_pyramid(im2, 3, 3)[0])
    # m1, m2 = match_features(desc1[1], desc2[1], 0.5)
    # print(len(m1), len(m2))
    # print(m1, m2)
