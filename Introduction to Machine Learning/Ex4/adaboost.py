"""
===================================================
     Introduction to Machine Learning (67577)
===================================================

Skeleton for the AdaBoost classifier.

Author: Gad Zalcberg
Date: February, 2019

"""
import numpy as np


class AdaBoost(object):

    def __init__(self, WL, T):
        """
        Parameters
        ----------
        WL : the class of the base weak learner
        T : the number of base learners to learn
        """
        self.WL = WL
        self.T = T
        self.h = [None]*T     # list of base learners
        self.w = np.zeros(T)  # weights

    def train(self, X, y):
        """
        Parameters
        ----------
        X : samples, shape=(num_samples, num_features)
        y : labels, shape=(num_samples)
        Train this classifier over the sample (X,y)
        After finish the training return the weights of the samples in the last iteration.
        """
        self.w = np.ones(shape=X.shape[0]) / X.shape[0]

        for i in range(self.T):

            # fit weak learner
            h_i = self.WL(self.w, X, y)

            # calculate error and stump weight from weak learner prediction
            h_pred = h_i.predict(X)
            err = self.w[(h_pred != y)].sum()
            h_weight = np.log((1 - err) / err) / 2

            # update sample weights
            if i < self.T - 1:
                self.w = self.w * np.exp(-h_weight * y * h_pred)
                self.w /= self.w.sum()

            # save results of iteration
            self.h[i] = h_i

        return self.w

    def predict(self, X, max_t):
        """
        Parameters
        ----------
        X : samples, shape=(num_samples, num_features)
        :param max_t: integer < self.T: the number of classifiers to use for the classification
        :return: y_hat : a prediction vector for X. shape=(num_samples)
        Predict only with max_t weak learners,
        """
        prediction = np.zeros(shape=X.shape[0])
        for i in range(max_t):
            prediction += self.h[i].predict(X)
        return np.sign(prediction)

    def error(self, X, y, max_t):
        """
        Parameters
        ----------
        X : samples, shape=(num_samples, num_features)
        y : labels, shape=(num_samples)
        :param max_t: integer < self.T: the number of classifiers to use for the classification
        :return: error : the ratio of the correct predictions when predict only with max_t weak learners (float)
        """
        return (self.predict(X, max_t) != y).sum() / float(X.shape[0])

