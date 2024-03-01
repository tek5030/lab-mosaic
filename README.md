# Image mosaics from feature matching

Welcome to this lab in the computer vision course [TEK5030] at the University of Oslo.

In this lab we will experiment with feature detection and matching to extract point correspondences between two images.
We will then use these correspondences to estimate a homography between the images, which will enable us to create a live video mosaic!

![Screenshot from the lab](lab-guide/img/screenshot_lab4.png)

**Start** by cloning this repository on your machine. 

Then, open the lab project in CLion using the cmake-file in the base directory:
`lab-mosaic/CMakeLists.txt`.
If you are uncertain about how this is done, please take a look at [the intro lab].

The lab is carried out by following these steps:

1. [Get an overview](lab-guide/1-get-an-overview.md)
2. [Features in OpenCV](lab-guide/2-features-in-opencv.md)
3. [Experiment with feature matching](lab-guide/3-experiment-with-feature-matching.md)
4. [Homography estimation](lab-guide/4-homography-estimation.md)
5. [Creating an image mosaic](lab-guide/5-creating-an-image-mosaic.md) 

You will find our proposed solution at https://github.com/tek5030/solution-mosaic. 
Please try to solve the lab with help from others instead of just jumping straight to the solution ;)

Start the lab by going to the [first step](lab-guide/1-get-an-overview.md).

## Prerequisites
- OpenCV must be installed on your system. If you are on a lab computer, you are all set.

  If you are on Ubuntu, but not on a lab computer, we refer to [setup_scripts](https://github.com/tek5030/setup_scripts) and [the intro lab] as a general getting started-guide for the C++ labs on Ubuntu 22.04.

  You will need to compile OpenCV from source to enable the `xfeatures`-module.

---

[TEK5030]: https://www.uio.no/studier/emner/matnat/its/TEK5030/
[the intro lab]: https://github.com/tek5030/lab-intro/blob/master/cpp/lab-guide/1-open-project-in-clion.md