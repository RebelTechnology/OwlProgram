for nm in OwlPatches/*Patch.hpp
	do echo $nm|sed 's:OwlPatches/\(.*\)Patch.hpp:\1:g'
done
