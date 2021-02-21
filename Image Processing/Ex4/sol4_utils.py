from scipy.signal import convolve2d
import numpy as np
from skimage.color import rgb2gray
import imageio


def gaussian_kernel(kernel_size):
    conv_kernel = np.array([1, 1], dtype=np.float64)[:, None]
    conv_kernel = convolve2d(conv_kernel, conv_kernel.T)
    kernel = np.array([1], dtype=np.float64)[:, None]
    for i in range(kernel_size - 1):
        kernel = convolve2d(kernel, conv_kernel, 'full')
    return kernel / kernel.sum()


def blur_spatial(img, kernel_size):
    kernel = gaussian_kernel(kernel_size)
    blur_img = np.zeros_like(img)
    if len(img.shape) == 2:
        blur_img = convolve2d(img, kernel, 'same', 'symm')
    else:
        for i in range(3):
            blur_img[..., i] = convolve2d(img[..., i], kernel, 'same', 'symm')
    return blur_img


def read_image(filename, representation):
    """
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
        if im.shape[0] < 32 or im.shape[1] < 32:
            break
        im = convolve2d(im, filter_vec, mode='same', boundary='wrap').T
        im = convolve2d(im, filter_vec, mode='same', boundary='wrap').T
        im = reduce(im)
        pyr.append(im)
    return pyr, filter_vec
