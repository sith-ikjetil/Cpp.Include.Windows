@ECHO OFF
REM --suppress=passedByValue
cppcheck --enable=all ^
		 --std=c++17 ^
		 --suppress=missingIncludeSystem ^
		 --suppress=unusedFunction ^
		 --suppress=unreadVariable ^
		 --suppress=missingInclude ^
		 --suppress=checkersReport ^
		 --suppress=normalCheckLevelMaxBranches ^
		 .