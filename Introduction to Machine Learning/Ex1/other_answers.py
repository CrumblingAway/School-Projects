import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.linalg import qr
import math

DEF_SAMPLE_SIZE = 100000
DEF_TOSS_SIZE = 1000

COLORS = ['red', 'yellow', 'blue', 'green', 'black']
data = np.random.binomial(1, 0.25, (DEF_SAMPLE_SIZE, DEF_TOSS_SIZE))
epsilon = [0.5, 0.25, 0.1, 0.01, 0.001]


def plot_mean_by_m():
    """
    Does as follows:
    1) Takes first five rows, representing first five sequences of 1000 coin tosses
    2) For each row calculates mean as a function of m tosses
    3) Plots all values on the same graph with a different color for each sequence
    :return:
    """
    temp = data[:5] # Extract first five sequences
    m_values = [m+1 for m in range(DEF_TOSS_SIZE)]
    for i in range(len(temp)):
        y_row = []
        cur_sum = 0 # Sum of first m tosses
        for j in range(DEF_TOSS_SIZE):
            cur_sum += temp[i][j]
            y_row.append(cur_sum / float(j+1)) # Add mean
        plt.plot(m_values, y_row, color = COLORS[i], label = "Row {}".format(i))
    plt.title("Mean estimate")
    plt.legend()
    plt.show()


def plot_chebyshev_hoeffding_percentage_by_m():
    """
    Plots Chebyshev and Hoeffding upper bounds
    :return:
    """
    m_values = np.arange(1, DEF_TOSS_SIZE+1)

    for eps in epsilon:
        data_cumulative = np.array([np.absolute(np.cumsum(data[i])/np.arange(1, DEF_TOSS_SIZE+1)-0.25)>=eps for i in range(len(data))])
        # data_cumulative = np.absolute(np.divide(np.cumsum(data, axis=1), np.tile(np.arange(1,DEF_TOSS_SIZE+1), (DEF_SAMPLE_SIZE,1)))-0.25)

        y_row1 = []
        y_row2 = []
        y_row3 = []
        for j in range(1, DEF_TOSS_SIZE+1):
            y_row1.append(1/(4*j*(eps**2))) # Chebyshev
            y_row2.append(2*(math.e**(-2*j*(eps**2)))) # Hoeffding

            # Percentage
            amount = np.where(data_cumulative[:,j-1]==True)[0].size
            # amount = np.where(data_cumulative[:,j-1]>=eps)[0].size
            y_row3.append(amount/float(DEF_SAMPLE_SIZE))


        # Set up plot
        plt.title("epsilon = " + str(eps))
        plt.plot(m_values, y_row1, color=COLORS[0], label="Chebyshev")
        plt.plot(m_values, y_row2, color=COLORS[2], label="Hoeffding")
        plt.plot(m_values, y_row3, color=COLORS[1], label="Percentage")
        plt.legend()
        plt.ylim(0, 2)
        plt.show()


if __name__ == '__main__':
    # a
    plot_mean_by_m()

    # b and c
    plot_chebyshev_hoeffding_percentage_by_m()



