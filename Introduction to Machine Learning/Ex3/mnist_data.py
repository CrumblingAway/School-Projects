import tensorflow as tf
import matplotlib.pyplot as plt
from models import *
from comparison import draw_grouped_bar_graph
import time

IMG_AMT = 3
m_arr = [50, 100, 300, 500]
ITER_AMT = 50


def q12(x_arr, y_arr) -> None:
    """
    Uses pyplot to show images of numbers with labels 0 and 1
    :param x_arr:
    :param y_arr:
    :return:
    """
    zero_indices = np.where(y_arr == 0)
    one_indices = np.where(y_arr == 1)
    if len(zero_indices[0]) >= IMG_AMT:
        for i in range(IMG_AMT):
            plt.imshow(x_arr[zero_indices[0][i]])
            plt.show()
    if len(one_indices[0]) >= IMG_AMT:
        for i in range(IMG_AMT):
            plt.imshow(x_arr[one_indices[0][i]])
            plt.show()


def rearrange_data(X: np.array) -> np.array:
    """
    Rearranges tensor of size mx28x28 into mx784
    :param X:
    :return:
    """
    return X.reshape(X.shape[0], X.shape[1]*X.shape[2])


def q14(x_train: np.array, y_train: np.array, x_test: np.array, y_test: np.array):
    """
    Plot accuracies of Logistic, Soft-SVM, DecisionTree and KNN
    :return:
    """
    m_accuracies = list()  # Store all accuracies for every m
    elapsed_running_time = list()
    for m in m_arr:
        cur_accuracies = [0, 0, 0, 0]
        cur_running_time = [0, 0, 0, 0]
        for i in range(ITER_AMT):
            # Sample m random indices until both 0 and 1 are present
            rand_int = np.random.randint(low=0, high=x_train.shape[1], size=m)
            cur_x_train = x_train[:, rand_int]
            cur_y_train = y_train[rand_int]
            while not (np.isin(0, y_train) and np.isin(1, y_train)):
                rand_int = np.random.random_integers(x_train.shape[1], size=m)
                cur_x_train = x_train[:, rand_int]
                cur_y_train = y_train[rand_int]

            # Substitute 0s for -1s
            cur_y_train = cur_y_train.astype(int)
            cur_y_train[cur_y_train == 0] = -1
            y_test = y_test.astype(int)
            y_test[y_test == 0] = -1

            alg_arr = [Logistic(), SVM(1e3), DecisionTree(25), KNN(10)]
            for i in range(len(alg_arr)):
                start = time.time()
                alg_arr[i].fit(cur_x_train, cur_y_train)
                cur_accuracies[i] += alg_arr[i].score(x_test, y_test)['accuracy']
                end = time.time()
                cur_running_time[i] += end - start
        cur_accuracies = np.array(cur_accuracies) / float(ITER_AMT)
        m_accuracies.append(cur_accuracies[:])
        elapsed_running_time.append(cur_running_time)

    draw_grouped_bar_graph(np.array(m_accuracies).T, names=['Logistic', 'Soft-SVM', 'Tree', 'KNN'],
                           colors=['red', 'green', 'blue', 'yellow'], width=0.15,
                           title="Accuracies of Logistic, Soft-SVM, Tree and KNN as function of m")


if __name__ == '__main__':
    mnist = tf.keras.datasets.mnist
    (x_train, y_train), (x_test, y_test) = mnist.load_data()
    train_images = np.logical_or((y_train == 0), (y_train == 1))
    test_images = np.logical_or((y_test == 0), (y_test == 1))
    x_train, y_train = x_train[train_images], y_train[train_images]
    x_test, y_test = x_test[test_images], y_test[test_images]

    q12(x_train, y_train)
    q14(rearrange_data(x_train).T, y_train.T, rearrange_data(x_test).T, y_test.T)

