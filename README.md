# Centroidal Voronoi Tessellation MAP-Elites

CVT-MAP-Elites module for Sferes2.

## Python version
This is the C++ version. It will be merged soon in sferes2. A python version of CVT-MAP-Elites is available here: https://gitlab.inria.fr/resibots/public/py_map-elites 


## Authors:
- Vassilis Vassiliades (vassilis.vassiliades@inria.fr)

## Description

Centroidal Voronoi Tessellation MAP-Elites is a natural extension of MAP-Elites to handle high dimensional behavior spaces.
The number of niches in MAP-Elites is dependent on the dimensionality of the problem and the discretizations used.
In CVT-MAP-Elites, the number of niches is set before-hand. The location of these niches is determined in a way that the behavior space is partitioned into equisized regions. This is done using a centroidal Voronoi tessellation (https://en.wikipedia.org/wiki/Centroidal_Voronoi_tessellation). New points fall into the region of the closest centroid.

## Example on Rastrigin function:

- MAP-Elites

![MAP-Elites](figures/map_elites.png)

- CVT-MAP-Elites

![CVT-MAP-Elites](figures/cvt_map_elites.png)
