#!/bin/awk -f


/^make/{
if ($2 == "Entering") dir = $4;
}
/^gcc.*[[:blank:]]([^[:blank:]]+)$/{
	print dir, "//", $NF;
}
