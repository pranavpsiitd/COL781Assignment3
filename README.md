# COL 781 Assignment3
Based on the paper Botanical Tree Image Generation: http://ieeexplore.ieee.org/document/4055766/ 


A Visual Studio 2015 project that uses OpenGL to simulate procedurally generated Botanical trees modeled as L-system. 
Some example tree models are given in various .txt files in the project folder. Each tree is placed in X-Z plane and is defined by set of 16 parameters.
These 16 params are stored in the these files, seperated by space:-

1.) xPos: X-coordinate of tree base

2.) zPos: Z-coordinate of tree base

3.) h1: Branching angle for left branch

4.) h2: Branching angle for right branch (h2 = 0 for monopodial branching trees)

5.) category: int value from 0-6 defining the stochastic branching. 0 is default case (symmetric branching). 1-5 represent category of GMT4 tree (refer paper). 6 represents GMT3 (ternary branching).

6.) h1min

7.) h1max

8.) h2min

9.) h2max: For bounding the branching angles.

10.) R1: Contraction ratio of left branch

11.) R2: Contraction ratio of right branch

12.) divergence: angle defining the amount of rotation of branches from parent plane (refer paper). 0 means no divergence

13.) R:

14.) L: radius and length of trunk

15.) level: no. of levels of growth

16.) R0: Contraction ratio of radius.

### Running the code
1.) Press L to insert a tree. Enter the name of the file in terminal.

2.) Press A (and Shift+A) to change the angle.

3.) Press M (and Shift+M) for moving the model along Y direction.

4.) Press X, Y and Z for changing the wind velocity vector.

5.) Press SPACE for wind-animation

6.) Press n or N for insering attractors/inhibitors. Enter their info in terminal.

7.) Press c for choosing aparticular attractor/inhibitor. Press 1,2,3 and 4,5,6 for increasing/decreasing the x,y,z co-ordinates of the chosen control point.

8.) Press g for growth animation of currently loaded tree model.


### Tree model examples
1.) tree_mono.txt and tree_di.txt are basic monopodial and dichotomous trees. As these are very symmetric models, they might not seem very realistic. To improve realism, add gravity (or wind in -y direction) by pressing Y. Gravity improves realism in all other models as well.

2.) tree_mono_dev.txt and tree_di_dev.txt are same models but with non zero deviation. Clearly, realism is greatly improved.

3.) tree_GMT3.txt is ternary branching tree.

4.) tree_gmt4.txt has stochastic branching model. Change the category param for other types of branchings.
