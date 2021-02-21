import numpy as np
import os
import imageio
import matplotlib.pyplot as plt
from scipy.signal import convolve2d
from skimage.color import rgb2gray


def read_image(filename, representation):
    """
    Returns representation of image at filename in float64
    param filename: the filename of an image on disk (could be grayscale or RGB)
    param representation: representation code, either 1 or 2 defining whether the output should be a grayscale
    image (1) or an RGB image (2)
    return: an image
    """
    img = imageio.imread(filename) / 255
    return img if representation == 2 else rgb2gray(img)


def gaussian_filter(size):
    """
    Normalized gaussian estimation using binomial coefficients
    :param size:
    :return:
    """
    current_filter = np.array([1])
    for _ in range(1, size):
        current_filter = np.convolve(current_filter, np.array([1, 1]))
    return (current_filter / np.sum(current_filter)).reshape(1, size)


def reduce(im):
    """
    Return even rows and cols of im
    :param im:
    :return:
    """
    return im[0::2, 0::2]


def expand(im, filter_vec):
    """
    Given im of dims (m, n) return expanded image of dims (2m, 2n)
    :param filter_vec:
    :param im:
    :return:
    """
    result = np.zeros((im.shape[0] * 2, im.shape[1] * 2))
    result[0::2, 0::2] = im
    result = convolve2d(result, 2 * filter_vec, mode='same', boundary='wrap').T
    result = convolve2d(result, 2 * filter_vec, mode='same', boundary='wrap').T
    return result


def build_gaussian_pyramid(im, max_levels, filter_size):
    """
    max_levels tall gaussian pyramid of im
    :param im: image
    :param max_levels: height of pyramid
    :param filter_size: size of gaussian filter
    :return: pyr, filter_vec: pyramid and filter for building it
    """
    assert filter_size % 2 == 1, "filter_size should be odd"
    assert im.shape[0] % (2 ** (max_levels - 1)) == 0 and\
           im.shape[1] % (2 ** (max_levels - 1)) == 0, "Both sides are not multiples of 2 ** (max_levels - 1)"

    pyr = [im]
    filter_vec = gaussian_filter(filter_size)

    for _ in range(max_levels - 1):
        if im.shape[0] < 32 or im.shape[1] < 32: break
        im = convolve2d(im, filter_vec, mode='same', boundary='wrap').T # TODO: switch for double convolution
        im = convolve2d(im, filter_vec, mode='same', boundary='wrap').T
        im = reduce(im)
        pyr.append(im)
    return pyr, filter_vec


def build_laplacian_pyramid(im, max_levels, filter_size):
    """
    max_levels tall laplacian pyramid of im
    :param im: image
    :param max_levels: height of pyramid
    :param filter_size: size of gaussian filter
    :return: pyr, filter_vec: pyramid and filter for building it
    """
    pyr_gaussian, filter_vec = build_gaussian_pyramid(im, max_levels, filter_size)
    pyr = []
    for idx in range(len(pyr_gaussian) - 1):
        pyr.append(pyr_gaussian[idx] - expand(pyr_gaussian[idx + 1], filter_vec))
    return pyr + [pyr_gaussian[-1]], filter_vec


def laplacian_to_image(lpyr, filter_vec, coeff):
    """
    Reconstruct image from laplacian pyramid
    :param lpyr:
    :param filter_vec:
    :param coeff:
    :return:
    """
    assert len(lpyr) == len(coeff), "lpyr and coeff lengths don't match"

    result = coeff[-1] * lpyr[-1]
    for idx in range(len(lpyr) - 2, -1, -1):
        result = expand(result, filter_vec) + lpyr[idx]
    return result


def render_pyramid(pyr, levels):
    """
    Stretch layer values to [0, 1] and stack
    :param pyr:
    :param levels:
    :return:
    """
    res = np.array([]).reshape(pyr[0].shape[0], 0)

    for layer in range(levels):
        xp = [np.min(pyr[layer]), np.max(pyr[layer])]
        fp = [0, 1]
        stretched_layer = np.interp(pyr[layer], xp, fp)

        padding_shape = (pyr[0].shape[0] - pyr[layer].shape[0], pyr[layer].shape[1])
        padding = np.zeros(padding_shape)
        res = np.hstack((res, np.vstack((stretched_layer, padding))))

    return res


def display_pyramid(pyr, levels):
    """
    Plot hstacked pyr layers
    :param pyr:
    :param levels:
    :return:
    """
    assert levels <= len(pyr), "levels should be less than or equal to len(pyr)"

    res = render_pyramid(pyr, levels)
    plt.imshow(res, cmap='gray')
    plt.show()


def pyramid_blending(im1, im2, mask, max_levels, filter_size_im, filter_size_mask):
    """

    :param im1:
    :param im2:
    :param mask:
    :param max_levels:
    :param filter_size_im:
    :param filter_size_mask:
    :return:
    """
    laplasian_pyr_im1, filter_vec_im1 = build_laplacian_pyramid(im1, max_levels, filter_size_im)
    laplacian_pyr_im2, filter_vec_im2 = build_laplacian_pyramid(im2, max_levels, filter_size_im)
    gaussian_pyr_mask, filter_vec_mask = build_gaussian_pyramid(mask, max_levels, filter_size_mask)
    im_blend = gaussian_pyr_mask[-1] * laplasian_pyr_im1[-1] + (1 - gaussian_pyr_mask[-1]) * laplacian_pyr_im2[-1]
    for idx in range(len(gaussian_pyr_mask) - 2, -1, -1):
        im_blend = expand(im_blend, filter_vec_mask)
        im_blend += (gaussian_pyr_mask[idx] * laplasian_pyr_im1[idx]) +\
                                                          ((1 - gaussian_pyr_mask[idx]) * laplacian_pyr_im2[idx])
    return im_blend


def blending_example1():
    """
    Karen + viking blending
    :return:
    """
    im1 = read_image(relpath('images/karen.jpg'), 1)
    im2 = read_image(relpath('images/viking.jpg'), 1)
    mask = read_image(relpath('images/karen_viking_mask.png'), 1).astype(np.bool)
    im_blend = pyramid_blending(im1, im2, mask, 4, 3, 3)
    fig, ax = plt.subplots(2, 2)
    ax[0][0].imshow(im1, cmap='gray')
    ax[0][1].imshow(im2, cmap='gray')
    ax[1][0].imshow(mask, cmap='gray')
    ax[1][1].imshow(im_blend, cmap='gray')
    plt.show()

    return im1, im2, mask, im_blend


def blending_example2():
    """
    Harvey dent blending
    :return:
    """
    im1 = read_image(relpath('images/harvey1.png'), 1)
    im2 = read_image(relpath('images/harvey2.png'), 1)
    mask = read_image(relpath('images/harvey_mask.png'), 1).astype(np.bool)
    im_blend = pyramid_blending(im1, im2, mask, 4, 3, 3)
    fig, ax = plt.subplots(2, 2)
    ax[0][0].imshow(im1, cmap='gray')
    ax[0][1].imshow(im2, cmap='gray')
    ax[1][0].imshow(mask, cmap='gray')
    ax[1][1].imshow(im_blend, cmap='gray')
    plt.show()

    return im1, im2, mask, im_blend


def relpath(filename):
    return os.path.join(os.path.dirname(__file__), filename)
