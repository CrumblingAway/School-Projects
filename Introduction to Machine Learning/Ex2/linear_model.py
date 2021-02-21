import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import sklearn.model_selection as sk
from numpy.linalg import *
from sklearn import *


### Q9 ###


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

### Q10 ###


def predict(X: np.array, w: np.array) -> np.array:
    """
    Uses design matrix X and coefficients vector w to make a prediction
    :param X: p*m matrix
    :param w: p*1 vector
    :return: vector with predicted values
    """
    return np.transpose(X).dot(w)

### Q11 ###


def mse(v: np.array, y: np.array) -> float:
    """
    Calculates MSE of predicted values v against labels in y
    :param v: m*1 vector
    :param y: m*1 vector
    :return: MSE of v
    """
    return metrics.mean_squared_error(v, y)

### Q12 + Q13 ###


def load_data(file_path: str):
    """
    Reads data from file_path (csv file) into a numpy array
    :param file_path: csv file location
    :return: matrix of data in csv file after preprocessing + price vector
    """
    df = pd.read_csv(file_path)

    # Drop columns that don't contribute to learning (explanation in PDF)
    df = df.drop(columns=["id"])
    df = df.drop(columns=["lat"])
    df = df.drop(columns=["long"])
    # df = df.drop(columns=["date"])
    # df = df.drop(columns=["yr_built"])

    # Translate date
    df['date'] = pd.to_datetime(df['date'], errors='coerce').dt.strftime("%y%m%d")
    df = df.loc[(df['price'] >= 0) & (df['bedrooms'] >= 1) & (df['bathrooms'] >= 0.75) & (df['sqft_living'] > 0) &
                (df['sqft_lot'] >= 0) & (df['floors'] >= 1) & (df['sqft_above'] >= 0) & (df['sqft_basement'] >= 0)]

    # Convert zipcode and date to dummy values
    df['zipcode'] = df['zipcode'].astype('category')
    df['zipcode'] = df['zipcode'].cat.codes
    df['date'] = df['date'].astype('category')
    df['date'] = df['date'].cat.codes
    y = df['price']
    df = df.drop(columns=["price"])
    return df, y


### Q14 ###


def plot_singular_values(s_values: np.array):
    """
    PLots singular values s_values in descending order (x axis running index, y axis s_values)
    :return:
    """
    s_values = np.flip(np.sort(s_values))
    indices = np.arange(s_values.size) + 1
    plt.scatter(indices, s_values)
    plt.title("Scree-plot")
    plt.xlabel("Index of singular value")
    plt.ylabel("Singular value")
    plt.yscale("log")
    plt.xticks(indices)
    plt.show()


### Q15 ###


def putting_it_all_together_1(file_path: str):
    """
    Reads and preprocesses data from file_path and plots singular values
    :param file_path:
    :return:
    """
    df, y = load_data(file_path)
    s = svd(df, compute_uv=False)
    plot_singular_values(s)


### Q16 ###


def putting_it_all_together_2(file_path: str):
    """
    Splits data from file into train and test sets and calculates the mse
    as a function of how many samples (in percents) were used to learn
    :param file_path:
    :return:
    """
    df, y = load_data(file_path)
    df['bias'] = 1
    df_train, df_test, y_train, y_test = sk.train_test_split(df, y)

    mse_arr = list()
    for p in range(1, 101):
        seg = int((p/100.0)*len(df_train))
        p_df = df_train[:seg]
        p_y = y_train[:seg]
        w, s = fit_linear_regression(np.transpose(p_df), p_y)
        val = predict(np.transpose(df_test), w)
        mse_arr.append(mse(y_test, val))
    indices = np.arange(100) + 1
    plt.scatter(indices, mse_arr)
    plt.title("Prediction accuracy by percentage of sample")
    plt.xlabel("Percent (whole)")
    plt.ylabel("MSE")
    plt.yscale("log")
    plt.show()


### Q17 ###


def feature_evaluation(df: pd.DataFrame, y: np.array):
    """
    Given a design matrix + response vector, plots the pearson correlation between
    non-categorical features and their response
    :param df:
    :param y:
    :return:
    """
    sig2 = np.std(y)
    for col in df:
        x_values = df[col]
        sig1 = np.std(x_values)
        plt.scatter(x_values, y)
        coef = np.cov(x_values, y)[0][1]/(sig1*sig2)
        plt.scatter([], [], label='Pearson Coefficient = {}'.format(coef))
        plt.title('evaluation of {}'.format(col))
        plt.xlabel(col)
        plt.ylabel('price')
        plt.legend()
        plt.show()


if __name__ == '__main__':
    # # Instagram question from recitation
    # X = np.array([[1, 1, 1, 1], [12.3, 57.8, 28.7, 4.2], [7.5, 1.12, 0.8, 14.25]])
    # y = np.transpose(np.array([46, 4, 11, 75]))
    # data = np.array([1, 24, 2.5])
    # w, s = fit_linear_regression(X, y)
    # v = predict(X, w)
    # print("w: ", w)
    # print("singular values: ", s)
    # print("prediction: ", data.dot(w))
    # print("predict(): ", v)
    # print("mse(): ", mse(v, y))
    # plot_singular_values(s)

    # file_path = "C:\\Users\\anton\\PycharmProjects\\EX2\\kc_house_data.csv"
    # x, y = load_data(file_path)
    # print(x)
    # print(y)
    # a = 3
    # b = 4
    # print(x.zipcode)
    # putting_it_all_together_2("C:\\Users\\anton\\PycharmProjects\\EX2\\kc_house_data.csv")
    df, y = load_data("C:\\Users\\anton\\PycharmProjects\\EX2\\kc_house_data.csv")
    putting_it_all_together_2("C:\\Users\\anton\\PycharmProjects\\EX2\\kc_house_data.csv")
    feature_evaluation(df, y)
    # putting_it_all_together_1("C:\\Users\\anton\\PycharmProjects\\EX2\\kc_house_data.csv")
