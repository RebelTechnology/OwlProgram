for nm in Libraries/OwlPatches/*Patch.hpp
	do echo $nm|sed 's:Libraries/OwlPatches/\(.*\)Patch.hpp:\1:g'
done
