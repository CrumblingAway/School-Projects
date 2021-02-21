from adaboost import *
from ex4_tools import *

SAMPLES_AMT = 5000
TEST_AMT = 200
ZERO_NOISE = 0
DEF_T = 500
MIN_T = {0: 15, 0.01: 49, 0.4: 100}
T_ARR = [5, 10, 50, 100, 200, 500]


def q10(samples, tests, noise):
    """
    Draws graphs for question 10
    :param samples: training set
    :param tests: test set
    :param noise: noise used for generating samples and tests
    :return:
    """
    t_arr = np.arange(DEF_T)

    # Calculate data for graphs
    error_arr1 = np.zeros(shape=DEF_T)
    error_arr2 = np.zeros(shape=DEF_T)
    for i in t_arr:
        error_arr1[i] = stump.error(samples[0], samples[1], i)
        error_arr2[i] = stump.error(tests[0], tests[1], i)

    plt.title("Error of Adaboost on training and test samples\n as function of iteration amount, noise={}".format(noise))
    plt.plot(t_arr, error_arr1, color="blue", label="training")
    plt.plot(t_arr, error_arr2, color="orange", label="test")
    plt.legend()
    plt.savefig("q10_noise_{}.png".format(noise))
    plt.show()
    plt.close()

def q11(tests, noise):
    """
    Draws graphs for question 11
    :param tests: test set
    :param noise: noise used to generate tests
    :return:
    """
    idx = 321
    plt.suptitle("Decision boundaries with noise = {}".format(noise))
    for t in T_ARR:
        plt.subplot(idx)
        decision_boundaries(stump, tests[0], tests[1], t)
        idx += 1
    plt.savefig("q11_noise_{}.png".format(noise))
    plt.show()
    plt.close()


def q12(samples, noise, t):
    """
    Draws graphs for question 12
    :param samples: training set
    :param noise: noise used to generate samples
    :param t: amount of learners
    :return:
    """
    decision_boundaries(stump, samples[0], samples[1], t)
    plt.suptitle("Decision boundaries for noise={} and T={}".format(noise, t))
    plt.savefig("q12_noise_{}.png".format(noise))
    plt.show()
    plt.close()


def q13(w, samples, noise):
    """
    Draws graphs for question 13
    :param w: weights of points in samples
    :param samples: training set
    :param noise: noies used to generate samples
    :return:
    """
    w = w / np.max(w) * 10
    decision_boundaries(stump, samples[0], samples[1], DEF_T, w)
    plt.suptitle("Size proprtional to weights with noise={}".format(noise))
    plt.savefig("q13_noise_{}.png".format(noise))
    plt.show()
    plt.close()


if __name__ == '__main__':
    for noise in [0, 0.01, 0.4]:
        samples = generate_data(SAMPLES_AMT, noise)
        stump = AdaBoost(DecisionStump, DEF_T)
        tests = generate_data(TEST_AMT, noise)
        w = stump.train(samples[0], samples[1])

        q10(samples, tests, noise)
        q11(tests, noise)
        q12(samples, noise, MIN_T[noise])
        q13(w, samples, noise)

