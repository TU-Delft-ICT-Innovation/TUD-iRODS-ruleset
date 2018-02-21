

DATA_SIZE_THRESHOLD = 40
FAST_REPLICATION_RESOURCE = "replResc1"
SLOW_REPLICATION_RESOURCE = "replResc2"

acPostProcForPut{
    tudReplicate();
}

acPreprocForDataObjOpen {
    # only for read operations set the last access time metadata
    if ($writeFlag == "0") {
        writeLine("serverLog", "acPreprocForDataObjOpen writeFlag: $writeFlag ")
        setLastAccessTime();
    }
}

setLastAccessTime() {
    msiGetSystemTime(*Time, "unix");
    createAVU("LAST_ACCESS_TIME", *Time, $objPath);
}

# Low-level helper function
createAVU(*key, *value, *path){
    #Creates a key-value pair and connects it to a data object or collection
    msiAddKeyVal(*Keyval,*key, *value);
    writeKeyValPairs("serverLog", *Keyval, " is : ");
    msiGetObjType(*path,*objType);
    msiSetKeyValuePairsToObj(*Keyval, *path, *objType);
}



tudReplicate{
    *home="/$rodsZoneClient/home/$userNameClient"
    writeLine("serverLog", "Starting tudReplicate on object: $objPath");
    *Resource=FAST_REPLICATION_RESOURCE

    if($dataSize > DATA_SIZE_THRESHOLD) {
        writeLine("serverLog", "Use slow replication resource");
       *Resource=SLOW_REPLICATION_RESOURCE
    }
    else { 
        writeLine("serverLog", "Use fast replication resource"); 
    }

    *err = errormsg( msiDataObjRepl($objPath,"destRescName=*Resource",*Status), *msg );
    if( 0 != *err ) {
        writeLine( "serverLog", "*err - *msg" );
    }
    else {
        writeLine("serverLog", "replication done");
    }
}
