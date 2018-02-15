

DATA_SIZE_THRESHOLD = 40
FAST_REPLICATION_RESOURCE = "replResc1"
SLOW_REPLICATION_RESOURCE = "replResc2"

acPostProcForPut{
    tudReplicate();
}


tudReplicate{
    *home="/$rodsZoneClient/home/$userNameClient"
    writeLine("serverLog", "Starting tudReplicate on object: $objPath");
    *Resource=FAST_REPLICATION_RESOURCE

    if($dataSize > DATA_SIZE_THRESHOLD) {
        writeLine("serverLog", "Use slow replication resource");
       *Resource=FAST_REPLICATION_RESOURCE
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
