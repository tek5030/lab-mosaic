# Step 3: Experiment with feature matching
First, lets play around with feature matching!

## 1. Experiment with blob and corner feature detectors
Lets first try to compile and run the code. 
You should see a video stream with detected keypoints.

What kind of detector are you currently using? 
Is this a blob- or corner detector? 
Try the other kind, and see what happens!

## 2. Implement the ratio test
In order to get robust matches, we will now implement the ratio test.

Remember from the lectures that we can compute the nearest neighbor distance ratio by computing

![\frac{d(f_a,f_b^1)}{d(f_a,f_b^2)}](img/math_ratio-test.png)

where *f*<sub>b</sub><sup>1</sup> is the best match, and *f*<sub>b</sub><sup>2</sup> is the second best match to feature *f*<sub>a</sub> in image *a*.

Take a look at the function declarations and documentation in [feature_utils.h](../feature_utils.h). 
What are the inputs and outputs in `extractGoodRatioMatches()`, and what are their data types?

Then go to the `extractGoodRatioMatches()` in [feature_utils.cpp](../feature_utils.cpp). 
Use the inputs `matches` and `max_ratio` to perform the ratio test, and return only the (best) matches that passes this test.

When your implementation seems reasonable, compile and run the program. 
Press <kbd>space</kbd> to set the current frame to the reference frame. 
You should now see results from matching the next current frames with the reference. 
Does your implementation still look reasonable?

## 3. Experiment with feature matching
Now that you have a working feature matching system, try different detectors, descriptors and parameters. 
Which methods are faster and which methods are more robust?

You might want to compile and run the program in *release mode* (see [lab-corners](https://github.com/tek5030/lab-corners/blob/master/lab-guide/3-detect-circles-from-corners-with-ransac.md#compile-and-run-with-build-type-release)).

In the [next part](4-homography-estimation.md), we will use this feature matching system to compute the transformation between the reference frame and the current frames.
