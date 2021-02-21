from timeit import default_timer as timer
import numpy as np
import imageio
import matplotlib.pyplot as plt
from skimage.color import rgb2gray
from scipy.io import wavfile
from scipy.signal import convolve2d
from scipy import signal
from scipy.ndimage.interpolation import map_coordinates


def read_image(filename, representation):
    """
    Returns representation of image at filename in float64
    :param filename: the filename of an image on disk (could be grayscale or RGB)
    :param representation: representation code, either 1 or 2 defining whether the output should be a grayscale
           image (1) or an RGB image (2)
    :return: an image
    """
    img = imageio.imread(filename) / 255
    return img if representation == 2 else rgb2gray(img)


def DFT_helper(vector, const):
    """
    The only difference between DFT and IDFT is multiplying by -2 instead of 2 respectively
    :param vector:
    :param const: either 2 or -2
    :return:
    """
    dimension = vector.shape[0]
    x = np.arange(dimension)
    exp_mat = np.exp(const * np.pi * 1j * x.reshape(dimension, 1) * x / dimension)
    return exp_mat @ vector


def DFT(signal):
    """
    Perform Discrete Fourier Transform on signal
    :param signal:
    :return:
    """
    return DFT_helper(signal, -2)


def IDFT(fourier_signal):
    """
    Perform Inerse Fourier Transform on fourier_signal
    :param fourier_signal:
    :return:
    """
    return DFT_helper(fourier_signal, 2) / fourier_signal.shape[0]


def DFT2_helper(image, const):
    """
    The only difference between DFT2 and IDFT2 is multiplying by -2 instead of 2 respectively
    :param image:
    :param const:
    :return:
    """
    m, n = image.shape[0], image.shape[1]
    x, y = np.arange(m), np.arange(n)
    left_exp_mat = np.exp(const * 1j * np.pi * x.reshape(m, 1) * x / m)
    right_exp_mat = np.exp(const * 1j * np.pi * y.reshape(n, 1) * y / n).T
    return (left_exp_mat @ image.reshape(m, n) @ right_exp_mat.T).reshape(image.shape)


def DFT2(image):
    """
    Perform 2D DFT on image
    :param image:
    :return:
    """
    return DFT2_helper(image, -2)


def IDFT2(fourier_image):
    """
    Perform 2D IDFT on fourier_image
    :param fourier_image:
    :return:
    """
    return DFT2_helper(fourier_image, 2) / (fourier_image.shape[0] * fourier_image.shape[1])


def change_rate(filename, ratio):
    """
    Changes duration of an audio file by changing the sample rate
    :param filename:
    :param ratio: between 0.25 and 4
    :return:
    """
    rate, data = wavfile.read(filename)
    wavfile.write('change_rate.wav', int(rate * ratio), data)


def change_samples(filename, ratio):
    """
    Changes duration of an audio file by reducing the number of samples using Fourier
    :param filename:
    :param ratio: between 0.25 and 4
    :return:
    """
    rate, data = wavfile.read(filename)
    data = resize(data, ratio)
    wavfile.write('change_samples.wav', rate, data)
    return data.astype(np.float64)


def resize(data, ratio):
    """
    Changes the number of samples by ratio
    :param data:
    :param ratio:
    :return:
    """
    output_type = data.dtype
    data = DFT(data)
    if ratio < 1:
        cutoff = 0.5 * (int(data.shape[0] / ratio) - data.shape[0])
        left, right = int(np.floor(cutoff)), int(np.ceil(cutoff))
        data = np.hstack((np.zeros(left), data.flatten(), np.zeros(right)))
    elif ratio > 1:
        cutoff = 0.5 * (data.shape[0] - int(data.shape[0] / ratio))
        data = data[int(cutoff): data.shape[0] - int(np.ceil(cutoff))]
    return IDFT(np.fft.ifftshift(data)).flatten().astype(output_type)


def resize_spectrogram(data, ratio):
    """
    Speeds up .wav file using spectrogram scaling
    :param data:
    :param ratio:
    :return:
    """
    result_stft = stft(data)
    resized_stft = np.array([resize(line, ratio) for line in result_stft])
    return istft(np.array(resized_stft)).astype(data.dtype)


