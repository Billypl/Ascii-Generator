# Ascii-Generator
Generates ascii version of an image.
For image handling using libraries: https://github.com/nothings/stb

Only thing to do is put the image in the same location as main.cpp file and you are ready to compile :D

**Functions included:**
- reduce() - reduces image to a given size. There are 3 ways to use this function:
	 1. *Pixels size:*
	    - Pass just new width SIZE, height will be deduced
	      -  i.e. image 1000x500 ==> reduce(100) ==> image 100x50 
	    - Pass both new width and height SIZE (full control over streching the image)
	 2. *Ratio size:*
	    - Pass new RATIO (applied for width AND height)
	    - Pass both new width and height RATIO 
	 
	 3.  *BONUS: Pixel + ratio of pixels*
	      - Pass one SIZE and one RATIO - ratio parameter will become the ratio of the SIZE argument
	        - i.e. image 1000x500 ==> reduce(100, 0.5) ==> image 100x25  
- print_ascii() - There are 2 ways to use it:
  - print_ascii() - prints the output to the console
  - print_ascii(\*.txt) - prints the output to the text file 
