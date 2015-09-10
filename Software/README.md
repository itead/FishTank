# FishTank Software

-	Author：huang xianming	
-	Time:2015-8-31 14:10:48

--------------------------------------------------------------------------------

# Brief Description

This program is used to control intelligent fish tank. Its structure is very 
simple. It is consisted of six sections, a main display section, and 5 functions
sections, including time setting(Time), pump (Pump), feed (Feed), light (Light) 
and ultraviolet (Uv). Because this project needs to realize these functions and
set a timer, a lot of nextion components has been used inside.But do not worry,
these components are set separately according to the function. I believe as long
as you understand one function implementation way, you will naturally understand
the others.In addition, the program increases the AT instruction set.You must pay
attention to the realization mechanism for error handling in the program.
	
# Folder Description

-	Libraries：Various software libraries required.(RTC Nextion DS18B20)
-	NextionEditorProject：Nextion project files
-	Source:Source code
	
# Details Description

-	Nextion use Serial port 3.
-	AT instruction use Serial port 2.
-	Detailed hardware interface(RBoard Pro) see below:

![](http://i.imgur.com/rsPXRgz.png)

# The End!