# Step 5: Creating an image mosaic
We will in this last part use the computed homography to combine the images in a common pixel coordinate system (a common image).

## 6. What does the similarity *S* do?
In `lab4()` at [lab_4.cpp:57](https://github.com/tek5030/lab_04/blob/master/lab_4.cpp#L57), we have defined the similarity *S* as:

```c++
cv::Matx33f S_cv{
    0.5f, 0.0f, 0.25f * frame_cols,
    0.0f, 0.5f, 0.25f * frame_rows,
    0.0f, 0.0f, 1.0f};
```

- What is the difference between `cv::Matx33f` and `cv::Mat`?
- What does this transformation do?

This similarity will define the transformation from the reference image to the *mosaic image*.

## 7. Transform the reference image and insert it into the mosaic
Use the similarity *S* above to warp the reference image into the mosaic image:

```c++
cv::Mat mosaic;
cv::warpPerspective(ref_image, mosaic, /* ? */, ref_image.size());
```

## 8. Transform the current frame
Use the similarity *S* and the computed homography *H* to warp the current frame into mosaic image coordinates:

```c++
cv::Mat frame_warp;
cv::warpPerspective(frame, frame_warp, /* ? */, frame.size());
```

What happens if you try to insert this image into the mosaic, rather than `frame_warp`?


## 9. Compute a mask for the transformed current frame
We will use a mask to define which pixels in the mosaic that should be updated with the current frame. 
We will do this by taking an image with all ones (all the pixels in the original current frame), and warping it in the exact same way as we did with the current frame. 
This should give us a warped mask that defines where in the mosaic image we want to insert the warped current frame:

```c++
cv::Mat mask = cv::Mat::ones(frame.size(), CV_8UC1);
cv::Mat mask_warp;
cv::warpPerspective(mask, mask_warp, /* ? */, mask.size());
```

## 10. Insert the current frame and remove the edge effects
We can use `cv::Mat::copyTo()` to insert the warped current frame into the mosaic:

```c++
frame_warp.copyTo(mosaic, mask_warp);
```

Do this, compile and run.
Cool, right?

Notice that there are some unwanted effects around the edges of the current frame. 
This is because of interpolations when warping the mask. 
How can we remove these effects with `cv::erode`? Try!


## Then...
Now you are finished with lab 4! 
But if you still have some time left, or want to continue with this lab at home, there is other cool stuff you can try:
- Try `cv::findHomography()` instead of our method.
- Use the estimated homography to search for more, weaker correspondences. Then recompute!
- Use the keypoint detector you implemented in lab 3.
- Apply blending to the mosaic, like we did in lab 2.
- Expand the program to make a mosaic of more than two images.
