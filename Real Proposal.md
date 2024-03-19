---
title: Camera Simulator in OpenGL
subtitle: COMP 371 Project Proposal
author: Andrae Lerone Lewis
abstract: This project aims to be a simple to use, real time digital/film camera simulation in OpenGL, implementing aperture (depth of field), shutter speed (motion blur), and ISO (light sensitivity, grain, and post processing). The user would be able to explore a 3D environment, compose their photo, adjust their camera settings and capture an image. The image would then be saved to disk.
bibliography: bib.bib
csl: https://www.zotero.org/styles/ieee
reference-section-title: \pagebreak References
table-of-contents: true

documentclass: scrartcl

header-includes:
- \usepackage{amsmath}

nocite: |
	@*
---

I am a photographer, I like taking pictures. Especially when I can take pretty pictures. I'm also a gamer. I'd like to transpose my photography skills into the digital realm. Hence, this project.

OpenGL's default camera has a few limitations for would-be virtual photographers. 
For one, everything in a rendered frame would be in focus.
<!-- Expand -->
Depth of field, defined as "the distance between the nearest and the furthest objects that are in acceptably sharp focus in an image captured with a camera" [@ContributorsWikimedia2023], allows for separation of subject and background, enabling clearer compositions (see @fig:lea). Notice also the pretty balls of light present in @fig:lea and @fig:rev. 
That is bokeh, an "artifact" of how lenses and focusing works that cannot be achieved by applying a simple blurring filter on the background layer.
For two, the images are perfectly still. There is no motion blur by default. Conveying a sense of motion in a single frame, as well as creative techniques such as light painting (see @fig:lightpainting) and intentional camera movement (ICM) (see @fig:icm), are thus impossible. 

\pagebreak

# Goals

The following is a list of key features I'm aiming to implement.

- Saving an image
- Depth of field
  - Bokeh
  - Focusing
- Motion Blur
  - Light Painting
  - ICM
- Variable Light Sensitivity (ISO)
- User Interface

While the goal of the project is to create a "realistic" camera simulator, features will be tweaked to favour aesthetics should the "realistic" option be visually unpleasant.


\pagebreak

# References

<div id="refs"></div>

\pagebreak

# Appendix: Figures

![Fireworks, an example of lightpainting. (Taken by the author)](http://beta.shadowlerone.ca/assets/full_sized/landscape/20230720__MG_0261.jpg){#fig:lightpainting}


<div id="fig:DepthOfField">

![Example of Bokeh](20230614__MG_0719.jpg){#fig:lea}

![Another example of Bokeh](20231220__Z9A0095.jpg){#fig:rev}

<!-- ![Example of creamy background](20221118__YUL1058.jpg){#fig:flowers} -->


Examples of DoF (Taken by the author)
</div>

<div id="fig:icm">

![](20230502-_MG_0264-1.jpg)

![](20240103-BZ9A1135-1.jpg)

Examples of long exposure photography. (Taken by the Author)

</div>