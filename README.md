# Machine Generated Logical/Linear Line Drawings

Automatic line drawing extraction from scenes/images using logical/linear operators framework.

This is part of my Ph.D. work and the codes would be soon uploaded to this git repository (please either check the page again in a couple of weeks or write me an email at: morteza@cim.mcgill.ca). 

Currently, the repository contains a folder named `LogicalLinear` represent a complete implementation of the Logical/Linear
image operators defined in the paper "Logical/Linear Operators for Image Curves" by **Lee Iverson and Steven W. Zucker** to be published in PAMI (also available as ftp://ftp.cim.mcgill.ca/pub/people/leei/loglin/loglin.ps.gz).  The implementation is both fast and general, written in portable C.

The original version of this program works with just `pgm` (portable gray map) input files (which is available here: ftp://ftp.cim.mcgill.ca/pub/people/leei/loglin.tar.gz).  The current version uploaded here (which is a fork on the original version), works with most of the standard image formats (`jpg`, `png` and etc.). When compiled, the program pgmloglin will input files and produce a machine readable PostScript file describing the oriented features in the image.  The program will selectively compute any or all of the edge, bright line and dark line labels in the image.


