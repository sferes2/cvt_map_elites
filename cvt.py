import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans

k = 7
DIM = 2
NumberOfSampedPoints = 50000

np.random.seed(0)
X = np.random.rand(NumberOfSampedPoints,DIM)

k_means = KMeans(init='k-means++', n_clusters=k, n_init=1, n_jobs=-1, verbose=1)
k_means.fit(X)
k_means_cluster_centers = k_means.cluster_centers_

filename = 'centroids_' + str(k) + '_' + str(DIM) + '.dat'

with open(filename, 'w') as f:
    for p in k_means_cluster_centers:
        for item in p:
            f.write(str(item) + ' ')
        f.write('\n')
