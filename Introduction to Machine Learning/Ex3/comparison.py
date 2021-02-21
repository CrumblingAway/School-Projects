from models import *
import matplotlib.pyplot as plt


ITER_AMT = 500
DEF_K = 10000
m_arr = [5, 10, 15, 25, 70]


def draw_points(m: int) -> (np.array, np.array):
    """
    Given an integer m returns a pair X,y where X is a 2xm matrix where
    each column represents an i.i.d sample from the distribution ~N(0, I2),
    and y is its corresponding label according to f(x) = sign(<[0.3, -0.5], x>+0.1)
    :param m:
    :return:
    """
    X = np.random.multivariate_normal([0, 0], np.eye(2), m).T
    y = np.sign(np.insert(X, 0, 1, axis=0).T @ np.array([0.1, 0.3, -0.5]))
    return X,y


def get_df1_df2(X: np.array, y: np.array) -> [DataFrame, DataFrame]:
    """
    Get DataFrames for points with labels 1 and -1
    :param X:
    :param y:
    :return:
    """
    x1 = np.array([X[:, i] for i in range(y.shape[0]) if y[i] == 1]).T
    x2 = np.array([X[:, i] for i in range(y.shape[0]) if y[i] == -1]).T
    df1 = DataFrame({'x': list(), 'y': list()})
    df2 = DataFrame({'x': list(), 'y': list()})
    if len(x1 > 0):
        df1 = DataFrame({'x': x1[0], 'y': x1[1]})
    if len(x2 > 0):
        df2 = DataFrame({'x': x2[0], 'y': x2[1]})
    return [df1, df2]


def get_real_df(X: np.array, y: np.array, xx: np.array) -> DataFrame:
    """
    Get DataFrame representing original hyperspace
    :param X:
    :param y:
    :return:
    """
    r_yy = 0.6 * xx + 0.2
    return DataFrame({'x': xx, 'y': r_yy})


def get_perceptron_df(X: np.array, y: np.array, xx: np.array) -> DataFrame:
    """
    Get DataFrame representing perceptron hyperspace
    :param X:
    :param y:
    :param xx:
    :return:
    """
    perc = Perceptron()
    perc.fit(X, y)
    w = perc.get_trained_model()
    slope = -w[1] / w[2]
    p_yy = slope * xx - w[0] / w[2]
    return DataFrame({'x': xx, 'y': p_yy})


def get_svm_df(X: np.array, y: np.array, xx: np.array) -> DataFrame:
    """
    Get DataFrame representing SVM hyperspace
    :param X:
    :param y:
    :param xx:
    :return:
    """
    svm = SVM(1e10)
    svm.fit(X, y)
    new_svm = svm.get_svm()
    w_s = new_svm.coef_[0]
    a = -w_s[0] / w_s[1]
    s_yy = a * xx - (new_svm.intercept_[0]) / w_s[1]
    return DataFrame({'x': xx, 'y': s_yy})


def q9():
    """
    Plot relevant plots for every m
    :return:
    """
    for m in m_arr:
        X, y = draw_points(m)
        xx = np.linspace(np.min(X), np.max(X))

        # Create data sets and calculate hyperplanes
        df_arr = get_df1_df2(X, y)
        df_arr.append(get_real_df(X, y, xx))
        df_arr.append(get_perceptron_df(X, y, xx))
        df_arr.append(get_svm_df(X, y, xx))

        colors = ["blue", "orange", "black", "yellow", "green"]
        labels = ["label 1", "label -1", "true sep.", "perceptron sep.", "SVM sep."]
        for i in range(2):
            plt.scatter(df_arr[i]['x'], df_arr[i]['y'], c=colors[i], label=labels[i])
        for i in range(2, len(df_arr)):
            plt.plot(df_arr[i]['x'], df_arr[i]['y'], c=colors[i], label=labels[i])
        plt.legend()
        plt.title("Hyperplanes for m={}".format(m))
        plt.show()


def draw_grouped_bar_graph(m_accuracies: np.array, colors: list, names: list, width: float, title: str) -> None:
    """
    Draw grouped bar graph based on m_accuracies
    :param m_accuracies:
    :return:
    """
    pos = list(range(m_accuracies.shape[1]))
    for i in range(m_accuracies.shape[0]):
        plt.bar([p + width * i for p in pos], m_accuracies[i], width=width, color=colors[i], edgecolor="white",
                label="{}".format(names[i]))
    plt.xlabel("m")
    plt.ylabel("accuracy")
    plt.xticks([r + width for r in range(m_accuracies.shape[1])], ["{}".format(m) for m in m_arr])
    plt.legend()
    plt.title(title)
    plt.show()


def q10():
    """
    Plot the accuracies of Perceptron, SVM and LDA for each m
    :return:
    """
    m_accuracies = list() # Store all accuracies for every m
    for m in m_arr:
        cur_accuracies = [0, 0, 0]
        for i in range(ITER_AMT):
            # Draw points until there dots with both labels 1 and -1
            X_train, y_train = draw_points(m)
            while not (np.isin(-1, y_train) and np.isin(1, y_train)):
                X_train, y_train = draw_points(m)

            # Draw DEF_K points and train models
            X_test, y_test = draw_points(DEF_K)
            alg_arr = [Perceptron(), SVM(1e10), LDA()]
            for i in range(len(alg_arr)):
                alg_arr[i].fit(X_train, y_train)
                cur_accuracies[i] += alg_arr[i].score(X_test, y_test)['accuracy']
        cur_accuracies = np.array(cur_accuracies) / float(ITER_AMT)
        m_accuracies.append(cur_accuracies[:])

    draw_grouped_bar_graph(np.array(m_accuracies).T, colors=['red', 'green', 'blue'],
                           names=['Perceptron', 'Hard-SVM', 'LDA'], width=0.25,
                           title="Accuracies of Perceptron, SVM and LDA as function of m")



if __name__ == '__main__':
    q9()
    q10()