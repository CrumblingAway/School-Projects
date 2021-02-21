import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.linalg import qr

mean = [0, 0, 0]
cov = np.eye(3)
x_y_z = np.random.multivariate_normal(mean, cov, 50000).T


def get_orthogonal_matrix(dim):
    H = np.random.randn(dim, dim)
    Q, R = qr(H)
    return Q


def plot_3d(x_y_z):
    '''
    plot points in 3D
    :param x_y_z: the points. numpy array with shape: 3 X num_samples (first dimension for x, y, z
    coordinate)
    '''
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(x_y_z[0], x_y_z[1], x_y_z[2], s=1, marker='.', depthshade=False)
    ax.set_xlim(-5, 5)
    ax.set_ylim(-5, 5)
    ax.set_zlim(-5, 5)
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('z')


def plot_2d(x_y):
    '''
    plot points in 2D
    :param x_y_z: the points. numpy array with shape: 2 X num_samples (first dimension for x, y
    coordinate)
    '''
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.scatter(x_y[0], x_y[1], s=1, marker='.')
    ax.set_xlim(-5, 5)
    ax.set_ylim(-5, 5)
    ax.set_xlabel('x')
    ax.set_ylabel('y')


def q11():
    plot_3d(x_y_z)
    plt.title("Question 11: identity matrix")
    plt.show()


def q12():
    m = np.array([[0.1, 0, 0], [0, 0.5, 0], [0, 0, 2]])
    x_y_z12 = m.dot(x_y_z)
    plot_3d(x_y_z12)
    plt.title("Question 12: scaling matrix")
    plt.show()
    return x_y_z12


def q13(x_y_z12):
    """
    :param x_y_z12: result from question 12 to multiply by orthogonal matrix
    :return:
    """
    o = get_orthogonal_matrix(3)
    x_y_z13 = o.dot(x_y_z12)
    plot_3d(x_y_z13)
    plt.title("Question 13: orthogonal matrix")
    plt.show()
    return x_y_z13


def q14(x_y_z13):
    """
    :param x_y_z13: result from question 13 to plot in 2d
    :return:
    """
    x_y14 = x_y_z13[0:2]
    plot_2d(x_y14)
    plt.title("Question 14: 2d projection of question 13")
    plt.show()


def q15(x_y_z13):
    """
    :param x_y_z13: result from question 13 to take points from
    :return:
    """
    x_coords = []
    y_coords = []
    for i in range(50000):
        if -0.4 < x_y_z13[2][i] < 0.1:
            x_coords.append(x_y_z13[0][i])
            y_coords.append(x_y_z13[1][i])
    x_y15 = np.array([x_coords, y_coords])
    plot_2d(x_y15)
    plt.title("Question 15: points where 0.1 > z > -0.4")
    plt.show()


if __name__ == '__main__':
    # Question answers in order
    q11()
    x_y_z12 = q12()
    x_y_z13 = q13(x_y_z12)
    q14(x_y_z13)
    q15(x_y_z13)