def resize_vocoder(data, ratio):
    """
    Spectrogram scaling with correction of the phase of each frequency according to the shift
    of each window
    :param data:
    :param ratio:
    :return:
    """
    return istft(phase_vocoder(stft(data), ratio)).astype(data.dtype)


def conv_der(im):
    """
    Computes the magnitude of the image derivatives using convolution
    :param im:
    :return:
    """
    x_convolution = np.array([[0.5, 0, -0.5]])
    y_convolution = x_convolution.T
    dx, dy = convolve2d(im, x_convolution, mode='same'), convolve2d(im, y_convolution, mode='same')
    return  np.sqrt (np.abs(dx)**2 + np.abs(dy)**2)


def fourier_der(im):
    """
    Computes the magnitude of the image derivatives using Fourier
    :param im:
    :return:
    """

    m, n = im.shape
    dft2_im = np.fft.fftshift(DFT2(im))
    m_range, n_range = (2j * np.pi / m) * np.arange(-m // 2, int(np.ceil(m // 2))).reshape(m, 1), \
                       (2j * np.pi / n) * np.arange(-n // 2, int(np.ceil(n // 2))).reshape(n, 1)
    dx, dy = IDFT2(np.fft.ifftshift(m_range * dft2_im)), IDFT2(np.fft.ifftshift(n_range * dft2_im.T)).T
    return  np.sqrt (np.abs(dx)**2 + np.abs(dy)**2)


def stft(y, win_length=640, hop_length=160):
    fft_window = signal.windows.hann(win_length, False)

    # Window the time series.
    n_frames = 1 + (len(y) - win_length) // hop_length
    frames = [y[s:s + win_length] for s in np.arange(n_frames) * hop_length]

    stft_matrix = np.fft.fft(fft_window * frames, axis=1)
    return stft_matrix.T


def istft(stft_matrix, win_length=640, hop_length=160):
    n_frames = stft_matrix.shape[1]
    y_rec = np.zeros(win_length + hop_length * (n_frames - 1), dtype=np.float)
    ifft_window_sum = np.zeros_like(y_rec)

    ifft_window = signal.windows.hann(win_length, False)[:, np.newaxis]
    win_sq = ifft_window.squeeze() ** 2

    # invert the block and apply the window function
    ytmp = ifft_window * np.fft.ifft(stft_matrix, axis=0).real

    for frame in range(n_frames):
        frame_start = frame * hop_length
        frame_end = frame_start + win_length
        y_rec[frame_start: frame_end] += ytmp[:, frame]
        ifft_window_sum[frame_start: frame_end] += win_sq

    # Normalize by sum of squared window
    y_rec[ifft_window_sum > 0] /= ifft_window_sum[ifft_window_sum > 0]
    return y_rec


def phase_vocoder(spec, ratio):
    num_timesteps = int(spec.shape[1] / ratio)
    time_steps = np.arange(num_timesteps) * ratio

    # interpolate magnitude
    yy = np.meshgrid(np.arange(time_steps.size), np.arange(spec.shape[0]))[1]
    xx = np.zeros_like(yy)
    coordiantes = [yy, time_steps + xx]
    warped_spec = map_coordinates(np.abs(spec), coordiantes, mode='reflect', order=1).astype(np.complex)

    # phase vocoder
    # Phase accumulator; initialize to the first sample
    spec_angle = np.pad(np.angle(spec), [(0, 0), (0, 1)], mode='constant')
    phase_acc = spec_angle[:, 0]

    for (t, step) in enumerate(np.floor(time_steps).astype(np.int)):
        # Store to output array
        warped_spec[:, t] *= np.exp(1j * phase_acc)

        # Compute phase advance
        dphase = (spec_angle[:, step + 1] - spec_angle[:, step])

        # Wrap to -pi:pi range
        dphase = np.mod(dphase - np.pi, 2 * np.pi) - np.pi

        # Accumulate phase
        phase_acc += dphase

    return warped_spec
