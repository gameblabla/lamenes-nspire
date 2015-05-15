=============
Lamenes-nspire
=============

LameNES for TI nspire (CX).
It's pretty slow on TI nspire unfortunely and it comes with a simple file browser.

To compile it for TI nspire, make sure you have compiled and installed the Ndless-SDK.
Export Ndless's path and then compile it with : make -f Makefile.nspire

A tns file will then be generated.

By default, this uses n2DLib for graphics on TI nspire.
If you want to exclusively use SDL for graphics, remove -Dndlib from the Makefile.

LameNES is licensed under the a 3-clause BSD license, copyright Joey Loman.
Full license can be found in LICENSE.txt or in README_original.txt.