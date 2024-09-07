cppcheck --enable=all ^
		 --std=c++17 ^
		 --suppress=missingIncludeSystem ^
		 --suppress=unusedFunction ^
		 --suppress=unreadVariable ^
		 --suppress=passedByValue ^
		 --suppress=missingInclude ^
		 --suppress=checkersReport ^
		 --suppress=normalCheckLevelMaxBranches ^
		 .