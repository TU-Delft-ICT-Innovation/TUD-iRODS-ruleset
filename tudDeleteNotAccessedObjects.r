tudDeleteNotAccessedObjects {
    msiGetSystemTime(*Tim,	"human");	
    writeLine("stdout", "*Tim: Checking LAST_ACCESS_TIME attribuut")                                                                                                  
    *Q1 = select COLL_NAME, DATA_NAME, DATA_SIZE, META_DATA_ATTR_VALUE where META_DATA_ATTR_NAME = 'LAST_ACCESS_TIME';	
    msiGetSystemTime(*Time, "unix");

    foreach (*R1 in *Q1) {
        *elapsed = *Time - double(*R1.META_DATA_ATTR_VALUE);
        *objPath = *R1.COLL_NAME ++ "/" ++ *R1.DATA_NAME;

        if (*elapsed > 10000) {
	    writeLine("stdout", "De volgende is te oud, gooi weg! *objPath");
	    msiDataObjUnlink("objPath=*objPath++++forceFlag=", *Status);
	} else {
            writeLine("stdout", "De volgende is niet zo oud, bewaren! *objPath  *elapsed");
        }
    }	
}
INPUT null
OUTPUT ruleExecOut
