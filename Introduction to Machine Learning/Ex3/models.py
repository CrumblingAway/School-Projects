import numpy as np
from sklearn.svm import SVC
from sklearn.linear_model import LogisticRegression
from sklearn.tree import DecisionTreeClassifier
from sklearn.neighbors import KNeighborsClassifier
import pandas as pd
from pandas import DataFrame


class Classifier:
    """
    Parent class for the five different classifiers: Half-space, LDA, SVM, Logistic Regression and Decision Tree
    """

    def fit(self, X: np.array, y: np.array) -> None:
        """
        Given training set X and labels y this method learns the parameters of the model
        and stores the trained model
        :param X: training set dxm
        :param y: labels mx1
        :return: None
        """
        pass

    def predict(self, X: np.array) -> np.array:
        """
        Given an unlabeled test set X predicts the label of each sample
        :param X: test set dxm'
        :return: vector of predicted labels m'x1
        """
        pass

    def score(self, X: np.array, y: np.array) -> dict:
        """
        Given an unlabeled test set X and true labels y of the test set returns a dictionary
        with the following fields:
        - num.samples: number of samples in the test set
        - error: error (misclassification) rate
        - accuracy: accuracy
        - FPR: false positive rate
        - TPR: true positive rate
        - precision: precision
        - recall: recall
        :param X: test set dxm'
        :param y: true labels m'x1
        :return: dictionary
        """
        result = dict()
        prediction = self.predict(X)
        result['num_samples'] = X.shape[1]
        result['accuracy'] = np.sum(prediction == y) / float(prediction.shape[0])
        result['error'] = 1 - result['accuracy']

        # Calculate FP, NP, P and N for FPR and TPR
        neg_y = (y == -1)
        pos_y = (y == 1)
        fp = np.count_nonzero(prediction == neg_y)
        p = np.count_nonzero(y == 1)
        n = np.count_nonzero(neg_y)
        tp = np.count_nonzero(pos_y == prediction)
        if n:
            result['FPR'] = fp / float(n)
        else:
            result['FPR'] = 1.0
        if p:
            result['TPR'] = tp / float(p)
        else:
            result['TPR'] = 1.0
        result['recall'] = result['TPR']

        return result


class Perceptron(Classifier):
    """
    Implementation of Half-space classifier
    """

    def __init__(self):
        self.__trained_model = None


    def get_trained_model(self):
        return self.__trained_model


    def is_feasible(self, X: np.array, y: np.array, w: np.array) -> (bool, int):
        """
        Checks if given vector w represents a feasible solution
        :param X: training set dxm
        :param y: labels mx1
        :param w: weight vector dx1
        :return: (true, -1)  if feasible, otherwise (false, index of sample)
        """
        for i in range(X.shape[0]):
            if y[i] * (w.dot(X[i])) <= 0:
                return False, i
        return True, -1

    def fit(self, X: np.array, y: np.array) -> None:
        X = np.insert(X, 0, 1, axis=0)  # Add row of 1's for bias
        self.__trained_model = np.zeros((X.shape[0],))
        flag, idx = self.is_feasible(X.T, y, self.__trained_model)
        while not flag:
            self.__trained_model += y[idx] * X.T[idx]
            flag, idx = self.is_feasible(X.T, y, self.__trained_model)

    def predict(self, X: np.array) -> np.array:
        return np.sign(np.insert(X, 0, 1, axis=0).T @ self.__trained_model)


class LDA(Classifier):
    """
    Implementation of LDA classifier
    """
    def __init__(self):
        self.__myu_1 = None
        self.__myu_2 = None
        self.__sigma = None
        self.__sigma_myu_1 = None
        self.__sigma_myu_2 = None
        self.__p_y1 = None
        self.__p_y2 = None
        self.__delta1 = None
        self.__delta2 = None

    def fit(self, X: np.array, y: np.array) -> None:
        # Calculate mean vector for 1 and -1 (represented by myu_1 and myu_2 respectively)
        X = X.T
        self.__myu_1 = np.mean(X[y == 1], axis=0)
        self.__myu_2 = np.mean(X[y == -1], axis=0)

        # Calculate covariance matrix
        self.__sigma = np.linalg.inv(np.cov(X.T))
        self.__sigma_myu_1 = self.__sigma @ self.__myu_1
        self.__sigma_myu_2 = self.__sigma @ self.__myu_2

        # Calculate probabilities of y=1 and y=-1
        self.__p_y1 = np.count_nonzero(y == 1) / float(X.shape[0])
        self.__p_y2 = 1 - self.__p_y1

        # Calculate part of delta not affected by x
        self.__delta1 = 0.5 * self.__myu_1.T @ self.__sigma @ self.__myu_1 + np.log(self.__p_y1)
        self.__delta2 = 0.5 * self.__myu_2.T @ self.__sigma @ self.__myu_2 + np.log(self.__p_y2)

    def predict(self, X: np.array) -> np.array:
        val1 = X.T @ self.__sigma_myu_1 - self.__delta1
        val2 = X.T @ self.__sigma_myu_2 - self.__delta2
        return np.sign(val1 - val2)

class SVM(Classifier):
    """
    Implementation of SVM classifier
    """
    def __init__(self, factor):
        self.__svm = SVC(C=factor, kernel='linear')

    def get_svm(self):
        return self.__svm

    def fit(self, X: np.array, y: np.array) -> None:
        self.__svm.fit(X.T,y)

    def predict(self, X: np.array) -> np.array:
        return self.__svm.predict(X.T)


class Logistic(Classifier):
    """
    Implementation of Logistic Regression classifier
    """
    def __init__(self):
        self.__logistic = LogisticRegression(solver='liblinear')

    def fit(self, X: np.array, y: np.array) -> None:
        self.__logistic.fit(X.T,y)

    def predict(self, X: np.array) -> np.array:
        return self.__logistic.predict(X.T)


class DecisionTree(Classifier):
    """
    Implementation of Decision Tree classifier
    """
    def __init__(self, depth):
        self.__tree = DecisionTreeClassifier(max_depth=depth)

    def fit(self, X: np.array, y: np.array) -> None:
        self.__tree.fit(X.T, y)

    def predict(self, X: np.array) -> np.array:
        return self.__tree.predict(X.T)


class KNN(Classifier):
    """
    Implementation of k-nearest neighbors classifier
    """
    def __init__(self, k):
        self.__knn = KNeighborsClassifier(n_neighbors=k)

    def fit(self, X: np.array, y: np.array) -> None:
        self.__knn.fit(X.T, y)

    def predict(self, X: np.array) -> np.array:
        return self.__knn.predict(X.T)