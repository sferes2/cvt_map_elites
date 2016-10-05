import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from scipy.spatial import Voronoi, voronoi_plot_2d
import sys
from matplotlib.ticker import FuncFormatter

cdict = {'red': [(0.0,  0.0, 0.0),
                 (0.33, 0.0, 0.0),
                 (0.66,  1.0, 1.0),
                 (1.0,  1.0, 1.0)],
         'blue': [(0.0,  0.0, 0.0),
                  (0.33, 1.0, 1.0),
                  (0.66,  0.0, 0.0),
                  (1.0,  0.0, 0.0)],
         'green': [(0.0,  0.0, 0.0),
                   (0.33, 0.0, 0.0),
                   (0.66,  0.0, 0.0),
                   (1.0,  1.0, 1.0)]}
my_cmap = matplotlib.colors.LinearSegmentedColormap('my_colormap', cdict, 256)

def scale(x, pos):
    'The two args are the value and tick position'
    return '%1.1f' % (x*10.0 - 5.0)
def scale2(x, pos):
    'The two args are the value and tick position'
    return '%1.1f' % (x*10.0 - 5.0)


def voronoi_finite_polygons_2d(vor, radius=None):
    """
    Reconstruct infinite voronoi regions in a 2D diagram to finite
    regions.

    Parameters
    ----------
    vor : Voronoi
        Input diagram
    radius : float, optional
        Distance to 'points at infinity'.

    Returns
    -------
    regions : list of tuples
        Indices of vertices in each revised Voronoi regions.
    vertices : list of tuples
        Coordinates for revised Voronoi vertices. Same as coordinates
        of input vertices, with 'points at infinity' appended to the
        end.

    """

    if vor.points.shape[1] != 2:
        raise ValueError("Requires 2D input")

    new_regions = []
    new_vertices = vor.vertices.tolist()

    center = vor.points.mean(axis=0)
    if radius is None:
        radius = vor.points.ptp().max()

    # Construct a map containing all ridges for a given point
    all_ridges = {}
    for (p1, p2), (v1, v2) in zip(vor.ridge_points, vor.ridge_vertices):
        all_ridges.setdefault(p1, []).append((p2, v1, v2))
        all_ridges.setdefault(p2, []).append((p1, v1, v2))

    # Reconstruct infinite regions
    for p1, region in enumerate(vor.point_region):
        vertices = vor.regions[region]

        if all(v >= 0 for v in vertices):
            # finite region
            new_regions.append(vertices)
            continue

        # reconstruct a non-finite region
        ridges = all_ridges[p1]
        new_region = [v for v in vertices if v >= 0]

        for p2, v1, v2 in ridges:
            if v2 < 0:
                v1, v2 = v2, v1
            if v1 >= 0:
                # finite ridge: already in the region
                continue

            # Compute the missing endpoint of an infinite ridge

            t = vor.points[p2] - vor.points[p1] # tangent
            t /= np.linalg.norm(t)
            n = np.array([-t[1], t[0]])  # normal

            midpoint = vor.points[[p1, p2]].mean(axis=0)
            direction = np.sign(np.dot(midpoint - center, n)) * n
            far_point = vor.vertices[v2] + direction * radius

            new_region.append(len(new_vertices))
            new_vertices.append(far_point.tolist())

        # sort region counterclockwise
        vs = np.asarray([new_vertices[v] for v in new_region])
        c = vs.mean(axis=0)
        angles = np.arctan2(vs[:,1] - c[1], vs[:,0] - c[0])
        new_region = np.array(new_region)[np.argsort(angles)]

        # finish
        new_regions.append(new_region.tolist())

    return new_regions, np.asarray(new_vertices)


points = np.loadtxt(sys.argv[1])
# points = np.loadtxt('generated_points_10000_2.dat')
rowsPoints, colsPoints = np.shape(points)

print 'Centroids: rows=',rowsPoints,' columns=',colsPoints

descriptorsFitnesses = np.loadtxt(sys.argv[2])[:,0:colsPoints+1]
descriptors = descriptorsFitnesses[:,0:2]
fitnesses = descriptorsFitnesses[:,2:3]
print np.shape(fitnesses)
rowsDescriptors, colsDescriptors = np.shape(descriptors)
print 'Loaded Descriptors: rows=',rowsDescriptors,' columns=',colsDescriptors

# compute Voronoi tesselation
vor = Voronoi(points)

# plot
regions, vertices = voronoi_finite_polygons_2d(vor)

cmap=my_cmap
norm = matplotlib.colors.Normalize(vmin=min(fitnesses), vmax=max(fitnesses))
normalizedFitnesses = (fitnesses - min(fitnesses)) / (max(fitnesses) - min(fitnesses))

fig = plt.figure()

ax = fig.add_subplot(111)

# colorize
i = 0
for region in regions:
    polygon = vertices[region]
    ax.fill(*zip(*polygon), color=cmap(norm(fitnesses[i]))[0])
    i = i+1

sc = plt.scatter(descriptors[:,0], descriptors[:,1], c=fitnesses, cmap=my_cmap,  s=60, zorder=0)
plt.colorbar(sc)
ax.set_ylim(0,1)
ax.set_xlim(0,1)

ax.yaxis.set_major_formatter(FuncFormatter(scale))
ax.xaxis.set_major_formatter(FuncFormatter(scale2))

plt.show()
