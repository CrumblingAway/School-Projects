import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from numpy.linalg import *


def fit_linear_regression(X: np.array, y: np.array) -> (np.array, np.array):
    """
    Calculates coefficients vector w given design matrix X (data) and response vector y (labels)
    :param X: p*n matrix
    :param y: n*1 vector
    :return: tuple of coefficients vector w (p*1) and singular values of X
    """
    w = np.linalg.pinv(X).T @ y
    s = svd(X, compute_uv=False)
    return w,s


def q20(df: pd.DataFrame):
    """
    Plots the graphs for question 20
    :param df:
    :return:
    """
    x_values = df['day_num']
    y_values1 = df['log_detected']
    y_values2 = df['detected']

    # Log graph
    plt.scatter(x_values, y_values1)
    plt.plot(x_values, X.dot(w))
    plt.xlabel('day_num')
    plt.ylabel('log_detected')
    plt.title('log_detected as function of day')
    plt.show()

    # Exponential grpah
    plt.scatter(x_values, y_values2)
    plt.plot(x_values, np.exp(X.dot(w)))
    plt.xlabel('day_num')
    plt.ylabel('detected')
    plt.title('detected as function of day')
    plt.show()


if __name__ == '__main__':
    # Q18
    df = pd.read_csv("C:\\Users\\anton\\PycharmProjects\\EX2\\covid19_israel.csv")

    # Q19
    df['log_detected'] = np.log(df['detected'])

    # Q20
    X = pd.DataFrame(df['day_num'].values.T)
    X['bias'] = 1
    y = df['log_detected']
    w, s = fit_linear_regression(X.T, y)

    # Q21
    q20(df)

