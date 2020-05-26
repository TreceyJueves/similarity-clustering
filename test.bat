@echo off
set no=1
g++ mychecker.cpp -o checker -Ofast -std=c++11
:L2
	echo Method%no%
	g++ similarity-clustering%no%.cpp -o cluster -Ofast -std=c++11
	set k=1
	:L1
		echo k=%k%
		echo k=%k% >> test%no%.out
		echo Running on 8_10
		cluster Out_OutGraph_Gop8_10.log Out_SliceSize_Gop8_10.log %k%
		echo Scoring 8_10
		echo Gop8_10 >> test%no%.out
		checker Out_OutGraph_Gop8_10.log Out_SliceSize_Gop8_10.log result.txt >>test%no%.out
		echo Running on 32_3
		cluster Out_OutGraph_Gop32_3.log Out_SliceSize_Gop32_3.log %k%
		echo Scoring 32_3
		echo Gop32_3 >> test%no%.out
		checker Out_OutGraph_Gop32_3.log Out_SliceSize_Gop32_3.log result.txt >>test%no%.out
		set /a k=k+1
		if %k% leq 5 goto :L1
	set /a no=no+1
	if %no% leq 10 goto :L2
pause